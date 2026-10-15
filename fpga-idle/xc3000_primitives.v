// Logical XC3000 primitive models recovered for the LabTool XC3030 work.
//
// These models intentionally contain no timing arcs or analogue drive-strength
// claims. INIT[0] is the all-zero input row. A is the LUT address LSB.

`timescale 1ns/1ps

module XC3000_LUT4 #(
    parameter [15:0] INIT = 16'h0000
) (
    input  wire A,
    input  wire I2,
    input  wire I3,
    input  wire I4,
    output wire Y
);
    // A mux tree preserves known outputs when an X/Z input is irrelevant
    // to this truth table. Vector indexing would pessimistically return X
    // even for a constant function or a configured wire with unused inputs.
    wire [7:0] level1;
    wire [3:0] level2;
    wire [1:0] level3;
    genvar i;
    generate
        for (i = 0; i < 8; i = i + 1) begin: mux_a
            assign level1[i] = A ? INIT[2*i+1] : INIT[2*i];
        end
        for (i = 0; i < 4; i = i + 1) begin: mux_i2
            assign level2[i] = I2 ? level1[2*i+1] : level1[2*i];
        end
        for (i = 0; i < 2; i = i + 1) begin: mux_i3
            assign level3[i] = I3 ? level2[2*i+1] : level2[2*i];
        end
    endgenerate
    assign Y = I4 ? level3[1] : level3[0];
endmodule

module XC3000_FGM #(
    parameter [15:0] INIT_F = 16'h0000,
    parameter [15:0] INIT_G = 16'h0000
) (
    input  wire A,
    input  wire F2,
    input  wire F3,
    input  wire F4,
    input  wire G2,
    input  wire G3,
    input  wire G4,
    input  wire E,
    output wire F,
    output wire G,
    output wire M
);
    XC3000_LUT4 #(.INIT(INIT_F)) f_lut(A, F2, F3, F4, F);
    XC3000_LUT4 #(.INIT(INIT_G)) g_lut(A, G2, G3, G4, G);
    assign M = E ? G : F;
endmodule

module XC3000_CLB_FF #(
    parameter integer CLOCK_INVERTED = 0,
    parameter integer EC_USED = 0,
    parameter integer RD_USED = 0
) (
    input  wire D,
    input  wire K,
    input  wire EC,
    input  wire RD,
    output reg  Q
);
    wire effective_k = K ^ CLOCK_INVERTED;
    wire effective_rd = RD_USED && RD;
    reg previous_k;

    // XC3000 configuration applies global reset before user operation.  A
    // deterministic zero here models that documented post-configuration
    // state and makes complete recovered designs replayable from time zero.
    initial begin
        Q = 1'b0;
        // Establish the initial clock level without inventing an edge when
        // an inverted or externally driven clock starts high.
        previous_k = 1'bx;
    end

    // Do not turn a 0->X transition in a partly recovered netlist into a
    // physical clock edge.  IEEE posedge includes that transition, while the
    // real device of course sees only 0 or 1.
    always @(effective_k or effective_rd) begin
        if (effective_rd === 1'b1)
            Q <= 1'b0;
        else if (previous_k === 1'b0 && effective_k === 1'b1)
            // An unresolved enable must merge the hold and capture cases.
            // Treating X as false concealed missing routing decodes as zeroes.
            Q <= effective_rd ? 1'b0 : (EC_USED ? (EC ? D : Q) : D);
        else if (effective_rd !== 1'b0)
            Q <= effective_rd ? 1'b0 : Q;
        if (effective_k === 1'b0 || effective_k === 1'b1)
            previous_k = effective_k;
    end
endmodule

module XC3000_IOB_INPUT_FF #(
    parameter integer CLOCK_INVERTED = 0
) (
    input  wire PAD_IN,
    input  wire IK,
    output reg  Q
);
    wire effective_ik = IK ^ CLOCK_INVERTED;
    reg previous_ik;
    initial begin
        Q = 1'b0;
        previous_ik = 1'bx;
    end
    always @(effective_ik) begin
        if (previous_ik === 1'b0 && effective_ik === 1'b1)
            Q <= PAD_IN;
        if (effective_ik === 1'b0 || effective_ik === 1'b1)
            previous_ik = effective_ik;
    end
endmodule

module XC3000_IOB_INPUT_LATCH #(
    parameter integer CLOCK_INVERTED = 0
) (
    input  wire PAD_IN,
    input  wire IK,
    output reg  Q
);
    wire effective_ik = IK ^ CLOCK_INVERTED;
    initial Q = 1'b0;
    always @* begin
        if (!effective_ik)
            Q = PAD_IN;
    end
endmodule

module XC3000_IOB_OUTPUT_FF #(
    parameter integer CLOCK_INVERTED = 0
) (
    input  wire D,
    input  wire OK,
    output reg  Q
);
    wire effective_ok = OK ^ CLOCK_INVERTED;
    reg previous_ok;
    initial begin
        Q = 1'b0;
        previous_ok = 1'bx;
    end
    always @(effective_ok) begin
        if (previous_ok === 1'b0 && effective_ok === 1'b1)
            Q <= D;
        if (effective_ok === 1'b0 || effective_ok === 1'b1)
            previous_ok = effective_ok;
    end
endmodule

module XC3000_IOB_DRIVER #(
    parameter integer DATA_INVERTED = 0,
    parameter integer T_INVERTED = 0
) (
    input  wire O,
    input  wire T,
    inout  wire PAD
);
    wire effective_o = O ^ DATA_INVERTED;
    wire effective_t = T ^ T_INVERTED;
    assign PAD = effective_t ? 1'bz : effective_o;
endmodule

module XC3000_TBUF #(
    parameter integer T_INVERTED = 0
) (
    input  wire I,
    input  wire T,
    output wire O
);
    wire effective_t = T ^ T_INVERTED;
    assign O = effective_t ? 1'bz : I;
endmodule

module XC3000_LONG_LINE_PULLUP (
    inout wire O
);
    pullup(O);
endmodule

// Attach one keeper to each modeled horizontal long line. A weak feedback
// driver retains the last known resolved level while permitting an active
// TBUF or pull-up to override it. This avoids simulator-specific trireg support.
module XC3000_LONG_LINE_KEEPER (
    inout wire O
);
    reg held;
    reg valid;
    assign (weak0, weak1) O = valid ? held : 1'bz;

    initial begin
        held = 1'b0;
        valid = 1'b0;
    end

    always @(O) begin
        if ((O === 1'b0 || O === 1'b1) && (!valid || O != held)) begin
            held <= O;
            valid <= 1'b1;
        end
    end
endmodule
