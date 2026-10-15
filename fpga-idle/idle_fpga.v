// Generated structural recovery of an XC3000-family configuration.
// Decoded configuration device: xc3030a
// Source stream: 1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab.bin
// Source SHA-256: 1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab
// Fitted physical target: XC3030A-7PC84C
// XC3000_* names refer to shared logical primitive models.
// Unknown routing, if present, is emitted explicitly and recorded in constraints.
`timescale 1ns/1ps

module xc3030_1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab(
    pin_P10,
    pin_P11,
    pin_P13,
    pin_P14,
    pin_P15,
    pin_P16,
    pin_P17,
    pin_P18,
    pin_P19,
    pin_P2,
    pin_P20,
    pin_P21,
    pin_P23,
    pin_P24,
    pin_P25,
    pin_P26,
    pin_P27,
    pin_P28,
    pin_P29,
    pin_P3,
    pin_P30,
    pin_P33,
    pin_P34,
    pin_P35,
    pin_P36,
    pin_P37,
    pin_P38,
    pin_P39,
    pin_P4,
    pin_P40,
    pin_P41,
    pin_P42,
    pin_P44,
    pin_P45,
    pin_P46,
    pin_P47,
    pin_P48,
    pin_P49,
    pin_P5,
    pin_P50,
    pin_P51,
    pin_P52,
    pin_P53,
    pin_P56,
    pin_P57,
    pin_P58,
    pin_P59,
    pin_P6,
    pin_P60,
    pin_P61,
    pin_P62,
    pin_P63,
    pin_P65,
    pin_P66,
    pin_P67,
    pin_P68,
    pin_P69,
    pin_P7,
    pin_P70,
    pin_P71,
    pin_P72,
    pin_P73,
    pin_P75,
    pin_P76,
    pin_P77,
    pin_P78,
    pin_P79,
    pin_P8,
    pin_P80,
    pin_P81,
    pin_P82,
    pin_P83,
    pin_P84,
    pin_P9,
    stimulus_D0X0Y9_MAIN_CLKIOB,
    stimulus_D0X9Y0_MAIN_CLKIOB,
    stimulus_D0X9Y0_MAIN_OSC
);
  inout wire pin_P10;
  inout wire pin_P11;
  inout wire pin_P13;
  inout wire pin_P14;
  inout wire pin_P15;
  inout wire pin_P16;
  inout wire pin_P17;
  inout wire pin_P18;
  inout wire pin_P19;
  inout wire pin_P2;
  inout wire pin_P20;
  inout wire pin_P21;
  inout wire pin_P23;
  inout wire pin_P24;
  inout wire pin_P25;
  inout wire pin_P26;
  inout wire pin_P27;
  inout wire pin_P28;
  inout wire pin_P29;
  inout wire pin_P3;
  inout wire pin_P30;
  inout wire pin_P33;
  inout wire pin_P34;
  inout wire pin_P35;
  inout wire pin_P36;
  inout wire pin_P37;
  inout wire pin_P38;
  inout wire pin_P39;
  inout wire pin_P4;
  inout wire pin_P40;
  inout wire pin_P41;
  inout wire pin_P42;
  inout wire pin_P44;
  inout wire pin_P45;
  inout wire pin_P46;
  inout wire pin_P47;
  inout wire pin_P48;
  inout wire pin_P49;
  inout wire pin_P5;
  inout wire pin_P50;
  inout wire pin_P51;
  inout wire pin_P52;
  inout wire pin_P53;
  inout wire pin_P56;
  inout wire pin_P57;
  inout wire pin_P58;
  inout wire pin_P59;
  inout wire pin_P6;
  inout wire pin_P60;
  inout wire pin_P61;
  inout wire pin_P62;
  inout wire pin_P63;
  inout wire pin_P65;
  inout wire pin_P66;
  inout wire pin_P67;
  inout wire pin_P68;
  inout wire pin_P69;
  inout wire pin_P7;
  inout wire pin_P70;
  inout wire pin_P71;
  inout wire pin_P72;
  inout wire pin_P73;
  inout wire pin_P75;
  inout wire pin_P76;
  inout wire pin_P77;
  inout wire pin_P78;
  inout wire pin_P79;
  inout wire pin_P8;
  inout wire pin_P80;
  inout wire pin_P81;
  inout wire pin_P82;
  inout wire pin_P83;
  inout wire pin_P84;
  inout wire pin_P9;
  input wire stimulus_D0X0Y9_MAIN_CLKIOB;
  input wire stimulus_D0X9Y0_MAIN_CLKIOB;
  input wire stimulus_D0X9Y0_MAIN_OSC;
  tri net_0000;
  tri net_0001;
  tri net_0002;
  tri net_0003;
  tri net_0004;
  tri net_0005;
  tri net_0006;
  tri net_0007;
  tri net_0008;
  tri net_0009;
  tri net_0010;
  tri net_0011;
  tri net_0012;
  tri net_0013;
  tri net_0014;
  tri net_0015;
  tri net_0016;
  tri net_0017;
  tri net_0018;
  tri net_0019;
  tri net_0020;
  tri net_0021;
  tri net_0022;
  tri net_0023;
  tri net_0024;
  tri net_0025;
  tri net_0026;
  tri net_0027;
  tri net_0028;
  tri net_0029;
  tri net_0030;
  tri net_0031;
  tri net_0032;
  tri net_0033;
  tri net_0034;
  tri net_0035;
  tri net_0036;
  tri net_0037;
  tri net_0038;
  tri net_0039;
  tri net_0040;
  tri net_0041;
  tri net_0042;
  tri net_0043;
  tri net_0044;
  tri net_0045;
  tri net_0046;
  tri net_0047;
  tri net_0048;
  tri net_0049;
  tri net_0050;
  tri net_0051;
  tri net_0052;
  tri net_0053;
  tri net_0054;
  tri net_0055;
  tri net_0056;
  tri net_0057;
  tri net_0058;
  tri net_0059;
  tri net_0060;
  tri net_0061;
  tri net_0062;
  tri net_0063;
  tri net_0064;
  tri net_0065;
  tri net_0066;
  tri net_0067;
  tri net_0068;
  tri net_0069;
  tri net_0070;
  tri net_0071;
  tri net_0072;
  tri net_0073;
  tri net_0074;
  tri net_0075;
  tri net_0076;
  tri net_0077;
  tri net_0078;
  tri net_0079;
  tri net_0080;
  tri net_0081;
  tri net_0082;
  tri net_0083;
  tri net_0084;
  tri net_0085;
  tri net_0086;
  tri net_0087;
  tri net_0088;
  tri net_0089;
  tri net_0090;
  tri net_0091;
  tri net_0092;
  tri net_0093;
  tri net_0094;
  tri net_0095;
  tri net_0096;
  tri net_0097;
  tri net_0098;
  tri net_0099;
  tri net_0100;
  tri net_0101;
  tri net_0102;
  tri net_0103;
  tri net_0104;
  tri net_0105;
  tri net_0106;
  tri net_0107;
  tri net_0108;
  tri net_0109;
  tri net_0110;
  tri net_0111;
  tri net_0112;
  tri net_0113;
  tri net_0114;
  tri net_0115;
  tri net_0116;
  tri net_0117;
  tri net_0118;
  tri net_0119;
  tri net_0120;
  tri net_0121;
  tri net_0122;
  tri net_0123;
  tri net_0124;
  tri net_0125;
  tri net_0126;
  tri net_0127;
  tri net_0128;
  tri net_0129;
  tri net_0130;
  tri net_0131;
  tri net_0132;
  tri net_0133;
  tri net_0134;
  tri net_0135;
  tri net_0136;
  tri net_0137;
  tri net_0138;
  tri net_0139;
  tri net_0140;
  tri net_0141;
  tri net_0142;
  tri net_0143;
  tri net_0144;
  tri net_0145;
  tri net_0146;
  tri net_0147;
  tri net_0148;
  tri net_0149;
  tri net_0150;
  tri net_0151;
  tri net_0152;
  tri net_0153;
  tri net_0154;
  tri net_0155;
  tri net_0156;
  tri net_0157;
  tri net_0158;
  tri net_0159;
  tri net_0160;
  tri net_0161;
  tri net_0162;
  tri net_0163;
  tri net_0164;
  tri net_0165;
  tri net_0166;
  tri net_0167;
  tri net_0168;
  tri net_0169;
  tri net_0170;
  tri net_0171;
  tri net_0172;
  tri net_0173;
  tri net_0174;
  tri net_0175;
  tri net_0176;
  tri net_0177;
  tri net_0178;
  tri net_0179;
  tri net_0180;
  tri net_0181;
  tri net_0182;
  tri net_0183;
  tri net_0184;
  tri net_0185;
  tri net_0186;
  tri net_0187;
  tri net_0188;
  tri net_0189;
  tri net_0190;
  tri net_0191;
  tri net_0192;
  tri net_0193;
  tri net_0194;
  tri net_0195;
  tri net_0196;
  tri net_0197;
  tri net_0198;
  tri net_0199;
  tri net_0200;
  tri net_0201;
  tri net_0202;
  tri net_0203;
  tri net_0204;
  tri net_0205;
  tri net_0206;
  tri net_0207;
  tri net_0208;
  tri net_0209;
  tri net_0210;
  tri net_0211;
  tri net_0212;
  tri net_0213;
  tri net_0214;
  tri net_0215;
  tri net_0216;
  tri net_0217;
  tri net_0218;
  tri net_0219;
  tri net_0220;
  tri net_0221;
  tri net_0222;
  tri net_0223;
  tri net_0224;
  tri net_0225;
  tri net_0226;
  tri net_0227;
  tri net_0228;
  tri net_0229;
  tri net_0230;
  tri net_0231;
  tri net_0232;
  tri net_0233;
  tri net_0234;
  tri net_0235;
  tri net_0236;
  tri net_0237;
  tri net_0238;
  tri net_0239;
  tri net_0240;
  tri net_0241;
  tri net_0242;
  tri net_0243;
  tri net_0244;
  tri net_0245;
  tri net_0246;
  tri net_0247;
  tri net_0248;
  tri net_0249;
  tri net_0250;
  tri net_0251;
  tri net_0252;
  tri net_0253;
  tri net_0254;
  tri net_0255;
  tri net_0256;
  tri net_0257;
  tri net_0258;
  tri net_0259;
  tri net_0260;
  tri net_0261;
  tri net_0262;
  tri net_0263;
  tri net_0264;
  tri net_0265;
  tri net_0266;
  tri net_0267;
  tri net_0268;
  tri net_0269;
  tri net_0270;
  tri net_0271;
  tri net_0272;
  tri net_0273;
  tri net_0274;
  tri net_0275;
  tri net_0276;
  tri net_0277;
  tri net_0278;
  tri net_0279;
  tri net_0280;
  tri net_0281;
  tri net_0282;
  tri net_0283;
  tri net_0284;
  tri net_0285;
  tri net_0286;
  tri net_0287;
  tri net_0288;
  tri net_0289;
  tri net_0290;
  tri net_0291;
  tri net_0292;
  tri net_0293;
  tri net_0294;
  tri net_0295;
  tri net_0296;
  tri net_0297;
  tri net_0298;
  tri net_0299;
  tri net_0300;
  tri net_0301;
  tri net_0302;
  tri net_0303;
  tri net_0304;
  tri net_0305;
  tri net_0306;
  tri net_0307;
  tri net_0308;
  tri net_0309;
  tri net_0310;
  tri net_0311;
  tri net_0312;
  tri net_0313;
  tri net_0314;
  tri net_0315;
  tri net_0316;
  tri net_0317;
  tri net_0318;
  tri net_0319;
  tri net_0320;
  tri net_0321;
  tri net_0322;
  tri net_0323;
  tri net_0324;
  tri net_0325;
  tri net_0326;
  tri net_0327;
  tri net_0328;
  tri net_0329;
  tri net_0330;
  tri net_0331;
  tri net_0332;
  tri net_0333;
  tri net_0334;
  tri net_0335;
  tri net_0336;
  tri net_0337;
  tri net_0338;
  tri net_0339;
  tri net_0340;
  tri net_0341;
  tri net_0342;
  tri net_0343;
  tri net_0344;
  tri net_0345;
  tri net_0346;
  tri net_0347;
  tri net_0348;
  tri net_0349;
  tri net_0350;
  tri net_0351;
  tri net_0352;
  tri net_0353;
  tri net_0354;
  tri net_0355;
  tri net_0356;
  tri net_0357;
  tri net_0358;
  tri net_0359;
  tri net_0360;
  tri net_0361;
  tri net_0362;
  tri net_0363;
  tri net_0364;
  tri net_0365;
  tri net_0366;
  tri net_0367;
  tri net_0368;
  tri net_0369;
  tri net_0370;
  tri net_0371;
  tri net_0372;
  tri net_0373;
  tri net_0374;
  tri net_0375;
  tri net_0376;
  tri net_0377;
  tri net_0378;
  tri net_0379;
  tri net_0380;
  tri net_0381;
  tri net_0382;
  tri net_0383;
  tri net_0384;
  tri net_0385;
  tri net_0386;
  tri net_0387;
  tri net_0388;
  tri net_0389;
  tri net_0390;
  tri net_0391;
  tri net_0392;
  tri net_0393;
  tri net_0394;
  tri net_0395;
  tri net_0396;
  tri net_0397;
  tri net_0398;
  tri net_0399;
  tri net_0400;
  tri net_0401;
  tri net_0402;
  tri net_0403;
  tri net_0404;
  tri net_0405;
  tri net_0406;
  tri net_0407;
  tri net_0408;
  tri net_0409;
  tri net_0410;
  tri net_0411;
  tri net_0412;
  tri net_0413;
  tri net_0414;
  tri net_0415;
  tri net_0416;
  tri net_0417;
  tri net_0418;
  tri net_0419;
  tri net_0420;
  tri net_0421;
  tri net_0422;
  tri net_0423;
  tri net_0424;
  tri net_0425;
  tri net_0426;
  tri net_0427;
  tri net_0428;
  tri net_0429;
  tri net_0430;
  tri net_0431;
  tri net_0432;
  tri net_0433;
  tri net_0434;
  tri net_0435;
  tri net_0436;
  tri net_0437;
  tri net_0438;
  tri net_0439;
  tri net_0440;
  tri net_0441;
  tri net_0442;
  tri net_0443;
  tri net_0444;
  tri net_0445;
  tri net_0446;
  tri net_0447;
  tri net_0448;
  tri net_0449;
  tri net_0450;
  tri net_0451;
  tri net_0452;
  tri net_0453;
  tri net_0454;
  tri net_0455;
  tri net_0456;
  tri net_0457;
  tri net_0458;
  tri net_0459;
  tri net_0460;
  tri net_0461;
  tri net_0462;
  tri net_0463;
  tri net_0464;
  tri net_0465;
  tri net_0466;
  tri net_0467;
  tri net_0468;
  tri net_0469;
  tri net_0470;
  tri net_0471;
  tri net_0472;
  tri net_0473;
  tri net_0474;
  tri net_0475;
  tri net_0476;
  tri net_0477;
  tri net_0478;
  tri net_0479;
  tri net_0480;
  tri net_0481;
  tri net_0482;
  tri net_0483;
  tri net_0484;
  tri net_0485;
  tri net_0486;
  tri net_0487;
  tri net_0488;
  tri net_0489;
  tri net_0490;
  tri net_0491;
  tri net_0492;
  tri net_0493;
  tri net_0494;
  tri net_0495;
  tri net_0496;
  tri net_0497;
  tri net_0498;
  tri net_0499;
  tri net_0500;
  tri net_0501;
  tri net_0502;
  tri net_0503;
  tri net_0504;
  tri net_0505;
  tri net_0506;
  tri net_0507;
  tri net_0508;
  tri net_0509;
  tri net_0510;
  tri net_0511;
  tri net_0512;
  tri net_0513;
  tri net_0514;
  tri net_0515;
  tri net_0516;
  tri net_0517;
  tri net_0518;
  tri net_0519;
  tri net_0520;
  tri net_0521;
  tri net_0522;
  tri net_0523;
  tri net_0524;
  tri net_0525;
  tri net_0526;
  tri net_0527;
  tri net_0528;
  tri net_0529;
  tri net_0530;
  tri net_0531;
  tri net_0532;
  tri net_0533;
  tri net_0534;
  tri net_0535;
  tri net_0536;
  tri net_0537;
  tri net_0538;
  tri net_0539;
  tri net_0540;
  tri net_0541;
  tri net_0542;
  tri net_0543;
  tri net_0544;
  tri net_0545;
  tri net_0546;
  tri net_0547;
  tri net_0548;
  tri net_0549;
  tri net_0550;
  tri net_0551;
  tri net_0552;
  tri net_0553;
  tri net_0554;
  tri net_0555;
  tri net_0556;
  tri net_0557;
  tri net_0558;
  tri net_0559;
  tri net_0560;
  tri net_0561;
  tri net_0562;
  tri net_0563;
  tri net_0564;
  tri net_0565;
  tri net_0566;
  tri net_0567;
  tri net_0568;
  tri net_0569;
  tri net_0570;
  tri net_0571;
  tri net_0572;
  tri net_0573;
  tri net_0574;
  tri net_0575;
  tri net_0576;
  tri net_0577;
  tri net_0578;
  tri net_0579;
  tri net_0580;
  tri net_0581;
  tri net_0582;
  tri net_0583;
  tri net_0584;
  tri net_0585;
  tri net_0586;
  tri net_0587;
  tri net_0588;
  tri net_0589;
  tri net_0590;
  tri net_0591;
  tri net_0592;
  tri net_0593;
  tri net_0594;
  tri net_0595;
  tri net_0596;
  tri net_0597;
  tri net_0598;
  tri net_0599;
  tri net_0600;
  tri net_0601;
  tri net_0602;
  tri net_0603;
  tri net_0604;
  tri net_0605;
  tri net_0606;
  tri net_0607;
  tri net_0608;
  tri net_0609;
  tri net_0610;
  tri net_0611;
  tri net_0612;
  tri net_0613;
  tri net_0614;
  tri net_0615;
  tri net_0616;
  tri net_0617;
  tri net_0618;
  tri net_0619;
  tri net_0620;
  tri net_0621;
  tri net_0622;
  tri net_0623;
  tri net_0624;
  tri net_0625;
  tri net_0626;
  tri net_0627;
  tri net_0628;
  tri net_0629;
  tri net_0630;
  tri net_0631;
  tri net_0632;
  tri net_0633;
  tri net_0634;
  tri net_0635;
  tri net_0636;
  tri net_0637;
  tri net_0638;
  tri net_0639;
  tri net_0640;
  tri net_0641;
  tri net_0642;
  tri net_0643;
  tri net_0644;
  tri net_0645;
  tri net_0646;
  tri net_0647;
  tri net_0648;
  tri net_0649;
  tri net_0650;
  tri net_0651;
  tri net_0652;
  tri net_0653;
  tri net_0654;
  tri net_0655;
  tri net_0656;
  tri net_0657;
  tri net_0658;
  tri net_0659;
  tri net_0660;
  tri net_0661;
  tri net_0662;
  tri net_0663;
  tri net_0664;
  tri net_0665;
  tri net_0666;
  tri net_0667;
  tri net_0668;
  tri net_0669;
  tri net_0670;
  tri net_0671;
  tri net_0672;
  tri net_0673;
  tri net_0674;
  tri net_0675;
  tri net_0676;
  tri net_0677;
  tri net_0678;
  tri net_0679;
  tri net_0680;
  tri net_0681;
  tri net_0682;
  tri net_0683;
  tri net_0684;
  tri net_0685;
  tri net_0686;
  tri net_0687;
  tri net_0688;
  tri net_0689;
  tri net_0690;
  tri net_0691;
  tri net_0692;
  tri net_0693;
  tri net_0694;
  tri net_0695;
  tri net_0696;
  tri net_0697;
  tri net_0698;
  tri net_0699;
  tri net_0700;
  tri net_0701;
  tri net_0702;
  tri net_0703;
  tri net_0704;
  tri net_0705;
  tri net_0706;
  tri net_0707;
  tri net_0708;
  tri net_0709;
  tri net_0710;
  tri net_0711;
  tri net_0712;
  tri net_0713;
  tri net_0714;
  tri net_0715;
  tri net_0716;
  tri net_0717;
  tri net_0718;
  tri net_0719;
  tri net_0720;
  tri net_0721;
  tri net_0722;
  tri net_0723;
  tri net_0724;
  assign net_0013 = ~net_0000;
  assign net_0015 = ~net_0000;
  assign net_0014 = 1'b1;
  assign net_0033 = 1'b1;
  assign net_0046 = 1'b1;
  assign net_0059 = 1'b1;
  assign net_0070 = 1'b1;
  assign net_0081 = 1'b1;
  assign net_0093 = 1'b1;
  assign net_0103 = 1'b1;
  assign net_0112 = 1'b1;
  assign net_0129 = 1'b1;
  assign net_0145 = 1'b1;
  assign net_0152 = 1'b1;
  assign net_0158 = 1'b1;
  assign net_0160 = 1'b1;
  assign net_0165 = 1'b1;
  assign net_0168 = 1'b1;
  assign net_0170 = 1'b0;
  assign net_0171 = 1'b1;
  assign net_0175 = 1'b1;
  assign net_0179 = 1'b1;
  assign net_0184 = 1'b1;
  assign net_0193 = 1'b1;
  assign net_0197 = 1'b0;
  assign net_0198 = 1'b1;
  assign net_0205 = 1'b1;
  assign net_0208 = 1'b1;
  assign net_0215 = 1'b1;
  assign net_0222 = 1'b1;
  assign net_0227 = 1'b1;
  assign net_0231 = 1'b1;
  assign net_0235 = 1'b1;
  assign net_0239 = 1'b1;
  assign net_0247 = 1'b1;
  assign net_0252 = 1'b1;
  assign net_0258 = 1'b1;
  assign net_0267 = 1'b1;
  assign net_0268 = 1'b0;
  assign net_0274 = 1'b1;
  assign net_0277 = 1'b0;
  assign net_0278 = 1'b1;
  assign net_0285 = 1'b1;
  assign net_0289 = 1'b1;
  assign net_0291 = 1'b1;
  assign net_0295 = 1'b1;
  assign net_0302 = 1'b1;
  assign net_0312 = 1'b1;
  assign net_0319 = 1'b1;
  assign net_0324 = 1'b1;
  assign net_0327 = 1'b0;
  assign net_0333 = 1'b1;
  assign net_0334 = 1'b0;
  assign net_0339 = 1'b1;
  assign net_0342 = 1'b1;
  assign net_0343 = 1'b0;
  assign net_0347 = 1'b1;
  assign net_0350 = 1'b1;
  assign net_0358 = 1'b1;
  assign net_0363 = 1'b1;
  assign net_0371 = 1'b1;
  assign net_0375 = 1'b1;
  assign net_0383 = 1'b1;
  assign net_0392 = 1'b1;
  assign net_0398 = 1'b1;
  assign net_0405 = 1'b1;
  assign net_0408 = 1'b1;
  assign net_0412 = 1'b1;
  assign net_0423 = 1'b1;
  assign net_0428 = 1'b1;
  assign net_0431 = 1'b1;
  assign net_0436 = 1'b1;
  assign net_0444 = 1'b1;
  assign net_0451 = 1'b1;
  assign net_0458 = 1'b1;
  assign net_0462 = 1'b0;
  assign net_0463 = 1'b1;
  assign net_0467 = 1'b1;
  assign net_0470 = 1'b1;
  assign net_0479 = 1'b1;
  assign net_0487 = 1'b1;
  assign net_0493 = 1'b1;
  assign net_0502 = 1'b1;
  assign net_0510 = 1'b1;
  assign net_0519 = 1'b1;
  assign net_0526 = 1'b1;
  assign net_0533 = 1'b1;
  assign net_0537 = 1'b1;
  assign net_0541 = 1'b1;
  assign net_0548 = 1'b1;
  assign net_0549 = 1'b0;
  assign net_0555 = 1'b1;
  assign net_0556 = 1'b0;
  assign net_0561 = 1'b0;
  assign net_0562 = 1'b1;
  assign net_0568 = 1'b1;
  assign net_0574 = 1'b1;
  assign net_0579 = 1'b1;
  assign net_0582 = 1'b1;
  assign net_0583 = 1'b0;
  assign net_0591 = 1'b1;
  assign net_0595 = 1'b0;
  assign net_0596 = 1'b1;
  assign net_0600 = 1'b1;
  assign net_0602 = 1'b0;
  assign net_0610 = 1'b1;
  assign net_0634 = 1'b1;
  assign net_0646 = 1'b1;
  assign net_0655 = 1'b1;
  assign net_0665 = 1'b1;
  assign net_0673 = 1'b1;
  assign net_0683 = 1'b1;
  assign net_0693 = 1'b1;
  assign net_0701 = 1'b1;
  assign net_0702 = 1'b0;
  assign net_0713 = 1'b1;
  assign net_0714 = 1'b0;
  XC3000_LONG_LINE_KEEPER keeper_net_0000 (.O(net_0000));
  XC3000_LONG_LINE_KEEPER keeper_net_0003 (.O(net_0003));
  XC3000_LONG_LINE_KEEPER keeper_net_0016 (.O(net_0016));
  XC3000_LONG_LINE_KEEPER keeper_net_0027 (.O(net_0027));
  XC3000_LONG_LINE_KEEPER keeper_net_0034 (.O(net_0034));
  XC3000_LONG_LINE_KEEPER keeper_net_0042 (.O(net_0042));
  XC3000_LONG_LINE_KEEPER keeper_net_0047 (.O(net_0047));
  XC3000_LONG_LINE_KEEPER keeper_net_0057 (.O(net_0057));
  XC3000_LONG_LINE_KEEPER keeper_net_0060 (.O(net_0060));
  XC3000_LONG_LINE_KEEPER keeper_net_0071 (.O(net_0071));
  XC3000_LONG_LINE_KEEPER keeper_net_0078 (.O(net_0078));
  XC3000_LONG_LINE_KEEPER keeper_net_0082 (.O(net_0082));
  XC3000_LONG_LINE_KEEPER keeper_net_0089 (.O(net_0089));
  XC3000_LONG_LINE_KEEPER keeper_net_0094 (.O(net_0094));
  XC3000_LONG_LINE_KEEPER keeper_net_0099 (.O(net_0099));
  XC3000_LONG_LINE_KEEPER keeper_net_0105 (.O(net_0105));
  XC3000_LONG_LINE_KEEPER keeper_net_0110 (.O(net_0110));
  XC3000_LONG_LINE_KEEPER keeper_net_0115 (.O(net_0115));
  XC3000_LONG_LINE_KEEPER keeper_net_0123 (.O(net_0123));

  // D0X0Y0.MAIN.CLB; MODE=FG
  wire D0X0Y0_MAIN_CLB_f, D0X0Y0_MAIN_CLB_g, D0X0Y0_MAIN_CLB_m, D0X0Y0_MAIN_CLB_qx, D0X0Y0_MAIN_CLB_qy, D0X0Y0_MAIN_CLB_dx, D0X0Y0_MAIN_CLB_dy;
  (* XC3000_LOC="D0X0Y0.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110010101010), .INIT_G(16'b1111000000110011)) D0X0Y0_MAIN_CLB_lut (
    .A(net_0003), .F2(D0X0Y0_MAIN_CLB_qx), .F3(net_0005), .F4(net_0008),
    .G2(net_0004), .G3(D0X0Y0_MAIN_CLB_qy), .G4(net_0008), .E(net_0008), .F(D0X0Y0_MAIN_CLB_f), .G(D0X0Y0_MAIN_CLB_g), .M(D0X0Y0_MAIN_CLB_m));
  assign D0X0Y0_MAIN_CLB_dx = D0X0Y0_MAIN_CLB_f;
  assign D0X0Y0_MAIN_CLB_dy = D0X0Y0_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y0_MAIN_CLB_ffx (.D(D0X0Y0_MAIN_CLB_dx), .K(net_0002), .EC(net_0009), .RD(net_0010), .Q(D0X0Y0_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y0_MAIN_CLB_ffy (.D(D0X0Y0_MAIN_CLB_dy), .K(net_0002), .EC(net_0009), .RD(net_0010), .Q(D0X0Y0_MAIN_CLB_qy));
  assign net_0019 = D0X0Y0_MAIN_CLB_qx;
  assign net_0011 = D0X0Y0_MAIN_CLB_qy;

  (* XC3000_LOC="D0X0Y0.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X0Y0_MAIN_TBUF_0_ (.I(net_0007), .T(net_0014), .O(net_0016));

  (* XC3000_LOC="D0X0Y0.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X0Y0_MAIN_TBUF_1_ (.I(net_0005), .T(net_0014), .O(net_0003));

  // D0X0Y0.MAIN.IO_W[0]
  assign net_0024 = pin_P29;
  wire D0X0Y0_MAIN_IO_W_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y0_MAIN_IO_W_0__input (.PAD_IN(pin_P29), .IK(net_0013), .Q(D0X0Y0_MAIN_IO_W_0__input_q));
  assign net_0026 = D0X0Y0_MAIN_IO_W_0__input_q;
  wire D0X0Y0_MAIN_IO_W_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y0_MAIN_IO_W_0__output_ff (.D(net_0007), .OK(net_0002), .Q(D0X0Y0_MAIN_IO_W_0__output_q));
  pullup(pin_P29);
  (* XC3000_LOC="D0X0Y0.MAIN", XC3000_BEL="IO_W[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y0_MAIN_IO_W_0__driver (.O(net_0007), .T(1'b1), .PAD(pin_P29));

  // D0X0Y0.MAIN.IO_W[1]
  assign net_0025 = pin_P30;
  wire D0X0Y0_MAIN_IO_W_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y0_MAIN_IO_W_1__input (.PAD_IN(pin_P30), .IK(net_0013), .Q(D0X0Y0_MAIN_IO_W_1__input_q));
  assign net_0007 = D0X0Y0_MAIN_IO_W_1__input_q;
  wire D0X0Y0_MAIN_IO_W_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y0_MAIN_IO_W_1__output_ff (.D(net_0009), .OK(net_0002), .Q(D0X0Y0_MAIN_IO_W_1__output_q));
  (* XC3000_LOC="D0X0Y0.MAIN", XC3000_BEL="IO_W[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y0_MAIN_IO_W_1__driver (.O(net_0009), .T(net_0014), .PAD(pin_P30));

  // D0X0Y0.MAIN.IO_S[0]
  assign net_0020 = pin_P33;
  wire D0X0Y0_MAIN_IO_S_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y0_MAIN_IO_S_0__input (.PAD_IN(pin_P33), .IK(net_0000), .Q(D0X0Y0_MAIN_IO_S_0__input_q));
  assign net_0022 = D0X0Y0_MAIN_IO_S_0__input_q;
  wire D0X0Y0_MAIN_IO_S_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y0_MAIN_IO_S_0__output_ff (.D(net_0006), .OK(net_0002), .Q(D0X0Y0_MAIN_IO_S_0__output_q));
  pullup(pin_P33);
  (* XC3000_LOC="D0X0Y0.MAIN", XC3000_BEL="IO_S[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y0_MAIN_IO_S_0__driver (.O(net_0006), .T(1'b1), .PAD(pin_P33));

  // D0X0Y0.MAIN.IO_S[1]
  assign net_0021 = pin_P34;
  wire D0X0Y0_MAIN_IO_S_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y0_MAIN_IO_S_1__input (.PAD_IN(pin_P34), .IK(net_0000), .Q(D0X0Y0_MAIN_IO_S_1__input_q));
  assign net_0023 = D0X0Y0_MAIN_IO_S_1__input_q;
  wire D0X0Y0_MAIN_IO_S_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y0_MAIN_IO_S_1__output_ff (.D(net_0011), .OK(net_0002), .Q(D0X0Y0_MAIN_IO_S_1__output_q));
  pullup(pin_P34);
  (* XC3000_LOC="D0X0Y0.MAIN", XC3000_BEL="IO_S[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y0_MAIN_IO_S_1__driver (.O(net_0011), .T(1'b1), .PAD(pin_P34));

  // D0X0Y1.MAIN.CLB; MODE=FG
  wire D0X0Y1_MAIN_CLB_f, D0X0Y1_MAIN_CLB_g, D0X0Y1_MAIN_CLB_m, D0X0Y1_MAIN_CLB_qx, D0X0Y1_MAIN_CLB_qy, D0X0Y1_MAIN_CLB_dx, D0X0Y1_MAIN_CLB_dy;
  (* XC3000_LOC="D0X0Y1.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X0Y1_MAIN_CLB_lut (
    .A(net_0009), .F2(D0X0Y1_MAIN_CLB_qx), .F3(net_0017), .F4(net_0027),
    .G2(net_0004), .G3(D0X0Y1_MAIN_CLB_qy), .G4(net_0027), .E(net_0007), .F(D0X0Y1_MAIN_CLB_f), .G(D0X0Y1_MAIN_CLB_g), .M(D0X0Y1_MAIN_CLB_m));
  assign D0X0Y1_MAIN_CLB_dx = D0X0Y1_MAIN_CLB_f;
  assign D0X0Y1_MAIN_CLB_dy = D0X0Y1_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y1_MAIN_CLB_ffx (.D(D0X0Y1_MAIN_CLB_dx), .K(net_0002), .EC(net_0009), .RD(net_0007), .Q(D0X0Y1_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y1_MAIN_CLB_ffy (.D(D0X0Y1_MAIN_CLB_dy), .K(net_0002), .EC(net_0009), .RD(net_0007), .Q(D0X0Y1_MAIN_CLB_qy));
  assign net_0029 = D0X0Y1_MAIN_CLB_qx;
  assign net_0030 = D0X0Y1_MAIN_CLB_qy;

  (* XC3000_LOC="D0X0Y1.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X0Y1_MAIN_TBUF_0_ (.I(net_0028), .T(net_0033), .O(net_0027));

  (* XC3000_LOC="D0X0Y1.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X0Y1_MAIN_TBUF_1_ (.I(net_0032), .T(net_0033), .O(net_0034));

  // D0X0Y1.MAIN.IO_W[0]
  assign net_0035 = pin_P28;
  wire D0X0Y1_MAIN_IO_W_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y1_MAIN_IO_W_0__input (.PAD_IN(pin_P28), .IK(net_0013), .Q(D0X0Y1_MAIN_IO_W_0__input_q));
  assign net_0037 = D0X0Y1_MAIN_IO_W_0__input_q;
  wire D0X0Y1_MAIN_IO_W_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y1_MAIN_IO_W_0__output_ff (.D(net_0029), .OK(net_0002), .Q(D0X0Y1_MAIN_IO_W_0__output_q));
  (* XC3000_LOC="D0X0Y1.MAIN", XC3000_BEL="IO_W[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X0Y1_MAIN_IO_W_0__driver (.O(net_0029), .T(~(net_0030)), .PAD(pin_P28));

  tri D0X0Y1_MAIN_IO_W_1__unbonded_pad;

  // D0X0Y1.MAIN.IO_W[1]
  assign net_0036 = D0X0Y1_MAIN_IO_W_1__unbonded_pad;
  wire D0X0Y1_MAIN_IO_W_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y1_MAIN_IO_W_1__input (.PAD_IN(D0X0Y1_MAIN_IO_W_1__unbonded_pad), .IK(net_0013), .Q(D0X0Y1_MAIN_IO_W_1__input_q));
  assign net_0038 = D0X0Y1_MAIN_IO_W_1__input_q;
  wire D0X0Y1_MAIN_IO_W_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y1_MAIN_IO_W_1__output_ff (.D(net_0009), .OK(net_0002), .Q(D0X0Y1_MAIN_IO_W_1__output_q));
  pullup(D0X0Y1_MAIN_IO_W_1__unbonded_pad);
  (* XC3000_LOC="D0X0Y1.MAIN", XC3000_BEL="IO_W[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y1_MAIN_IO_W_1__driver (.O(net_0009), .T(1'b1), .PAD(D0X0Y1_MAIN_IO_W_1__unbonded_pad));

  // D0X0Y2.MAIN.CLB; MODE=FGM
  wire D0X0Y2_MAIN_CLB_f, D0X0Y2_MAIN_CLB_g, D0X0Y2_MAIN_CLB_m, D0X0Y2_MAIN_CLB_qx, D0X0Y2_MAIN_CLB_qy, D0X0Y2_MAIN_CLB_dx, D0X0Y2_MAIN_CLB_dy;
  (* XC3000_LOC="D0X0Y2.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1100110011110000)) D0X0Y2_MAIN_CLB_lut (
    .A(net_0039), .F2(net_0040), .F3(net_0041), .F4(net_0042),
    .G2(net_0040), .G3(net_0041), .G4(net_0042), .E(net_0007), .F(D0X0Y2_MAIN_CLB_f), .G(D0X0Y2_MAIN_CLB_g), .M(D0X0Y2_MAIN_CLB_m));
  assign D0X0Y2_MAIN_CLB_dx = D0X0Y2_MAIN_CLB_m;
  assign D0X0Y2_MAIN_CLB_dy = D0X0Y2_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X0Y2_MAIN_CLB_ffx (.D(D0X0Y2_MAIN_CLB_dx), .K(net_0016), .EC(net_0039), .RD(net_0035), .Q(D0X0Y2_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X0Y2_MAIN_CLB_ffy (.D(D0X0Y2_MAIN_CLB_dy), .K(net_0016), .EC(net_0039), .RD(net_0035), .Q(D0X0Y2_MAIN_CLB_qy));
  assign net_0048 = D0X0Y2_MAIN_CLB_m;
  assign net_0049 = D0X0Y2_MAIN_CLB_qy;

  (* XC3000_LOC="D0X0Y2.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X0Y2_MAIN_TBUF_0_ (.I(net_0009), .T(net_0046), .O(net_0042));

  (* XC3000_LOC="D0X0Y2.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X0Y2_MAIN_TBUF_1_ (.I(net_0041), .T(net_0046), .O(net_0047));

  // D0X0Y2.MAIN.IO_W[0]
  assign net_0040 = pin_P27;
  wire D0X0Y2_MAIN_IO_W_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y2_MAIN_IO_W_0__input (.PAD_IN(pin_P27), .IK(net_0013), .Q(D0X0Y2_MAIN_IO_W_0__input_q));
  assign net_0051 = D0X0Y2_MAIN_IO_W_0__input_q;
  wire D0X0Y2_MAIN_IO_W_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y2_MAIN_IO_W_0__output_ff (.D(net_0043), .OK(net_0002), .Q(D0X0Y2_MAIN_IO_W_0__output_q));
  (* XC3000_LOC="D0X0Y2.MAIN", XC3000_BEL="IO_W[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X0Y2_MAIN_IO_W_0__driver (.O(net_0043), .T(~(net_0044)), .PAD(pin_P27));

  tri D0X0Y2_MAIN_IO_W_1__unbonded_pad;

  // D0X0Y2.MAIN.IO_W[1]
  assign net_0050 = D0X0Y2_MAIN_IO_W_1__unbonded_pad;
  wire D0X0Y2_MAIN_IO_W_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y2_MAIN_IO_W_1__input (.PAD_IN(D0X0Y2_MAIN_IO_W_1__unbonded_pad), .IK(net_0013), .Q(D0X0Y2_MAIN_IO_W_1__input_q));
  assign net_0052 = D0X0Y2_MAIN_IO_W_1__input_q;
  wire D0X0Y2_MAIN_IO_W_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y2_MAIN_IO_W_1__output_ff (.D(net_0039), .OK(net_0002), .Q(D0X0Y2_MAIN_IO_W_1__output_q));
  pullup(D0X0Y2_MAIN_IO_W_1__unbonded_pad);
  (* XC3000_LOC="D0X0Y2.MAIN", XC3000_BEL="IO_W[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y2_MAIN_IO_W_1__driver (.O(net_0039), .T(1'b1), .PAD(D0X0Y2_MAIN_IO_W_1__unbonded_pad));

  // D0X0Y3.MAIN.CLB; MODE=FG
  wire D0X0Y3_MAIN_CLB_f, D0X0Y3_MAIN_CLB_g, D0X0Y3_MAIN_CLB_m, D0X0Y3_MAIN_CLB_qx, D0X0Y3_MAIN_CLB_qy, D0X0Y3_MAIN_CLB_dx, D0X0Y3_MAIN_CLB_dy;
  (* XC3000_LOC="D0X0Y3.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X0Y3_MAIN_CLB_lut (
    .A(net_0056), .F2(D0X0Y3_MAIN_CLB_qx), .F3(net_0017), .F4(net_0057),
    .G2(net_0004), .G3(D0X0Y3_MAIN_CLB_qy), .G4(net_0057), .E(net_0009), .F(D0X0Y3_MAIN_CLB_f), .G(D0X0Y3_MAIN_CLB_g), .M(D0X0Y3_MAIN_CLB_m));
  assign D0X0Y3_MAIN_CLB_dx = D0X0Y3_MAIN_CLB_f;
  assign D0X0Y3_MAIN_CLB_dy = D0X0Y3_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y3_MAIN_CLB_ffx (.D(D0X0Y3_MAIN_CLB_dx), .K(net_0002), .EC(net_0009), .RD(net_0043), .Q(D0X0Y3_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y3_MAIN_CLB_ffy (.D(D0X0Y3_MAIN_CLB_dy), .K(net_0002), .EC(net_0009), .RD(net_0043), .Q(D0X0Y3_MAIN_CLB_qy));
  assign net_0058 = D0X0Y3_MAIN_CLB_qx;
  assign net_0018 = D0X0Y3_MAIN_CLB_qy;

  (* XC3000_LOC="D0X0Y3.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X0Y3_MAIN_TBUF_0_ (.I(net_0007), .T(net_0059), .O(net_0057));

  (* XC3000_LOC="D0X0Y3.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X0Y3_MAIN_TBUF_1_ (.I(net_0053), .T(net_0059), .O(net_0060));

  // D0X0Y3.MAIN.IO_W[0]
  assign net_0041 = pin_P25;
  wire D0X0Y3_MAIN_IO_W_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y3_MAIN_IO_W_0__input (.PAD_IN(pin_P25), .IK(net_0013), .Q(D0X0Y3_MAIN_IO_W_0__input_q));
  assign net_0061 = D0X0Y3_MAIN_IO_W_0__input_q;
  wire D0X0Y3_MAIN_IO_W_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y3_MAIN_IO_W_0__output_ff (.D(net_0058), .OK(net_0002), .Q(D0X0Y3_MAIN_IO_W_0__output_q));
  (* XC3000_LOC="D0X0Y3.MAIN", XC3000_BEL="IO_W[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X0Y3_MAIN_IO_W_0__driver (.O(net_0058), .T(~(net_0018)), .PAD(pin_P25));

  // D0X0Y3.MAIN.IO_W[1]
  assign net_0054 = pin_P26;
  wire D0X0Y3_MAIN_IO_W_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y3_MAIN_IO_W_1__input (.PAD_IN(pin_P26), .IK(net_0013), .Q(D0X0Y3_MAIN_IO_W_1__input_q));
  assign net_0062 = D0X0Y3_MAIN_IO_W_1__input_q;
  wire D0X0Y3_MAIN_IO_W_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y3_MAIN_IO_W_1__output_ff (.D(net_0055), .OK(net_0002), .Q(D0X0Y3_MAIN_IO_W_1__output_q));
  (* XC3000_LOC="D0X0Y3.MAIN", XC3000_BEL="IO_W[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X0Y3_MAIN_IO_W_1__driver (.O(net_0055), .T(~(net_0053)), .PAD(pin_P26));

  // D0X0Y4.MAIN.CLB; MODE=FG
  wire D0X0Y4_MAIN_CLB_f, D0X0Y4_MAIN_CLB_g, D0X0Y4_MAIN_CLB_m, D0X0Y4_MAIN_CLB_qx, D0X0Y4_MAIN_CLB_qy, D0X0Y4_MAIN_CLB_dx, D0X0Y4_MAIN_CLB_dy;
  (* XC3000_LOC="D0X0Y4.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000100000), .INIT_G(16'b0000000000000010)) D0X0Y4_MAIN_CLB_lut (
    .A(net_0065), .F2(net_0066), .F3(net_0007), .F4(net_0067),
    .G2(net_0066), .G3(net_0007), .G4(net_0067), .E(net_0068), .F(D0X0Y4_MAIN_CLB_f), .G(D0X0Y4_MAIN_CLB_g), .M(D0X0Y4_MAIN_CLB_m));
  assign D0X0Y4_MAIN_CLB_dx = D0X0Y4_MAIN_CLB_f;
  assign D0X0Y4_MAIN_CLB_dy = D0X0Y4_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X0Y4_MAIN_CLB_ffx (.D(D0X0Y4_MAIN_CLB_dx), .K(net_0064), .EC(net_0069), .RD(net_0007), .Q(D0X0Y4_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X0Y4_MAIN_CLB_ffy (.D(D0X0Y4_MAIN_CLB_dy), .K(net_0064), .EC(net_0069), .RD(net_0007), .Q(D0X0Y4_MAIN_CLB_qy));
  assign net_0009 = D0X0Y4_MAIN_CLB_f;
  assign net_0072 = D0X0Y4_MAIN_CLB_g;

  (* XC3000_LOC="D0X0Y4.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X0Y4_MAIN_TBUF_0_ (.I(net_0067), .T(net_0070), .O(net_0071));

  (* XC3000_LOC="D0X0Y4.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X0Y4_MAIN_TBUF_1_ (.I(net_0065), .T(net_0070), .O(net_0000));

  // D0X0Y4.MAIN.IO_W[0]
  assign net_0073 = pin_P23;
  wire D0X0Y4_MAIN_IO_W_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y4_MAIN_IO_W_0__input (.PAD_IN(pin_P23), .IK(net_0013), .Q(D0X0Y4_MAIN_IO_W_0__input_q));
  assign net_0065 = D0X0Y4_MAIN_IO_W_0__input_q;
  wire D0X0Y4_MAIN_IO_W_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y4_MAIN_IO_W_0__output_ff (.D(net_0067), .OK(net_0002), .Q(D0X0Y4_MAIN_IO_W_0__output_q));
  (* XC3000_LOC="D0X0Y4.MAIN", XC3000_BEL="IO_W[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y4_MAIN_IO_W_0__driver (.O(net_0067), .T(net_0070), .PAD(pin_P23));

  // D0X0Y4.MAIN.IO_W[1]
  assign net_0068 = pin_P24;
  wire D0X0Y4_MAIN_IO_W_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y4_MAIN_IO_W_1__input (.PAD_IN(pin_P24), .IK(net_0013), .Q(D0X0Y4_MAIN_IO_W_1__input_q));
  assign net_0074 = D0X0Y4_MAIN_IO_W_1__input_q;
  wire D0X0Y4_MAIN_IO_W_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y4_MAIN_IO_W_1__output_ff (.D(net_0063), .OK(net_0002), .Q(D0X0Y4_MAIN_IO_W_1__output_q));
  (* XC3000_LOC="D0X0Y4.MAIN", XC3000_BEL="IO_W[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X0Y4_MAIN_IO_W_1__driver (.O(net_0063), .T(~(net_0069)), .PAD(pin_P24));

  // D0X0Y5.MAIN.CLB; MODE=FGM
  wire D0X0Y5_MAIN_CLB_f, D0X0Y5_MAIN_CLB_g, D0X0Y5_MAIN_CLB_m, D0X0Y5_MAIN_CLB_qx, D0X0Y5_MAIN_CLB_qy, D0X0Y5_MAIN_CLB_dx, D0X0Y5_MAIN_CLB_dy;
  (* XC3000_LOC="D0X0Y5.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110000000000), .INIT_G(16'b1100110011111111)) D0X0Y5_MAIN_CLB_lut (
    .A(net_0076), .F2(net_0077), .F3(net_0066), .F4(net_0078),
    .G2(net_0077), .G3(net_0066), .G4(net_0078), .E(net_0080), .F(D0X0Y5_MAIN_CLB_f), .G(D0X0Y5_MAIN_CLB_g), .M(D0X0Y5_MAIN_CLB_m));
  assign D0X0Y5_MAIN_CLB_dx = D0X0Y5_MAIN_CLB_m;
  assign D0X0Y5_MAIN_CLB_dy = D0X0Y5_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X0Y5_MAIN_CLB_ffx (.D(D0X0Y5_MAIN_CLB_dx), .K(net_0069), .EC(net_0076), .RD(net_0067), .Q(D0X0Y5_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X0Y5_MAIN_CLB_ffy (.D(D0X0Y5_MAIN_CLB_dy), .K(net_0069), .EC(net_0076), .RD(net_0067), .Q(D0X0Y5_MAIN_CLB_qy));
  assign net_0083 = D0X0Y5_MAIN_CLB_qx;
  assign net_0084 = D0X0Y5_MAIN_CLB_m;

  (* XC3000_LOC="D0X0Y5.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X0Y5_MAIN_TBUF_0_ (.I(net_0079), .T(net_0081), .O(net_0078));

  (* XC3000_LOC="D0X0Y5.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X0Y5_MAIN_TBUF_1_ (.I(net_0066), .T(net_0081), .O(net_0082));

  // D0X0Y5.MAIN.IO_W[0]
  assign net_0085 = pin_P20;
  wire D0X0Y5_MAIN_IO_W_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y5_MAIN_IO_W_0__input (.PAD_IN(pin_P20), .IK(net_0013), .Q(D0X0Y5_MAIN_IO_W_0__input_q));
  assign net_0066 = D0X0Y5_MAIN_IO_W_0__input_q;
  wire D0X0Y5_MAIN_IO_W_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y5_MAIN_IO_W_0__output_ff (.D(net_0079), .OK(net_0002), .Q(D0X0Y5_MAIN_IO_W_0__output_q));
  (* XC3000_LOC="D0X0Y5.MAIN", XC3000_BEL="IO_W[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y5_MAIN_IO_W_0__driver (.O(net_0079), .T(net_0081), .PAD(pin_P20));

  // D0X0Y5.MAIN.IO_W[1]
  assign net_0080 = pin_P21;
  wire D0X0Y5_MAIN_IO_W_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y5_MAIN_IO_W_1__input (.PAD_IN(pin_P21), .IK(net_0013), .Q(D0X0Y5_MAIN_IO_W_1__input_q));
  assign net_0086 = D0X0Y5_MAIN_IO_W_1__input_q;
  wire D0X0Y5_MAIN_IO_W_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y5_MAIN_IO_W_1__output_ff (.D(net_0075), .OK(net_0002), .Q(D0X0Y5_MAIN_IO_W_1__output_q));
  (* XC3000_LOC="D0X0Y5.MAIN", XC3000_BEL="IO_W[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X0Y5_MAIN_IO_W_1__driver (.O(net_0075), .T(~(net_0076)), .PAD(pin_P21));

  // D0X0Y6.MAIN.CLB; MODE=FG
  wire D0X0Y6_MAIN_CLB_f, D0X0Y6_MAIN_CLB_g, D0X0Y6_MAIN_CLB_m, D0X0Y6_MAIN_CLB_qx, D0X0Y6_MAIN_CLB_qy, D0X0Y6_MAIN_CLB_dx, D0X0Y6_MAIN_CLB_dy;
  (* XC3000_LOC="D0X0Y6.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1011000110110001)) D0X0Y6_MAIN_CLB_lut (
    .A(net_0089), .F2(D0X0Y6_MAIN_CLB_qx), .F3(net_0017), .F4(net_0077),
    .G2(net_0004), .G3(D0X0Y6_MAIN_CLB_qy), .G4(net_0077), .E(net_0088), .F(D0X0Y6_MAIN_CLB_f), .G(D0X0Y6_MAIN_CLB_g), .M(D0X0Y6_MAIN_CLB_m));
  assign D0X0Y6_MAIN_CLB_dx = D0X0Y6_MAIN_CLB_f;
  assign D0X0Y6_MAIN_CLB_dy = D0X0Y6_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y6_MAIN_CLB_ffx (.D(D0X0Y6_MAIN_CLB_dx), .K(net_0002), .EC(net_0091), .RD(net_0066), .Q(D0X0Y6_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y6_MAIN_CLB_ffy (.D(D0X0Y6_MAIN_CLB_dy), .K(net_0002), .EC(net_0091), .RD(net_0066), .Q(D0X0Y6_MAIN_CLB_qy));
  assign net_0092 = D0X0Y6_MAIN_CLB_qx;
  assign net_0090 = D0X0Y6_MAIN_CLB_qy;

  (* XC3000_LOC="D0X0Y6.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X0Y6_MAIN_TBUF_0_ (.I(net_0090), .T(net_0093), .O(net_0094));

  (* XC3000_LOC="D0X0Y6.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X0Y6_MAIN_TBUF_1_ (.I(net_0087), .T(net_0093), .O(net_0089));

  // D0X0Y6.MAIN.IO_W[0]
  assign net_0095 = pin_P18;
  wire D0X0Y6_MAIN_IO_W_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y6_MAIN_IO_W_0__input (.PAD_IN(pin_P18), .IK(net_0013), .Q(D0X0Y6_MAIN_IO_W_0__input_q));
  assign net_0096 = D0X0Y6_MAIN_IO_W_0__input_q;
  wire D0X0Y6_MAIN_IO_W_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y6_MAIN_IO_W_0__output_ff (.D(net_0092), .OK(net_0002), .Q(D0X0Y6_MAIN_IO_W_0__output_q));
  (* XC3000_LOC="D0X0Y6.MAIN", XC3000_BEL="IO_W[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X0Y6_MAIN_IO_W_0__driver (.O(net_0092), .T(~(net_0090)), .PAD(pin_P18));

  // D0X0Y6.MAIN.IO_W[1]
  assign net_0077 = pin_P19;
  wire D0X0Y6_MAIN_IO_W_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y6_MAIN_IO_W_1__input (.PAD_IN(pin_P19), .IK(net_0013), .Q(D0X0Y6_MAIN_IO_W_1__input_q));
  assign net_0097 = D0X0Y6_MAIN_IO_W_1__input_q;
  wire D0X0Y6_MAIN_IO_W_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y6_MAIN_IO_W_1__output_ff (.D(net_0088), .OK(net_0002), .Q(D0X0Y6_MAIN_IO_W_1__output_q));
  (* XC3000_LOC="D0X0Y6.MAIN", XC3000_BEL="IO_W[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X0Y6_MAIN_IO_W_1__driver (.O(net_0088), .T(~(net_0087)), .PAD(pin_P19));

  // D0X0Y7.MAIN.CLB; MODE=FG
  wire D0X0Y7_MAIN_CLB_f, D0X0Y7_MAIN_CLB_g, D0X0Y7_MAIN_CLB_m, D0X0Y7_MAIN_CLB_qx, D0X0Y7_MAIN_CLB_qy, D0X0Y7_MAIN_CLB_dx, D0X0Y7_MAIN_CLB_dy;
  (* XC3000_LOC="D0X0Y7.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1011000110110001)) D0X0Y7_MAIN_CLB_lut (
    .A(net_0099), .F2(D0X0Y7_MAIN_CLB_qx), .F3(net_0017), .F4(net_0098),
    .G2(net_0004), .G3(D0X0Y7_MAIN_CLB_qy), .G4(net_0098), .E(net_0098), .F(D0X0Y7_MAIN_CLB_f), .G(D0X0Y7_MAIN_CLB_g), .M(D0X0Y7_MAIN_CLB_m));
  assign D0X0Y7_MAIN_CLB_dx = D0X0Y7_MAIN_CLB_f;
  assign D0X0Y7_MAIN_CLB_dy = D0X0Y7_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y7_MAIN_CLB_ffx (.D(D0X0Y7_MAIN_CLB_dx), .K(net_0002), .EC(net_0091), .RD(net_0095), .Q(D0X0Y7_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y7_MAIN_CLB_ffy (.D(D0X0Y7_MAIN_CLB_dy), .K(net_0002), .EC(net_0091), .RD(net_0095), .Q(D0X0Y7_MAIN_CLB_qy));
  assign net_0100 = D0X0Y7_MAIN_CLB_qx;
  assign net_0102 = D0X0Y7_MAIN_CLB_qy;

  (* XC3000_LOC="D0X0Y7.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X0Y7_MAIN_TBUF_0_ (.I(net_0067), .T(net_0103), .O(net_0105));

  (* XC3000_LOC="D0X0Y7.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X0Y7_MAIN_TBUF_1_ (.I(net_0104), .T(net_0103), .O(net_0099));

  // D0X0Y7.MAIN.IO_W[0]
  assign net_0106 = pin_P16;
  wire D0X0Y7_MAIN_IO_W_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y7_MAIN_IO_W_0__input (.PAD_IN(pin_P16), .IK(net_0013), .Q(D0X0Y7_MAIN_IO_W_0__input_q));
  assign net_0108 = D0X0Y7_MAIN_IO_W_0__input_q;
  wire D0X0Y7_MAIN_IO_W_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y7_MAIN_IO_W_0__output_ff (.D(net_0100), .OK(net_0002), .Q(D0X0Y7_MAIN_IO_W_0__output_q));
  (* XC3000_LOC="D0X0Y7.MAIN", XC3000_BEL="IO_W[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X0Y7_MAIN_IO_W_0__driver (.O(net_0100), .T(~(net_0102)), .PAD(pin_P16));

  // D0X0Y7.MAIN.IO_W[1]
  assign net_0107 = pin_P17;
  wire D0X0Y7_MAIN_IO_W_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y7_MAIN_IO_W_1__input (.PAD_IN(pin_P17), .IK(net_0013), .Q(D0X0Y7_MAIN_IO_W_1__input_q));
  assign net_0067 = D0X0Y7_MAIN_IO_W_1__input_q;
  wire D0X0Y7_MAIN_IO_W_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y7_MAIN_IO_W_1__output_ff (.D(net_0101), .OK(net_0002), .Q(D0X0Y7_MAIN_IO_W_1__output_q));
  (* XC3000_LOC="D0X0Y7.MAIN", XC3000_BEL="IO_W[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y7_MAIN_IO_W_1__driver (.O(net_0101), .T(net_0103), .PAD(pin_P17));

  // D0X0Y8.MAIN.CLB; MODE=FG
  wire D0X0Y8_MAIN_CLB_f, D0X0Y8_MAIN_CLB_g, D0X0Y8_MAIN_CLB_m, D0X0Y8_MAIN_CLB_qx, D0X0Y8_MAIN_CLB_qy, D0X0Y8_MAIN_CLB_dx, D0X0Y8_MAIN_CLB_dy;
  (* XC3000_LOC="D0X0Y8.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X0Y8_MAIN_CLB_lut (
    .A(net_0109), .F2(D0X0Y8_MAIN_CLB_qx), .F3(net_0017), .F4(net_0110),
    .G2(net_0004), .G3(D0X0Y8_MAIN_CLB_qy), .G4(net_0110), .E(net_0098), .F(D0X0Y8_MAIN_CLB_f), .G(D0X0Y8_MAIN_CLB_g), .M(D0X0Y8_MAIN_CLB_m));
  assign D0X0Y8_MAIN_CLB_dx = D0X0Y8_MAIN_CLB_f;
  assign D0X0Y8_MAIN_CLB_dy = D0X0Y8_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y8_MAIN_CLB_ffx (.D(D0X0Y8_MAIN_CLB_dx), .K(net_0002), .EC(net_0091), .RD(net_0106), .Q(D0X0Y8_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y8_MAIN_CLB_ffy (.D(D0X0Y8_MAIN_CLB_dy), .K(net_0002), .EC(net_0091), .RD(net_0106), .Q(D0X0Y8_MAIN_CLB_qy));
  assign net_0116 = D0X0Y8_MAIN_CLB_qx;
  assign net_0117 = D0X0Y8_MAIN_CLB_qy;

  (* XC3000_LOC="D0X0Y8.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X0Y8_MAIN_TBUF_0_ (.I(net_0111), .T(net_0112), .O(net_0110));

  (* XC3000_LOC="D0X0Y8.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X0Y8_MAIN_TBUF_1_ (.I(net_0114), .T(net_0112), .O(net_0115));

  // D0X0Y8.MAIN.IO_W[0]
  assign net_0118 = pin_P15;
  wire D0X0Y8_MAIN_IO_W_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y8_MAIN_IO_W_0__input (.PAD_IN(pin_P15), .IK(net_0013), .Q(D0X0Y8_MAIN_IO_W_0__input_q));
  assign net_0120 = D0X0Y8_MAIN_IO_W_0__input_q;
  wire D0X0Y8_MAIN_IO_W_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y8_MAIN_IO_W_0__output_ff (.D(net_0111), .OK(net_0002), .Q(D0X0Y8_MAIN_IO_W_0__output_q));
  (* XC3000_LOC="D0X0Y8.MAIN", XC3000_BEL="IO_W[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y8_MAIN_IO_W_0__driver (.O(net_0111), .T(net_0112), .PAD(pin_P15));

  tri D0X0Y8_MAIN_IO_W_1__unbonded_pad;

  // D0X0Y8.MAIN.IO_W[1]
  assign net_0119 = D0X0Y8_MAIN_IO_W_1__unbonded_pad;
  wire D0X0Y8_MAIN_IO_W_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y8_MAIN_IO_W_1__input (.PAD_IN(D0X0Y8_MAIN_IO_W_1__unbonded_pad), .IK(net_0013), .Q(D0X0Y8_MAIN_IO_W_1__input_q));
  assign net_0121 = D0X0Y8_MAIN_IO_W_1__input_q;
  wire D0X0Y8_MAIN_IO_W_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y8_MAIN_IO_W_1__output_ff (.D(net_0109), .OK(net_0002), .Q(D0X0Y8_MAIN_IO_W_1__output_q));
  pullup(D0X0Y8_MAIN_IO_W_1__unbonded_pad);
  (* XC3000_LOC="D0X0Y8.MAIN", XC3000_BEL="IO_W[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y8_MAIN_IO_W_1__driver (.O(net_0109), .T(1'b1), .PAD(D0X0Y8_MAIN_IO_W_1__unbonded_pad));

  // D0X0Y9.MAIN.CLB; MODE=FG
  wire D0X0Y9_MAIN_CLB_f, D0X0Y9_MAIN_CLB_g, D0X0Y9_MAIN_CLB_m, D0X0Y9_MAIN_CLB_qx, D0X0Y9_MAIN_CLB_qy, D0X0Y9_MAIN_CLB_dx, D0X0Y9_MAIN_CLB_dy;
  (* XC3000_LOC="D0X0Y9.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110010101010)) D0X0Y9_MAIN_CLB_lut (
    .A(net_0123), .F2(net_0004), .F3(D0X0Y9_MAIN_CLB_qx), .F4(net_0001),
    .G2(D0X0Y9_MAIN_CLB_qy), .G3(net_0124), .G4(net_0001), .E(net_0098), .F(D0X0Y9_MAIN_CLB_f), .G(D0X0Y9_MAIN_CLB_g), .M(D0X0Y9_MAIN_CLB_m));
  assign D0X0Y9_MAIN_CLB_dx = D0X0Y9_MAIN_CLB_f;
  assign D0X0Y9_MAIN_CLB_dy = D0X0Y9_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y9_MAIN_CLB_ffx (.D(D0X0Y9_MAIN_CLB_dx), .K(net_0002), .EC(net_0125), .RD(net_0122), .Q(D0X0Y9_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X0Y9_MAIN_CLB_ffy (.D(D0X0Y9_MAIN_CLB_dy), .K(net_0002), .EC(net_0125), .RD(net_0122), .Q(D0X0Y9_MAIN_CLB_qy));
  assign net_0128 = D0X0Y9_MAIN_CLB_qx;
  assign net_0126 = D0X0Y9_MAIN_CLB_qy;

  (* XC3000_LOC="D0X0Y9.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X0Y9_MAIN_TBUF_0_ (.I(net_0001), .T(net_0129), .O(net_0016));

  (* XC3000_LOC="D0X0Y9.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X0Y9_MAIN_TBUF_1_ (.I(net_0124), .T(net_0129), .O(net_0123));

  // D0X0Y9.MAIN.IO_W[0]
  assign net_0002 = pin_P13;
  wire D0X0Y9_MAIN_IO_W_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y9_MAIN_IO_W_0__input (.PAD_IN(pin_P13), .IK(net_0013), .Q(D0X0Y9_MAIN_IO_W_0__input_q));
  assign net_0137 = D0X0Y9_MAIN_IO_W_0__input_q;
  wire D0X0Y9_MAIN_IO_W_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y9_MAIN_IO_W_0__output_ff (.D(net_0001), .OK(net_0002), .Q(D0X0Y9_MAIN_IO_W_0__output_q));
  (* XC3000_LOC="D0X0Y9.MAIN", XC3000_BEL="IO_W[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y9_MAIN_IO_W_0__driver (.O(net_0001), .T(net_0129), .PAD(pin_P13));

  // D0X0Y9.MAIN.IO_W[1]
  assign net_0136 = pin_P14;
  wire D0X0Y9_MAIN_IO_W_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y9_MAIN_IO_W_1__input (.PAD_IN(pin_P14), .IK(net_0013), .Q(D0X0Y9_MAIN_IO_W_1__input_q));
  assign net_0138 = D0X0Y9_MAIN_IO_W_1__input_q;
  wire D0X0Y9_MAIN_IO_W_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y9_MAIN_IO_W_1__output_ff (.D(net_0002), .OK(net_0002), .Q(D0X0Y9_MAIN_IO_W_1__output_q));
  pullup(pin_P14);
  (* XC3000_LOC="D0X0Y9.MAIN", XC3000_BEL="IO_W[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y9_MAIN_IO_W_1__driver (.O(net_0002), .T(1'b1), .PAD(pin_P14));

  // D0X0Y9.MAIN.IO_N[0]
  assign net_0132 = pin_P11;
  wire D0X0Y9_MAIN_IO_N_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y9_MAIN_IO_N_0__input (.PAD_IN(pin_P11), .IK(net_0002), .Q(D0X0Y9_MAIN_IO_N_0__input_q));
  assign net_0134 = D0X0Y9_MAIN_IO_N_0__input_q;
  wire D0X0Y9_MAIN_IO_N_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y9_MAIN_IO_N_0__output_ff (.D(net_0125), .OK(net_0015), .Q(D0X0Y9_MAIN_IO_N_0__output_q));
  pullup(pin_P11);
  (* XC3000_LOC="D0X0Y9.MAIN", XC3000_BEL="IO_N[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X0Y9_MAIN_IO_N_0__driver (.O(net_0125), .T(1'b1), .PAD(pin_P11));

  // D0X0Y9.MAIN.IO_N[1]
  assign net_0133 = pin_P10;
  wire D0X0Y9_MAIN_IO_N_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X0Y9_MAIN_IO_N_1__input (.PAD_IN(pin_P10), .IK(net_0002), .Q(D0X0Y9_MAIN_IO_N_1__input_q));
  assign net_0135 = D0X0Y9_MAIN_IO_N_1__input_q;
  wire D0X0Y9_MAIN_IO_N_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X0Y9_MAIN_IO_N_1__output_ff (.D(net_0126), .OK(net_0015), .Q(D0X0Y9_MAIN_IO_N_1__output_q));
  (* XC3000_LOC="D0X0Y9.MAIN", XC3000_BEL="IO_N[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X0Y9_MAIN_IO_N_1__driver (.O(net_0126), .T(~(net_0128)), .PAD(pin_P10));
  assign net_0131 = stimulus_D0X0Y9_MAIN_CLKIOB;

  // D0X1Y0.MAIN.CLB; MODE=FG
  wire D0X1Y0_MAIN_CLB_f, D0X1Y0_MAIN_CLB_g, D0X1Y0_MAIN_CLB_m, D0X1Y0_MAIN_CLB_qx, D0X1Y0_MAIN_CLB_qy, D0X1Y0_MAIN_CLB_dx, D0X1Y0_MAIN_CLB_dy;
  (* XC3000_LOC="D0X1Y0.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110010101010), .INIT_G(16'b1111000000110011)) D0X1Y0_MAIN_CLB_lut (
    .A(net_0003), .F2(D0X1Y0_MAIN_CLB_qx), .F3(net_0139), .F4(net_0008),
    .G2(net_0004), .G3(D0X1Y0_MAIN_CLB_qy), .G4(net_0008), .E(net_0140), .F(D0X1Y0_MAIN_CLB_f), .G(D0X1Y0_MAIN_CLB_g), .M(D0X1Y0_MAIN_CLB_m));
  assign D0X1Y0_MAIN_CLB_dx = D0X1Y0_MAIN_CLB_f;
  assign D0X1Y0_MAIN_CLB_dy = D0X1Y0_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y0_MAIN_CLB_ffx (.D(D0X1Y0_MAIN_CLB_dx), .K(net_0002), .EC(net_0141), .RD(net_0142), .Q(D0X1Y0_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y0_MAIN_CLB_ffy (.D(D0X1Y0_MAIN_CLB_dy), .K(net_0002), .EC(net_0141), .RD(net_0142), .Q(D0X1Y0_MAIN_CLB_qy));
  assign net_0146 = D0X1Y0_MAIN_CLB_qx;
  assign net_0143 = D0X1Y0_MAIN_CLB_qy;

  (* XC3000_LOC="D0X1Y0.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X1Y0_MAIN_TBUF_0_ (.I(net_0019), .T(net_0145), .O(net_0016));

  (* XC3000_LOC="D0X1Y0.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X1Y0_MAIN_TBUF_1_ (.I(net_0139), .T(net_0145), .O(net_0003));

  // D0X1Y0.MAIN.IO_S[0]
  assign net_0147 = pin_P35;
  wire D0X1Y0_MAIN_IO_S_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X1Y0_MAIN_IO_S_0__input (.PAD_IN(pin_P35), .IK(net_0000), .Q(D0X1Y0_MAIN_IO_S_0__input_q));
  assign net_0149 = D0X1Y0_MAIN_IO_S_0__input_q;
  wire D0X1Y0_MAIN_IO_S_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X1Y0_MAIN_IO_S_0__output_ff (.D(net_0019), .OK(net_0002), .Q(D0X1Y0_MAIN_IO_S_0__output_q));
  (* XC3000_LOC="D0X1Y0.MAIN", XC3000_BEL="IO_S[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X1Y0_MAIN_IO_S_0__driver (.O(net_0019), .T(~(net_0011)), .PAD(pin_P35));

  // D0X1Y0.MAIN.IO_S[1]
  assign net_0148 = pin_P36;
  wire D0X1Y0_MAIN_IO_S_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X1Y0_MAIN_IO_S_1__input (.PAD_IN(pin_P36), .IK(net_0000), .Q(D0X1Y0_MAIN_IO_S_1__input_q));
  assign net_0150 = D0X1Y0_MAIN_IO_S_1__input_q;
  wire D0X1Y0_MAIN_IO_S_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X1Y0_MAIN_IO_S_1__output_ff (.D(net_0143), .OK(net_0002), .Q(D0X1Y0_MAIN_IO_S_1__output_q));
  pullup(pin_P36);
  (* XC3000_LOC="D0X1Y0.MAIN", XC3000_BEL="IO_S[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X1Y0_MAIN_IO_S_1__driver (.O(net_0143), .T(1'b1), .PAD(pin_P36));

  // D0X1Y1.MAIN.CLB; MODE=FGM
  wire D0X1Y1_MAIN_CLB_f, D0X1Y1_MAIN_CLB_g, D0X1Y1_MAIN_CLB_m, D0X1Y1_MAIN_CLB_qx, D0X1Y1_MAIN_CLB_qy, D0X1Y1_MAIN_CLB_dx, D0X1Y1_MAIN_CLB_dy;
  (* XC3000_LOC="D0X1Y1.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100101011000000), .INIT_G(16'b1100111111000101)) D0X1Y1_MAIN_CLB_lut (
    .A(net_0034), .F2(net_0048), .F3(net_0120), .F4(net_0147),
    .G2(net_0048), .G3(net_0120), .G4(net_0147), .E(net_0035), .F(D0X1Y1_MAIN_CLB_f), .G(D0X1Y1_MAIN_CLB_g), .M(D0X1Y1_MAIN_CLB_m));
  assign D0X1Y1_MAIN_CLB_dx = D0X1Y1_MAIN_CLB_m;
  assign D0X1Y1_MAIN_CLB_dy = D0X1Y1_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X1Y1_MAIN_CLB_ffx (.D(D0X1Y1_MAIN_CLB_dx), .K(net_0151), .EC(net_0035), .RD(net_0120), .Q(D0X1Y1_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X1Y1_MAIN_CLB_ffy (.D(D0X1Y1_MAIN_CLB_dy), .K(net_0151), .EC(net_0035), .RD(net_0120), .Q(D0X1Y1_MAIN_CLB_qy));
  assign net_0153 = D0X1Y1_MAIN_CLB_qx;
  assign net_0154 = D0X1Y1_MAIN_CLB_m;

  (* XC3000_LOC="D0X1Y1.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X1Y1_MAIN_TBUF_0_ (.I(net_0007), .T(net_0152), .O(net_0027));

  (* XC3000_LOC="D0X1Y1.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X1Y1_MAIN_TBUF_1_ (.I(net_0147), .T(net_0152), .O(net_0034));

  // D0X1Y2.MAIN.CLB; MODE=FG
  wire D0X1Y2_MAIN_CLB_f, D0X1Y2_MAIN_CLB_g, D0X1Y2_MAIN_CLB_m, D0X1Y2_MAIN_CLB_qx, D0X1Y2_MAIN_CLB_qy, D0X1Y2_MAIN_CLB_dx, D0X1Y2_MAIN_CLB_dy;
  (* XC3000_LOC="D0X1Y2.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X1Y2_MAIN_CLB_lut (
    .A(net_0016), .F2(D0X1Y2_MAIN_CLB_qx), .F3(net_0017), .F4(net_0016),
    .G2(net_0004), .G3(D0X1Y2_MAIN_CLB_qy), .G4(net_0016), .E(net_0016), .F(D0X1Y2_MAIN_CLB_f), .G(D0X1Y2_MAIN_CLB_g), .M(D0X1Y2_MAIN_CLB_m));
  assign D0X1Y2_MAIN_CLB_dx = D0X1Y2_MAIN_CLB_f;
  assign D0X1Y2_MAIN_CLB_dy = D0X1Y2_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y2_MAIN_CLB_ffx (.D(D0X1Y2_MAIN_CLB_dx), .K(net_0002), .EC(net_0009), .RD(net_0007), .Q(D0X1Y2_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y2_MAIN_CLB_ffy (.D(D0X1Y2_MAIN_CLB_dy), .K(net_0002), .EC(net_0009), .RD(net_0007), .Q(D0X1Y2_MAIN_CLB_qy));
  assign net_0043 = D0X1Y2_MAIN_CLB_qx;
  assign net_0044 = D0X1Y2_MAIN_CLB_qy;

  (* XC3000_LOC="D0X1Y2.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X1Y2_MAIN_TBUF_0_ (.I(net_0156), .T(net_0158), .O(net_0042));

  (* XC3000_LOC="D0X1Y2.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X1Y2_MAIN_TBUF_1_ (.I(net_0157), .T(net_0158), .O(net_0047));

  // D0X1Y3.MAIN.CLB; MODE=FG
  wire D0X1Y3_MAIN_CLB_f, D0X1Y3_MAIN_CLB_g, D0X1Y3_MAIN_CLB_m, D0X1Y3_MAIN_CLB_qx, D0X1Y3_MAIN_CLB_qy, D0X1Y3_MAIN_CLB_dx, D0X1Y3_MAIN_CLB_dy;
  (* XC3000_LOC="D0X1Y3.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1011000110110001)) D0X1Y3_MAIN_CLB_lut (
    .A(net_0060), .F2(D0X1Y3_MAIN_CLB_qx), .F3(net_0017), .F4(net_0043),
    .G2(net_0004), .G3(D0X1Y3_MAIN_CLB_qy), .G4(net_0043), .E(net_0018), .F(D0X1Y3_MAIN_CLB_f), .G(D0X1Y3_MAIN_CLB_g), .M(D0X1Y3_MAIN_CLB_m));
  assign D0X1Y3_MAIN_CLB_dx = D0X1Y3_MAIN_CLB_f;
  assign D0X1Y3_MAIN_CLB_dy = D0X1Y3_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y3_MAIN_CLB_ffx (.D(D0X1Y3_MAIN_CLB_dx), .K(net_0002), .EC(net_0072), .RD(net_0055), .Q(D0X1Y3_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y3_MAIN_CLB_ffy (.D(D0X1Y3_MAIN_CLB_dy), .K(net_0002), .EC(net_0072), .RD(net_0055), .Q(D0X1Y3_MAIN_CLB_qy));
  assign net_0055 = D0X1Y3_MAIN_CLB_qx;
  assign net_0053 = D0X1Y3_MAIN_CLB_qy;

  (* XC3000_LOC="D0X1Y3.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X1Y3_MAIN_TBUF_0_ (.I(net_0159), .T(net_0160), .O(net_0057));

  (* XC3000_LOC="D0X1Y3.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X1Y3_MAIN_TBUF_1_ (.I(net_0054), .T(net_0160), .O(net_0060));

  // D0X1Y4.MAIN.CLB; MODE=FG
  wire D0X1Y4_MAIN_CLB_f, D0X1Y4_MAIN_CLB_g, D0X1Y4_MAIN_CLB_m, D0X1Y4_MAIN_CLB_qx, D0X1Y4_MAIN_CLB_qy, D0X1Y4_MAIN_CLB_dx, D0X1Y4_MAIN_CLB_dy;
  (* XC3000_LOC="D0X1Y4.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X1Y4_MAIN_CLB_lut (
    .A(net_0072), .F2(D0X1Y4_MAIN_CLB_qx), .F3(net_0017), .F4(net_0071),
    .G2(net_0004), .G3(D0X1Y4_MAIN_CLB_qy), .G4(net_0071), .E(net_0163), .F(D0X1Y4_MAIN_CLB_f), .G(D0X1Y4_MAIN_CLB_g), .M(D0X1Y4_MAIN_CLB_m));
  assign D0X1Y4_MAIN_CLB_dx = D0X1Y4_MAIN_CLB_f;
  assign D0X1Y4_MAIN_CLB_dy = D0X1Y4_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y4_MAIN_CLB_ffx (.D(D0X1Y4_MAIN_CLB_dx), .K(net_0002), .EC(net_0072), .RD(net_0161), .Q(D0X1Y4_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y4_MAIN_CLB_ffy (.D(D0X1Y4_MAIN_CLB_dy), .K(net_0002), .EC(net_0072), .RD(net_0161), .Q(D0X1Y4_MAIN_CLB_qy));
  assign net_0063 = D0X1Y4_MAIN_CLB_qx;
  assign net_0069 = D0X1Y4_MAIN_CLB_qy;

  (* XC3000_LOC="D0X1Y4.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X1Y4_MAIN_TBUF_0_ (.I(net_0162), .T(net_0165), .O(net_0071));

  (* XC3000_LOC="D0X1Y4.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X1Y4_MAIN_TBUF_1_ (.I(net_0164), .T(net_0165), .O(net_0000));

  // D0X1Y5.MAIN.CLB; MODE=FG
  wire D0X1Y5_MAIN_CLB_f, D0X1Y5_MAIN_CLB_g, D0X1Y5_MAIN_CLB_m, D0X1Y5_MAIN_CLB_qx, D0X1Y5_MAIN_CLB_qy, D0X1Y5_MAIN_CLB_dx, D0X1Y5_MAIN_CLB_dy;
  (* XC3000_LOC="D0X1Y5.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1011000110110001)) D0X1Y5_MAIN_CLB_lut (
    .A(net_0057), .F2(D0X1Y5_MAIN_CLB_qx), .F3(net_0017), .F4(net_0084),
    .G2(net_0004), .G3(D0X1Y5_MAIN_CLB_qy), .G4(net_0084), .E(net_0167), .F(D0X1Y5_MAIN_CLB_f), .G(D0X1Y5_MAIN_CLB_g), .M(D0X1Y5_MAIN_CLB_m));
  assign D0X1Y5_MAIN_CLB_dx = D0X1Y5_MAIN_CLB_f;
  assign D0X1Y5_MAIN_CLB_dy = D0X1Y5_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y5_MAIN_CLB_ffx (.D(D0X1Y5_MAIN_CLB_dx), .K(net_0002), .EC(net_0072), .RD(net_0166), .Q(D0X1Y5_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y5_MAIN_CLB_ffy (.D(D0X1Y5_MAIN_CLB_dy), .K(net_0002), .EC(net_0072), .RD(net_0166), .Q(D0X1Y5_MAIN_CLB_qy));
  assign net_0075 = D0X1Y5_MAIN_CLB_qx;
  assign net_0076 = D0X1Y5_MAIN_CLB_qy;

  (* XC3000_LOC="D0X1Y5.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X1Y5_MAIN_TBUF_0_ (.I(net_0067), .T(net_0168), .O(net_0078));

  (* XC3000_LOC="D0X1Y5.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X1Y5_MAIN_TBUF_1_ (.I(net_0084), .T(net_0168), .O(net_0082));

  // D0X1Y6.MAIN.CLB; MODE=FG
  wire D0X1Y6_MAIN_CLB_f, D0X1Y6_MAIN_CLB_g, D0X1Y6_MAIN_CLB_m, D0X1Y6_MAIN_CLB_qx, D0X1Y6_MAIN_CLB_qy, D0X1Y6_MAIN_CLB_dx, D0X1Y6_MAIN_CLB_dy;
  (* XC3000_LOC="D0X1Y6.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1011000110110001)) D0X1Y6_MAIN_CLB_lut (
    .A(net_0089), .F2(D0X1Y6_MAIN_CLB_qx), .F3(net_0017), .F4(net_0065),
    .G2(net_0004), .G3(D0X1Y6_MAIN_CLB_qy), .G4(net_0065), .E(net_0067), .F(D0X1Y6_MAIN_CLB_f), .G(D0X1Y6_MAIN_CLB_g), .M(D0X1Y6_MAIN_CLB_m));
  assign D0X1Y6_MAIN_CLB_dx = D0X1Y6_MAIN_CLB_f;
  assign D0X1Y6_MAIN_CLB_dy = D0X1Y6_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y6_MAIN_CLB_ffx (.D(D0X1Y6_MAIN_CLB_dx), .K(net_0002), .EC(net_0072), .RD(net_0057), .Q(D0X1Y6_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y6_MAIN_CLB_ffy (.D(D0X1Y6_MAIN_CLB_dy), .K(net_0002), .EC(net_0072), .RD(net_0057), .Q(D0X1Y6_MAIN_CLB_qy));
  assign net_0088 = D0X1Y6_MAIN_CLB_qx;
  assign net_0087 = D0X1Y6_MAIN_CLB_qy;

  (* XC3000_LOC="D0X1Y6.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X1Y6_MAIN_TBUF_0_ (.I(net_0066), .T(net_0170), .O(net_0094));

  (* XC3000_LOC="D0X1Y6.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X1Y6_MAIN_TBUF_1_ (.I(net_0169), .T(net_0171), .O(net_0089));

  // D0X1Y7.MAIN.CLB; MODE=FGM
  wire D0X1Y7_MAIN_CLB_f, D0X1Y7_MAIN_CLB_g, D0X1Y7_MAIN_CLB_m, D0X1Y7_MAIN_CLB_qx, D0X1Y7_MAIN_CLB_qy, D0X1Y7_MAIN_CLB_dx, D0X1Y7_MAIN_CLB_dy;
  (* XC3000_LOC="D0X1Y7.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000000000), .INIT_G(16'b1111000011111111)) D0X1Y7_MAIN_CLB_lut (
    .A(net_0172), .F2(net_0106), .F3(net_0095), .F4(net_0105),
    .G2(net_0106), .G3(net_0095), .G4(net_0105), .E(net_0173), .F(D0X1Y7_MAIN_CLB_f), .G(D0X1Y7_MAIN_CLB_g), .M(D0X1Y7_MAIN_CLB_m));
  assign D0X1Y7_MAIN_CLB_dx = D0X1Y7_MAIN_CLB_m;
  assign D0X1Y7_MAIN_CLB_dy = D0X1Y7_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X1Y7_MAIN_CLB_ffx (.D(D0X1Y7_MAIN_CLB_dx), .K(net_0090), .EC(net_0172), .RD(net_0095), .Q(D0X1Y7_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X1Y7_MAIN_CLB_ffy (.D(D0X1Y7_MAIN_CLB_dy), .K(net_0090), .EC(net_0172), .RD(net_0095), .Q(D0X1Y7_MAIN_CLB_qy));
  assign net_0176 = D0X1Y7_MAIN_CLB_m;
  assign net_0177 = D0X1Y7_MAIN_CLB_qy;

  (* XC3000_LOC="D0X1Y7.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X1Y7_MAIN_TBUF_0_ (.I(net_0067), .T(net_0175), .O(net_0105));

  (* XC3000_LOC="D0X1Y7.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X1Y7_MAIN_TBUF_1_ (.I(net_0174), .T(net_0175), .O(net_0099));

  // D0X1Y8.MAIN.CLB; MODE=FG
  wire D0X1Y8_MAIN_CLB_f, D0X1Y8_MAIN_CLB_g, D0X1Y8_MAIN_CLB_m, D0X1Y8_MAIN_CLB_qx, D0X1Y8_MAIN_CLB_qy, D0X1Y8_MAIN_CLB_dx, D0X1Y8_MAIN_CLB_dy;
  (* XC3000_LOC="D0X1Y8.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1011000110110001)) D0X1Y8_MAIN_CLB_lut (
    .A(net_0008), .F2(D0X1Y8_MAIN_CLB_qx), .F3(net_0017), .F4(net_0106),
    .G2(net_0004), .G3(D0X1Y8_MAIN_CLB_qy), .G4(net_0106), .E(net_0173), .F(D0X1Y8_MAIN_CLB_f), .G(D0X1Y8_MAIN_CLB_g), .M(D0X1Y8_MAIN_CLB_m));
  assign D0X1Y8_MAIN_CLB_dx = D0X1Y8_MAIN_CLB_f;
  assign D0X1Y8_MAIN_CLB_dy = D0X1Y8_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y8_MAIN_CLB_ffx (.D(D0X1Y8_MAIN_CLB_dx), .K(net_0002), .EC(net_0091), .RD(net_0067), .Q(D0X1Y8_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y8_MAIN_CLB_ffy (.D(D0X1Y8_MAIN_CLB_dy), .K(net_0002), .EC(net_0091), .RD(net_0067), .Q(D0X1Y8_MAIN_CLB_qy));
  assign net_0180 = D0X1Y8_MAIN_CLB_qx;
  assign net_0181 = D0X1Y8_MAIN_CLB_qy;

  (* XC3000_LOC="D0X1Y8.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X1Y8_MAIN_TBUF_0_ (.I(net_0116), .T(net_0179), .O(net_0110));

  (* XC3000_LOC="D0X1Y8.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X1Y8_MAIN_TBUF_1_ (.I(net_0178), .T(net_0179), .O(net_0115));

  // D0X1Y9.MAIN.CLB; MODE=FG
  wire D0X1Y9_MAIN_CLB_f, D0X1Y9_MAIN_CLB_g, D0X1Y9_MAIN_CLB_m, D0X1Y9_MAIN_CLB_qx, D0X1Y9_MAIN_CLB_qy, D0X1Y9_MAIN_CLB_dx, D0X1Y9_MAIN_CLB_dy;
  (* XC3000_LOC="D0X1Y9.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110010101010)) D0X1Y9_MAIN_CLB_lut (
    .A(net_0123), .F2(net_0004), .F3(D0X1Y9_MAIN_CLB_qx), .F4(net_0008),
    .G2(D0X1Y9_MAIN_CLB_qy), .G3(net_0133), .G4(net_0008), .E(net_0116), .F(D0X1Y9_MAIN_CLB_f), .G(D0X1Y9_MAIN_CLB_g), .M(D0X1Y9_MAIN_CLB_m));
  assign D0X1Y9_MAIN_CLB_dx = D0X1Y9_MAIN_CLB_f;
  assign D0X1Y9_MAIN_CLB_dy = D0X1Y9_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y9_MAIN_CLB_ffx (.D(D0X1Y9_MAIN_CLB_dx), .K(net_0002), .EC(net_0125), .RD(net_0120), .Q(D0X1Y9_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X1Y9_MAIN_CLB_ffy (.D(D0X1Y9_MAIN_CLB_dy), .K(net_0002), .EC(net_0125), .RD(net_0120), .Q(D0X1Y9_MAIN_CLB_qy));
  assign net_0183 = D0X1Y9_MAIN_CLB_qx;
  assign net_0182 = D0X1Y9_MAIN_CLB_qy;

  (* XC3000_LOC="D0X1Y9.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X1Y9_MAIN_TBUF_0_ (.I(net_0173), .T(net_0184), .O(net_0016));

  (* XC3000_LOC="D0X1Y9.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X1Y9_MAIN_TBUF_1_ (.I(net_0133), .T(net_0184), .O(net_0123));

  // D0X1Y9.MAIN.IO_N[0]
  assign net_0173 = pin_P9;
  wire D0X1Y9_MAIN_IO_N_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X1Y9_MAIN_IO_N_0__input (.PAD_IN(pin_P9), .IK(net_0002), .Q(D0X1Y9_MAIN_IO_N_0__input_q));
  assign net_0186 = D0X1Y9_MAIN_IO_N_0__input_q;
  wire D0X1Y9_MAIN_IO_N_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X1Y9_MAIN_IO_N_0__output_ff (.D(net_0116), .OK(net_0015), .Q(D0X1Y9_MAIN_IO_N_0__output_q));
  (* XC3000_LOC="D0X1Y9.MAIN", XC3000_BEL="IO_N[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X1Y9_MAIN_IO_N_0__driver (.O(net_0116), .T(~(net_0117)), .PAD(pin_P9));

  // D0X1Y9.MAIN.IO_N[1]
  assign net_0185 = pin_P8;
  wire D0X1Y9_MAIN_IO_N_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X1Y9_MAIN_IO_N_1__input (.PAD_IN(pin_P8), .IK(net_0002), .Q(D0X1Y9_MAIN_IO_N_1__input_q));
  assign net_0187 = D0X1Y9_MAIN_IO_N_1__input_q;
  wire D0X1Y9_MAIN_IO_N_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X1Y9_MAIN_IO_N_1__output_ff (.D(net_0182), .OK(net_0015), .Q(D0X1Y9_MAIN_IO_N_1__output_q));
  (* XC3000_LOC="D0X1Y9.MAIN", XC3000_BEL="IO_N[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X1Y9_MAIN_IO_N_1__driver (.O(net_0182), .T(~(net_0183)), .PAD(pin_P8));

  // D0X2Y0.MAIN.CLB; MODE=FG
  wire D0X2Y0_MAIN_CLB_f, D0X2Y0_MAIN_CLB_g, D0X2Y0_MAIN_CLB_m, D0X2Y0_MAIN_CLB_qx, D0X2Y0_MAIN_CLB_qy, D0X2Y0_MAIN_CLB_dx, D0X2Y0_MAIN_CLB_dy;
  (* XC3000_LOC="D0X2Y0.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110010101010)) D0X2Y0_MAIN_CLB_lut (
    .A(net_0003), .F2(net_0004), .F3(D0X2Y0_MAIN_CLB_qx), .F4(net_0016),
    .G2(D0X2Y0_MAIN_CLB_qy), .G3(net_0188), .G4(net_0016), .E(net_0190), .F(D0X2Y0_MAIN_CLB_f), .G(D0X2Y0_MAIN_CLB_g), .M(D0X2Y0_MAIN_CLB_m));
  assign D0X2Y0_MAIN_CLB_dx = D0X2Y0_MAIN_CLB_f;
  assign D0X2Y0_MAIN_CLB_dy = D0X2Y0_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X2Y0_MAIN_CLB_ffx (.D(D0X2Y0_MAIN_CLB_dx), .K(net_0002), .EC(net_0141), .RD(net_0008), .Q(D0X2Y0_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X2Y0_MAIN_CLB_ffy (.D(D0X2Y0_MAIN_CLB_dy), .K(net_0002), .EC(net_0141), .RD(net_0008), .Q(D0X2Y0_MAIN_CLB_qy));
  assign net_0192 = D0X2Y0_MAIN_CLB_qx;
  assign net_0191 = D0X2Y0_MAIN_CLB_qy;

  (* XC3000_LOC="D0X2Y0.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X2Y0_MAIN_TBUF_0_ (.I(net_0189), .T(net_0193), .O(net_0016));

  (* XC3000_LOC="D0X2Y0.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X2Y0_MAIN_TBUF_1_ (.I(net_0188), .T(net_0193), .O(net_0003));

  // D0X2Y0.MAIN.IO_S[0]
  assign net_0189 = pin_P37;
  wire D0X2Y0_MAIN_IO_S_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X2Y0_MAIN_IO_S_0__input (.PAD_IN(pin_P37), .IK(net_0000), .Q(D0X2Y0_MAIN_IO_S_0__input_q));
  assign net_0195 = D0X2Y0_MAIN_IO_S_0__input_q;
  wire D0X2Y0_MAIN_IO_S_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X2Y0_MAIN_IO_S_0__output_ff (.D(net_0146), .OK(net_0002), .Q(D0X2Y0_MAIN_IO_S_0__output_q));
  (* XC3000_LOC="D0X2Y0.MAIN", XC3000_BEL="IO_S[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X2Y0_MAIN_IO_S_0__driver (.O(net_0146), .T(~(net_0143)), .PAD(pin_P37));

  // D0X2Y0.MAIN.IO_S[1]
  assign net_0194 = pin_P38;
  wire D0X2Y0_MAIN_IO_S_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X2Y0_MAIN_IO_S_1__input (.PAD_IN(pin_P38), .IK(net_0000), .Q(D0X2Y0_MAIN_IO_S_1__input_q));
  assign net_0196 = D0X2Y0_MAIN_IO_S_1__input_q;
  wire D0X2Y0_MAIN_IO_S_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X2Y0_MAIN_IO_S_1__output_ff (.D(net_0191), .OK(net_0002), .Q(D0X2Y0_MAIN_IO_S_1__output_q));
  (* XC3000_LOC="D0X2Y0.MAIN", XC3000_BEL="IO_S[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X2Y0_MAIN_IO_S_1__driver (.O(net_0191), .T(~(net_0192)), .PAD(pin_P38));

  // D0X2Y1.MAIN.CLB; MODE=FG
  wire D0X2Y1_MAIN_CLB_f, D0X2Y1_MAIN_CLB_g, D0X2Y1_MAIN_CLB_m, D0X2Y1_MAIN_CLB_qx, D0X2Y1_MAIN_CLB_qy, D0X2Y1_MAIN_CLB_dx, D0X2Y1_MAIN_CLB_dy;
  (* XC3000_LOC="D0X2Y1.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X2Y1_MAIN_CLB_lut (
    .A(net_0057), .F2(D0X2Y1_MAIN_CLB_qx), .F3(net_0017), .F4(net_0057),
    .G2(net_0004), .G3(D0X2Y1_MAIN_CLB_qy), .G4(net_0057), .E(net_0057), .F(D0X2Y1_MAIN_CLB_f), .G(D0X2Y1_MAIN_CLB_g), .M(D0X2Y1_MAIN_CLB_m));
  assign D0X2Y1_MAIN_CLB_dx = D0X2Y1_MAIN_CLB_f;
  assign D0X2Y1_MAIN_CLB_dy = D0X2Y1_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X2Y1_MAIN_CLB_ffx (.D(D0X2Y1_MAIN_CLB_dx), .K(net_0002), .EC(net_0141), .RD(net_0120), .Q(D0X2Y1_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X2Y1_MAIN_CLB_ffy (.D(D0X2Y1_MAIN_CLB_dy), .K(net_0002), .EC(net_0141), .RD(net_0120), .Q(D0X2Y1_MAIN_CLB_qy));
  assign net_0199 = D0X2Y1_MAIN_CLB_qx;
  assign net_0200 = D0X2Y1_MAIN_CLB_qy;

  (* XC3000_LOC="D0X2Y1.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X2Y1_MAIN_TBUF_0_ (.I(net_0001), .T(net_0197), .O(net_0027));

  (* XC3000_LOC="D0X2Y1.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X2Y1_MAIN_TBUF_1_ (.I(net_0017), .T(net_0198), .O(net_0034));

  // D0X2Y2.MAIN.CLB; MODE=FG
  wire D0X2Y2_MAIN_CLB_f, D0X2Y2_MAIN_CLB_g, D0X2Y2_MAIN_CLB_m, D0X2Y2_MAIN_CLB_qx, D0X2Y2_MAIN_CLB_qy, D0X2Y2_MAIN_CLB_dx, D0X2Y2_MAIN_CLB_dy;
  (* XC3000_LOC="D0X2Y2.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X2Y2_MAIN_CLB_lut (
    .A(net_0057), .F2(D0X2Y2_MAIN_CLB_qy), .F3(D0X2Y2_MAIN_CLB_qy), .F4(net_0204),
    .G2(D0X2Y2_MAIN_CLB_qy), .G3(D0X2Y2_MAIN_CLB_qy), .G4(net_0204), .E(net_0204), .F(D0X2Y2_MAIN_CLB_f), .G(D0X2Y2_MAIN_CLB_g), .M(D0X2Y2_MAIN_CLB_m));
  assign D0X2Y2_MAIN_CLB_dx = D0X2Y2_MAIN_CLB_f;
  assign D0X2Y2_MAIN_CLB_dy = D0X2Y2_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y2_MAIN_CLB_ffx (.D(D0X2Y2_MAIN_CLB_dx), .K(net_0154), .EC(net_0057), .RD(net_0001), .Q(D0X2Y2_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y2_MAIN_CLB_ffy (.D(D0X2Y2_MAIN_CLB_dy), .K(net_0154), .EC(net_0057), .RD(net_0001), .Q(D0X2Y2_MAIN_CLB_qy));
  assign net_0206 = D0X2Y2_MAIN_CLB_qx;
  assign net_0207 = D0X2Y2_MAIN_CLB_qy;

  (* XC3000_LOC="D0X2Y2.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X2Y2_MAIN_TBUF_0_ (.I(net_0203), .T(net_0205), .O(net_0042));

  (* XC3000_LOC="D0X2Y2.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X2Y2_MAIN_TBUF_1_ (.I(net_0202), .T(net_0205), .O(net_0047));

  // D0X2Y3.MAIN.CLB; MODE=FG
  wire D0X2Y3_MAIN_CLB_f, D0X2Y3_MAIN_CLB_g, D0X2Y3_MAIN_CLB_m, D0X2Y3_MAIN_CLB_qx, D0X2Y3_MAIN_CLB_qy, D0X2Y3_MAIN_CLB_dx, D0X2Y3_MAIN_CLB_dy;
  (* XC3000_LOC="D0X2Y3.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X2Y3_MAIN_CLB_lut (
    .A(net_0057), .F2(D0X2Y3_MAIN_CLB_qy), .F3(D0X2Y3_MAIN_CLB_qy), .F4(net_0009),
    .G2(D0X2Y3_MAIN_CLB_qy), .G3(D0X2Y3_MAIN_CLB_qy), .G4(net_0009), .E(net_0009), .F(D0X2Y3_MAIN_CLB_f), .G(D0X2Y3_MAIN_CLB_g), .M(D0X2Y3_MAIN_CLB_m));
  assign D0X2Y3_MAIN_CLB_dx = D0X2Y3_MAIN_CLB_f;
  assign D0X2Y3_MAIN_CLB_dy = D0X2Y3_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y3_MAIN_CLB_ffx (.D(D0X2Y3_MAIN_CLB_dx), .K(net_0044), .EC(net_0057), .RD(net_0120), .Q(D0X2Y3_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y3_MAIN_CLB_ffy (.D(D0X2Y3_MAIN_CLB_dy), .K(net_0044), .EC(net_0057), .RD(net_0120), .Q(D0X2Y3_MAIN_CLB_qy));
  assign net_0209 = D0X2Y3_MAIN_CLB_qx;
  assign net_0210 = D0X2Y3_MAIN_CLB_qy;

  (* XC3000_LOC="D0X2Y3.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X2Y3_MAIN_TBUF_0_ (.I(net_0055), .T(net_0208), .O(net_0057));

  (* XC3000_LOC="D0X2Y3.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X2Y3_MAIN_TBUF_1_ (.I(net_0053), .T(net_0208), .O(net_0060));

  // D0X2Y4.MAIN.CLB; MODE=FGM
  wire D0X2Y4_MAIN_CLB_f, D0X2Y4_MAIN_CLB_g, D0X2Y4_MAIN_CLB_m, D0X2Y4_MAIN_CLB_qx, D0X2Y4_MAIN_CLB_qy, D0X2Y4_MAIN_CLB_dx, D0X2Y4_MAIN_CLB_dy;
  (* XC3000_LOC="D0X2Y4.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1010111110100000), .INIT_G(16'b1100110011001100)) D0X2Y4_MAIN_CLB_lut (
    .A(net_0068), .F2(net_0084), .F3(net_0212), .F4(net_0054),
    .G2(net_0084), .G3(net_0212), .G4(net_0054), .E(net_0120), .F(D0X2Y4_MAIN_CLB_f), .G(D0X2Y4_MAIN_CLB_g), .M(D0X2Y4_MAIN_CLB_m));
  assign D0X2Y4_MAIN_CLB_dx = D0X2Y4_MAIN_CLB_m;
  assign D0X2Y4_MAIN_CLB_dy = D0X2Y4_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y4_MAIN_CLB_ffx (.D(D0X2Y4_MAIN_CLB_dx), .K(net_0211), .EC(net_0057), .RD(net_0007), .Q(D0X2Y4_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y4_MAIN_CLB_ffy (.D(D0X2Y4_MAIN_CLB_dy), .K(net_0211), .EC(net_0057), .RD(net_0007), .Q(D0X2Y4_MAIN_CLB_qy));
  assign net_0216 = D0X2Y4_MAIN_CLB_qx;
  assign net_0217 = D0X2Y4_MAIN_CLB_m;

  (* XC3000_LOC="D0X2Y4.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X2Y4_MAIN_TBUF_0_ (.I(net_0213), .T(net_0215), .O(net_0071));

  (* XC3000_LOC="D0X2Y4.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X2Y4_MAIN_TBUF_1_ (.I(net_0214), .T(net_0215), .O(net_0000));

  // D0X2Y5.MAIN.CLB; MODE=FG
  wire D0X2Y5_MAIN_CLB_f, D0X2Y5_MAIN_CLB_g, D0X2Y5_MAIN_CLB_m, D0X2Y5_MAIN_CLB_qx, D0X2Y5_MAIN_CLB_qy, D0X2Y5_MAIN_CLB_dx, D0X2Y5_MAIN_CLB_dy;
  (* XC3000_LOC="D0X2Y5.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X2Y5_MAIN_CLB_lut (
    .A(net_0057), .F2(D0X2Y5_MAIN_CLB_qy), .F3(D0X2Y5_MAIN_CLB_qy), .F4(net_0221),
    .G2(D0X2Y5_MAIN_CLB_qy), .G3(D0X2Y5_MAIN_CLB_qy), .G4(net_0221), .E(net_0221), .F(D0X2Y5_MAIN_CLB_f), .G(D0X2Y5_MAIN_CLB_g), .M(D0X2Y5_MAIN_CLB_m));
  assign D0X2Y5_MAIN_CLB_dx = D0X2Y5_MAIN_CLB_f;
  assign D0X2Y5_MAIN_CLB_dy = D0X2Y5_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y5_MAIN_CLB_ffx (.D(D0X2Y5_MAIN_CLB_dx), .K(net_0069), .EC(net_0057), .RD(net_0068), .Q(D0X2Y5_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y5_MAIN_CLB_ffy (.D(D0X2Y5_MAIN_CLB_dy), .K(net_0069), .EC(net_0057), .RD(net_0068), .Q(D0X2Y5_MAIN_CLB_qy));
  assign net_0223 = D0X2Y5_MAIN_CLB_qx;
  assign net_0224 = D0X2Y5_MAIN_CLB_qy;

  (* XC3000_LOC="D0X2Y5.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X2Y5_MAIN_TBUF_0_ (.I(net_0220), .T(net_0222), .O(net_0078));

  (* XC3000_LOC="D0X2Y5.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X2Y5_MAIN_TBUF_1_ (.I(net_0219), .T(net_0222), .O(net_0082));

  // D0X2Y6.MAIN.CLB; MODE=FG
  wire D0X2Y6_MAIN_CLB_f, D0X2Y6_MAIN_CLB_g, D0X2Y6_MAIN_CLB_m, D0X2Y6_MAIN_CLB_qx, D0X2Y6_MAIN_CLB_qy, D0X2Y6_MAIN_CLB_dx, D0X2Y6_MAIN_CLB_dy;
  (* XC3000_LOC="D0X2Y6.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000001), .INIT_G(16'b0000000000000100)) D0X2Y6_MAIN_CLB_lut (
    .A(net_0067), .F2(net_0007), .F3(net_0065), .F4(net_0094),
    .G2(net_0007), .G3(net_0065), .G4(net_0094), .E(net_0226), .F(D0X2Y6_MAIN_CLB_f), .G(D0X2Y6_MAIN_CLB_g), .M(D0X2Y6_MAIN_CLB_m));
  assign D0X2Y6_MAIN_CLB_dx = D0X2Y6_MAIN_CLB_f;
  assign D0X2Y6_MAIN_CLB_dy = D0X2Y6_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y6_MAIN_CLB_ffx (.D(D0X2Y6_MAIN_CLB_dx), .K(net_0076), .EC(net_0067), .RD(net_0067), .Q(D0X2Y6_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y6_MAIN_CLB_ffy (.D(D0X2Y6_MAIN_CLB_dy), .K(net_0076), .EC(net_0067), .RD(net_0067), .Q(D0X2Y6_MAIN_CLB_qy));
  assign net_0125 = D0X2Y6_MAIN_CLB_f;
  assign net_0098 = D0X2Y6_MAIN_CLB_g;

  (* XC3000_LOC="D0X2Y6.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X2Y6_MAIN_TBUF_0_ (.I(net_0225), .T(net_0227), .O(net_0094));

  (* XC3000_LOC="D0X2Y6.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X2Y6_MAIN_TBUF_1_ (.I(net_0087), .T(net_0227), .O(net_0089));

  // D0X2Y7.MAIN.CLB; MODE=FGM
  wire D0X2Y7_MAIN_CLB_f, D0X2Y7_MAIN_CLB_g, D0X2Y7_MAIN_CLB_m, D0X2Y7_MAIN_CLB_qx, D0X2Y7_MAIN_CLB_qy, D0X2Y7_MAIN_CLB_dx, D0X2Y7_MAIN_CLB_dy;
  (* XC3000_LOC="D0X2Y7.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000010101010), .INIT_G(16'b1100110011001100)) D0X2Y7_MAIN_CLB_lut (
    .A(net_0106), .F2(net_0176), .F3(net_0229), .F4(net_0105),
    .G2(net_0176), .G3(net_0229), .G4(net_0105), .E(net_0120), .F(D0X2Y7_MAIN_CLB_f), .G(D0X2Y7_MAIN_CLB_g), .M(D0X2Y7_MAIN_CLB_m));
  assign D0X2Y7_MAIN_CLB_dx = D0X2Y7_MAIN_CLB_m;
  assign D0X2Y7_MAIN_CLB_dy = D0X2Y7_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y7_MAIN_CLB_ffx (.D(D0X2Y7_MAIN_CLB_dx), .K(net_0228), .EC(net_0067), .RD(net_0067), .Q(D0X2Y7_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y7_MAIN_CLB_ffy (.D(D0X2Y7_MAIN_CLB_dy), .K(net_0228), .EC(net_0067), .RD(net_0067), .Q(D0X2Y7_MAIN_CLB_qy));
  assign net_0232 = D0X2Y7_MAIN_CLB_qx;
  assign net_0233 = D0X2Y7_MAIN_CLB_m;

  (* XC3000_LOC="D0X2Y7.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X2Y7_MAIN_TBUF_0_ (.I(net_0230), .T(net_0231), .O(net_0105));

  (* XC3000_LOC="D0X2Y7.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X2Y7_MAIN_TBUF_1_ (.I(net_0229), .T(net_0231), .O(net_0099));

  // D0X2Y8.MAIN.CLB; MODE=FG
  wire D0X2Y8_MAIN_CLB_f, D0X2Y8_MAIN_CLB_g, D0X2Y8_MAIN_CLB_m, D0X2Y8_MAIN_CLB_qx, D0X2Y8_MAIN_CLB_qy, D0X2Y8_MAIN_CLB_dx, D0X2Y8_MAIN_CLB_dy;
  (* XC3000_LOC="D0X2Y8.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000101000001100), .INIT_G(16'b1111111111110000)) D0X2Y8_MAIN_CLB_lut (
    .A(net_0185), .F2(net_0133), .F3(net_0120), .F4(net_0212),
    .G2(D0X2Y8_MAIN_CLB_qx), .G3(net_0120), .G4(net_0212), .E(net_0091), .F(D0X2Y8_MAIN_CLB_f), .G(D0X2Y8_MAIN_CLB_g), .M(D0X2Y8_MAIN_CLB_m));
  assign D0X2Y8_MAIN_CLB_dx = D0X2Y8_MAIN_CLB_f;
  assign D0X2Y8_MAIN_CLB_dy = D0X2Y8_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y8_MAIN_CLB_ffx (.D(D0X2Y8_MAIN_CLB_dx), .K(net_0212), .EC(net_0229), .RD(net_0234), .Q(D0X2Y8_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X2Y8_MAIN_CLB_ffy (.D(D0X2Y8_MAIN_CLB_dy), .K(net_0212), .EC(net_0229), .RD(net_0234), .Q(D0X2Y8_MAIN_CLB_qy));
  assign net_0236 = D0X2Y8_MAIN_CLB_f;
  assign net_0001 = D0X2Y8_MAIN_CLB_g;

  (* XC3000_LOC="D0X2Y8.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X2Y8_MAIN_TBUF_0_ (.I(net_0180), .T(net_0235), .O(net_0110));

  (* XC3000_LOC="D0X2Y8.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X2Y8_MAIN_TBUF_1_ (.I(net_0120), .T(net_0235), .O(net_0115));

  // D0X2Y9.MAIN.CLB; MODE=FG
  wire D0X2Y9_MAIN_CLB_f, D0X2Y9_MAIN_CLB_g, D0X2Y9_MAIN_CLB_m, D0X2Y9_MAIN_CLB_qx, D0X2Y9_MAIN_CLB_qy, D0X2Y9_MAIN_CLB_dx, D0X2Y9_MAIN_CLB_dy;
  (* XC3000_LOC="D0X2Y9.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110010101010)) D0X2Y9_MAIN_CLB_lut (
    .A(net_0123), .F2(net_0004), .F3(D0X2Y9_MAIN_CLB_qx), .F4(net_0016),
    .G2(D0X2Y9_MAIN_CLB_qy), .G3(net_0185), .G4(net_0016), .E(net_0180), .F(D0X2Y9_MAIN_CLB_f), .G(D0X2Y9_MAIN_CLB_g), .M(D0X2Y9_MAIN_CLB_m));
  assign D0X2Y9_MAIN_CLB_dx = D0X2Y9_MAIN_CLB_f;
  assign D0X2Y9_MAIN_CLB_dy = D0X2Y9_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X2Y9_MAIN_CLB_ffx (.D(D0X2Y9_MAIN_CLB_dx), .K(net_0002), .EC(net_0125), .RD(net_0002), .Q(D0X2Y9_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X2Y9_MAIN_CLB_ffy (.D(D0X2Y9_MAIN_CLB_dy), .K(net_0002), .EC(net_0125), .RD(net_0002), .Q(D0X2Y9_MAIN_CLB_qy));
  assign net_0238 = D0X2Y9_MAIN_CLB_qx;
  assign net_0237 = D0X2Y9_MAIN_CLB_qy;

  (* XC3000_LOC="D0X2Y9.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X2Y9_MAIN_TBUF_0_ (.I(net_0229), .T(net_0239), .O(net_0016));

  (* XC3000_LOC="D0X2Y9.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X2Y9_MAIN_TBUF_1_ (.I(net_0185), .T(net_0239), .O(net_0123));

  // D0X2Y9.MAIN.IO_N[0]
  assign net_0229 = pin_P7;
  wire D0X2Y9_MAIN_IO_N_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X2Y9_MAIN_IO_N_0__input (.PAD_IN(pin_P7), .IK(net_0002), .Q(D0X2Y9_MAIN_IO_N_0__input_q));
  assign net_0241 = D0X2Y9_MAIN_IO_N_0__input_q;
  wire D0X2Y9_MAIN_IO_N_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X2Y9_MAIN_IO_N_0__output_ff (.D(net_0180), .OK(net_0015), .Q(D0X2Y9_MAIN_IO_N_0__output_q));
  (* XC3000_LOC="D0X2Y9.MAIN", XC3000_BEL="IO_N[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X2Y9_MAIN_IO_N_0__driver (.O(net_0180), .T(~(net_0181)), .PAD(pin_P7));

  // D0X2Y9.MAIN.IO_N[1]
  assign net_0240 = pin_P6;
  wire D0X2Y9_MAIN_IO_N_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X2Y9_MAIN_IO_N_1__input (.PAD_IN(pin_P6), .IK(net_0002), .Q(D0X2Y9_MAIN_IO_N_1__input_q));
  assign net_0242 = D0X2Y9_MAIN_IO_N_1__input_q;
  wire D0X2Y9_MAIN_IO_N_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X2Y9_MAIN_IO_N_1__output_ff (.D(net_0237), .OK(net_0015), .Q(D0X2Y9_MAIN_IO_N_1__output_q));
  (* XC3000_LOC="D0X2Y9.MAIN", XC3000_BEL="IO_N[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X2Y9_MAIN_IO_N_1__driver (.O(net_0237), .T(~(net_0238)), .PAD(pin_P6));

  // D0X3Y0.MAIN.CLB; MODE=FG
  wire D0X3Y0_MAIN_CLB_f, D0X3Y0_MAIN_CLB_g, D0X3Y0_MAIN_CLB_m, D0X3Y0_MAIN_CLB_qx, D0X3Y0_MAIN_CLB_qy, D0X3Y0_MAIN_CLB_dx, D0X3Y0_MAIN_CLB_dy;
  (* XC3000_LOC="D0X3Y0.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110010101010)) D0X3Y0_MAIN_CLB_lut (
    .A(net_0003), .F2(net_0004), .F3(D0X3Y0_MAIN_CLB_qx), .F4(net_0016),
    .G2(D0X3Y0_MAIN_CLB_qy), .G3(net_0194), .G4(net_0016), .E(net_0199), .F(D0X3Y0_MAIN_CLB_f), .G(D0X3Y0_MAIN_CLB_g), .M(D0X3Y0_MAIN_CLB_m));
  assign D0X3Y0_MAIN_CLB_dx = D0X3Y0_MAIN_CLB_f;
  assign D0X3Y0_MAIN_CLB_dy = D0X3Y0_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X3Y0_MAIN_CLB_ffx (.D(D0X3Y0_MAIN_CLB_dx), .K(net_0002), .EC(net_0244), .RD(net_0245), .Q(D0X3Y0_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X3Y0_MAIN_CLB_ffy (.D(D0X3Y0_MAIN_CLB_dy), .K(net_0002), .EC(net_0244), .RD(net_0245), .Q(D0X3Y0_MAIN_CLB_qy));
  assign net_0245 = D0X3Y0_MAIN_CLB_qx;
  assign net_0246 = D0X3Y0_MAIN_CLB_qy;

  (* XC3000_LOC="D0X3Y0.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X3Y0_MAIN_TBUF_0_ (.I(net_0243), .T(net_0247), .O(net_0016));

  (* XC3000_LOC="D0X3Y0.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X3Y0_MAIN_TBUF_1_ (.I(net_0194), .T(net_0247), .O(net_0003));

  // D0X3Y0.MAIN.IO_S[0]
  assign net_0248 = pin_P39;
  wire D0X3Y0_MAIN_IO_S_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X3Y0_MAIN_IO_S_0__input (.PAD_IN(pin_P39), .IK(net_0000), .Q(D0X3Y0_MAIN_IO_S_0__input_q));
  assign net_0250 = D0X3Y0_MAIN_IO_S_0__input_q;
  wire D0X3Y0_MAIN_IO_S_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X3Y0_MAIN_IO_S_0__output_ff (.D(net_0199), .OK(net_0002), .Q(D0X3Y0_MAIN_IO_S_0__output_q));
  (* XC3000_LOC="D0X3Y0.MAIN", XC3000_BEL="IO_S[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X3Y0_MAIN_IO_S_0__driver (.O(net_0199), .T(~(net_0200)), .PAD(pin_P39));

  // D0X3Y0.MAIN.IO_S[1]
  assign net_0249 = pin_P40;
  wire D0X3Y0_MAIN_IO_S_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X3Y0_MAIN_IO_S_1__input (.PAD_IN(pin_P40), .IK(net_0000), .Q(D0X3Y0_MAIN_IO_S_1__input_q));
  assign net_0251 = D0X3Y0_MAIN_IO_S_1__input_q;
  wire D0X3Y0_MAIN_IO_S_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X3Y0_MAIN_IO_S_1__output_ff (.D(net_0246), .OK(net_0002), .Q(D0X3Y0_MAIN_IO_S_1__output_q));
  (* XC3000_LOC="D0X3Y0.MAIN", XC3000_BEL="IO_S[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X3Y0_MAIN_IO_S_1__driver (.O(net_0246), .T(~(net_0245)), .PAD(pin_P40));

  // D0X3Y1.MAIN.CLB; MODE=FG
  wire D0X3Y1_MAIN_CLB_f, D0X3Y1_MAIN_CLB_g, D0X3Y1_MAIN_CLB_m, D0X3Y1_MAIN_CLB_qx, D0X3Y1_MAIN_CLB_qy, D0X3Y1_MAIN_CLB_dx, D0X3Y1_MAIN_CLB_dy;
  (* XC3000_LOC="D0X3Y1.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X3Y1_MAIN_CLB_lut (
    .A(net_0200), .F2(D0X3Y1_MAIN_CLB_qx), .F3(net_0017), .F4(net_0027),
    .G2(net_0004), .G3(D0X3Y1_MAIN_CLB_qy), .G4(net_0027), .E(net_0141), .F(D0X3Y1_MAIN_CLB_f), .G(D0X3Y1_MAIN_CLB_g), .M(D0X3Y1_MAIN_CLB_m));
  assign D0X3Y1_MAIN_CLB_dx = D0X3Y1_MAIN_CLB_f;
  assign D0X3Y1_MAIN_CLB_dy = D0X3Y1_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X3Y1_MAIN_CLB_ffx (.D(D0X3Y1_MAIN_CLB_dx), .K(net_0002), .EC(net_0141), .RD(net_0017), .Q(D0X3Y1_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X3Y1_MAIN_CLB_ffy (.D(D0X3Y1_MAIN_CLB_dy), .K(net_0002), .EC(net_0141), .RD(net_0017), .Q(D0X3Y1_MAIN_CLB_qy));
  assign net_0253 = D0X3Y1_MAIN_CLB_qx;
  assign net_0254 = D0X3Y1_MAIN_CLB_qy;

  (* XC3000_LOC="D0X3Y1.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X3Y1_MAIN_TBUF_0_ (.I(net_0199), .T(net_0252), .O(net_0027));

  (* XC3000_LOC="D0X3Y1.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X3Y1_MAIN_TBUF_1_ (.I(net_0248), .T(net_0252), .O(net_0034));

  // D0X3Y2.MAIN.CLB; MODE=FGM
  wire D0X3Y2_MAIN_CLB_f, D0X3Y2_MAIN_CLB_g, D0X3Y2_MAIN_CLB_m, D0X3Y2_MAIN_CLB_qx, D0X3Y2_MAIN_CLB_qy, D0X3Y2_MAIN_CLB_dx, D0X3Y2_MAIN_CLB_dy;
  (* XC3000_LOC="D0X3Y2.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000010101010), .INIT_G(16'b1111000010101010)) D0X3Y2_MAIN_CLB_lut (
    .A(net_0248), .F2(net_0120), .F3(net_0194), .F4(net_0042),
    .G2(net_0120), .G3(net_0194), .G4(net_0042), .E(net_0257), .F(D0X3Y2_MAIN_CLB_f), .G(D0X3Y2_MAIN_CLB_g), .M(D0X3Y2_MAIN_CLB_m));
  assign D0X3Y2_MAIN_CLB_dx = D0X3Y2_MAIN_CLB_m;
  assign D0X3Y2_MAIN_CLB_dy = D0X3Y2_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y2_MAIN_CLB_ffx (.D(D0X3Y2_MAIN_CLB_dx), .K(net_0255), .EC(net_0001), .RD(net_0154), .Q(D0X3Y2_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y2_MAIN_CLB_ffy (.D(D0X3Y2_MAIN_CLB_dy), .K(net_0255), .EC(net_0001), .RD(net_0154), .Q(D0X3Y2_MAIN_CLB_qy));
  assign net_0259 = D0X3Y2_MAIN_CLB_qx;
  assign net_0260 = D0X3Y2_MAIN_CLB_m;

  (* XC3000_LOC="D0X3Y2.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X3Y2_MAIN_TBUF_0_ (.I(net_0256), .T(net_0258), .O(net_0042));

  (* XC3000_LOC="D0X3Y2.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X3Y2_MAIN_TBUF_1_ (.I(net_0194), .T(net_0258), .O(net_0047));

  // D0X3Y3.MAIN.CLB; MODE=FG
  wire D0X3Y3_MAIN_CLB_f, D0X3Y3_MAIN_CLB_g, D0X3Y3_MAIN_CLB_m, D0X3Y3_MAIN_CLB_qx, D0X3Y3_MAIN_CLB_qy, D0X3Y3_MAIN_CLB_dx, D0X3Y3_MAIN_CLB_dy;
  (* XC3000_LOC="D0X3Y3.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X3Y3_MAIN_CLB_lut (
    .A(net_0263), .F2(D0X3Y3_MAIN_CLB_qy), .F3(D0X3Y3_MAIN_CLB_qy), .F4(net_0266),
    .G2(D0X3Y3_MAIN_CLB_qy), .G3(D0X3Y3_MAIN_CLB_qy), .G4(net_0266), .E(net_0266), .F(D0X3Y3_MAIN_CLB_f), .G(D0X3Y3_MAIN_CLB_g), .M(D0X3Y3_MAIN_CLB_m));
  assign D0X3Y3_MAIN_CLB_dx = D0X3Y3_MAIN_CLB_f;
  assign D0X3Y3_MAIN_CLB_dy = D0X3Y3_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y3_MAIN_CLB_ffx (.D(D0X3Y3_MAIN_CLB_dx), .K(net_0262), .EC(net_0263), .RD(net_0261), .Q(D0X3Y3_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y3_MAIN_CLB_ffy (.D(D0X3Y3_MAIN_CLB_dy), .K(net_0262), .EC(net_0263), .RD(net_0261), .Q(D0X3Y3_MAIN_CLB_qy));
  assign net_0269 = D0X3Y3_MAIN_CLB_qx;
  assign net_0270 = D0X3Y3_MAIN_CLB_qy;

  (* XC3000_LOC="D0X3Y3.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X3Y3_MAIN_TBUF_0_ (.I(net_0265), .T(net_0267), .O(net_0057));

  (* XC3000_LOC="D0X3Y3.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X3Y3_MAIN_TBUF_1_ (.I(net_0001), .T(net_0268), .O(net_0060));

  // D0X3Y4.MAIN.CLB; MODE=FG
  wire D0X3Y4_MAIN_CLB_f, D0X3Y4_MAIN_CLB_g, D0X3Y4_MAIN_CLB_m, D0X3Y4_MAIN_CLB_qx, D0X3Y4_MAIN_CLB_qy, D0X3Y4_MAIN_CLB_dx, D0X3Y4_MAIN_CLB_dy;
  (* XC3000_LOC="D0X3Y4.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X3Y4_MAIN_CLB_lut (
    .A(net_0007), .F2(D0X3Y4_MAIN_CLB_qy), .F3(D0X3Y4_MAIN_CLB_qy), .F4(net_0212),
    .G2(D0X3Y4_MAIN_CLB_qy), .G3(D0X3Y4_MAIN_CLB_qy), .G4(net_0212), .E(net_0212), .F(D0X3Y4_MAIN_CLB_f), .G(D0X3Y4_MAIN_CLB_g), .M(D0X3Y4_MAIN_CLB_m));
  assign D0X3Y4_MAIN_CLB_dx = D0X3Y4_MAIN_CLB_f;
  assign D0X3Y4_MAIN_CLB_dy = D0X3Y4_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y4_MAIN_CLB_ffx (.D(D0X3Y4_MAIN_CLB_dx), .K(net_0271), .EC(net_0007), .RD(net_0007), .Q(D0X3Y4_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y4_MAIN_CLB_ffy (.D(D0X3Y4_MAIN_CLB_dy), .K(net_0271), .EC(net_0007), .RD(net_0007), .Q(D0X3Y4_MAIN_CLB_qy));
  assign net_0275 = D0X3Y4_MAIN_CLB_qx;
  assign net_0276 = D0X3Y4_MAIN_CLB_qy;

  (* XC3000_LOC="D0X3Y4.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X3Y4_MAIN_TBUF_0_ (.I(net_0273), .T(net_0274), .O(net_0071));

  (* XC3000_LOC="D0X3Y4.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X3Y4_MAIN_TBUF_1_ (.I(net_0001), .T(net_0274), .O(net_0000));

  // D0X3Y5.MAIN.CLB; MODE=FGM
  wire D0X3Y5_MAIN_CLB_f, D0X3Y5_MAIN_CLB_g, D0X3Y5_MAIN_CLB_m, D0X3Y5_MAIN_CLB_qx, D0X3Y5_MAIN_CLB_qy, D0X3Y5_MAIN_CLB_dx, D0X3Y5_MAIN_CLB_dy;
  (* XC3000_LOC="D0X3Y5.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000011111111), .INIT_G(16'b0000000011111111)) D0X3Y5_MAIN_CLB_lut (
    .A(net_0067), .F2(net_0007), .F3(net_0007), .F4(net_0002),
    .G2(net_0007), .G3(net_0007), .G4(net_0002), .E(net_0002), .F(D0X3Y5_MAIN_CLB_f), .G(D0X3Y5_MAIN_CLB_g), .M(D0X3Y5_MAIN_CLB_m));
  assign D0X3Y5_MAIN_CLB_dx = D0X3Y5_MAIN_CLB_m;
  assign D0X3Y5_MAIN_CLB_dy = D0X3Y5_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y5_MAIN_CLB_ffx (.D(D0X3Y5_MAIN_CLB_dx), .K(net_0217), .EC(net_0067), .RD(net_0067), .Q(D0X3Y5_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y5_MAIN_CLB_ffy (.D(D0X3Y5_MAIN_CLB_dy), .K(net_0217), .EC(net_0067), .RD(net_0067), .Q(D0X3Y5_MAIN_CLB_qy));
  assign net_0279 = D0X3Y5_MAIN_CLB_qx;
  assign net_0280 = D0X3Y5_MAIN_CLB_m;

  (* XC3000_LOC="D0X3Y5.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X3Y5_MAIN_TBUF_0_ (.I(net_0212), .T(net_0277), .O(net_0078));

  (* XC3000_LOC="D0X3Y5.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X3Y5_MAIN_TBUF_1_ (.I(net_0007), .T(net_0278), .O(net_0082));

  // D0X3Y6.MAIN.CLB; MODE=FG
  wire D0X3Y6_MAIN_CLB_f, D0X3Y6_MAIN_CLB_g, D0X3Y6_MAIN_CLB_m, D0X3Y6_MAIN_CLB_qx, D0X3Y6_MAIN_CLB_qy, D0X3Y6_MAIN_CLB_dx, D0X3Y6_MAIN_CLB_dy;
  (* XC3000_LOC="D0X3Y6.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X3Y6_MAIN_CLB_lut (
    .A(net_0283), .F2(D0X3Y6_MAIN_CLB_qy), .F3(D0X3Y6_MAIN_CLB_qy), .F4(net_0212),
    .G2(D0X3Y6_MAIN_CLB_qy), .G3(D0X3Y6_MAIN_CLB_qy), .G4(net_0212), .E(net_0212), .F(D0X3Y6_MAIN_CLB_f), .G(D0X3Y6_MAIN_CLB_g), .M(D0X3Y6_MAIN_CLB_m));
  assign D0X3Y6_MAIN_CLB_dx = D0X3Y6_MAIN_CLB_f;
  assign D0X3Y6_MAIN_CLB_dy = D0X3Y6_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y6_MAIN_CLB_ffx (.D(D0X3Y6_MAIN_CLB_dx), .K(net_0282), .EC(net_0283), .RD(net_0281), .Q(D0X3Y6_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y6_MAIN_CLB_ffy (.D(D0X3Y6_MAIN_CLB_dy), .K(net_0282), .EC(net_0283), .RD(net_0281), .Q(D0X3Y6_MAIN_CLB_qy));
  assign net_0286 = D0X3Y6_MAIN_CLB_qx;
  assign net_0287 = D0X3Y6_MAIN_CLB_qy;

  (* XC3000_LOC="D0X3Y6.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X3Y6_MAIN_TBUF_0_ (.I(net_0125), .T(net_0285), .O(net_0094));

  (* XC3000_LOC="D0X3Y6.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X3Y6_MAIN_TBUF_1_ (.I(net_0098), .T(net_0285), .O(net_0089));

  // D0X3Y7.MAIN.CLB; MODE=FG
  wire D0X3Y7_MAIN_CLB_f, D0X3Y7_MAIN_CLB_g, D0X3Y7_MAIN_CLB_m, D0X3Y7_MAIN_CLB_qx, D0X3Y7_MAIN_CLB_qy, D0X3Y7_MAIN_CLB_dx, D0X3Y7_MAIN_CLB_dy;
  (* XC3000_LOC="D0X3Y7.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1000100011000000), .INIT_G(16'b1100110011111111)) D0X3Y7_MAIN_CLB_lut (
    .A(net_0240), .F2(net_0120), .F3(net_0288), .F4(net_0105),
    .G2(net_0120), .G3(D0X3Y7_MAIN_CLB_qx), .G4(net_0105), .E(net_0288), .F(D0X3Y7_MAIN_CLB_f), .G(D0X3Y7_MAIN_CLB_g), .M(D0X3Y7_MAIN_CLB_m));
  assign D0X3Y7_MAIN_CLB_dx = D0X3Y7_MAIN_CLB_f;
  assign D0X3Y7_MAIN_CLB_dy = D0X3Y7_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y7_MAIN_CLB_ffx (.D(D0X3Y7_MAIN_CLB_dx), .K(net_0212), .EC(net_0125), .RD(net_0067), .Q(D0X3Y7_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X3Y7_MAIN_CLB_ffy (.D(D0X3Y7_MAIN_CLB_dy), .K(net_0212), .EC(net_0125), .RD(net_0067), .Q(D0X3Y7_MAIN_CLB_qy));
  assign net_0290 = D0X3Y7_MAIN_CLB_f;
  assign net_0008 = D0X3Y7_MAIN_CLB_g;

  (* XC3000_LOC="D0X3Y7.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X3Y7_MAIN_TBUF_0_ (.I(net_0212), .T(net_0289), .O(net_0105));

  (* XC3000_LOC="D0X3Y7.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X3Y7_MAIN_TBUF_1_ (.I(net_0098), .T(net_0289), .O(net_0099));

  // D0X3Y8.MAIN.CLB; MODE=FG
  wire D0X3Y8_MAIN_CLB_f, D0X3Y8_MAIN_CLB_g, D0X3Y8_MAIN_CLB_m, D0X3Y8_MAIN_CLB_qx, D0X3Y8_MAIN_CLB_qy, D0X3Y8_MAIN_CLB_dx, D0X3Y8_MAIN_CLB_dy;
  (* XC3000_LOC="D0X3Y8.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110010101010), .INIT_G(16'b1111000000110011)) D0X3Y8_MAIN_CLB_lut (
    .A(net_0115), .F2(D0X3Y8_MAIN_CLB_qx), .F3(net_0098), .F4(net_0110),
    .G2(net_0004), .G3(D0X3Y8_MAIN_CLB_qy), .G4(net_0110), .E(net_0288), .F(D0X3Y8_MAIN_CLB_f), .G(D0X3Y8_MAIN_CLB_g), .M(D0X3Y8_MAIN_CLB_m));
  assign D0X3Y8_MAIN_CLB_dx = D0X3Y8_MAIN_CLB_f;
  assign D0X3Y8_MAIN_CLB_dy = D0X3Y8_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X3Y8_MAIN_CLB_ffx (.D(D0X3Y8_MAIN_CLB_dx), .K(net_0002), .EC(net_0125), .RD(net_0240), .Q(D0X3Y8_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X3Y8_MAIN_CLB_ffy (.D(D0X3Y8_MAIN_CLB_dy), .K(net_0002), .EC(net_0125), .RD(net_0240), .Q(D0X3Y8_MAIN_CLB_qy));
  assign net_0130 = D0X3Y8_MAIN_CLB_qx;
  assign net_0292 = D0X3Y8_MAIN_CLB_qy;

  (* XC3000_LOC="D0X3Y8.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X3Y8_MAIN_TBUF_0_ (.I(net_0240), .T(net_0291), .O(net_0110));

  (* XC3000_LOC="D0X3Y8.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X3Y8_MAIN_TBUF_1_ (.I(net_0098), .T(net_0291), .O(net_0115));

  // D0X3Y9.MAIN.CLB; MODE=FG
  wire D0X3Y9_MAIN_CLB_f, D0X3Y9_MAIN_CLB_g, D0X3Y9_MAIN_CLB_m, D0X3Y9_MAIN_CLB_qx, D0X3Y9_MAIN_CLB_qy, D0X3Y9_MAIN_CLB_dx, D0X3Y9_MAIN_CLB_dy;
  (* XC3000_LOC="D0X3Y9.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110010101010)) D0X3Y9_MAIN_CLB_lut (
    .A(net_0123), .F2(net_0004), .F3(D0X3Y9_MAIN_CLB_qx), .F4(net_0016),
    .G2(D0X3Y9_MAIN_CLB_qy), .G3(net_0125), .G4(net_0016), .E(net_0240), .F(D0X3Y9_MAIN_CLB_f), .G(D0X3Y9_MAIN_CLB_g), .M(D0X3Y9_MAIN_CLB_m));
  assign D0X3Y9_MAIN_CLB_dx = D0X3Y9_MAIN_CLB_f;
  assign D0X3Y9_MAIN_CLB_dy = D0X3Y9_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X3Y9_MAIN_CLB_ffx (.D(D0X3Y9_MAIN_CLB_dx), .K(net_0002), .EC(net_0098), .RD(net_0002), .Q(D0X3Y9_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X3Y9_MAIN_CLB_ffy (.D(D0X3Y9_MAIN_CLB_dy), .K(net_0002), .EC(net_0098), .RD(net_0002), .Q(D0X3Y9_MAIN_CLB_qy));
  assign net_0294 = D0X3Y9_MAIN_CLB_qx;
  assign net_0293 = D0X3Y9_MAIN_CLB_qy;

  (* XC3000_LOC="D0X3Y9.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X3Y9_MAIN_TBUF_0_ (.I(net_0288), .T(net_0295), .O(net_0016));

  (* XC3000_LOC="D0X3Y9.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X3Y9_MAIN_TBUF_1_ (.I(net_0125), .T(net_0295), .O(net_0123));

  // D0X3Y9.MAIN.IO_N[0]
  assign net_0288 = pin_P5;
  wire D0X3Y9_MAIN_IO_N_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X3Y9_MAIN_IO_N_0__input (.PAD_IN(pin_P5), .IK(net_0002), .Q(D0X3Y9_MAIN_IO_N_0__input_q));
  assign net_0297 = D0X3Y9_MAIN_IO_N_0__input_q;
  wire D0X3Y9_MAIN_IO_N_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X3Y9_MAIN_IO_N_0__output_ff (.D(net_0130), .OK(net_0015), .Q(D0X3Y9_MAIN_IO_N_0__output_q));
  (* XC3000_LOC="D0X3Y9.MAIN", XC3000_BEL="IO_N[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X3Y9_MAIN_IO_N_0__driver (.O(net_0130), .T(~(net_0292)), .PAD(pin_P5));

  // D0X3Y9.MAIN.IO_N[1]
  assign net_0296 = pin_P4;
  wire D0X3Y9_MAIN_IO_N_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X3Y9_MAIN_IO_N_1__input (.PAD_IN(pin_P4), .IK(net_0002), .Q(D0X3Y9_MAIN_IO_N_1__input_q));
  assign net_0298 = D0X3Y9_MAIN_IO_N_1__input_q;
  wire D0X3Y9_MAIN_IO_N_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X3Y9_MAIN_IO_N_1__output_ff (.D(net_0293), .OK(net_0015), .Q(D0X3Y9_MAIN_IO_N_1__output_q));
  (* XC3000_LOC="D0X3Y9.MAIN", XC3000_BEL="IO_N[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X3Y9_MAIN_IO_N_1__driver (.O(net_0293), .T(~(net_0294)), .PAD(pin_P4));

  // D0X4Y0.MAIN.CLB; MODE=FG
  wire D0X4Y0_MAIN_CLB_f, D0X4Y0_MAIN_CLB_g, D0X4Y0_MAIN_CLB_m, D0X4Y0_MAIN_CLB_qx, D0X4Y0_MAIN_CLB_qy, D0X4Y0_MAIN_CLB_dx, D0X4Y0_MAIN_CLB_dy;
  (* XC3000_LOC="D0X4Y0.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110010101010), .INIT_G(16'b1111000000110011)) D0X4Y0_MAIN_CLB_lut (
    .A(net_0003), .F2(D0X4Y0_MAIN_CLB_qx), .F3(net_0249), .F4(net_0057),
    .G2(net_0004), .G3(D0X4Y0_MAIN_CLB_qy), .G4(net_0057), .E(net_0057), .F(D0X4Y0_MAIN_CLB_f), .G(D0X4Y0_MAIN_CLB_g), .M(D0X4Y0_MAIN_CLB_m));
  assign D0X4Y0_MAIN_CLB_dx = D0X4Y0_MAIN_CLB_f;
  assign D0X4Y0_MAIN_CLB_dy = D0X4Y0_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X4Y0_MAIN_CLB_ffx (.D(D0X4Y0_MAIN_CLB_dx), .K(net_0002), .EC(net_0244), .RD(net_0008), .Q(D0X4Y0_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X4Y0_MAIN_CLB_ffy (.D(D0X4Y0_MAIN_CLB_dy), .K(net_0002), .EC(net_0244), .RD(net_0008), .Q(D0X4Y0_MAIN_CLB_qy));
  assign net_0303 = D0X4Y0_MAIN_CLB_qx;
  assign net_0304 = D0X4Y0_MAIN_CLB_qy;

  (* XC3000_LOC="D0X4Y0.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X4Y0_MAIN_TBUF_0_ (.I(net_0245), .T(net_0302), .O(net_0016));

  (* XC3000_LOC="D0X4Y0.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X4Y0_MAIN_TBUF_1_ (.I(net_0249), .T(net_0302), .O(net_0003));

  // D0X4Y0.MAIN.IO_S[0]
  assign net_0305 = pin_P41;
  wire D0X4Y0_MAIN_IO_S_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X4Y0_MAIN_IO_S_0__input (.PAD_IN(pin_P41), .IK(net_0000), .Q(D0X4Y0_MAIN_IO_S_0__input_q));
  assign net_0307 = D0X4Y0_MAIN_IO_S_0__input_q;
  wire D0X4Y0_MAIN_IO_S_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X4Y0_MAIN_IO_S_0__output_ff (.D(net_0253), .OK(net_0002), .Q(D0X4Y0_MAIN_IO_S_0__output_q));
  (* XC3000_LOC="D0X4Y0.MAIN", XC3000_BEL="IO_S[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X4Y0_MAIN_IO_S_0__driver (.O(net_0253), .T(~(net_0254)), .PAD(pin_P41));

  // D0X4Y0.MAIN.IO_S[1]
  assign net_0306 = pin_P42;
  wire D0X4Y0_MAIN_IO_S_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X4Y0_MAIN_IO_S_1__input (.PAD_IN(pin_P42), .IK(net_0000), .Q(D0X4Y0_MAIN_IO_S_1__input_q));
  assign net_0308 = D0X4Y0_MAIN_IO_S_1__input_q;
  wire D0X4Y0_MAIN_IO_S_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X4Y0_MAIN_IO_S_1__output_ff (.D(net_0300), .OK(net_0002), .Q(D0X4Y0_MAIN_IO_S_1__output_q));
  pullup(pin_P42);
  (* XC3000_LOC="D0X4Y0.MAIN", XC3000_BEL="IO_S[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X4Y0_MAIN_IO_S_1__driver (.O(net_0300), .T(1'b1), .PAD(pin_P42));

  // D0X4Y1.MAIN.CLB; MODE=FGM
  wire D0X4Y1_MAIN_CLB_f, D0X4Y1_MAIN_CLB_g, D0X4Y1_MAIN_CLB_m, D0X4Y1_MAIN_CLB_qx, D0X4Y1_MAIN_CLB_qy, D0X4Y1_MAIN_CLB_dx, D0X4Y1_MAIN_CLB_dy;
  (* XC3000_LOC="D0X4Y1.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1110010011100100), .INIT_G(16'b1110010011100100)) D0X4Y1_MAIN_CLB_lut (
    .A(net_0034), .F2(net_0305), .F3(net_0189), .F4(net_0244),
    .G2(net_0305), .G3(net_0189), .G4(net_0244), .E(net_0311), .F(D0X4Y1_MAIN_CLB_f), .G(D0X4Y1_MAIN_CLB_g), .M(D0X4Y1_MAIN_CLB_m));
  assign D0X4Y1_MAIN_CLB_dx = D0X4Y1_MAIN_CLB_m;
  assign D0X4Y1_MAIN_CLB_dy = D0X4Y1_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y1_MAIN_CLB_ffx (.D(D0X4Y1_MAIN_CLB_dx), .K(net_0310), .EC(net_0249), .RD(net_0309), .Q(D0X4Y1_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y1_MAIN_CLB_ffy (.D(D0X4Y1_MAIN_CLB_dy), .K(net_0310), .EC(net_0249), .RD(net_0309), .Q(D0X4Y1_MAIN_CLB_qy));
  assign net_0313 = D0X4Y1_MAIN_CLB_qx;
  assign net_0314 = D0X4Y1_MAIN_CLB_m;

  (* XC3000_LOC="D0X4Y1.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X4Y1_MAIN_TBUF_0_ (.I(net_0253), .T(net_0312), .O(net_0027));

  (* XC3000_LOC="D0X4Y1.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X4Y1_MAIN_TBUF_1_ (.I(net_0254), .T(net_0312), .O(net_0034));

  // D0X4Y2.MAIN.CLB; MODE=FGM
  wire D0X4Y2_MAIN_CLB_f, D0X4Y2_MAIN_CLB_g, D0X4Y2_MAIN_CLB_m, D0X4Y2_MAIN_CLB_qx, D0X4Y2_MAIN_CLB_qy, D0X4Y2_MAIN_CLB_dx, D0X4Y2_MAIN_CLB_dy;
  (* XC3000_LOC="D0X4Y2.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000011110000), .INIT_G(16'b1111111111110000)) D0X4Y2_MAIN_CLB_lut (
    .A(net_0249), .F2(net_0317), .F3(net_0315), .F4(net_0042),
    .G2(net_0317), .G3(net_0315), .G4(net_0042), .E(net_0249), .F(D0X4Y2_MAIN_CLB_f), .G(D0X4Y2_MAIN_CLB_g), .M(D0X4Y2_MAIN_CLB_m));
  assign D0X4Y2_MAIN_CLB_dx = D0X4Y2_MAIN_CLB_m;
  assign D0X4Y2_MAIN_CLB_dy = D0X4Y2_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y2_MAIN_CLB_ffx (.D(D0X4Y2_MAIN_CLB_dx), .K(net_0316), .EC(net_0249), .RD(net_0315), .Q(D0X4Y2_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y2_MAIN_CLB_ffy (.D(D0X4Y2_MAIN_CLB_dy), .K(net_0316), .EC(net_0249), .RD(net_0315), .Q(D0X4Y2_MAIN_CLB_qy));
  assign net_0320 = D0X4Y2_MAIN_CLB_m;
  assign net_0321 = D0X4Y2_MAIN_CLB_qy;

  (* XC3000_LOC="D0X4Y2.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X4Y2_MAIN_TBUF_0_ (.I(net_0154), .T(net_0319), .O(net_0042));

  (* XC3000_LOC="D0X4Y2.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X4Y2_MAIN_TBUF_1_ (.I(net_0318), .T(net_0319), .O(net_0047));

  // D0X4Y3.MAIN.CLB; MODE=FG
  wire D0X4Y3_MAIN_CLB_f, D0X4Y3_MAIN_CLB_g, D0X4Y3_MAIN_CLB_m, D0X4Y3_MAIN_CLB_qx, D0X4Y3_MAIN_CLB_qy, D0X4Y3_MAIN_CLB_dx, D0X4Y3_MAIN_CLB_dy;
  (* XC3000_LOC="D0X4Y3.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0010000000000000), .INIT_G(16'b0000001000000000)) D0X4Y3_MAIN_CLB_lut (
    .A(net_0065), .F2(net_0094), .F3(net_0007), .F4(net_0067),
    .G2(net_0094), .G3(net_0007), .G4(net_0067), .E(net_0067), .F(D0X4Y3_MAIN_CLB_f), .G(D0X4Y3_MAIN_CLB_g), .M(D0X4Y3_MAIN_CLB_m));
  assign D0X4Y3_MAIN_CLB_dx = D0X4Y3_MAIN_CLB_f;
  assign D0X4Y3_MAIN_CLB_dy = D0X4Y3_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y3_MAIN_CLB_ffx (.D(D0X4Y3_MAIN_CLB_dx), .K(net_0260), .EC(net_0067), .RD(net_0322), .Q(D0X4Y3_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y3_MAIN_CLB_ffy (.D(D0X4Y3_MAIN_CLB_dy), .K(net_0260), .EC(net_0067), .RD(net_0322), .Q(D0X4Y3_MAIN_CLB_qy));
  assign net_0325 = D0X4Y3_MAIN_CLB_f;
  assign net_0244 = D0X4Y3_MAIN_CLB_g;

  (* XC3000_LOC="D0X4Y3.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X4Y3_MAIN_TBUF_0_ (.I(net_0323), .T(net_0324), .O(net_0057));

  (* XC3000_LOC="D0X4Y3.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X4Y3_MAIN_TBUF_1_ (.I(net_0007), .T(net_0324), .O(net_0060));

  // D0X4Y4.MAIN.CLB; MODE=FGM
  wire D0X4Y4_MAIN_CLB_f, D0X4Y4_MAIN_CLB_g, D0X4Y4_MAIN_CLB_m, D0X4Y4_MAIN_CLB_qx, D0X4Y4_MAIN_CLB_qy, D0X4Y4_MAIN_CLB_dx, D0X4Y4_MAIN_CLB_dy;
  (* XC3000_LOC="D0X4Y4.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0101000001000100), .INIT_G(16'b1111101011101110)) D0X4Y4_MAIN_CLB_lut (
    .A(net_0067), .F2(net_0217), .F3(net_0154), .F4(net_0007),
    .G2(net_0217), .G3(net_0154), .G4(net_0007), .E(net_0326), .F(D0X4Y4_MAIN_CLB_f), .G(D0X4Y4_MAIN_CLB_g), .M(D0X4Y4_MAIN_CLB_m));
  assign D0X4Y4_MAIN_CLB_dx = D0X4Y4_MAIN_CLB_m;
  assign D0X4Y4_MAIN_CLB_dy = D0X4Y4_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y4_MAIN_CLB_ffx (.D(D0X4Y4_MAIN_CLB_dx), .K(net_0094), .EC(net_0067), .RD(net_0057), .Q(D0X4Y4_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y4_MAIN_CLB_ffy (.D(D0X4Y4_MAIN_CLB_dy), .K(net_0094), .EC(net_0067), .RD(net_0057), .Q(D0X4Y4_MAIN_CLB_qy));
  assign net_0328 = D0X4Y4_MAIN_CLB_qx;
  assign net_0329 = D0X4Y4_MAIN_CLB_m;

  (* XC3000_LOC="D0X4Y4.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X4Y4_MAIN_TBUF_0_ (.I(net_0008), .T(net_0327), .O(net_0071));

  (* XC3000_LOC="D0X4Y4.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X4Y4_MAIN_TBUF_1_ (.I(net_0280), .T(net_0327), .O(net_0000));

  // D0X4Y5.MAIN.CLB; MODE=FG
  wire D0X4Y5_MAIN_CLB_f, D0X4Y5_MAIN_CLB_g, D0X4Y5_MAIN_CLB_m, D0X4Y5_MAIN_CLB_qx, D0X4Y5_MAIN_CLB_qy, D0X4Y5_MAIN_CLB_dx, D0X4Y5_MAIN_CLB_dy;
  (* XC3000_LOC="D0X4Y5.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X4Y5_MAIN_CLB_lut (
    .A(net_0280), .F2(D0X4Y5_MAIN_CLB_qy), .F3(D0X4Y5_MAIN_CLB_qy), .F4(net_0008),
    .G2(D0X4Y5_MAIN_CLB_qy), .G3(D0X4Y5_MAIN_CLB_qy), .G4(net_0008), .E(net_0008), .F(D0X4Y5_MAIN_CLB_f), .G(D0X4Y5_MAIN_CLB_g), .M(D0X4Y5_MAIN_CLB_m));
  assign D0X4Y5_MAIN_CLB_dx = D0X4Y5_MAIN_CLB_f;
  assign D0X4Y5_MAIN_CLB_dy = D0X4Y5_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y5_MAIN_CLB_ffx (.D(D0X4Y5_MAIN_CLB_dx), .K(net_0217), .EC(net_0280), .RD(net_0331), .Q(D0X4Y5_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y5_MAIN_CLB_ffy (.D(D0X4Y5_MAIN_CLB_dy), .K(net_0217), .EC(net_0280), .RD(net_0331), .Q(D0X4Y5_MAIN_CLB_qy));
  assign net_0335 = D0X4Y5_MAIN_CLB_qx;
  assign net_0336 = D0X4Y5_MAIN_CLB_qy;

  (* XC3000_LOC="D0X4Y5.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X4Y5_MAIN_TBUF_0_ (.I(net_0332), .T(net_0333), .O(net_0078));

  (* XC3000_LOC="D0X4Y5.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X4Y5_MAIN_TBUF_1_ (.I(net_0001), .T(net_0334), .O(net_0082));

  // D0X4Y6.MAIN.CLB; MODE=FG
  wire D0X4Y6_MAIN_CLB_f, D0X4Y6_MAIN_CLB_g, D0X4Y6_MAIN_CLB_m, D0X4Y6_MAIN_CLB_qx, D0X4Y6_MAIN_CLB_qy, D0X4Y6_MAIN_CLB_dx, D0X4Y6_MAIN_CLB_dy;
  (* XC3000_LOC="D0X4Y6.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000001000), .INIT_G(16'b0000000000000100)) D0X4Y6_MAIN_CLB_lut (
    .A(net_0007), .F2(net_0067), .F3(net_0065), .F4(net_0094),
    .G2(net_0067), .G3(net_0065), .G4(net_0094), .E(net_0338), .F(D0X4Y6_MAIN_CLB_f), .G(D0X4Y6_MAIN_CLB_g), .M(D0X4Y6_MAIN_CLB_m));
  assign D0X4Y6_MAIN_CLB_dx = D0X4Y6_MAIN_CLB_f;
  assign D0X4Y6_MAIN_CLB_dy = D0X4Y6_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y6_MAIN_CLB_ffx (.D(D0X4Y6_MAIN_CLB_dx), .K(net_0337), .EC(net_0001), .RD(net_0007), .Q(D0X4Y6_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y6_MAIN_CLB_ffy (.D(D0X4Y6_MAIN_CLB_dy), .K(net_0337), .EC(net_0001), .RD(net_0007), .Q(D0X4Y6_MAIN_CLB_qy));
  assign net_0091 = D0X4Y6_MAIN_CLB_f;
  assign net_0340 = D0X4Y6_MAIN_CLB_g;

  (* XC3000_LOC="D0X4Y6.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X4Y6_MAIN_TBUF_0_ (.I(net_0008), .T(net_0339), .O(net_0094));

  (* XC3000_LOC="D0X4Y6.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X4Y6_MAIN_TBUF_1_ (.I(net_0130), .T(net_0339), .O(net_0089));

  // D0X4Y7.MAIN.CLB; MODE=FGM
  wire D0X4Y7_MAIN_CLB_f, D0X4Y7_MAIN_CLB_g, D0X4Y7_MAIN_CLB_m, D0X4Y7_MAIN_CLB_qx, D0X4Y7_MAIN_CLB_qy, D0X4Y7_MAIN_CLB_dx, D0X4Y7_MAIN_CLB_dy;
  (* XC3000_LOC="D0X4Y7.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000011101110), .INIT_G(16'b0000000011110000)) D0X4Y7_MAIN_CLB_lut (
    .A(net_0236), .F2(net_0290), .F3(net_0341), .F4(net_0067),
    .G2(net_0290), .G3(net_0341), .G4(net_0067), .E(net_0007), .F(D0X4Y7_MAIN_CLB_f), .G(D0X4Y7_MAIN_CLB_g), .M(D0X4Y7_MAIN_CLB_m));
  assign D0X4Y7_MAIN_CLB_dx = D0X4Y7_MAIN_CLB_m;
  assign D0X4Y7_MAIN_CLB_dy = D0X4Y7_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y7_MAIN_CLB_ffx (.D(D0X4Y7_MAIN_CLB_dx), .K(net_0091), .EC(net_0212), .RD(net_0008), .Q(D0X4Y7_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X4Y7_MAIN_CLB_ffy (.D(D0X4Y7_MAIN_CLB_dy), .K(net_0091), .EC(net_0212), .RD(net_0008), .Q(D0X4Y7_MAIN_CLB_qy));
  assign net_0344 = D0X4Y7_MAIN_CLB_m;
  assign net_0345 = D0X4Y7_MAIN_CLB_qy;

  (* XC3000_LOC="D0X4Y7.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X4Y7_MAIN_TBUF_0_ (.I(net_0001), .T(net_0342), .O(net_0105));

  (* XC3000_LOC="D0X4Y7.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X4Y7_MAIN_TBUF_1_ (.I(net_0001), .T(net_0343), .O(net_0099));

  // D0X4Y8.MAIN.CLB; MODE=FG
  wire D0X4Y8_MAIN_CLB_f, D0X4Y8_MAIN_CLB_g, D0X4Y8_MAIN_CLB_m, D0X4Y8_MAIN_CLB_qx, D0X4Y8_MAIN_CLB_qy, D0X4Y8_MAIN_CLB_dx, D0X4Y8_MAIN_CLB_dy;
  (* XC3000_LOC="D0X4Y8.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110010101010), .INIT_G(16'b1111000000110011)) D0X4Y8_MAIN_CLB_lut (
    .A(net_0115), .F2(D0X4Y8_MAIN_CLB_qx), .F3(net_0212), .F4(net_0110),
    .G2(net_0004), .G3(D0X4Y8_MAIN_CLB_qy), .G4(net_0110), .E(net_0001), .F(D0X4Y8_MAIN_CLB_f), .G(D0X4Y8_MAIN_CLB_g), .M(D0X4Y8_MAIN_CLB_m));
  assign D0X4Y8_MAIN_CLB_dx = D0X4Y8_MAIN_CLB_f;
  assign D0X4Y8_MAIN_CLB_dy = D0X4Y8_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X4Y8_MAIN_CLB_ffx (.D(D0X4Y8_MAIN_CLB_dx), .K(net_0002), .EC(net_0098), .RD(net_0120), .Q(D0X4Y8_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X4Y8_MAIN_CLB_ffy (.D(D0X4Y8_MAIN_CLB_dy), .K(net_0002), .EC(net_0098), .RD(net_0120), .Q(D0X4Y8_MAIN_CLB_qy));
  assign net_0348 = D0X4Y8_MAIN_CLB_qx;
  assign net_0349 = D0X4Y8_MAIN_CLB_qy;

  (* XC3000_LOC="D0X4Y8.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X4Y8_MAIN_TBUF_0_ (.I(net_0346), .T(net_0347), .O(net_0110));

  (* XC3000_LOC="D0X4Y8.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X4Y8_MAIN_TBUF_1_ (.I(net_0212), .T(net_0347), .O(net_0115));

  // D0X4Y9.MAIN.CLB; MODE=FG
  wire D0X4Y9_MAIN_CLB_f, D0X4Y9_MAIN_CLB_g, D0X4Y9_MAIN_CLB_m, D0X4Y9_MAIN_CLB_qx, D0X4Y9_MAIN_CLB_qy, D0X4Y9_MAIN_CLB_dx, D0X4Y9_MAIN_CLB_dy;
  (* XC3000_LOC="D0X4Y9.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110010101010), .INIT_G(16'b1111000000110011)) D0X4Y9_MAIN_CLB_lut (
    .A(net_0123), .F2(D0X4Y9_MAIN_CLB_qx), .F3(net_0212), .F4(net_0008),
    .G2(net_0004), .G3(D0X4Y9_MAIN_CLB_qy), .G4(net_0008), .E(net_0008), .F(D0X4Y9_MAIN_CLB_f), .G(D0X4Y9_MAIN_CLB_g), .M(D0X4Y9_MAIN_CLB_m));
  assign D0X4Y9_MAIN_CLB_dx = D0X4Y9_MAIN_CLB_f;
  assign D0X4Y9_MAIN_CLB_dy = D0X4Y9_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X4Y9_MAIN_CLB_ffx (.D(D0X4Y9_MAIN_CLB_dx), .K(net_0002), .EC(net_0098), .RD(net_0008), .Q(D0X4Y9_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X4Y9_MAIN_CLB_ffy (.D(D0X4Y9_MAIN_CLB_dy), .K(net_0002), .EC(net_0098), .RD(net_0008), .Q(D0X4Y9_MAIN_CLB_qy));
  assign net_0351 = D0X4Y9_MAIN_CLB_qx;
  assign net_0352 = D0X4Y9_MAIN_CLB_qy;

  (* XC3000_LOC="D0X4Y9.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X4Y9_MAIN_TBUF_0_ (.I(net_0001), .T(net_0350), .O(net_0016));

  (* XC3000_LOC="D0X4Y9.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X4Y9_MAIN_TBUF_1_ (.I(net_0212), .T(net_0350), .O(net_0123));

  // D0X4Y9.MAIN.IO_N[0]
  assign net_0353 = pin_P3;
  wire D0X4Y9_MAIN_IO_N_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X4Y9_MAIN_IO_N_0__input (.PAD_IN(pin_P3), .IK(net_0015), .Q(D0X4Y9_MAIN_IO_N_0__input_q));
  assign net_0212 = D0X4Y9_MAIN_IO_N_0__input_q;
  wire D0X4Y9_MAIN_IO_N_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X4Y9_MAIN_IO_N_0__output_ff (.D(net_0294), .OK(net_0015), .Q(D0X4Y9_MAIN_IO_N_0__output_q));
  (* XC3000_LOC="D0X4Y9.MAIN", XC3000_BEL="IO_N[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X4Y9_MAIN_IO_N_0__driver (.O(net_0294), .T(net_0350), .PAD(pin_P3));

  // D0X4Y9.MAIN.IO_N[1]
  assign net_0354 = pin_P2;
  wire D0X4Y9_MAIN_IO_N_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X4Y9_MAIN_IO_N_1__input (.PAD_IN(pin_P2), .IK(net_0002), .Q(D0X4Y9_MAIN_IO_N_1__input_q));
  assign net_0355 = D0X4Y9_MAIN_IO_N_1__input_q;
  wire D0X4Y9_MAIN_IO_N_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X4Y9_MAIN_IO_N_1__output_ff (.D(net_0348), .OK(net_0015), .Q(D0X4Y9_MAIN_IO_N_1__output_q));
  (* XC3000_LOC="D0X4Y9.MAIN", XC3000_BEL="IO_N[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X4Y9_MAIN_IO_N_1__driver (.O(net_0348), .T(~(net_0349)), .PAD(pin_P2));

  // D0X5Y0.MAIN.CLB; MODE=FG
  wire D0X5Y0_MAIN_CLB_f, D0X5Y0_MAIN_CLB_g, D0X5Y0_MAIN_CLB_m, D0X5Y0_MAIN_CLB_qx, D0X5Y0_MAIN_CLB_qy, D0X5Y0_MAIN_CLB_dx, D0X5Y0_MAIN_CLB_dy;
  (* XC3000_LOC="D0X5Y0.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110010101010)) D0X5Y0_MAIN_CLB_lut (
    .A(net_0003), .F2(net_0004), .F3(D0X5Y0_MAIN_CLB_qx), .F4(net_0008),
    .G2(D0X5Y0_MAIN_CLB_qy), .G3(net_0304), .G4(net_0008), .E(net_0008), .F(D0X5Y0_MAIN_CLB_f), .G(D0X5Y0_MAIN_CLB_g), .M(D0X5Y0_MAIN_CLB_m));
  assign D0X5Y0_MAIN_CLB_dx = D0X5Y0_MAIN_CLB_f;
  assign D0X5Y0_MAIN_CLB_dy = D0X5Y0_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X5Y0_MAIN_CLB_ffx (.D(D0X5Y0_MAIN_CLB_dx), .K(net_0002), .EC(net_0244), .RD(net_0008), .Q(D0X5Y0_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X5Y0_MAIN_CLB_ffy (.D(D0X5Y0_MAIN_CLB_dy), .K(net_0002), .EC(net_0244), .RD(net_0008), .Q(D0X5Y0_MAIN_CLB_qy));
  assign net_0357 = D0X5Y0_MAIN_CLB_qx;
  assign net_0356 = D0X5Y0_MAIN_CLB_qy;

  (* XC3000_LOC="D0X5Y0.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X5Y0_MAIN_TBUF_0_ (.I(net_0315), .T(net_0358), .O(net_0016));

  (* XC3000_LOC="D0X5Y0.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X5Y0_MAIN_TBUF_1_ (.I(net_0304), .T(net_0358), .O(net_0003));

  // D0X5Y0.MAIN.IO_S[0]
  assign net_0315 = pin_P44;
  wire D0X5Y0_MAIN_IO_S_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X5Y0_MAIN_IO_S_0__input (.PAD_IN(pin_P44), .IK(net_0000), .Q(D0X5Y0_MAIN_IO_S_0__input_q));
  assign net_0360 = D0X5Y0_MAIN_IO_S_0__input_q;
  wire D0X5Y0_MAIN_IO_S_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X5Y0_MAIN_IO_S_0__output_ff (.D(net_0303), .OK(net_0002), .Q(D0X5Y0_MAIN_IO_S_0__output_q));
  (* XC3000_LOC="D0X5Y0.MAIN", XC3000_BEL="IO_S[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X5Y0_MAIN_IO_S_0__driver (.O(net_0303), .T(~(net_0304)), .PAD(pin_P44));

  // D0X5Y0.MAIN.IO_S[1]
  assign net_0359 = pin_P45;
  wire D0X5Y0_MAIN_IO_S_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X5Y0_MAIN_IO_S_1__input (.PAD_IN(pin_P45), .IK(net_0000), .Q(D0X5Y0_MAIN_IO_S_1__input_q));
  assign net_0361 = D0X5Y0_MAIN_IO_S_1__input_q;
  wire D0X5Y0_MAIN_IO_S_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X5Y0_MAIN_IO_S_1__output_ff (.D(net_0356), .OK(net_0002), .Q(D0X5Y0_MAIN_IO_S_1__output_q));
  (* XC3000_LOC="D0X5Y0.MAIN", XC3000_BEL="IO_S[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X5Y0_MAIN_IO_S_1__driver (.O(net_0356), .T(~(net_0357)), .PAD(pin_P45));

  // D0X5Y1.MAIN.CLB; MODE=FG
  wire D0X5Y1_MAIN_CLB_f, D0X5Y1_MAIN_CLB_g, D0X5Y1_MAIN_CLB_m, D0X5Y1_MAIN_CLB_qx, D0X5Y1_MAIN_CLB_qy, D0X5Y1_MAIN_CLB_dx, D0X5Y1_MAIN_CLB_dy;
  (* XC3000_LOC="D0X5Y1.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X5Y1_MAIN_CLB_lut (
    .A(net_0244), .F2(D0X5Y1_MAIN_CLB_qx), .F3(net_0017), .F4(net_0027),
    .G2(net_0004), .G3(D0X5Y1_MAIN_CLB_qy), .G4(net_0027), .E(net_0314), .F(D0X5Y1_MAIN_CLB_f), .G(D0X5Y1_MAIN_CLB_g), .M(D0X5Y1_MAIN_CLB_m));
  assign D0X5Y1_MAIN_CLB_dx = D0X5Y1_MAIN_CLB_f;
  assign D0X5Y1_MAIN_CLB_dy = D0X5Y1_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X5Y1_MAIN_CLB_ffx (.D(D0X5Y1_MAIN_CLB_dx), .K(net_0002), .EC(net_0244), .RD(net_0017), .Q(D0X5Y1_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X5Y1_MAIN_CLB_ffy (.D(D0X5Y1_MAIN_CLB_dy), .K(net_0002), .EC(net_0244), .RD(net_0017), .Q(D0X5Y1_MAIN_CLB_qy));
  assign net_0364 = D0X5Y1_MAIN_CLB_qx;
  assign net_0365 = D0X5Y1_MAIN_CLB_qy;

  (* XC3000_LOC="D0X5Y1.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X5Y1_MAIN_TBUF_0_ (.I(net_0315), .T(net_0363), .O(net_0027));

  (* XC3000_LOC="D0X5Y1.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X5Y1_MAIN_TBUF_1_ (.I(net_0362), .T(net_0363), .O(net_0034));

  // D0X5Y2.MAIN.CLB; MODE=FGM
  wire D0X5Y2_MAIN_CLB_f, D0X5Y2_MAIN_CLB_g, D0X5Y2_MAIN_CLB_m, D0X5Y2_MAIN_CLB_qx, D0X5Y2_MAIN_CLB_qy, D0X5Y2_MAIN_CLB_dx, D0X5Y2_MAIN_CLB_dy;
  (* XC3000_LOC="D0X5Y2.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1111111100000000)) D0X5Y2_MAIN_CLB_lut (
    .A(net_0120), .F2(net_0320), .F3(net_0368), .F4(net_0366),
    .G2(net_0320), .G3(net_0368), .G4(net_0366), .E(net_0007), .F(D0X5Y2_MAIN_CLB_f), .G(D0X5Y2_MAIN_CLB_g), .M(D0X5Y2_MAIN_CLB_m));
  assign D0X5Y2_MAIN_CLB_dx = D0X5Y2_MAIN_CLB_m;
  assign D0X5Y2_MAIN_CLB_dy = D0X5Y2_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y2_MAIN_CLB_ffx (.D(D0X5Y2_MAIN_CLB_dx), .K(net_0315), .EC(net_0244), .RD(net_0367), .Q(D0X5Y2_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y2_MAIN_CLB_ffy (.D(D0X5Y2_MAIN_CLB_dy), .K(net_0315), .EC(net_0244), .RD(net_0367), .Q(D0X5Y2_MAIN_CLB_qy));
  assign net_0372 = D0X5Y2_MAIN_CLB_qx;
  assign net_0326 = D0X5Y2_MAIN_CLB_m;

  (* XC3000_LOC="D0X5Y2.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X5Y2_MAIN_TBUF_0_ (.I(net_0369), .T(net_0371), .O(net_0042));

  (* XC3000_LOC="D0X5Y2.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X5Y2_MAIN_TBUF_1_ (.I(net_0370), .T(net_0371), .O(net_0047));

  // D0X5Y3.MAIN.CLB; MODE=FGM
  wire D0X5Y3_MAIN_CLB_f, D0X5Y3_MAIN_CLB_g, D0X5Y3_MAIN_CLB_m, D0X5Y3_MAIN_CLB_qx, D0X5Y3_MAIN_CLB_qy, D0X5Y3_MAIN_CLB_dx, D0X5Y3_MAIN_CLB_dy;
  (* XC3000_LOC="D0X5Y3.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000010001000), .INIT_G(16'b1111000011011101)) D0X5Y3_MAIN_CLB_lut (
    .A(net_0120), .F2(net_0260), .F3(net_0373), .F4(net_0007),
    .G2(net_0260), .G3(net_0373), .G4(net_0007), .E(net_0314), .F(D0X5Y3_MAIN_CLB_f), .G(D0X5Y3_MAIN_CLB_g), .M(D0X5Y3_MAIN_CLB_m));
  assign D0X5Y3_MAIN_CLB_dx = D0X5Y3_MAIN_CLB_m;
  assign D0X5Y3_MAIN_CLB_dy = D0X5Y3_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y3_MAIN_CLB_ffx (.D(D0X5Y3_MAIN_CLB_dx), .K(net_0260), .EC(net_0057), .RD(net_0374), .Q(D0X5Y3_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y3_MAIN_CLB_ffy (.D(D0X5Y3_MAIN_CLB_dy), .K(net_0260), .EC(net_0057), .RD(net_0374), .Q(D0X5Y3_MAIN_CLB_qy));
  assign net_0376 = D0X5Y3_MAIN_CLB_qx;
  assign net_0377 = D0X5Y3_MAIN_CLB_m;

  (* XC3000_LOC="D0X5Y3.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X5Y3_MAIN_TBUF_0_ (.I(net_0007), .T(net_0375), .O(net_0057));

  (* XC3000_LOC="D0X5Y3.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X5Y3_MAIN_TBUF_1_ (.I(net_0244), .T(net_0375), .O(net_0060));

  // D0X5Y4.MAIN.CLB; MODE=FG
  wire D0X5Y4_MAIN_CLB_f, D0X5Y4_MAIN_CLB_g, D0X5Y4_MAIN_CLB_m, D0X5Y4_MAIN_CLB_qx, D0X5Y4_MAIN_CLB_qy, D0X5Y4_MAIN_CLB_dx, D0X5Y4_MAIN_CLB_dy;
  (* XC3000_LOC="D0X5Y4.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X5Y4_MAIN_CLB_lut (
    .A(net_0379), .F2(D0X5Y4_MAIN_CLB_qy), .F3(D0X5Y4_MAIN_CLB_qy), .F4(net_0329),
    .G2(D0X5Y4_MAIN_CLB_qy), .G3(D0X5Y4_MAIN_CLB_qy), .G4(net_0329), .E(net_0329), .F(D0X5Y4_MAIN_CLB_f), .G(D0X5Y4_MAIN_CLB_g), .M(D0X5Y4_MAIN_CLB_m));
  assign D0X5Y4_MAIN_CLB_dx = D0X5Y4_MAIN_CLB_f;
  assign D0X5Y4_MAIN_CLB_dy = D0X5Y4_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y4_MAIN_CLB_ffx (.D(D0X5Y4_MAIN_CLB_dx), .K(net_0094), .EC(net_0379), .RD(net_0378), .Q(D0X5Y4_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y4_MAIN_CLB_ffy (.D(D0X5Y4_MAIN_CLB_dy), .K(net_0094), .EC(net_0379), .RD(net_0378), .Q(D0X5Y4_MAIN_CLB_qy));
  assign net_0384 = D0X5Y4_MAIN_CLB_qx;
  assign net_0385 = D0X5Y4_MAIN_CLB_qy;

  (* XC3000_LOC="D0X5Y4.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X5Y4_MAIN_TBUF_0_ (.I(net_0382), .T(net_0383), .O(net_0071));

  (* XC3000_LOC="D0X5Y4.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X5Y4_MAIN_TBUF_1_ (.I(net_0381), .T(net_0383), .O(net_0000));

  // D0X5Y5.MAIN.CLB; MODE=FG
  wire D0X5Y5_MAIN_CLB_f, D0X5Y5_MAIN_CLB_g, D0X5Y5_MAIN_CLB_m, D0X5Y5_MAIN_CLB_qx, D0X5Y5_MAIN_CLB_qy, D0X5Y5_MAIN_CLB_dx, D0X5Y5_MAIN_CLB_dy;
  (* XC3000_LOC="D0X5Y5.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X5Y5_MAIN_CLB_lut (
    .A(net_0388), .F2(D0X5Y5_MAIN_CLB_qy), .F3(D0X5Y5_MAIN_CLB_qy), .F4(net_0329),
    .G2(D0X5Y5_MAIN_CLB_qy), .G3(D0X5Y5_MAIN_CLB_qy), .G4(net_0329), .E(net_0329), .F(D0X5Y5_MAIN_CLB_f), .G(D0X5Y5_MAIN_CLB_g), .M(D0X5Y5_MAIN_CLB_m));
  assign D0X5Y5_MAIN_CLB_dx = D0X5Y5_MAIN_CLB_f;
  assign D0X5Y5_MAIN_CLB_dy = D0X5Y5_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y5_MAIN_CLB_ffx (.D(D0X5Y5_MAIN_CLB_dx), .K(net_0387), .EC(net_0388), .RD(net_0386), .Q(D0X5Y5_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y5_MAIN_CLB_ffy (.D(D0X5Y5_MAIN_CLB_dy), .K(net_0387), .EC(net_0388), .RD(net_0386), .Q(D0X5Y5_MAIN_CLB_qy));
  assign net_0393 = D0X5Y5_MAIN_CLB_qx;
  assign net_0394 = D0X5Y5_MAIN_CLB_qy;

  (* XC3000_LOC="D0X5Y5.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X5Y5_MAIN_TBUF_0_ (.I(net_0391), .T(net_0392), .O(net_0078));

  (* XC3000_LOC="D0X5Y5.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X5Y5_MAIN_TBUF_1_ (.I(net_0390), .T(net_0392), .O(net_0082));

  // D0X5Y6.MAIN.CLB; MODE=FGM
  wire D0X5Y6_MAIN_CLB_f, D0X5Y6_MAIN_CLB_g, D0X5Y6_MAIN_CLB_m, D0X5Y6_MAIN_CLB_qx, D0X5Y6_MAIN_CLB_qy, D0X5Y6_MAIN_CLB_dx, D0X5Y6_MAIN_CLB_dy;
  (* XC3000_LOC="D0X5Y6.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000001110), .INIT_G(16'b0000000011111110)) D0X5Y6_MAIN_CLB_lut (
    .A(net_0397), .F2(net_0344), .F3(net_0065), .F4(net_0094),
    .G2(net_0344), .G3(net_0065), .G4(net_0094), .E(net_0329), .F(D0X5Y6_MAIN_CLB_f), .G(D0X5Y6_MAIN_CLB_g), .M(D0X5Y6_MAIN_CLB_m));
  assign D0X5Y6_MAIN_CLB_dx = D0X5Y6_MAIN_CLB_m;
  assign D0X5Y6_MAIN_CLB_dy = D0X5Y6_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y6_MAIN_CLB_ffx (.D(D0X5Y6_MAIN_CLB_dx), .K(net_0396), .EC(net_0007), .RD(net_0395), .Q(D0X5Y6_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y6_MAIN_CLB_ffy (.D(D0X5Y6_MAIN_CLB_dy), .K(net_0396), .EC(net_0007), .RD(net_0395), .Q(D0X5Y6_MAIN_CLB_qy));
  assign net_0399 = D0X5Y6_MAIN_CLB_qx;
  assign net_0400 = D0X5Y6_MAIN_CLB_m;

  (* XC3000_LOC="D0X5Y6.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X5Y6_MAIN_TBUF_0_ (.I(net_0091), .T(net_0398), .O(net_0094));

  (* XC3000_LOC="D0X5Y6.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X5Y6_MAIN_TBUF_1_ (.I(net_0065), .T(net_0398), .O(net_0089));

  // D0X5Y7.MAIN.CLB; MODE=FGM
  wire D0X5Y7_MAIN_CLB_f, D0X5Y7_MAIN_CLB_g, D0X5Y7_MAIN_CLB_m, D0X5Y7_MAIN_CLB_qx, D0X5Y7_MAIN_CLB_qy, D0X5Y7_MAIN_CLB_dx, D0X5Y7_MAIN_CLB_dy;
  (* XC3000_LOC="D0X5Y7.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111101000000000), .INIT_G(16'b1100110000000000)) D0X5Y7_MAIN_CLB_lut (
    .A(net_0402), .F2(net_0233), .F3(net_0401), .F4(net_0067),
    .G2(net_0233), .G3(net_0401), .G4(net_0067), .E(net_0007), .F(D0X5Y7_MAIN_CLB_f), .G(D0X5Y7_MAIN_CLB_g), .M(D0X5Y7_MAIN_CLB_m));
  assign D0X5Y7_MAIN_CLB_dx = D0X5Y7_MAIN_CLB_m;
  assign D0X5Y7_MAIN_CLB_dy = D0X5Y7_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y7_MAIN_CLB_ffx (.D(D0X5Y7_MAIN_CLB_dx), .K(net_0340), .EC(net_0007), .RD(net_0401), .Q(D0X5Y7_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y7_MAIN_CLB_ffy (.D(D0X5Y7_MAIN_CLB_dy), .K(net_0340), .EC(net_0007), .RD(net_0401), .Q(D0X5Y7_MAIN_CLB_qy));
  assign net_0406 = D0X5Y7_MAIN_CLB_qx;
  assign net_0397 = D0X5Y7_MAIN_CLB_m;

  (* XC3000_LOC="D0X5Y7.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X5Y7_MAIN_TBUF_0_ (.I(net_0403), .T(net_0405), .O(net_0105));

  (* XC3000_LOC="D0X5Y7.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X5Y7_MAIN_TBUF_1_ (.I(net_0404), .T(net_0405), .O(net_0099));

  // D0X5Y8.MAIN.CLB; MODE=FGM
  wire D0X5Y8_MAIN_CLB_f, D0X5Y8_MAIN_CLB_g, D0X5Y8_MAIN_CLB_m, D0X5Y8_MAIN_CLB_qx, D0X5Y8_MAIN_CLB_qy, D0X5Y8_MAIN_CLB_dx, D0X5Y8_MAIN_CLB_dy;
  (* XC3000_LOC="D0X5Y8.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110000000000), .INIT_G(16'b1111111100110011)) D0X5Y8_MAIN_CLB_lut (
    .A(net_0354), .F2(net_0212), .F3(net_0296), .F4(net_0296),
    .G2(net_0212), .G3(net_0296), .G4(net_0296), .E(net_0354), .F(D0X5Y8_MAIN_CLB_f), .G(D0X5Y8_MAIN_CLB_g), .M(D0X5Y8_MAIN_CLB_m));
  assign D0X5Y8_MAIN_CLB_dx = D0X5Y8_MAIN_CLB_m;
  assign D0X5Y8_MAIN_CLB_dy = D0X5Y8_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y8_MAIN_CLB_ffx (.D(D0X5Y8_MAIN_CLB_dx), .K(net_0233), .EC(net_0354), .RD(net_0008), .Q(D0X5Y8_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X5Y8_MAIN_CLB_ffy (.D(D0X5Y8_MAIN_CLB_dy), .K(net_0233), .EC(net_0354), .RD(net_0008), .Q(D0X5Y8_MAIN_CLB_qy));
  assign net_0409 = D0X5Y8_MAIN_CLB_m;
  assign net_0410 = D0X5Y8_MAIN_CLB_qy;

  (* XC3000_LOC="D0X5Y8.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X5Y8_MAIN_TBUF_0_ (.I(net_0407), .T(net_0408), .O(net_0110));

  (* XC3000_LOC="D0X5Y8.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X5Y8_MAIN_TBUF_1_ (.I(net_0296), .T(net_0408), .O(net_0115));

  // D0X5Y9.MAIN.CLB; MODE=FG
  wire D0X5Y9_MAIN_CLB_f, D0X5Y9_MAIN_CLB_g, D0X5Y9_MAIN_CLB_m, D0X5Y9_MAIN_CLB_qx, D0X5Y9_MAIN_CLB_qy, D0X5Y9_MAIN_CLB_dx, D0X5Y9_MAIN_CLB_dy;
  (* XC3000_LOC="D0X5Y9.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110010101010), .INIT_G(16'b1111000000110011)) D0X5Y9_MAIN_CLB_lut (
    .A(net_0123), .F2(D0X5Y9_MAIN_CLB_qx), .F3(net_0348), .F4(net_0001),
    .G2(net_0004), .G3(D0X5Y9_MAIN_CLB_qy), .G4(net_0001), .E(net_0349), .F(D0X5Y9_MAIN_CLB_f), .G(D0X5Y9_MAIN_CLB_g), .M(D0X5Y9_MAIN_CLB_m));
  assign D0X5Y9_MAIN_CLB_dx = D0X5Y9_MAIN_CLB_f;
  assign D0X5Y9_MAIN_CLB_dy = D0X5Y9_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X5Y9_MAIN_CLB_ffx (.D(D0X5Y9_MAIN_CLB_dx), .K(net_0002), .EC(net_0098), .RD(net_0411), .Q(D0X5Y9_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X5Y9_MAIN_CLB_ffy (.D(D0X5Y9_MAIN_CLB_dy), .K(net_0002), .EC(net_0098), .RD(net_0411), .Q(D0X5Y9_MAIN_CLB_qy));
  assign net_0413 = D0X5Y9_MAIN_CLB_qx;
  assign net_0414 = D0X5Y9_MAIN_CLB_qy;

  (* XC3000_LOC="D0X5Y9.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X5Y9_MAIN_TBUF_0_ (.I(net_0354), .T(net_0412), .O(net_0016));

  (* XC3000_LOC="D0X5Y9.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X5Y9_MAIN_TBUF_1_ (.I(net_0348), .T(net_0412), .O(net_0123));

  // D0X5Y9.MAIN.IO_N[0]
  assign net_0415 = pin_P84;
  wire D0X5Y9_MAIN_IO_N_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X5Y9_MAIN_IO_N_0__input (.PAD_IN(pin_P84), .IK(net_0002), .Q(D0X5Y9_MAIN_IO_N_0__input_q));
  assign net_0417 = D0X5Y9_MAIN_IO_N_0__input_q;
  wire D0X5Y9_MAIN_IO_N_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X5Y9_MAIN_IO_N_0__output_ff (.D(net_0296), .OK(net_0015), .Q(D0X5Y9_MAIN_IO_N_0__output_q));
  (* XC3000_LOC="D0X5Y9.MAIN", XC3000_BEL="IO_N[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X5Y9_MAIN_IO_N_0__driver (.O(net_0296), .T(net_0412), .PAD(pin_P84));

  // D0X5Y9.MAIN.IO_N[1]
  assign net_0416 = pin_P83;
  wire D0X5Y9_MAIN_IO_N_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X5Y9_MAIN_IO_N_1__input (.PAD_IN(pin_P83), .IK(net_0002), .Q(D0X5Y9_MAIN_IO_N_1__input_q));
  assign net_0418 = D0X5Y9_MAIN_IO_N_1__input_q;
  wire D0X5Y9_MAIN_IO_N_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X5Y9_MAIN_IO_N_1__output_ff (.D(net_0351), .OK(net_0015), .Q(D0X5Y9_MAIN_IO_N_1__output_q));
  (* XC3000_LOC="D0X5Y9.MAIN", XC3000_BEL="IO_N[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X5Y9_MAIN_IO_N_1__driver (.O(net_0351), .T(~(net_0352)), .PAD(pin_P83));

  // D0X6Y0.MAIN.CLB; MODE=FG
  wire D0X6Y0_MAIN_CLB_f, D0X6Y0_MAIN_CLB_g, D0X6Y0_MAIN_CLB_m, D0X6Y0_MAIN_CLB_qx, D0X6Y0_MAIN_CLB_qy, D0X6Y0_MAIN_CLB_dx, D0X6Y0_MAIN_CLB_dy;
  (* XC3000_LOC="D0X6Y0.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110000001111), .INIT_G(16'b1111110010101010)) D0X6Y0_MAIN_CLB_lut (
    .A(net_0003), .F2(D0X6Y0_MAIN_CLB_qx), .F3(net_0004), .F4(net_0008),
    .G2(D0X6Y0_MAIN_CLB_qy), .G3(D0X6Y0_MAIN_CLB_qy), .G4(net_0008), .E(net_0364), .F(D0X6Y0_MAIN_CLB_f), .G(D0X6Y0_MAIN_CLB_g), .M(D0X6Y0_MAIN_CLB_m));
  assign D0X6Y0_MAIN_CLB_dx = D0X6Y0_MAIN_CLB_f;
  assign D0X6Y0_MAIN_CLB_dy = D0X6Y0_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X6Y0_MAIN_CLB_ffx (.D(D0X6Y0_MAIN_CLB_dx), .K(net_0002), .EC(net_0325), .RD(net_0004), .Q(D0X6Y0_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X6Y0_MAIN_CLB_ffy (.D(D0X6Y0_MAIN_CLB_dy), .K(net_0002), .EC(net_0325), .RD(net_0004), .Q(D0X6Y0_MAIN_CLB_qy));
  assign net_0422 = D0X6Y0_MAIN_CLB_qx;
  assign net_0421 = D0X6Y0_MAIN_CLB_qy;

  (* XC3000_LOC="D0X6Y0.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X6Y0_MAIN_TBUF_0_ (.I(net_0420), .T(net_0423), .O(net_0016));

  (* XC3000_LOC="D0X6Y0.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X6Y0_MAIN_TBUF_1_ (.I(net_0359), .T(net_0423), .O(net_0003));

  // D0X6Y0.MAIN.IO_S[0]
  assign net_0420 = pin_P46;
  wire D0X6Y0_MAIN_IO_S_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X6Y0_MAIN_IO_S_0__input (.PAD_IN(pin_P46), .IK(net_0000), .Q(D0X6Y0_MAIN_IO_S_0__input_q));
  assign net_0425 = D0X6Y0_MAIN_IO_S_0__input_q;
  wire D0X6Y0_MAIN_IO_S_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X6Y0_MAIN_IO_S_0__output_ff (.D(net_0364), .OK(net_0002), .Q(D0X6Y0_MAIN_IO_S_0__output_q));
  (* XC3000_LOC="D0X6Y0.MAIN", XC3000_BEL="IO_S[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X6Y0_MAIN_IO_S_0__driver (.O(net_0364), .T(~(net_0365)), .PAD(pin_P46));

  // D0X6Y0.MAIN.IO_S[1]
  assign net_0424 = pin_P47;
  wire D0X6Y0_MAIN_IO_S_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X6Y0_MAIN_IO_S_1__input (.PAD_IN(pin_P47), .IK(net_0000), .Q(D0X6Y0_MAIN_IO_S_1__input_q));
  assign net_0426 = D0X6Y0_MAIN_IO_S_1__input_q;
  wire D0X6Y0_MAIN_IO_S_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X6Y0_MAIN_IO_S_1__output_ff (.D(net_0421), .OK(net_0002), .Q(D0X6Y0_MAIN_IO_S_1__output_q));
  (* XC3000_LOC="D0X6Y0.MAIN", XC3000_BEL="IO_S[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X6Y0_MAIN_IO_S_1__driver (.O(net_0421), .T(~(net_0422)), .PAD(pin_P47));

  // D0X6Y1.MAIN.CLB; MODE=FG
  wire D0X6Y1_MAIN_CLB_f, D0X6Y1_MAIN_CLB_g, D0X6Y1_MAIN_CLB_m, D0X6Y1_MAIN_CLB_qx, D0X6Y1_MAIN_CLB_qy, D0X6Y1_MAIN_CLB_dx, D0X6Y1_MAIN_CLB_dy;
  (* XC3000_LOC="D0X6Y1.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111110001010101)) D0X6Y1_MAIN_CLB_lut (
    .A(net_0004), .F2(D0X6Y1_MAIN_CLB_qx), .F3(net_0017), .F4(net_0027),
    .G2(D0X6Y1_MAIN_CLB_qy), .G3(D0X6Y1_MAIN_CLB_qy), .G4(net_0027), .E(net_0420), .F(D0X6Y1_MAIN_CLB_f), .G(D0X6Y1_MAIN_CLB_g), .M(D0X6Y1_MAIN_CLB_m));
  assign D0X6Y1_MAIN_CLB_dx = D0X6Y1_MAIN_CLB_f;
  assign D0X6Y1_MAIN_CLB_dy = D0X6Y1_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X6Y1_MAIN_CLB_ffx (.D(D0X6Y1_MAIN_CLB_dx), .K(net_0002), .EC(net_0325), .RD(net_0017), .Q(D0X6Y1_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X6Y1_MAIN_CLB_ffy (.D(D0X6Y1_MAIN_CLB_dy), .K(net_0002), .EC(net_0325), .RD(net_0017), .Q(D0X6Y1_MAIN_CLB_qy));
  assign net_0429 = D0X6Y1_MAIN_CLB_qx;
  assign net_0430 = D0X6Y1_MAIN_CLB_qy;

  (* XC3000_LOC="D0X6Y1.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X6Y1_MAIN_TBUF_0_ (.I(net_0364), .T(net_0428), .O(net_0027));

  (* XC3000_LOC="D0X6Y1.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X6Y1_MAIN_TBUF_1_ (.I(net_0359), .T(net_0428), .O(net_0034));

  // D0X6Y2.MAIN.CLB; MODE=FGM
  wire D0X6Y2_MAIN_CLB_f, D0X6Y2_MAIN_CLB_g, D0X6Y2_MAIN_CLB_m, D0X6Y2_MAIN_CLB_qx, D0X6Y2_MAIN_CLB_qy, D0X6Y2_MAIN_CLB_dx, D0X6Y2_MAIN_CLB_dy;
  (* XC3000_LOC="D0X6Y2.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1010101011110000), .INIT_G(16'b1010101011110000)) D0X6Y2_MAIN_CLB_lut (
    .A(net_0359), .F2(net_0007), .F3(net_0420), .F4(net_0042),
    .G2(net_0007), .G3(net_0420), .G4(net_0042), .E(net_0325), .F(D0X6Y2_MAIN_CLB_f), .G(D0X6Y2_MAIN_CLB_g), .M(D0X6Y2_MAIN_CLB_m));
  assign D0X6Y2_MAIN_CLB_dx = D0X6Y2_MAIN_CLB_m;
  assign D0X6Y2_MAIN_CLB_dy = D0X6Y2_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y2_MAIN_CLB_ffx (.D(D0X6Y2_MAIN_CLB_dx), .K(net_0141), .EC(net_0260), .RD(net_0366), .Q(D0X6Y2_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y2_MAIN_CLB_ffy (.D(D0X6Y2_MAIN_CLB_dy), .K(net_0141), .EC(net_0260), .RD(net_0366), .Q(D0X6Y2_MAIN_CLB_qy));
  assign net_0368 = D0X6Y2_MAIN_CLB_m;
  assign net_0432 = D0X6Y2_MAIN_CLB_qy;

  (* XC3000_LOC="D0X6Y2.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X6Y2_MAIN_TBUF_0_ (.I(net_0420), .T(net_0431), .O(net_0042));

  (* XC3000_LOC="D0X6Y2.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X6Y2_MAIN_TBUF_1_ (.I(net_0326), .T(net_0431), .O(net_0047));

  // D0X6Y3.MAIN.CLB; MODE=FG
  wire D0X6Y3_MAIN_CLB_f, D0X6Y3_MAIN_CLB_g, D0X6Y3_MAIN_CLB_m, D0X6Y3_MAIN_CLB_qx, D0X6Y3_MAIN_CLB_qy, D0X6Y3_MAIN_CLB_dx, D0X6Y3_MAIN_CLB_dy;
  (* XC3000_LOC="D0X6Y3.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0010001000000000), .INIT_G(16'b0000000000100010)) D0X6Y3_MAIN_CLB_lut (
    .A(net_0094), .F2(net_0067), .F3(net_0260), .F4(net_0007),
    .G2(net_0067), .G3(net_0260), .G4(net_0007), .E(net_0377), .F(D0X6Y3_MAIN_CLB_f), .G(D0X6Y3_MAIN_CLB_g), .M(D0X6Y3_MAIN_CLB_m));
  assign D0X6Y3_MAIN_CLB_dx = D0X6Y3_MAIN_CLB_f;
  assign D0X6Y3_MAIN_CLB_dy = D0X6Y3_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y3_MAIN_CLB_ffx (.D(D0X6Y3_MAIN_CLB_dx), .K(net_0434), .EC(net_0094), .RD(net_0433), .Q(D0X6Y3_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y3_MAIN_CLB_ffy (.D(D0X6Y3_MAIN_CLB_dy), .K(net_0434), .EC(net_0094), .RD(net_0433), .Q(D0X6Y3_MAIN_CLB_qy));
  assign net_0437 = D0X6Y3_MAIN_CLB_f;
  assign net_0141 = D0X6Y3_MAIN_CLB_g;

  (* XC3000_LOC="D0X6Y3.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X6Y3_MAIN_TBUF_0_ (.I(net_0435), .T(net_0436), .O(net_0057));

  (* XC3000_LOC="D0X6Y3.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X6Y3_MAIN_TBUF_1_ (.I(net_0260), .T(net_0436), .O(net_0060));

  // D0X6Y4.MAIN.CLB; MODE=FG
  wire D0X6Y4_MAIN_CLB_f, D0X6Y4_MAIN_CLB_g, D0X6Y4_MAIN_CLB_m, D0X6Y4_MAIN_CLB_qx, D0X6Y4_MAIN_CLB_qy, D0X6Y4_MAIN_CLB_dx, D0X6Y4_MAIN_CLB_dy;
  (* XC3000_LOC="D0X6Y4.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X6Y4_MAIN_CLB_lut (
    .A(net_0094), .F2(D0X6Y4_MAIN_CLB_qy), .F3(D0X6Y4_MAIN_CLB_qy), .F4(net_0377),
    .G2(D0X6Y4_MAIN_CLB_qy), .G3(D0X6Y4_MAIN_CLB_qy), .G4(net_0377), .E(net_0377), .F(D0X6Y4_MAIN_CLB_f), .G(D0X6Y4_MAIN_CLB_g), .M(D0X6Y4_MAIN_CLB_m));
  assign D0X6Y4_MAIN_CLB_dx = D0X6Y4_MAIN_CLB_f;
  assign D0X6Y4_MAIN_CLB_dy = D0X6Y4_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y4_MAIN_CLB_ffx (.D(D0X6Y4_MAIN_CLB_dx), .K(net_0440), .EC(net_0094), .RD(net_0439), .Q(D0X6Y4_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y4_MAIN_CLB_ffy (.D(D0X6Y4_MAIN_CLB_dy), .K(net_0440), .EC(net_0094), .RD(net_0439), .Q(D0X6Y4_MAIN_CLB_qy));
  assign net_0445 = D0X6Y4_MAIN_CLB_qx;
  assign net_0446 = D0X6Y4_MAIN_CLB_qy;

  (* XC3000_LOC="D0X6Y4.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X6Y4_MAIN_TBUF_0_ (.I(net_0443), .T(net_0444), .O(net_0071));

  (* XC3000_LOC="D0X6Y4.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X6Y4_MAIN_TBUF_1_ (.I(net_0442), .T(net_0444), .O(net_0000));

  // D0X6Y5.MAIN.CLB; MODE=FG
  wire D0X6Y5_MAIN_CLB_f, D0X6Y5_MAIN_CLB_g, D0X6Y5_MAIN_CLB_m, D0X6Y5_MAIN_CLB_qx, D0X6Y5_MAIN_CLB_qy, D0X6Y5_MAIN_CLB_dx, D0X6Y5_MAIN_CLB_dy;
  (* XC3000_LOC="D0X6Y5.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1000100000000000), .INIT_G(16'b0000000010001000)) D0X6Y5_MAIN_CLB_lut (
    .A(net_0094), .F2(net_0067), .F3(net_0449), .F4(net_0007),
    .G2(net_0067), .G3(net_0449), .G4(net_0007), .E(net_0007), .F(D0X6Y5_MAIN_CLB_f), .G(D0X6Y5_MAIN_CLB_g), .M(D0X6Y5_MAIN_CLB_m));
  assign D0X6Y5_MAIN_CLB_dx = D0X6Y5_MAIN_CLB_f;
  assign D0X6Y5_MAIN_CLB_dy = D0X6Y5_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y5_MAIN_CLB_ffx (.D(D0X6Y5_MAIN_CLB_dx), .K(net_0448), .EC(net_0094), .RD(net_0447), .Q(D0X6Y5_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y5_MAIN_CLB_ffy (.D(D0X6Y5_MAIN_CLB_dy), .K(net_0448), .EC(net_0094), .RD(net_0447), .Q(D0X6Y5_MAIN_CLB_qy));
  assign net_0452 = D0X6Y5_MAIN_CLB_f;
  assign net_0453 = D0X6Y5_MAIN_CLB_g;

  (* XC3000_LOC="D0X6Y5.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X6Y5_MAIN_TBUF_0_ (.I(net_0450), .T(net_0451), .O(net_0078));

  (* XC3000_LOC="D0X6Y5.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X6Y5_MAIN_TBUF_1_ (.I(net_0449), .T(net_0451), .O(net_0082));

  // D0X6Y6.MAIN.CLB; MODE=FGM
  wire D0X6Y6_MAIN_CLB_f, D0X6Y6_MAIN_CLB_g, D0X6Y6_MAIN_CLB_m, D0X6Y6_MAIN_CLB_qx, D0X6Y6_MAIN_CLB_qy, D0X6Y6_MAIN_CLB_dx, D0X6Y6_MAIN_CLB_dy;
  (* XC3000_LOC="D0X6Y6.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1011100000000000), .INIT_G(16'b1111111111111111)) D0X6Y6_MAIN_CLB_lut (
    .A(net_0454), .F2(net_0067), .F3(net_0377), .F4(net_0094),
    .G2(net_0067), .G3(net_0377), .G4(net_0094), .E(net_0400), .F(D0X6Y6_MAIN_CLB_f), .G(D0X6Y6_MAIN_CLB_g), .M(D0X6Y6_MAIN_CLB_m));
  assign D0X6Y6_MAIN_CLB_dx = D0X6Y6_MAIN_CLB_m;
  assign D0X6Y6_MAIN_CLB_dy = D0X6Y6_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y6_MAIN_CLB_ffx (.D(D0X6Y6_MAIN_CLB_dx), .K(net_0456), .EC(net_0094), .RD(net_0455), .Q(D0X6Y6_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y6_MAIN_CLB_ffy (.D(D0X6Y6_MAIN_CLB_dy), .K(net_0456), .EC(net_0094), .RD(net_0455), .Q(D0X6Y6_MAIN_CLB_qy));
  assign net_0459 = D0X6Y6_MAIN_CLB_qx;
  assign net_0460 = D0X6Y6_MAIN_CLB_m;

  (* XC3000_LOC="D0X6Y6.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X6Y6_MAIN_TBUF_0_ (.I(net_0377), .T(net_0458), .O(net_0094));

  (* XC3000_LOC="D0X6Y6.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X6Y6_MAIN_TBUF_1_ (.I(net_0457), .T(net_0458), .O(net_0089));

  // D0X6Y7.MAIN.CLB; MODE=FG
  wire D0X6Y7_MAIN_CLB_f, D0X6Y7_MAIN_CLB_g, D0X6Y7_MAIN_CLB_m, D0X6Y7_MAIN_CLB_qx, D0X6Y7_MAIN_CLB_qy, D0X6Y7_MAIN_CLB_dx, D0X6Y7_MAIN_CLB_dy;
  (* XC3000_LOC="D0X6Y7.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X6Y7_MAIN_CLB_lut (
    .A(net_0340), .F2(D0X6Y7_MAIN_CLB_qy), .F3(D0X6Y7_MAIN_CLB_qy), .F4(net_0341),
    .G2(D0X6Y7_MAIN_CLB_qy), .G3(D0X6Y7_MAIN_CLB_qy), .G4(net_0341), .E(net_0341), .F(D0X6Y7_MAIN_CLB_f), .G(D0X6Y7_MAIN_CLB_g), .M(D0X6Y7_MAIN_CLB_m));
  assign D0X6Y7_MAIN_CLB_dx = D0X6Y7_MAIN_CLB_f;
  assign D0X6Y7_MAIN_CLB_dy = D0X6Y7_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y7_MAIN_CLB_ffx (.D(D0X6Y7_MAIN_CLB_dx), .K(net_0461), .EC(net_0340), .RD(net_0401), .Q(D0X6Y7_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y7_MAIN_CLB_ffy (.D(D0X6Y7_MAIN_CLB_dy), .K(net_0461), .EC(net_0340), .RD(net_0401), .Q(D0X6Y7_MAIN_CLB_qy));
  assign net_0464 = D0X6Y7_MAIN_CLB_qx;
  assign net_0465 = D0X6Y7_MAIN_CLB_qy;

  (* XC3000_LOC="D0X6Y7.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X6Y7_MAIN_TBUF_0_ (.I(net_0212), .T(net_0462), .O(net_0105));

  (* XC3000_LOC="D0X6Y7.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X6Y7_MAIN_TBUF_1_ (.I(net_0212), .T(net_0463), .O(net_0099));

  // D0X6Y8.MAIN.CLB; MODE=FGM
  wire D0X6Y8_MAIN_CLB_f, D0X6Y8_MAIN_CLB_g, D0X6Y8_MAIN_CLB_m, D0X6Y8_MAIN_CLB_qx, D0X6Y8_MAIN_CLB_qy, D0X6Y8_MAIN_CLB_dx, D0X6Y8_MAIN_CLB_dy;
  (* XC3000_LOC="D0X6Y8.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100010001000), .INIT_G(16'b1101100011011101)) D0X6Y8_MAIN_CLB_lut (
    .A(net_0120), .F2(net_0409), .F3(net_0416), .F4(net_0212),
    .G2(net_0409), .G3(net_0416), .G4(net_0212), .E(net_0466), .F(D0X6Y8_MAIN_CLB_f), .G(D0X6Y8_MAIN_CLB_g), .M(D0X6Y8_MAIN_CLB_m));
  assign D0X6Y8_MAIN_CLB_dx = D0X6Y8_MAIN_CLB_m;
  assign D0X6Y8_MAIN_CLB_dy = D0X6Y8_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y8_MAIN_CLB_ffx (.D(D0X6Y8_MAIN_CLB_dx), .K(net_0340), .EC(net_0466), .RD(net_0008), .Q(D0X6Y8_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X6Y8_MAIN_CLB_ffy (.D(D0X6Y8_MAIN_CLB_dy), .K(net_0340), .EC(net_0466), .RD(net_0008), .Q(D0X6Y8_MAIN_CLB_qy));
  assign net_0341 = D0X6Y8_MAIN_CLB_m;
  assign net_0468 = D0X6Y8_MAIN_CLB_qy;

  (* XC3000_LOC="D0X6Y8.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X6Y8_MAIN_TBUF_0_ (.I(net_0340), .T(net_0467), .O(net_0110));

  (* XC3000_LOC="D0X6Y8.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X6Y8_MAIN_TBUF_1_ (.I(net_0212), .T(net_0467), .O(net_0115));

  // D0X6Y9.MAIN.CLB; MODE=FG
  wire D0X6Y9_MAIN_CLB_f, D0X6Y9_MAIN_CLB_g, D0X6Y9_MAIN_CLB_m, D0X6Y9_MAIN_CLB_qx, D0X6Y9_MAIN_CLB_qy, D0X6Y9_MAIN_CLB_dx, D0X6Y9_MAIN_CLB_dy;
  (* XC3000_LOC="D0X6Y9.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111110010101010), .INIT_G(16'b1100110000001111)) D0X6Y9_MAIN_CLB_lut (
    .A(net_0123), .F2(D0X6Y9_MAIN_CLB_qx), .F3(D0X6Y9_MAIN_CLB_qx), .F4(net_0001),
    .G2(D0X6Y9_MAIN_CLB_qy), .G3(net_0004), .G4(net_0001), .E(net_0001), .F(D0X6Y9_MAIN_CLB_f), .G(D0X6Y9_MAIN_CLB_g), .M(D0X6Y9_MAIN_CLB_m));
  assign D0X6Y9_MAIN_CLB_dx = D0X6Y9_MAIN_CLB_f;
  assign D0X6Y9_MAIN_CLB_dy = D0X6Y9_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X6Y9_MAIN_CLB_ffx (.D(D0X6Y9_MAIN_CLB_dx), .K(net_0002), .EC(net_0340), .RD(net_0001), .Q(D0X6Y9_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X6Y9_MAIN_CLB_ffy (.D(D0X6Y9_MAIN_CLB_dy), .K(net_0002), .EC(net_0340), .RD(net_0001), .Q(D0X6Y9_MAIN_CLB_qy));
  assign net_0471 = D0X6Y9_MAIN_CLB_qx;
  assign net_0472 = D0X6Y9_MAIN_CLB_qy;

  (* XC3000_LOC="D0X6Y9.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X6Y9_MAIN_TBUF_0_ (.I(net_0466), .T(net_0470), .O(net_0016));

  (* XC3000_LOC="D0X6Y9.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X6Y9_MAIN_TBUF_1_ (.I(net_0351), .T(net_0470), .O(net_0123));

  // D0X6Y9.MAIN.IO_N[0]
  assign net_0466 = pin_P82;
  wire D0X6Y9_MAIN_IO_N_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X6Y9_MAIN_IO_N_0__input (.PAD_IN(pin_P82), .IK(net_0002), .Q(D0X6Y9_MAIN_IO_N_0__input_q));
  assign net_0473 = D0X6Y9_MAIN_IO_N_0__input_q;
  wire D0X6Y9_MAIN_IO_N_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X6Y9_MAIN_IO_N_0__output_ff (.D(net_0413), .OK(net_0015), .Q(D0X6Y9_MAIN_IO_N_0__output_q));
  (* XC3000_LOC="D0X6Y9.MAIN", XC3000_BEL="IO_N[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X6Y9_MAIN_IO_N_0__driver (.O(net_0413), .T(~(net_0414)), .PAD(pin_P82));

  // D0X6Y9.MAIN.IO_N[1]
  assign net_0017 = pin_P81;
  wire D0X6Y9_MAIN_IO_N_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X6Y9_MAIN_IO_N_1__input (.PAD_IN(pin_P81), .IK(net_0002), .Q(D0X6Y9_MAIN_IO_N_1__input_q));
  assign net_0474 = D0X6Y9_MAIN_IO_N_1__input_q;
  wire D0X6Y9_MAIN_IO_N_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X6Y9_MAIN_IO_N_1__output_ff (.D(net_0460), .OK(net_0015), .Q(D0X6Y9_MAIN_IO_N_1__output_q));
  (* XC3000_LOC="D0X6Y9.MAIN", XC3000_BEL="IO_N[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X6Y9_MAIN_IO_N_1__driver (.O(net_0460), .T(net_0415), .PAD(pin_P81));

  // D0X7Y0.MAIN.CLB; MODE=FG
  wire D0X7Y0_MAIN_CLB_f, D0X7Y0_MAIN_CLB_g, D0X7Y0_MAIN_CLB_m, D0X7Y0_MAIN_CLB_qx, D0X7Y0_MAIN_CLB_qy, D0X7Y0_MAIN_CLB_dx, D0X7Y0_MAIN_CLB_dy;
  (* XC3000_LOC="D0X7Y0.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1110001011100010), .INIT_G(16'b1100000011110011)) D0X7Y0_MAIN_CLB_lut (
    .A(net_0003), .F2(net_0057), .F3(D0X7Y0_MAIN_CLB_qx), .F4(net_0429),
    .G2(net_0057), .G3(D0X7Y0_MAIN_CLB_qy), .G4(net_0004), .E(net_0429), .F(D0X7Y0_MAIN_CLB_f), .G(D0X7Y0_MAIN_CLB_g), .M(D0X7Y0_MAIN_CLB_m));
  assign D0X7Y0_MAIN_CLB_dx = D0X7Y0_MAIN_CLB_f;
  assign D0X7Y0_MAIN_CLB_dy = D0X7Y0_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X7Y0_MAIN_CLB_ffx (.D(D0X7Y0_MAIN_CLB_dx), .K(net_0002), .EC(net_0325), .RD(net_0476), .Q(D0X7Y0_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X7Y0_MAIN_CLB_ffy (.D(D0X7Y0_MAIN_CLB_dy), .K(net_0002), .EC(net_0325), .RD(net_0476), .Q(D0X7Y0_MAIN_CLB_qy));
  assign net_0480 = D0X7Y0_MAIN_CLB_qx;
  assign net_0477 = D0X7Y0_MAIN_CLB_qy;

  (* XC3000_LOC="D0X7Y0.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X7Y0_MAIN_TBUF_0_ (.I(net_0475), .T(net_0479), .O(net_0016));

  (* XC3000_LOC="D0X7Y0.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X7Y0_MAIN_TBUF_1_ (.I(net_0424), .T(net_0479), .O(net_0003));

  // D0X7Y0.MAIN.IO_S[0]
  assign net_0475 = pin_P48;
  wire D0X7Y0_MAIN_IO_S_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X7Y0_MAIN_IO_S_0__input (.PAD_IN(pin_P48), .IK(net_0000), .Q(D0X7Y0_MAIN_IO_S_0__input_q));
  assign net_0482 = D0X7Y0_MAIN_IO_S_0__input_q;
  wire D0X7Y0_MAIN_IO_S_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X7Y0_MAIN_IO_S_0__output_ff (.D(net_0429), .OK(net_0002), .Q(D0X7Y0_MAIN_IO_S_0__output_q));
  (* XC3000_LOC="D0X7Y0.MAIN", XC3000_BEL="IO_S[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X7Y0_MAIN_IO_S_0__driver (.O(net_0429), .T(~(net_0430)), .PAD(pin_P48));

  // D0X7Y0.MAIN.IO_S[1]
  assign net_0481 = pin_P49;
  wire D0X7Y0_MAIN_IO_S_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X7Y0_MAIN_IO_S_1__input (.PAD_IN(pin_P49), .IK(net_0000), .Q(D0X7Y0_MAIN_IO_S_1__input_q));
  assign net_0483 = D0X7Y0_MAIN_IO_S_1__input_q;
  wire D0X7Y0_MAIN_IO_S_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X7Y0_MAIN_IO_S_1__output_ff (.D(net_0477), .OK(net_0002), .Q(D0X7Y0_MAIN_IO_S_1__output_q));
  pullup(pin_P49);
  (* XC3000_LOC="D0X7Y0.MAIN", XC3000_BEL="IO_S[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X7Y0_MAIN_IO_S_1__driver (.O(net_0477), .T(1'b1), .PAD(pin_P49));

  // D0X7Y1.MAIN.CLB; MODE=FGM
  wire D0X7Y1_MAIN_CLB_f, D0X7Y1_MAIN_CLB_g, D0X7Y1_MAIN_CLB_m, D0X7Y1_MAIN_CLB_qx, D0X7Y1_MAIN_CLB_qy, D0X7Y1_MAIN_CLB_dx, D0X7Y1_MAIN_CLB_dy;
  (* XC3000_LOC="D0X7Y1.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1011101110001000), .INIT_G(16'b1111000011110000)) D0X7Y1_MAIN_CLB_lut (
    .A(net_0424), .F2(net_0212), .F3(net_0485), .F4(net_0475),
    .G2(net_0212), .G3(net_0485), .G4(net_0475), .E(net_0120), .F(D0X7Y1_MAIN_CLB_f), .G(D0X7Y1_MAIN_CLB_g), .M(D0X7Y1_MAIN_CLB_m));
  assign D0X7Y1_MAIN_CLB_dx = D0X7Y1_MAIN_CLB_m;
  assign D0X7Y1_MAIN_CLB_dy = D0X7Y1_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y1_MAIN_CLB_ffx (.D(D0X7Y1_MAIN_CLB_dx), .K(net_0475), .EC(net_0486), .RD(net_0484), .Q(D0X7Y1_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y1_MAIN_CLB_ffy (.D(D0X7Y1_MAIN_CLB_dy), .K(net_0475), .EC(net_0486), .RD(net_0484), .Q(D0X7Y1_MAIN_CLB_qy));
  assign net_0488 = D0X7Y1_MAIN_CLB_qx;
  assign net_0366 = D0X7Y1_MAIN_CLB_m;

  (* XC3000_LOC="D0X7Y1.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X7Y1_MAIN_TBUF_0_ (.I(net_0429), .T(net_0487), .O(net_0027));

  (* XC3000_LOC="D0X7Y1.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X7Y1_MAIN_TBUF_1_ (.I(net_0430), .T(net_0487), .O(net_0034));

  // D0X7Y2.MAIN.CLB; MODE=FG
  wire D0X7Y2_MAIN_CLB_f, D0X7Y2_MAIN_CLB_g, D0X7Y2_MAIN_CLB_m, D0X7Y2_MAIN_CLB_qx, D0X7Y2_MAIN_CLB_qy, D0X7Y2_MAIN_CLB_dx, D0X7Y2_MAIN_CLB_dy;
  (* XC3000_LOC="D0X7Y2.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X7Y2_MAIN_CLB_lut (
    .A(net_0141), .F2(D0X7Y2_MAIN_CLB_qy), .F3(D0X7Y2_MAIN_CLB_qy), .F4(net_0492),
    .G2(D0X7Y2_MAIN_CLB_qy), .G3(D0X7Y2_MAIN_CLB_qy), .G4(net_0492), .E(net_0492), .F(D0X7Y2_MAIN_CLB_f), .G(D0X7Y2_MAIN_CLB_g), .M(D0X7Y2_MAIN_CLB_m));
  assign D0X7Y2_MAIN_CLB_dx = D0X7Y2_MAIN_CLB_f;
  assign D0X7Y2_MAIN_CLB_dy = D0X7Y2_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y2_MAIN_CLB_ffx (.D(D0X7Y2_MAIN_CLB_dx), .K(net_0366), .EC(net_0141), .RD(net_0489), .Q(D0X7Y2_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y2_MAIN_CLB_ffy (.D(D0X7Y2_MAIN_CLB_dy), .K(net_0366), .EC(net_0141), .RD(net_0489), .Q(D0X7Y2_MAIN_CLB_qy));
  assign net_0494 = D0X7Y2_MAIN_CLB_qx;
  assign net_0495 = D0X7Y2_MAIN_CLB_qy;

  (* XC3000_LOC="D0X7Y2.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X7Y2_MAIN_TBUF_0_ (.I(net_0491), .T(net_0493), .O(net_0042));

  (* XC3000_LOC="D0X7Y2.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X7Y2_MAIN_TBUF_1_ (.I(net_0490), .T(net_0493), .O(net_0047));

  // D0X7Y3.MAIN.CLB; MODE=FGM
  wire D0X7Y3_MAIN_CLB_f, D0X7Y3_MAIN_CLB_g, D0X7Y3_MAIN_CLB_m, D0X7Y3_MAIN_CLB_qx, D0X7Y3_MAIN_CLB_qy, D0X7Y3_MAIN_CLB_dx, D0X7Y3_MAIN_CLB_dy;
  (* XC3000_LOC="D0X7Y3.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1110111000100010), .INIT_G(16'b1111000011110000)) D0X7Y3_MAIN_CLB_lut (
    .A(net_0499), .F2(net_0212), .F3(net_0496), .F4(net_0498),
    .G2(net_0212), .G3(net_0496), .G4(net_0498), .E(net_0120), .F(D0X7Y3_MAIN_CLB_f), .G(D0X7Y3_MAIN_CLB_g), .M(D0X7Y3_MAIN_CLB_m));
  assign D0X7Y3_MAIN_CLB_dx = D0X7Y3_MAIN_CLB_m;
  assign D0X7Y3_MAIN_CLB_dy = D0X7Y3_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y3_MAIN_CLB_ffx (.D(D0X7Y3_MAIN_CLB_dx), .K(net_0498), .EC(net_0141), .RD(net_0497), .Q(D0X7Y3_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y3_MAIN_CLB_ffy (.D(D0X7Y3_MAIN_CLB_dy), .K(net_0498), .EC(net_0141), .RD(net_0497), .Q(D0X7Y3_MAIN_CLB_qy));
  assign net_0373 = D0X7Y3_MAIN_CLB_m;
  assign net_0503 = D0X7Y3_MAIN_CLB_qy;

  (* XC3000_LOC="D0X7Y3.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X7Y3_MAIN_TBUF_0_ (.I(net_0500), .T(net_0502), .O(net_0057));

  (* XC3000_LOC="D0X7Y3.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X7Y3_MAIN_TBUF_1_ (.I(net_0501), .T(net_0502), .O(net_0060));

  // D0X7Y4.MAIN.CLB; MODE=FGM
  wire D0X7Y4_MAIN_CLB_f, D0X7Y4_MAIN_CLB_g, D0X7Y4_MAIN_CLB_m, D0X7Y4_MAIN_CLB_qx, D0X7Y4_MAIN_CLB_qy, D0X7Y4_MAIN_CLB_dx, D0X7Y4_MAIN_CLB_dy;
  (* XC3000_LOC="D0X7Y4.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111110000110000), .INIT_G(16'b1111110000110000)) D0X7Y4_MAIN_CLB_lut (
    .A(net_0507), .F2(net_0212), .F3(net_0506), .F4(net_0504),
    .G2(net_0212), .G3(net_0506), .G4(net_0504), .E(net_0504), .F(D0X7Y4_MAIN_CLB_f), .G(D0X7Y4_MAIN_CLB_g), .M(D0X7Y4_MAIN_CLB_m));
  assign D0X7Y4_MAIN_CLB_dx = D0X7Y4_MAIN_CLB_m;
  assign D0X7Y4_MAIN_CLB_dy = D0X7Y4_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y4_MAIN_CLB_ffx (.D(D0X7Y4_MAIN_CLB_dx), .K(net_0505), .EC(net_0507), .RD(net_0499), .Q(D0X7Y4_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y4_MAIN_CLB_ffy (.D(D0X7Y4_MAIN_CLB_dy), .K(net_0505), .EC(net_0507), .RD(net_0499), .Q(D0X7Y4_MAIN_CLB_qy));
  assign net_0511 = D0X7Y4_MAIN_CLB_qx;
  assign net_0512 = D0X7Y4_MAIN_CLB_m;

  (* XC3000_LOC="D0X7Y4.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X7Y4_MAIN_TBUF_0_ (.I(net_0508), .T(net_0510), .O(net_0071));

  (* XC3000_LOC="D0X7Y4.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X7Y4_MAIN_TBUF_1_ (.I(net_0509), .T(net_0510), .O(net_0000));

  // D0X7Y5.MAIN.CLB; MODE=FGM
  wire D0X7Y5_MAIN_CLB_f, D0X7Y5_MAIN_CLB_g, D0X7Y5_MAIN_CLB_m, D0X7Y5_MAIN_CLB_qx, D0X7Y5_MAIN_CLB_qy, D0X7Y5_MAIN_CLB_dx, D0X7Y5_MAIN_CLB_dy;
  (* XC3000_LOC="D0X7Y5.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1110001011100010), .INIT_G(16'b1111111100000000)) D0X7Y5_MAIN_CLB_lut (
    .A(net_0515), .F2(net_0212), .F3(net_0513), .F4(net_0512),
    .G2(net_0212), .G3(net_0513), .G4(net_0512), .E(net_0120), .F(D0X7Y5_MAIN_CLB_f), .G(D0X7Y5_MAIN_CLB_g), .M(D0X7Y5_MAIN_CLB_m));
  assign D0X7Y5_MAIN_CLB_dx = D0X7Y5_MAIN_CLB_m;
  assign D0X7Y5_MAIN_CLB_dy = D0X7Y5_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y5_MAIN_CLB_ffx (.D(D0X7Y5_MAIN_CLB_dx), .K(net_0514), .EC(net_0517), .RD(net_0513), .Q(D0X7Y5_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y5_MAIN_CLB_ffy (.D(D0X7Y5_MAIN_CLB_dy), .K(net_0514), .EC(net_0517), .RD(net_0513), .Q(D0X7Y5_MAIN_CLB_qy));
  assign net_0520 = D0X7Y5_MAIN_CLB_m;
  assign net_0521 = D0X7Y5_MAIN_CLB_qy;

  (* XC3000_LOC="D0X7Y5.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X7Y5_MAIN_TBUF_0_ (.I(net_0516), .T(net_0519), .O(net_0078));

  (* XC3000_LOC="D0X7Y5.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X7Y5_MAIN_TBUF_1_ (.I(net_0518), .T(net_0519), .O(net_0082));

  // D0X7Y6.MAIN.CLB; MODE=FGM
  wire D0X7Y6_MAIN_CLB_f, D0X7Y6_MAIN_CLB_g, D0X7Y6_MAIN_CLB_m, D0X7Y6_MAIN_CLB_qx, D0X7Y6_MAIN_CLB_qy, D0X7Y6_MAIN_CLB_dx, D0X7Y6_MAIN_CLB_dy;
  (* XC3000_LOC="D0X7Y6.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1011101110001000), .INIT_G(16'b1011101110001000)) D0X7Y6_MAIN_CLB_lut (
    .A(net_0522), .F2(net_0212), .F3(net_0460), .F4(net_0524),
    .G2(net_0212), .G3(net_0460), .G4(net_0524), .E(net_0524), .F(D0X7Y6_MAIN_CLB_f), .G(D0X7Y6_MAIN_CLB_g), .M(D0X7Y6_MAIN_CLB_m));
  assign D0X7Y6_MAIN_CLB_dx = D0X7Y6_MAIN_CLB_m;
  assign D0X7Y6_MAIN_CLB_dy = D0X7Y6_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y6_MAIN_CLB_ffx (.D(D0X7Y6_MAIN_CLB_dx), .K(net_0453), .EC(net_0525), .RD(net_0515), .Q(D0X7Y6_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y6_MAIN_CLB_ffy (.D(D0X7Y6_MAIN_CLB_dy), .K(net_0453), .EC(net_0525), .RD(net_0515), .Q(D0X7Y6_MAIN_CLB_qy));
  assign net_0527 = D0X7Y6_MAIN_CLB_m;
  assign net_0528 = D0X7Y6_MAIN_CLB_qy;

  (* XC3000_LOC="D0X7Y6.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X7Y6_MAIN_TBUF_0_ (.I(net_0523), .T(net_0526), .O(net_0094));

  (* XC3000_LOC="D0X7Y6.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X7Y6_MAIN_TBUF_1_ (.I(net_0460), .T(net_0526), .O(net_0089));

  // D0X7Y7.MAIN.CLB; MODE=FG
  wire D0X7Y7_MAIN_CLB_f, D0X7Y7_MAIN_CLB_g, D0X7Y7_MAIN_CLB_m, D0X7Y7_MAIN_CLB_qx, D0X7Y7_MAIN_CLB_qy, D0X7Y7_MAIN_CLB_dx, D0X7Y7_MAIN_CLB_dy;
  (* XC3000_LOC="D0X7Y7.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1110000000100000), .INIT_G(16'b0011111100111111)) D0X7Y7_MAIN_CLB_lut (
    .A(net_0530), .F2(net_0212), .F3(net_0120), .F4(net_0531),
    .G2(net_0212), .G3(net_0120), .G4(net_0401), .E(net_0531), .F(D0X7Y7_MAIN_CLB_f), .G(D0X7Y7_MAIN_CLB_g), .M(D0X7Y7_MAIN_CLB_m));
  assign D0X7Y7_MAIN_CLB_dx = D0X7Y7_MAIN_CLB_f;
  assign D0X7Y7_MAIN_CLB_dy = D0X7Y7_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y7_MAIN_CLB_ffx (.D(D0X7Y7_MAIN_CLB_dx), .K(net_0529), .EC(net_0531), .RD(net_0120), .Q(D0X7Y7_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y7_MAIN_CLB_ffy (.D(D0X7Y7_MAIN_CLB_dy), .K(net_0529), .EC(net_0531), .RD(net_0120), .Q(D0X7Y7_MAIN_CLB_qy));
  assign net_0401 = D0X7Y7_MAIN_CLB_f;
  assign net_0534 = D0X7Y7_MAIN_CLB_g;

  (* XC3000_LOC="D0X7Y7.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X7Y7_MAIN_TBUF_0_ (.I(net_0008), .T(net_0533), .O(net_0105));

  (* XC3000_LOC="D0X7Y7.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X7Y7_MAIN_TBUF_1_ (.I(net_0532), .T(net_0533), .O(net_0099));

  // D0X7Y8.MAIN.CLB; MODE=FG
  wire D0X7Y8_MAIN_CLB_f, D0X7Y8_MAIN_CLB_g, D0X7Y8_MAIN_CLB_m, D0X7Y8_MAIN_CLB_qx, D0X7Y8_MAIN_CLB_qy, D0X7Y8_MAIN_CLB_dx, D0X7Y8_MAIN_CLB_dy;
  (* XC3000_LOC="D0X7Y8.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000111000000010), .INIT_G(16'b1100111111001111)) D0X7Y8_MAIN_CLB_lut (
    .A(net_0535), .F2(net_0212), .F3(net_0120), .F4(net_0536),
    .G2(net_0212), .G3(net_0120), .G4(net_0008), .E(net_0008), .F(D0X7Y8_MAIN_CLB_f), .G(D0X7Y8_MAIN_CLB_g), .M(D0X7Y8_MAIN_CLB_m));
  assign D0X7Y8_MAIN_CLB_dx = D0X7Y8_MAIN_CLB_f;
  assign D0X7Y8_MAIN_CLB_dy = D0X7Y8_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y8_MAIN_CLB_ffx (.D(D0X7Y8_MAIN_CLB_dx), .K(net_0340), .EC(net_0535), .RD(net_0530), .Q(D0X7Y8_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X7Y8_MAIN_CLB_ffy (.D(D0X7Y8_MAIN_CLB_dy), .K(net_0340), .EC(net_0535), .RD(net_0530), .Q(D0X7Y8_MAIN_CLB_qy));
  assign net_0402 = D0X7Y8_MAIN_CLB_f;
  assign net_0057 = D0X7Y8_MAIN_CLB_g;

  (* XC3000_LOC="D0X7Y8.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X7Y8_MAIN_TBUF_0_ (.I(net_0531), .T(net_0537), .O(net_0110));

  (* XC3000_LOC="D0X7Y8.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X7Y8_MAIN_TBUF_1_ (.I(net_0536), .T(net_0537), .O(net_0115));

  // D0X7Y9.MAIN.CLB; MODE=FG
  wire D0X7Y9_MAIN_CLB_f, D0X7Y9_MAIN_CLB_g, D0X7Y9_MAIN_CLB_m, D0X7Y9_MAIN_CLB_qx, D0X7Y9_MAIN_CLB_qy, D0X7Y9_MAIN_CLB_dx, D0X7Y9_MAIN_CLB_dy;
  (* XC3000_LOC="D0X7Y9.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110000001111), .INIT_G(16'b1111110010101010)) D0X7Y9_MAIN_CLB_lut (
    .A(net_0123), .F2(D0X7Y9_MAIN_CLB_qx), .F3(net_0004), .F4(net_0008),
    .G2(D0X7Y9_MAIN_CLB_qy), .G3(D0X7Y9_MAIN_CLB_qy), .G4(net_0008), .E(net_0008), .F(D0X7Y9_MAIN_CLB_f), .G(D0X7Y9_MAIN_CLB_g), .M(D0X7Y9_MAIN_CLB_m));
  assign D0X7Y9_MAIN_CLB_dx = D0X7Y9_MAIN_CLB_f;
  assign D0X7Y9_MAIN_CLB_dy = D0X7Y9_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X7Y9_MAIN_CLB_ffx (.D(D0X7Y9_MAIN_CLB_dx), .K(net_0002), .EC(net_0340), .RD(net_0120), .Q(D0X7Y9_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X7Y9_MAIN_CLB_ffy (.D(D0X7Y9_MAIN_CLB_dy), .K(net_0002), .EC(net_0340), .RD(net_0120), .Q(D0X7Y9_MAIN_CLB_qy));
  assign net_0540 = D0X7Y9_MAIN_CLB_qx;
  assign net_0539 = D0X7Y9_MAIN_CLB_qy;

  (* XC3000_LOC="D0X7Y9.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X7Y9_MAIN_TBUF_0_ (.I(net_0535), .T(net_0541), .O(net_0016));

  (* XC3000_LOC="D0X7Y9.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X7Y9_MAIN_TBUF_1_ (.I(net_0460), .T(net_0541), .O(net_0123));

  // D0X7Y9.MAIN.IO_N[0]
  assign net_0535 = pin_P80;
  wire D0X7Y9_MAIN_IO_N_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X7Y9_MAIN_IO_N_0__input (.PAD_IN(pin_P80), .IK(net_0002), .Q(D0X7Y9_MAIN_IO_N_0__input_q));
  assign net_0542 = D0X7Y9_MAIN_IO_N_0__input_q;
  wire D0X7Y9_MAIN_IO_N_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X7Y9_MAIN_IO_N_0__output_ff (.D(net_0471), .OK(net_0015), .Q(D0X7Y9_MAIN_IO_N_0__output_q));
  (* XC3000_LOC="D0X7Y9.MAIN", XC3000_BEL="IO_N[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X7Y9_MAIN_IO_N_0__driver (.O(net_0471), .T(~(net_0472)), .PAD(pin_P80));

  // D0X7Y9.MAIN.IO_N[1]
  assign net_0536 = pin_P79;
  wire D0X7Y9_MAIN_IO_N_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X7Y9_MAIN_IO_N_1__input (.PAD_IN(pin_P79), .IK(net_0002), .Q(D0X7Y9_MAIN_IO_N_1__input_q));
  assign net_0543 = D0X7Y9_MAIN_IO_N_1__input_q;
  wire D0X7Y9_MAIN_IO_N_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X7Y9_MAIN_IO_N_1__output_ff (.D(net_0539), .OK(net_0015), .Q(D0X7Y9_MAIN_IO_N_1__output_q));
  (* XC3000_LOC="D0X7Y9.MAIN", XC3000_BEL="IO_N[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X7Y9_MAIN_IO_N_1__driver (.O(net_0539), .T(~(net_0540)), .PAD(pin_P79));

  // D0X8Y0.MAIN.CLB; MODE=FG
  wire D0X8Y0_MAIN_CLB_f, D0X8Y0_MAIN_CLB_g, D0X8Y0_MAIN_CLB_m, D0X8Y0_MAIN_CLB_qx, D0X8Y0_MAIN_CLB_qy, D0X8Y0_MAIN_CLB_dx, D0X8Y0_MAIN_CLB_dy;
  (* XC3000_LOC="D0X8Y0.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110010101010)) D0X8Y0_MAIN_CLB_lut (
    .A(net_0003), .F2(net_0004), .F3(D0X8Y0_MAIN_CLB_qx), .F4(net_0016),
    .G2(D0X8Y0_MAIN_CLB_qy), .G3(net_0017), .G4(net_0016), .E(net_0545), .F(D0X8Y0_MAIN_CLB_f), .G(D0X8Y0_MAIN_CLB_g), .M(D0X8Y0_MAIN_CLB_m));
  assign D0X8Y0_MAIN_CLB_dx = D0X8Y0_MAIN_CLB_f;
  assign D0X8Y0_MAIN_CLB_dy = D0X8Y0_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y0_MAIN_CLB_ffx (.D(D0X8Y0_MAIN_CLB_dx), .K(net_0002), .EC(net_0325), .RD(net_0017), .Q(D0X8Y0_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y0_MAIN_CLB_ffy (.D(D0X8Y0_MAIN_CLB_dy), .K(net_0002), .EC(net_0325), .RD(net_0017), .Q(D0X8Y0_MAIN_CLB_qy));
  assign net_0547 = D0X8Y0_MAIN_CLB_qx;
  assign net_0546 = D0X8Y0_MAIN_CLB_qy;

  (* XC3000_LOC="D0X8Y0.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X8Y0_MAIN_TBUF_0_ (.I(net_0544), .T(net_0548), .O(net_0016));

  (* XC3000_LOC="D0X8Y0.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X8Y0_MAIN_TBUF_1_ (.I(net_0017), .T(net_0549), .O(net_0003));

  // D0X8Y0.MAIN.IO_S[0]
  assign net_0544 = pin_P50;
  wire D0X8Y0_MAIN_IO_S_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X8Y0_MAIN_IO_S_0__input (.PAD_IN(pin_P50), .IK(net_0000), .Q(D0X8Y0_MAIN_IO_S_0__input_q));
  assign net_0551 = D0X8Y0_MAIN_IO_S_0__input_q;
  wire D0X8Y0_MAIN_IO_S_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X8Y0_MAIN_IO_S_0__output_ff (.D(net_0480), .OK(net_0002), .Q(D0X8Y0_MAIN_IO_S_0__output_q));
  (* XC3000_LOC="D0X8Y0.MAIN", XC3000_BEL="IO_S[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X8Y0_MAIN_IO_S_0__driver (.O(net_0480), .T(~(net_0477)), .PAD(pin_P50));

  // D0X8Y0.MAIN.IO_S[1]
  assign net_0550 = pin_P51;
  wire D0X8Y0_MAIN_IO_S_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X8Y0_MAIN_IO_S_1__input (.PAD_IN(pin_P51), .IK(net_0000), .Q(D0X8Y0_MAIN_IO_S_1__input_q));
  assign net_0552 = D0X8Y0_MAIN_IO_S_1__input_q;
  wire D0X8Y0_MAIN_IO_S_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X8Y0_MAIN_IO_S_1__output_ff (.D(net_0546), .OK(net_0002), .Q(D0X8Y0_MAIN_IO_S_1__output_q));
  (* XC3000_LOC="D0X8Y0.MAIN", XC3000_BEL="IO_S[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X8Y0_MAIN_IO_S_1__driver (.O(net_0546), .T(~(net_0547)), .PAD(pin_P51));

  // D0X8Y1.MAIN.CLB; MODE=FGM
  wire D0X8Y1_MAIN_CLB_f, D0X8Y1_MAIN_CLB_g, D0X8Y1_MAIN_CLB_m, D0X8Y1_MAIN_CLB_qx, D0X8Y1_MAIN_CLB_qy, D0X8Y1_MAIN_CLB_dx, D0X8Y1_MAIN_CLB_dy;
  (* XC3000_LOC="D0X8Y1.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1010101000000000), .INIT_G(16'b1111111101010101)) D0X8Y1_MAIN_CLB_lut (
    .A(net_0034), .F2(net_0553), .F3(net_0212), .F4(net_0550),
    .G2(net_0553), .G3(net_0212), .G4(net_0550), .E(net_0544), .F(D0X8Y1_MAIN_CLB_f), .G(D0X8Y1_MAIN_CLB_g), .M(D0X8Y1_MAIN_CLB_m));
  assign D0X8Y1_MAIN_CLB_dx = D0X8Y1_MAIN_CLB_m;
  assign D0X8Y1_MAIN_CLB_dy = D0X8Y1_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X8Y1_MAIN_CLB_ffx (.D(D0X8Y1_MAIN_CLB_dx), .K(net_0499), .EC(net_0554), .RD(net_0057), .Q(D0X8Y1_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X8Y1_MAIN_CLB_ffy (.D(D0X8Y1_MAIN_CLB_dy), .K(net_0499), .EC(net_0554), .RD(net_0057), .Q(D0X8Y1_MAIN_CLB_qy));
  assign net_0485 = D0X8Y1_MAIN_CLB_m;
  assign net_0557 = D0X8Y1_MAIN_CLB_qy;

  (* XC3000_LOC="D0X8Y1.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X8Y1_MAIN_TBUF_0_ (.I(net_0499), .T(net_0555), .O(net_0027));

  (* XC3000_LOC="D0X8Y1.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X8Y1_MAIN_TBUF_1_ (.I(net_0212), .T(net_0556), .O(net_0034));

  // D0X8Y2.MAIN.CLB; MODE=FGM
  wire D0X8Y2_MAIN_CLB_f, D0X8Y2_MAIN_CLB_g, D0X8Y2_MAIN_CLB_m, D0X8Y2_MAIN_CLB_qx, D0X8Y2_MAIN_CLB_qy, D0X8Y2_MAIN_CLB_dx, D0X8Y2_MAIN_CLB_dy;
  (* XC3000_LOC="D0X8Y2.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000011110000), .INIT_G(16'b1111111111110000)) D0X8Y2_MAIN_CLB_lut (
    .A(net_0499), .F2(net_0496), .F3(net_0559), .F4(net_0042),
    .G2(net_0496), .G3(net_0559), .G4(net_0042), .E(net_0558), .F(D0X8Y2_MAIN_CLB_f), .G(D0X8Y2_MAIN_CLB_g), .M(D0X8Y2_MAIN_CLB_m));
  assign D0X8Y2_MAIN_CLB_dx = D0X8Y2_MAIN_CLB_m;
  assign D0X8Y2_MAIN_CLB_dy = D0X8Y2_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X8Y2_MAIN_CLB_ffx (.D(D0X8Y2_MAIN_CLB_dx), .K(net_0366), .EC(net_0499), .RD(net_0212), .Q(D0X8Y2_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X8Y2_MAIN_CLB_ffy (.D(D0X8Y2_MAIN_CLB_dy), .K(net_0366), .EC(net_0499), .RD(net_0212), .Q(D0X8Y2_MAIN_CLB_qy));
  assign net_0563 = D0X8Y2_MAIN_CLB_qx;
  assign net_0496 = D0X8Y2_MAIN_CLB_m;

  (* XC3000_LOC="D0X8Y2.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X8Y2_MAIN_TBUF_0_ (.I(net_0212), .T(net_0561), .O(net_0042));

  (* XC3000_LOC="D0X8Y2.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X8Y2_MAIN_TBUF_1_ (.I(net_0560), .T(net_0562), .O(net_0047));

  // D0X8Y3.MAIN.CLB; MODE=FG
  wire D0X8Y3_MAIN_CLB_f, D0X8Y3_MAIN_CLB_g, D0X8Y3_MAIN_CLB_m, D0X8Y3_MAIN_CLB_qx, D0X8Y3_MAIN_CLB_qy, D0X8Y3_MAIN_CLB_dx, D0X8Y3_MAIN_CLB_dy;
  (* XC3000_LOC="D0X8Y3.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X8Y3_MAIN_CLB_lut (
    .A(net_0499), .F2(D0X8Y3_MAIN_CLB_qx), .F3(net_0017), .F4(net_0057),
    .G2(net_0004), .G3(D0X8Y3_MAIN_CLB_qy), .G4(net_0057), .E(net_0566), .F(D0X8Y3_MAIN_CLB_f), .G(D0X8Y3_MAIN_CLB_g), .M(D0X8Y3_MAIN_CLB_m));
  assign D0X8Y3_MAIN_CLB_dx = D0X8Y3_MAIN_CLB_f;
  assign D0X8Y3_MAIN_CLB_dy = D0X8Y3_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y3_MAIN_CLB_ffx (.D(D0X8Y3_MAIN_CLB_dx), .K(net_0002), .EC(net_0452), .RD(net_0564), .Q(D0X8Y3_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y3_MAIN_CLB_ffy (.D(D0X8Y3_MAIN_CLB_dy), .K(net_0002), .EC(net_0452), .RD(net_0564), .Q(D0X8Y3_MAIN_CLB_qy));
  assign net_0569 = D0X8Y3_MAIN_CLB_qx;
  assign net_0570 = D0X8Y3_MAIN_CLB_qy;

  (* XC3000_LOC="D0X8Y3.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X8Y3_MAIN_TBUF_0_ (.I(net_0565), .T(net_0568), .O(net_0057));

  (* XC3000_LOC="D0X8Y3.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X8Y3_MAIN_TBUF_1_ (.I(net_0567), .T(net_0568), .O(net_0060));

  // D0X8Y4.MAIN.CLB; MODE=FG
  wire D0X8Y4_MAIN_CLB_f, D0X8Y4_MAIN_CLB_g, D0X8Y4_MAIN_CLB_m, D0X8Y4_MAIN_CLB_qx, D0X8Y4_MAIN_CLB_qy, D0X8Y4_MAIN_CLB_dx, D0X8Y4_MAIN_CLB_dy;
  (* XC3000_LOC="D0X8Y4.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X8Y4_MAIN_CLB_lut (
    .A(net_0534), .F2(D0X8Y4_MAIN_CLB_qx), .F3(net_0017), .F4(net_0534),
    .G2(net_0004), .G3(D0X8Y4_MAIN_CLB_qy), .G4(net_0534), .E(net_0534), .F(D0X8Y4_MAIN_CLB_f), .G(D0X8Y4_MAIN_CLB_g), .M(D0X8Y4_MAIN_CLB_m));
  assign D0X8Y4_MAIN_CLB_dx = D0X8Y4_MAIN_CLB_f;
  assign D0X8Y4_MAIN_CLB_dy = D0X8Y4_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y4_MAIN_CLB_ffx (.D(D0X8Y4_MAIN_CLB_dx), .K(net_0002), .EC(net_0452), .RD(net_0571), .Q(D0X8Y4_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y4_MAIN_CLB_ffy (.D(D0X8Y4_MAIN_CLB_dy), .K(net_0002), .EC(net_0452), .RD(net_0571), .Q(D0X8Y4_MAIN_CLB_qy));
  assign net_0575 = D0X8Y4_MAIN_CLB_qx;
  assign net_0576 = D0X8Y4_MAIN_CLB_qy;

  (* XC3000_LOC="D0X8Y4.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X8Y4_MAIN_TBUF_0_ (.I(net_0572), .T(net_0574), .O(net_0071));

  (* XC3000_LOC="D0X8Y4.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X8Y4_MAIN_TBUF_1_ (.I(net_0573), .T(net_0574), .O(net_0000));

  // D0X8Y5.MAIN.CLB; MODE=FGM
  wire D0X8Y5_MAIN_CLB_f, D0X8Y5_MAIN_CLB_g, D0X8Y5_MAIN_CLB_m, D0X8Y5_MAIN_CLB_qx, D0X8Y5_MAIN_CLB_qy, D0X8Y5_MAIN_CLB_dx, D0X8Y5_MAIN_CLB_dy;
  (* XC3000_LOC="D0X8Y5.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110010101010), .INIT_G(16'b1100110011110000)) D0X8Y5_MAIN_CLB_lut (
    .A(net_0527), .F2(net_0520), .F3(net_0577), .F4(net_0007),
    .G2(net_0520), .G3(net_0577), .G4(net_0007), .E(net_0120), .F(D0X8Y5_MAIN_CLB_f), .G(D0X8Y5_MAIN_CLB_g), .M(D0X8Y5_MAIN_CLB_m));
  assign D0X8Y5_MAIN_CLB_dx = D0X8Y5_MAIN_CLB_m;
  assign D0X8Y5_MAIN_CLB_dy = D0X8Y5_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X8Y5_MAIN_CLB_ffx (.D(D0X8Y5_MAIN_CLB_dx), .K(net_0452), .EC(net_0534), .RD(net_0513), .Q(D0X8Y5_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X8Y5_MAIN_CLB_ffy (.D(D0X8Y5_MAIN_CLB_dy), .K(net_0452), .EC(net_0534), .RD(net_0513), .Q(D0X8Y5_MAIN_CLB_qy));
  assign net_0580 = D0X8Y5_MAIN_CLB_qx;
  assign net_0454 = D0X8Y5_MAIN_CLB_m;

  (* XC3000_LOC="D0X8Y5.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X8Y5_MAIN_TBUF_0_ (.I(net_0578), .T(net_0579), .O(net_0078));

  (* XC3000_LOC="D0X8Y5.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X8Y5_MAIN_TBUF_1_ (.I(net_0007), .T(net_0579), .O(net_0082));

  // D0X8Y6.MAIN.CLB; MODE=FG
  wire D0X8Y6_MAIN_CLB_f, D0X8Y6_MAIN_CLB_g, D0X8Y6_MAIN_CLB_m, D0X8Y6_MAIN_CLB_qx, D0X8Y6_MAIN_CLB_qy, D0X8Y6_MAIN_CLB_dx, D0X8Y6_MAIN_CLB_dy;
  (* XC3000_LOC="D0X8Y6.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1011000110110001)) D0X8Y6_MAIN_CLB_lut (
    .A(net_0089), .F2(D0X8Y6_MAIN_CLB_qx), .F3(net_0017), .F4(net_0454),
    .G2(net_0004), .G3(D0X8Y6_MAIN_CLB_qy), .G4(net_0454), .E(net_0581), .F(D0X8Y6_MAIN_CLB_f), .G(D0X8Y6_MAIN_CLB_g), .M(D0X8Y6_MAIN_CLB_m));
  assign D0X8Y6_MAIN_CLB_dx = D0X8Y6_MAIN_CLB_f;
  assign D0X8Y6_MAIN_CLB_dy = D0X8Y6_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y6_MAIN_CLB_ffx (.D(D0X8Y6_MAIN_CLB_dx), .K(net_0002), .EC(net_0453), .RD(net_0515), .Q(D0X8Y6_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y6_MAIN_CLB_ffy (.D(D0X8Y6_MAIN_CLB_dy), .K(net_0002), .EC(net_0453), .RD(net_0515), .Q(D0X8Y6_MAIN_CLB_qy));
  assign net_0584 = D0X8Y6_MAIN_CLB_qx;
  assign net_0585 = D0X8Y6_MAIN_CLB_qy;

  (* XC3000_LOC="D0X8Y6.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X8Y6_MAIN_TBUF_0_ (.I(net_0120), .T(net_0582), .O(net_0094));

  (* XC3000_LOC="D0X8Y6.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X8Y6_MAIN_TBUF_1_ (.I(net_0534), .T(net_0583), .O(net_0089));

  // D0X8Y7.MAIN.CLB; MODE=FGM
  wire D0X8Y7_MAIN_CLB_f, D0X8Y7_MAIN_CLB_g, D0X8Y7_MAIN_CLB_m, D0X8Y7_MAIN_CLB_qx, D0X8Y7_MAIN_CLB_qy, D0X8Y7_MAIN_CLB_dx, D0X8Y7_MAIN_CLB_dy;
  (* XC3000_LOC="D0X8Y7.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000011001100), .INIT_G(16'b1111000011001100)) D0X8Y7_MAIN_CLB_lut (
    .A(net_0534), .F2(net_0588), .F3(net_0586), .F4(net_0105),
    .G2(net_0588), .G3(net_0586), .G4(net_0105), .E(net_0534), .F(D0X8Y7_MAIN_CLB_f), .G(D0X8Y7_MAIN_CLB_g), .M(D0X8Y7_MAIN_CLB_m));
  assign D0X8Y7_MAIN_CLB_dx = D0X8Y7_MAIN_CLB_m;
  assign D0X8Y7_MAIN_CLB_dy = D0X8Y7_MAIN_CLB_m;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X8Y7_MAIN_CLB_ffx (.D(D0X8Y7_MAIN_CLB_dx), .K(net_0587), .EC(net_0534), .RD(net_0586), .Q(D0X8Y7_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X8Y7_MAIN_CLB_ffy (.D(D0X8Y7_MAIN_CLB_dy), .K(net_0587), .EC(net_0534), .RD(net_0586), .Q(D0X8Y7_MAIN_CLB_qy));
  assign net_0592 = D0X8Y7_MAIN_CLB_qx;
  assign net_0577 = D0X8Y7_MAIN_CLB_m;

  (* XC3000_LOC="D0X8Y7.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X8Y7_MAIN_TBUF_0_ (.I(net_0589), .T(net_0591), .O(net_0105));

  (* XC3000_LOC="D0X8Y7.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X8Y7_MAIN_TBUF_1_ (.I(net_0590), .T(net_0591), .O(net_0099));

  // D0X8Y8.MAIN.CLB; MODE=FG
  wire D0X8Y8_MAIN_CLB_f, D0X8Y8_MAIN_CLB_g, D0X8Y8_MAIN_CLB_m, D0X8Y8_MAIN_CLB_qx, D0X8Y8_MAIN_CLB_qy, D0X8Y8_MAIN_CLB_dx, D0X8Y8_MAIN_CLB_dy;
  (* XC3000_LOC="D0X8Y8.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110011110000)) D0X8Y8_MAIN_CLB_lut (
    .A(net_0340), .F2(net_0004), .F3(D0X8Y8_MAIN_CLB_qx), .F4(net_0110),
    .G2(D0X8Y8_MAIN_CLB_qy), .G3(net_0017), .G4(net_0110), .E(net_0534), .F(D0X8Y8_MAIN_CLB_f), .G(D0X8Y8_MAIN_CLB_g), .M(D0X8Y8_MAIN_CLB_m));
  assign D0X8Y8_MAIN_CLB_dx = D0X8Y8_MAIN_CLB_f;
  assign D0X8Y8_MAIN_CLB_dy = D0X8Y8_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y8_MAIN_CLB_ffx (.D(D0X8Y8_MAIN_CLB_dx), .K(net_0002), .EC(net_0340), .RD(net_0593), .Q(D0X8Y8_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y8_MAIN_CLB_ffy (.D(D0X8Y8_MAIN_CLB_dy), .K(net_0002), .EC(net_0340), .RD(net_0593), .Q(D0X8Y8_MAIN_CLB_qy));
  assign net_0597 = D0X8Y8_MAIN_CLB_qx;
  assign net_0598 = D0X8Y8_MAIN_CLB_qy;

  (* XC3000_LOC="D0X8Y8.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X8Y8_MAIN_TBUF_0_ (.I(net_0057), .T(net_0595), .O(net_0110));

  (* XC3000_LOC="D0X8Y8.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X8Y8_MAIN_TBUF_1_ (.I(net_0594), .T(net_0596), .O(net_0115));

  // D0X8Y9.MAIN.CLB; MODE=FG
  wire D0X8Y9_MAIN_CLB_f, D0X8Y9_MAIN_CLB_g, D0X8Y9_MAIN_CLB_m, D0X8Y9_MAIN_CLB_qx, D0X8Y9_MAIN_CLB_qy, D0X8Y9_MAIN_CLB_dx, D0X8Y9_MAIN_CLB_dy;
  (* XC3000_LOC="D0X8Y9.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110010101010)) D0X8Y9_MAIN_CLB_lut (
    .A(net_0123), .F2(net_0004), .F3(D0X8Y9_MAIN_CLB_qx), .F4(net_0016),
    .G2(D0X8Y9_MAIN_CLB_qy), .G3(net_0536), .G4(net_0016), .E(net_0004), .F(D0X8Y9_MAIN_CLB_f), .G(D0X8Y9_MAIN_CLB_g), .M(D0X8Y9_MAIN_CLB_m));
  assign D0X8Y9_MAIN_CLB_dx = D0X8Y9_MAIN_CLB_f;
  assign D0X8Y9_MAIN_CLB_dy = D0X8Y9_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y9_MAIN_CLB_ffx (.D(D0X8Y9_MAIN_CLB_dx), .K(net_0002), .EC(net_0340), .RD(net_0004), .Q(D0X8Y9_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X8Y9_MAIN_CLB_ffy (.D(D0X8Y9_MAIN_CLB_dy), .K(net_0002), .EC(net_0340), .RD(net_0004), .Q(D0X8Y9_MAIN_CLB_qy));
  assign net_0601 = D0X8Y9_MAIN_CLB_qx;
  assign net_0599 = D0X8Y9_MAIN_CLB_qy;

  (* XC3000_LOC="D0X8Y9.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X8Y9_MAIN_TBUF_0_ (.I(net_0534), .T(net_0602), .O(net_0016));

  (* XC3000_LOC="D0X8Y9.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X8Y9_MAIN_TBUF_1_ (.I(net_0536), .T(net_0600), .O(net_0123));

  // D0X8Y9.MAIN.IO_N[0]
  assign net_0004 = pin_P78;
  wire D0X8Y9_MAIN_IO_N_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X8Y9_MAIN_IO_N_0__input (.PAD_IN(pin_P78), .IK(net_0002), .Q(D0X8Y9_MAIN_IO_N_0__input_q));
  assign net_0603 = D0X8Y9_MAIN_IO_N_0__input_q;
  wire D0X8Y9_MAIN_IO_N_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X8Y9_MAIN_IO_N_0__output_ff (.D(net_0540), .OK(net_0015), .Q(D0X8Y9_MAIN_IO_N_0__output_q));
  (* XC3000_LOC="D0X8Y9.MAIN", XC3000_BEL="IO_N[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X8Y9_MAIN_IO_N_0__driver (.O(net_0540), .T(net_0600), .PAD(pin_P78));

  // D0X8Y9.MAIN.IO_N[1]
  assign net_0531 = pin_P77;
  wire D0X8Y9_MAIN_IO_N_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X8Y9_MAIN_IO_N_1__input (.PAD_IN(pin_P77), .IK(net_0002), .Q(D0X8Y9_MAIN_IO_N_1__input_q));
  assign net_0604 = D0X8Y9_MAIN_IO_N_1__input_q;
  wire D0X8Y9_MAIN_IO_N_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X8Y9_MAIN_IO_N_1__output_ff (.D(net_0599), .OK(net_0015), .Q(D0X8Y9_MAIN_IO_N_1__output_q));
  (* XC3000_LOC="D0X8Y9.MAIN", XC3000_BEL="IO_N[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X8Y9_MAIN_IO_N_1__driver (.O(net_0599), .T(~(net_0601)), .PAD(pin_P77));

  // D0X9Y0.MAIN.CLB; MODE=FG
  wire D0X9Y0_MAIN_CLB_f, D0X9Y0_MAIN_CLB_g, D0X9Y0_MAIN_CLB_m, D0X9Y0_MAIN_CLB_qx, D0X9Y0_MAIN_CLB_qy, D0X9Y0_MAIN_CLB_dx, D0X9Y0_MAIN_CLB_dy;
  (* XC3000_LOC="D0X9Y0.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110010101010), .INIT_G(16'b1111000000110011)) D0X9Y0_MAIN_CLB_lut (
    .A(net_0003), .F2(D0X9Y0_MAIN_CLB_qx), .F3(net_0550), .F4(net_0016),
    .G2(net_0004), .G3(D0X9Y0_MAIN_CLB_qy), .G4(net_0016), .E(net_0606), .F(D0X9Y0_MAIN_CLB_f), .G(D0X9Y0_MAIN_CLB_g), .M(D0X9Y0_MAIN_CLB_m));
  assign D0X9Y0_MAIN_CLB_dx = D0X9Y0_MAIN_CLB_f;
  assign D0X9Y0_MAIN_CLB_dy = D0X9Y0_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y0_MAIN_CLB_ffx (.D(D0X9Y0_MAIN_CLB_dx), .K(net_0002), .EC(net_0437), .RD(net_0607), .Q(D0X9Y0_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y0_MAIN_CLB_ffy (.D(D0X9Y0_MAIN_CLB_dy), .K(net_0002), .EC(net_0437), .RD(net_0607), .Q(D0X9Y0_MAIN_CLB_qy));
  assign net_0609 = D0X9Y0_MAIN_CLB_qx;
  assign net_0611 = D0X9Y0_MAIN_CLB_qy;

  (* XC3000_LOC="D0X9Y0.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X9Y0_MAIN_TBUF_0_ (.I(net_0605), .T(net_0610), .O(net_0016));

  (* XC3000_LOC="D0X9Y0.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X9Y0_MAIN_TBUF_1_ (.I(net_0550), .T(net_0610), .O(net_0003));

  (* XC3000_LOC="D0X9Y0.MAIN", XC3000_BEL="TBUF_E[0]" *)
  XC3000_TBUF D0X9Y0_MAIN_TBUF_E_0_ (.I(net_0615), .T(net_0610), .O(net_0016));

  (* XC3000_LOC="D0X9Y0.MAIN", XC3000_BEL="TBUF_E[1]" *)
  XC3000_TBUF D0X9Y0_MAIN_TBUF_E_1_ (.I(net_0558), .T(net_0610), .O(net_0003));

  // D0X9Y0.MAIN.IO_E[0]
  assign net_0617 = pin_P57;
  wire D0X9Y0_MAIN_IO_E_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y0_MAIN_IO_E_0__input (.PAD_IN(pin_P57), .IK(net_0002), .Q(D0X9Y0_MAIN_IO_E_0__input_q));
  assign net_0618 = D0X9Y0_MAIN_IO_E_0__input_q;
  wire D0X9Y0_MAIN_IO_E_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y0_MAIN_IO_E_0__output_ff (.D(net_0608), .OK(net_0000), .Q(D0X9Y0_MAIN_IO_E_0__output_q));
  (* XC3000_LOC="D0X9Y0.MAIN", XC3000_BEL="IO_E[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y0_MAIN_IO_E_0__driver (.O(net_0608), .T(net_0610), .PAD(pin_P57));

  // D0X9Y0.MAIN.IO_E[1]
  assign net_0558 = pin_P56;
  wire D0X9Y0_MAIN_IO_E_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y0_MAIN_IO_E_1__input (.PAD_IN(pin_P56), .IK(net_0002), .Q(D0X9Y0_MAIN_IO_E_1__input_q));
  assign net_0619 = D0X9Y0_MAIN_IO_E_1__input_q;
  wire D0X9Y0_MAIN_IO_E_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y0_MAIN_IO_E_1__output_ff (.D(net_0609), .OK(net_0000), .Q(D0X9Y0_MAIN_IO_E_1__output_q));
  (* XC3000_LOC="D0X9Y0.MAIN", XC3000_BEL="IO_E[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y0_MAIN_IO_E_1__driver (.O(net_0609), .T(~(net_0611)), .PAD(pin_P56));

  // D0X9Y0.MAIN.IO_S[0]
  assign net_0620 = pin_P52;
  wire D0X9Y0_MAIN_IO_S_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y0_MAIN_IO_S_0__input (.PAD_IN(pin_P52), .IK(net_0000), .Q(D0X9Y0_MAIN_IO_S_0__input_q));
  assign net_0622 = D0X9Y0_MAIN_IO_S_0__input_q;
  wire D0X9Y0_MAIN_IO_S_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y0_MAIN_IO_S_0__output_ff (.D(net_0612), .OK(net_0002), .Q(D0X9Y0_MAIN_IO_S_0__output_q));
  pullup(pin_P52);
  (* XC3000_LOC="D0X9Y0.MAIN", XC3000_BEL="IO_S[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y0_MAIN_IO_S_0__driver (.O(net_0612), .T(1'b1), .PAD(pin_P52));

  // D0X9Y0.MAIN.IO_S[1]
  assign net_0621 = pin_P53;
  wire D0X9Y0_MAIN_IO_S_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y0_MAIN_IO_S_1__input (.PAD_IN(pin_P53), .IK(net_0000), .Q(D0X9Y0_MAIN_IO_S_1__input_q));
  assign net_0623 = D0X9Y0_MAIN_IO_S_1__input_q;
  wire D0X9Y0_MAIN_IO_S_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y0_MAIN_IO_S_1__output_ff (.D(net_0613), .OK(net_0002), .Q(D0X9Y0_MAIN_IO_S_1__output_q));
  pullup(pin_P53);
  (* XC3000_LOC="D0X9Y0.MAIN", XC3000_BEL="IO_S[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y0_MAIN_IO_S_1__driver (.O(net_0613), .T(1'b1), .PAD(pin_P53));
  assign net_0616 = stimulus_D0X9Y0_MAIN_CLKIOB;
  assign net_0624 = stimulus_D0X9Y0_MAIN_OSC;

  // D0X9Y1.MAIN.CLB; MODE=FG
  wire D0X9Y1_MAIN_CLB_f, D0X9Y1_MAIN_CLB_g, D0X9Y1_MAIN_CLB_m, D0X9Y1_MAIN_CLB_qx, D0X9Y1_MAIN_CLB_qy, D0X9Y1_MAIN_CLB_dx, D0X9Y1_MAIN_CLB_dy;
  (* XC3000_LOC="D0X9Y1.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X9Y1_MAIN_CLB_lut (
    .A(net_0437), .F2(D0X9Y1_MAIN_CLB_qx), .F3(net_0017), .F4(net_0027),
    .G2(net_0004), .G3(D0X9Y1_MAIN_CLB_qy), .G4(net_0027), .E(net_0626), .F(D0X9Y1_MAIN_CLB_f), .G(D0X9Y1_MAIN_CLB_g), .M(D0X9Y1_MAIN_CLB_m));
  assign D0X9Y1_MAIN_CLB_dx = D0X9Y1_MAIN_CLB_f;
  assign D0X9Y1_MAIN_CLB_dy = D0X9Y1_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y1_MAIN_CLB_ffx (.D(D0X9Y1_MAIN_CLB_dx), .K(net_0002), .EC(net_0437), .RD(net_0625), .Q(D0X9Y1_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y1_MAIN_CLB_ffy (.D(D0X9Y1_MAIN_CLB_dy), .K(net_0002), .EC(net_0437), .RD(net_0625), .Q(D0X9Y1_MAIN_CLB_qy));
  assign net_0628 = D0X9Y1_MAIN_CLB_qx;
  assign net_0630 = D0X9Y1_MAIN_CLB_qy;

  (* XC3000_LOC="D0X9Y1.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X9Y1_MAIN_TBUF_0_ (.I(net_0057), .T(net_0634), .O(net_0027));

  (* XC3000_LOC="D0X9Y1.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X9Y1_MAIN_TBUF_1_ (.I(net_0631), .T(net_0634), .O(net_0034));

  (* XC3000_LOC="D0X9Y1.MAIN", XC3000_BEL="TBUF_E[0]" *)
  XC3000_TBUF D0X9Y1_MAIN_TBUF_E_0_ (.I(net_0632), .T(net_0634), .O(net_0027));

  (* XC3000_LOC="D0X9Y1.MAIN", XC3000_BEL="TBUF_E[1]" *)
  XC3000_TBUF D0X9Y1_MAIN_TBUF_E_1_ (.I(net_0633), .T(net_0634), .O(net_0034));

  tri D0X9Y1_MAIN_IO_E_0__unbonded_pad;

  // D0X9Y1.MAIN.IO_E[0]
  assign net_0635 = D0X9Y1_MAIN_IO_E_0__unbonded_pad;
  wire D0X9Y1_MAIN_IO_E_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y1_MAIN_IO_E_0__input (.PAD_IN(D0X9Y1_MAIN_IO_E_0__unbonded_pad), .IK(net_0002), .Q(D0X9Y1_MAIN_IO_E_0__input_q));
  assign net_0636 = D0X9Y1_MAIN_IO_E_0__input_q;
  wire D0X9Y1_MAIN_IO_E_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y1_MAIN_IO_E_0__output_ff (.D(net_0627), .OK(net_0000), .Q(D0X9Y1_MAIN_IO_E_0__output_q));
  pullup(D0X9Y1_MAIN_IO_E_0__unbonded_pad);
  (* XC3000_LOC="D0X9Y1.MAIN", XC3000_BEL="IO_E[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y1_MAIN_IO_E_0__driver (.O(net_0627), .T(1'b1), .PAD(D0X9Y1_MAIN_IO_E_0__unbonded_pad));

  // D0X9Y1.MAIN.IO_E[1]
  assign net_0499 = pin_P58;
  wire D0X9Y1_MAIN_IO_E_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y1_MAIN_IO_E_1__input (.PAD_IN(pin_P58), .IK(net_0002), .Q(D0X9Y1_MAIN_IO_E_1__input_q));
  assign net_0637 = D0X9Y1_MAIN_IO_E_1__input_q;
  wire D0X9Y1_MAIN_IO_E_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y1_MAIN_IO_E_1__output_ff (.D(net_0628), .OK(net_0000), .Q(D0X9Y1_MAIN_IO_E_1__output_q));
  (* XC3000_LOC="D0X9Y1.MAIN", XC3000_BEL="IO_E[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y1_MAIN_IO_E_1__driver (.O(net_0628), .T(~(net_0630)), .PAD(pin_P58));

  // D0X9Y2.MAIN.CLB; MODE=FG
  wire D0X9Y2_MAIN_CLB_f, D0X9Y2_MAIN_CLB_g, D0X9Y2_MAIN_CLB_m, D0X9Y2_MAIN_CLB_qx, D0X9Y2_MAIN_CLB_qy, D0X9Y2_MAIN_CLB_dx, D0X9Y2_MAIN_CLB_dy;
  (* XC3000_LOC="D0X9Y2.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1011000110110001)) D0X9Y2_MAIN_CLB_lut (
    .A(net_0057), .F2(D0X9Y2_MAIN_CLB_qx), .F3(net_0017), .F4(net_0558),
    .G2(net_0004), .G3(D0X9Y2_MAIN_CLB_qy), .G4(net_0558), .E(net_0639), .F(D0X9Y2_MAIN_CLB_f), .G(D0X9Y2_MAIN_CLB_g), .M(D0X9Y2_MAIN_CLB_m));
  assign D0X9Y2_MAIN_CLB_dx = D0X9Y2_MAIN_CLB_f;
  assign D0X9Y2_MAIN_CLB_dy = D0X9Y2_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y2_MAIN_CLB_ffx (.D(D0X9Y2_MAIN_CLB_dx), .K(net_0002), .EC(net_0437), .RD(net_0638), .Q(D0X9Y2_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y2_MAIN_CLB_ffy (.D(D0X9Y2_MAIN_CLB_dy), .K(net_0002), .EC(net_0437), .RD(net_0638), .Q(D0X9Y2_MAIN_CLB_qy));
  assign net_0641 = D0X9Y2_MAIN_CLB_qx;
  assign net_0643 = D0X9Y2_MAIN_CLB_qy;

  (* XC3000_LOC="D0X9Y2.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X9Y2_MAIN_TBUF_0_ (.I(net_0057), .T(net_0646), .O(net_0042));

  (* XC3000_LOC="D0X9Y2.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X9Y2_MAIN_TBUF_1_ (.I(net_0496), .T(net_0646), .O(net_0047));

  (* XC3000_LOC="D0X9Y2.MAIN", XC3000_BEL="TBUF_E[0]" *)
  XC3000_TBUF D0X9Y2_MAIN_TBUF_E_0_ (.I(net_0644), .T(net_0646), .O(net_0042));

  (* XC3000_LOC="D0X9Y2.MAIN", XC3000_BEL="TBUF_E[1]" *)
  XC3000_TBUF D0X9Y2_MAIN_TBUF_E_1_ (.I(net_0645), .T(net_0646), .O(net_0047));

  tri D0X9Y2_MAIN_IO_E_0__unbonded_pad;

  // D0X9Y2.MAIN.IO_E[0]
  assign net_0647 = D0X9Y2_MAIN_IO_E_0__unbonded_pad;
  wire D0X9Y2_MAIN_IO_E_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y2_MAIN_IO_E_0__input (.PAD_IN(D0X9Y2_MAIN_IO_E_0__unbonded_pad), .IK(net_0002), .Q(D0X9Y2_MAIN_IO_E_0__input_q));
  assign net_0648 = D0X9Y2_MAIN_IO_E_0__input_q;
  wire D0X9Y2_MAIN_IO_E_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y2_MAIN_IO_E_0__output_ff (.D(net_0640), .OK(net_0000), .Q(D0X9Y2_MAIN_IO_E_0__output_q));
  pullup(D0X9Y2_MAIN_IO_E_0__unbonded_pad);
  (* XC3000_LOC="D0X9Y2.MAIN", XC3000_BEL="IO_E[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y2_MAIN_IO_E_0__driver (.O(net_0640), .T(1'b1), .PAD(D0X9Y2_MAIN_IO_E_0__unbonded_pad));

  // D0X9Y2.MAIN.IO_E[1]
  assign net_0559 = pin_P59;
  wire D0X9Y2_MAIN_IO_E_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y2_MAIN_IO_E_1__input (.PAD_IN(pin_P59), .IK(net_0002), .Q(D0X9Y2_MAIN_IO_E_1__input_q));
  assign net_0649 = D0X9Y2_MAIN_IO_E_1__input_q;
  wire D0X9Y2_MAIN_IO_E_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y2_MAIN_IO_E_1__output_ff (.D(net_0641), .OK(net_0000), .Q(D0X9Y2_MAIN_IO_E_1__output_q));
  (* XC3000_LOC="D0X9Y2.MAIN", XC3000_BEL="IO_E[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y2_MAIN_IO_E_1__driver (.O(net_0641), .T(~(net_0643)), .PAD(pin_P59));

  // D0X9Y3.MAIN.CLB; MODE=FG
  wire D0X9Y3_MAIN_CLB_f, D0X9Y3_MAIN_CLB_g, D0X9Y3_MAIN_CLB_m, D0X9Y3_MAIN_CLB_qx, D0X9Y3_MAIN_CLB_qy, D0X9Y3_MAIN_CLB_dx, D0X9Y3_MAIN_CLB_dy;
  (* XC3000_LOC="D0X9Y3.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X9Y3_MAIN_CLB_lut (
    .A(net_0437), .F2(D0X9Y3_MAIN_CLB_qx), .F3(net_0017), .F4(net_0071),
    .G2(net_0004), .G3(D0X9Y3_MAIN_CLB_qy), .G4(net_0071), .E(net_0651), .F(D0X9Y3_MAIN_CLB_f), .G(D0X9Y3_MAIN_CLB_g), .M(D0X9Y3_MAIN_CLB_m));
  assign D0X9Y3_MAIN_CLB_dx = D0X9Y3_MAIN_CLB_f;
  assign D0X9Y3_MAIN_CLB_dy = D0X9Y3_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y3_MAIN_CLB_ffx (.D(D0X9Y3_MAIN_CLB_dx), .K(net_0002), .EC(net_0437), .RD(net_0057), .Q(D0X9Y3_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y3_MAIN_CLB_ffy (.D(D0X9Y3_MAIN_CLB_dy), .K(net_0002), .EC(net_0437), .RD(net_0057), .Q(D0X9Y3_MAIN_CLB_qy));
  assign net_0652 = D0X9Y3_MAIN_CLB_qx;
  assign net_0653 = D0X9Y3_MAIN_CLB_qy;

  (* XC3000_LOC="D0X9Y3.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X9Y3_MAIN_TBUF_0_ (.I(net_0650), .T(net_0655), .O(net_0057));

  (* XC3000_LOC="D0X9Y3.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X9Y3_MAIN_TBUF_1_ (.I(net_0071), .T(net_0655), .O(net_0060));

  (* XC3000_LOC="D0X9Y3.MAIN", XC3000_BEL="TBUF_E[0]" *)
  XC3000_TBUF D0X9Y3_MAIN_TBUF_E_0_ (.I(net_0570), .T(net_0655), .O(net_0057));

  (* XC3000_LOC="D0X9Y3.MAIN", XC3000_BEL="TBUF_E[1]" *)
  XC3000_TBUF D0X9Y3_MAIN_TBUF_E_1_ (.I(net_0654), .T(net_0655), .O(net_0060));

  // D0X9Y3.MAIN.IO_E[0]
  assign net_0506 = pin_P61;
  wire D0X9Y3_MAIN_IO_E_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y3_MAIN_IO_E_0__input (.PAD_IN(pin_P61), .IK(net_0002), .Q(D0X9Y3_MAIN_IO_E_0__input_q));
  assign net_0656 = D0X9Y3_MAIN_IO_E_0__input_q;
  wire D0X9Y3_MAIN_IO_E_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y3_MAIN_IO_E_0__output_ff (.D(net_0569), .OK(net_0000), .Q(D0X9Y3_MAIN_IO_E_0__output_q));
  (* XC3000_LOC="D0X9Y3.MAIN", XC3000_BEL="IO_E[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y3_MAIN_IO_E_0__driver (.O(net_0569), .T(~(net_0570)), .PAD(pin_P61));

  // D0X9Y3.MAIN.IO_E[1]
  assign net_0498 = pin_P60;
  wire D0X9Y3_MAIN_IO_E_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y3_MAIN_IO_E_1__input (.PAD_IN(pin_P60), .IK(net_0002), .Q(D0X9Y3_MAIN_IO_E_1__input_q));
  assign net_0657 = D0X9Y3_MAIN_IO_E_1__input_q;
  wire D0X9Y3_MAIN_IO_E_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y3_MAIN_IO_E_1__output_ff (.D(net_0652), .OK(net_0000), .Q(D0X9Y3_MAIN_IO_E_1__output_q));
  (* XC3000_LOC="D0X9Y3.MAIN", XC3000_BEL="IO_E[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y3_MAIN_IO_E_1__driver (.O(net_0652), .T(~(net_0653)), .PAD(pin_P60));

  // D0X9Y4.MAIN.CLB; MODE=FG
  wire D0X9Y4_MAIN_CLB_f, D0X9Y4_MAIN_CLB_g, D0X9Y4_MAIN_CLB_m, D0X9Y4_MAIN_CLB_qx, D0X9Y4_MAIN_CLB_qy, D0X9Y4_MAIN_CLB_dx, D0X9Y4_MAIN_CLB_dy;
  (* XC3000_LOC="D0X9Y4.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110011110000)) D0X9Y4_MAIN_CLB_lut (
    .A(net_0071), .F2(net_0004), .F3(D0X9Y4_MAIN_CLB_qx), .F4(net_0071),
    .G2(D0X9Y4_MAIN_CLB_qy), .G3(net_0017), .G4(net_0071), .E(net_0659), .F(D0X9Y4_MAIN_CLB_f), .G(D0X9Y4_MAIN_CLB_g), .M(D0X9Y4_MAIN_CLB_m));
  assign D0X9Y4_MAIN_CLB_dx = D0X9Y4_MAIN_CLB_f;
  assign D0X9Y4_MAIN_CLB_dy = D0X9Y4_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y4_MAIN_CLB_ffx (.D(D0X9Y4_MAIN_CLB_dx), .K(net_0002), .EC(net_0452), .RD(net_0506), .Q(D0X9Y4_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y4_MAIN_CLB_ffy (.D(D0X9Y4_MAIN_CLB_dy), .K(net_0002), .EC(net_0452), .RD(net_0506), .Q(D0X9Y4_MAIN_CLB_qy));
  assign net_0661 = D0X9Y4_MAIN_CLB_qx;
  assign net_0660 = D0X9Y4_MAIN_CLB_qy;

  (* XC3000_LOC="D0X9Y4.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X9Y4_MAIN_TBUF_0_ (.I(net_0658), .T(net_0665), .O(net_0071));

  (* XC3000_LOC="D0X9Y4.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X9Y4_MAIN_TBUF_1_ (.I(net_0662), .T(net_0665), .O(net_0000));

  (* XC3000_LOC="D0X9Y4.MAIN", XC3000_BEL="TBUF_E[0]" *)
  XC3000_TBUF D0X9Y4_MAIN_TBUF_E_0_ (.I(net_0663), .T(net_0665), .O(net_0071));

  (* XC3000_LOC="D0X9Y4.MAIN", XC3000_BEL="TBUF_E[1]" *)
  XC3000_TBUF D0X9Y4_MAIN_TBUF_E_1_ (.I(net_0664), .T(net_0665), .O(net_0000));

  // D0X9Y4.MAIN.IO_E[0]
  assign net_0513 = pin_P63;
  wire D0X9Y4_MAIN_IO_E_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y4_MAIN_IO_E_0__input (.PAD_IN(pin_P63), .IK(net_0002), .Q(D0X9Y4_MAIN_IO_E_0__input_q));
  assign net_0666 = D0X9Y4_MAIN_IO_E_0__input_q;
  wire D0X9Y4_MAIN_IO_E_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y4_MAIN_IO_E_0__output_ff (.D(net_0660), .OK(net_0000), .Q(D0X9Y4_MAIN_IO_E_0__output_q));
  (* XC3000_LOC="D0X9Y4.MAIN", XC3000_BEL="IO_E[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y4_MAIN_IO_E_0__driver (.O(net_0660), .T(~(net_0661)), .PAD(pin_P63));

  // D0X9Y4.MAIN.IO_E[1]
  assign net_0504 = pin_P62;
  wire D0X9Y4_MAIN_IO_E_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y4_MAIN_IO_E_1__input (.PAD_IN(pin_P62), .IK(net_0002), .Q(D0X9Y4_MAIN_IO_E_1__input_q));
  assign net_0667 = D0X9Y4_MAIN_IO_E_1__input_q;
  wire D0X9Y4_MAIN_IO_E_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y4_MAIN_IO_E_1__output_ff (.D(net_0575), .OK(net_0000), .Q(D0X9Y4_MAIN_IO_E_1__output_q));
  (* XC3000_LOC="D0X9Y4.MAIN", XC3000_BEL="IO_E[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y4_MAIN_IO_E_1__driver (.O(net_0575), .T(~(net_0576)), .PAD(pin_P62));

  // D0X9Y5.MAIN.CLB; MODE=FG
  wire D0X9Y5_MAIN_CLB_f, D0X9Y5_MAIN_CLB_g, D0X9Y5_MAIN_CLB_m, D0X9Y5_MAIN_CLB_qx, D0X9Y5_MAIN_CLB_qy, D0X9Y5_MAIN_CLB_dx, D0X9Y5_MAIN_CLB_dy;
  (* XC3000_LOC="D0X9Y5.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1011000110110001)) D0X9Y5_MAIN_CLB_lut (
    .A(net_0082), .F2(D0X9Y5_MAIN_CLB_qx), .F3(net_0017), .F4(net_0452),
    .G2(net_0004), .G3(D0X9Y5_MAIN_CLB_qy), .G4(net_0452), .E(net_0577), .F(D0X9Y5_MAIN_CLB_f), .G(D0X9Y5_MAIN_CLB_g), .M(D0X9Y5_MAIN_CLB_m));
  assign D0X9Y5_MAIN_CLB_dx = D0X9Y5_MAIN_CLB_f;
  assign D0X9Y5_MAIN_CLB_dy = D0X9Y5_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y5_MAIN_CLB_ffx (.D(D0X9Y5_MAIN_CLB_dx), .K(net_0002), .EC(net_0452), .RD(net_0513), .Q(D0X9Y5_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y5_MAIN_CLB_ffy (.D(D0X9Y5_MAIN_CLB_dy), .K(net_0002), .EC(net_0452), .RD(net_0513), .Q(D0X9Y5_MAIN_CLB_qy));
  assign net_0669 = D0X9Y5_MAIN_CLB_qx;
  assign net_0671 = D0X9Y5_MAIN_CLB_qy;

  (* XC3000_LOC="D0X9Y5.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X9Y5_MAIN_TBUF_0_ (.I(net_0071), .T(net_0673), .O(net_0078));

  (* XC3000_LOC="D0X9Y5.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X9Y5_MAIN_TBUF_1_ (.I(net_0454), .T(net_0673), .O(net_0082));

  (* XC3000_LOC="D0X9Y5.MAIN", XC3000_BEL="TBUF_E[0]" *)
  XC3000_TBUF D0X9Y5_MAIN_TBUF_E_0_ (.I(net_0672), .T(net_0673), .O(net_0078));

  (* XC3000_LOC="D0X9Y5.MAIN", XC3000_BEL="TBUF_E[1]" *)
  XC3000_TBUF D0X9Y5_MAIN_TBUF_E_1_ (.I(net_0515), .T(net_0673), .O(net_0082));

  // D0X9Y5.MAIN.IO_E[0]
  assign net_0674 = pin_P66;
  wire D0X9Y5_MAIN_IO_E_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y5_MAIN_IO_E_0__input (.PAD_IN(pin_P66), .IK(net_0002), .Q(D0X9Y5_MAIN_IO_E_0__input_q));
  assign net_0675 = D0X9Y5_MAIN_IO_E_0__input_q;
  wire D0X9Y5_MAIN_IO_E_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y5_MAIN_IO_E_0__output_ff (.D(net_0668), .OK(net_0000), .Q(D0X9Y5_MAIN_IO_E_0__output_q));
  pullup(pin_P66);
  (* XC3000_LOC="D0X9Y5.MAIN", XC3000_BEL="IO_E[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y5_MAIN_IO_E_0__driver (.O(net_0668), .T(1'b1), .PAD(pin_P66));

  // D0X9Y5.MAIN.IO_E[1]
  assign net_0515 = pin_P65;
  wire D0X9Y5_MAIN_IO_E_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y5_MAIN_IO_E_1__input (.PAD_IN(pin_P65), .IK(net_0002), .Q(D0X9Y5_MAIN_IO_E_1__input_q));
  assign net_0676 = D0X9Y5_MAIN_IO_E_1__input_q;
  wire D0X9Y5_MAIN_IO_E_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y5_MAIN_IO_E_1__output_ff (.D(net_0669), .OK(net_0000), .Q(D0X9Y5_MAIN_IO_E_1__output_q));
  (* XC3000_LOC="D0X9Y5.MAIN", XC3000_BEL="IO_E[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y5_MAIN_IO_E_1__driver (.O(net_0669), .T(~(net_0671)), .PAD(pin_P65));

  // D0X9Y6.MAIN.CLB; MODE=FG
  wire D0X9Y6_MAIN_CLB_f, D0X9Y6_MAIN_CLB_g, D0X9Y6_MAIN_CLB_m, D0X9Y6_MAIN_CLB_qx, D0X9Y6_MAIN_CLB_qy, D0X9Y6_MAIN_CLB_dx, D0X9Y6_MAIN_CLB_dy;
  (* XC3000_LOC="D0X9Y6.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1111000000110011), .INIT_G(16'b1100110011110000)) D0X9Y6_MAIN_CLB_lut (
    .A(net_0453), .F2(net_0004), .F3(D0X9Y6_MAIN_CLB_qx), .F4(net_0071),
    .G2(D0X9Y6_MAIN_CLB_qy), .G3(net_0017), .G4(net_0071), .E(net_0577), .F(D0X9Y6_MAIN_CLB_f), .G(D0X9Y6_MAIN_CLB_g), .M(D0X9Y6_MAIN_CLB_m));
  assign D0X9Y6_MAIN_CLB_dx = D0X9Y6_MAIN_CLB_f;
  assign D0X9Y6_MAIN_CLB_dy = D0X9Y6_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y6_MAIN_CLB_ffx (.D(D0X9Y6_MAIN_CLB_dx), .K(net_0002), .EC(net_0453), .RD(net_0677), .Q(D0X9Y6_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y6_MAIN_CLB_ffy (.D(D0X9Y6_MAIN_CLB_dy), .K(net_0002), .EC(net_0453), .RD(net_0677), .Q(D0X9Y6_MAIN_CLB_qy));
  assign net_0680 = D0X9Y6_MAIN_CLB_qx;
  assign net_0679 = D0X9Y6_MAIN_CLB_qy;

  (* XC3000_LOC="D0X9Y6.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X9Y6_MAIN_TBUF_0_ (.I(net_0678), .T(net_0683), .O(net_0094));

  (* XC3000_LOC="D0X9Y6.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X9Y6_MAIN_TBUF_1_ (.I(net_0681), .T(net_0683), .O(net_0089));

  (* XC3000_LOC="D0X9Y6.MAIN", XC3000_BEL="TBUF_E[0]" *)
  XC3000_TBUF D0X9Y6_MAIN_TBUF_E_0_ (.I(net_0682), .T(net_0683), .O(net_0094));

  (* XC3000_LOC="D0X9Y6.MAIN", XC3000_BEL="TBUF_E[1]" *)
  XC3000_TBUF D0X9Y6_MAIN_TBUF_E_1_ (.I(net_0586), .T(net_0683), .O(net_0089));

  // D0X9Y6.MAIN.IO_E[0]
  assign net_0522 = pin_P68;
  wire D0X9Y6_MAIN_IO_E_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y6_MAIN_IO_E_0__input (.PAD_IN(pin_P68), .IK(net_0002), .Q(D0X9Y6_MAIN_IO_E_0__input_q));
  assign net_0684 = D0X9Y6_MAIN_IO_E_0__input_q;
  wire D0X9Y6_MAIN_IO_E_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y6_MAIN_IO_E_0__output_ff (.D(net_0679), .OK(net_0000), .Q(D0X9Y6_MAIN_IO_E_0__output_q));
  (* XC3000_LOC="D0X9Y6.MAIN", XC3000_BEL="IO_E[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y6_MAIN_IO_E_0__driver (.O(net_0679), .T(~(net_0680)), .PAD(pin_P68));

  // D0X9Y6.MAIN.IO_E[1]
  assign net_0586 = pin_P67;
  wire D0X9Y6_MAIN_IO_E_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y6_MAIN_IO_E_1__input (.PAD_IN(pin_P67), .IK(net_0002), .Q(D0X9Y6_MAIN_IO_E_1__input_q));
  assign net_0685 = D0X9Y6_MAIN_IO_E_1__input_q;
  wire D0X9Y6_MAIN_IO_E_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y6_MAIN_IO_E_1__output_ff (.D(net_0584), .OK(net_0000), .Q(D0X9Y6_MAIN_IO_E_1__output_q));
  (* XC3000_LOC="D0X9Y6.MAIN", XC3000_BEL="IO_E[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y6_MAIN_IO_E_1__driver (.O(net_0584), .T(~(net_0585)), .PAD(pin_P67));

  // D0X9Y7.MAIN.CLB; MODE=FG
  wire D0X9Y7_MAIN_CLB_f, D0X9Y7_MAIN_CLB_g, D0X9Y7_MAIN_CLB_m, D0X9Y7_MAIN_CLB_qx, D0X9Y7_MAIN_CLB_qy, D0X9Y7_MAIN_CLB_dx, D0X9Y7_MAIN_CLB_dy;
  (* XC3000_LOC="D0X9Y7.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1101100011011000), .INIT_G(16'b1011000110110001)) D0X9Y7_MAIN_CLB_lut (
    .A(net_0099), .F2(D0X9Y7_MAIN_CLB_qx), .F3(net_0017), .F4(net_0586),
    .G2(net_0004), .G3(D0X9Y7_MAIN_CLB_qy), .G4(net_0586), .E(net_0577), .F(D0X9Y7_MAIN_CLB_f), .G(D0X9Y7_MAIN_CLB_g), .M(D0X9Y7_MAIN_CLB_m));
  assign D0X9Y7_MAIN_CLB_dx = D0X9Y7_MAIN_CLB_f;
  assign D0X9Y7_MAIN_CLB_dy = D0X9Y7_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y7_MAIN_CLB_ffx (.D(D0X9Y7_MAIN_CLB_dx), .K(net_0002), .EC(net_0453), .RD(net_0522), .Q(D0X9Y7_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y7_MAIN_CLB_ffy (.D(D0X9Y7_MAIN_CLB_dy), .K(net_0002), .EC(net_0453), .RD(net_0522), .Q(D0X9Y7_MAIN_CLB_qy));
  assign net_0687 = D0X9Y7_MAIN_CLB_qx;
  assign net_0689 = D0X9Y7_MAIN_CLB_qy;

  (* XC3000_LOC="D0X9Y7.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X9Y7_MAIN_TBUF_0_ (.I(net_0686), .T(net_0693), .O(net_0105));

  (* XC3000_LOC="D0X9Y7.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X9Y7_MAIN_TBUF_1_ (.I(net_0690), .T(net_0693), .O(net_0099));

  (* XC3000_LOC="D0X9Y7.MAIN", XC3000_BEL="TBUF_E[0]" *)
  XC3000_TBUF D0X9Y7_MAIN_TBUF_E_0_ (.I(net_0691), .T(net_0693), .O(net_0105));

  (* XC3000_LOC="D0X9Y7.MAIN", XC3000_BEL="TBUF_E[1]" *)
  XC3000_TBUF D0X9Y7_MAIN_TBUF_E_1_ (.I(net_0692), .T(net_0693), .O(net_0099));

  tri D0X9Y7_MAIN_IO_E_0__unbonded_pad;

  // D0X9Y7.MAIN.IO_E[0]
  assign net_0694 = D0X9Y7_MAIN_IO_E_0__unbonded_pad;
  wire D0X9Y7_MAIN_IO_E_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y7_MAIN_IO_E_0__input (.PAD_IN(D0X9Y7_MAIN_IO_E_0__unbonded_pad), .IK(net_0002), .Q(D0X9Y7_MAIN_IO_E_0__input_q));
  assign net_0695 = D0X9Y7_MAIN_IO_E_0__input_q;
  wire D0X9Y7_MAIN_IO_E_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y7_MAIN_IO_E_0__output_ff (.D(net_0597), .OK(net_0000), .Q(D0X9Y7_MAIN_IO_E_0__output_q));
  pullup(D0X9Y7_MAIN_IO_E_0__unbonded_pad);
  (* XC3000_LOC="D0X9Y7.MAIN", XC3000_BEL="IO_E[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y7_MAIN_IO_E_0__driver (.O(net_0597), .T(1'b1), .PAD(D0X9Y7_MAIN_IO_E_0__unbonded_pad));

  // D0X9Y7.MAIN.IO_E[1]
  assign net_0524 = pin_P69;
  wire D0X9Y7_MAIN_IO_E_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y7_MAIN_IO_E_1__input (.PAD_IN(pin_P69), .IK(net_0002), .Q(D0X9Y7_MAIN_IO_E_1__input_q));
  assign net_0696 = D0X9Y7_MAIN_IO_E_1__input_q;
  wire D0X9Y7_MAIN_IO_E_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y7_MAIN_IO_E_1__output_ff (.D(net_0687), .OK(net_0000), .Q(D0X9Y7_MAIN_IO_E_1__output_q));
  (* XC3000_LOC="D0X9Y7.MAIN", XC3000_BEL="IO_E[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y7_MAIN_IO_E_1__driver (.O(net_0687), .T(~(net_0689)), .PAD(pin_P69));

  // D0X9Y8.MAIN.CLB; MODE=FG
  wire D0X9Y8_MAIN_CLB_f, D0X9Y8_MAIN_CLB_g, D0X9Y8_MAIN_CLB_m, D0X9Y8_MAIN_CLB_qx, D0X9Y8_MAIN_CLB_qy, D0X9Y8_MAIN_CLB_dx, D0X9Y8_MAIN_CLB_dy;
  (* XC3000_LOC="D0X9Y8.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b1100110011110000), .INIT_G(16'b1111000000110011)) D0X9Y8_MAIN_CLB_lut (
    .A(net_0453), .F2(D0X9Y8_MAIN_CLB_qx), .F3(net_0017), .F4(net_0110),
    .G2(net_0004), .G3(D0X9Y8_MAIN_CLB_qy), .G4(net_0110), .E(net_0598), .F(D0X9Y8_MAIN_CLB_f), .G(D0X9Y8_MAIN_CLB_g), .M(D0X9Y8_MAIN_CLB_m));
  assign D0X9Y8_MAIN_CLB_dx = D0X9Y8_MAIN_CLB_f;
  assign D0X9Y8_MAIN_CLB_dy = D0X9Y8_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y8_MAIN_CLB_ffx (.D(D0X9Y8_MAIN_CLB_dx), .K(net_0002), .EC(net_0453), .RD(net_0530), .Q(D0X9Y8_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(1), .RD_USED(0)) D0X9Y8_MAIN_CLB_ffy (.D(D0X9Y8_MAIN_CLB_dy), .K(net_0002), .EC(net_0453), .RD(net_0530), .Q(D0X9Y8_MAIN_CLB_qy));
  assign net_0698 = D0X9Y8_MAIN_CLB_qx;
  assign net_0699 = D0X9Y8_MAIN_CLB_qy;

  (* XC3000_LOC="D0X9Y8.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X9Y8_MAIN_TBUF_0_ (.I(net_0697), .T(net_0701), .O(net_0110));

  (* XC3000_LOC="D0X9Y8.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X9Y8_MAIN_TBUF_1_ (.I(net_0017), .T(net_0702), .O(net_0115));

  (* XC3000_LOC="D0X9Y8.MAIN", XC3000_BEL="TBUF_E[0]" *)
  XC3000_TBUF D0X9Y8_MAIN_TBUF_E_0_ (.I(net_0530), .T(net_0701), .O(net_0110));

  (* XC3000_LOC="D0X9Y8.MAIN", XC3000_BEL="TBUF_E[1]" *)
  XC3000_TBUF D0X9Y8_MAIN_TBUF_E_1_ (.I(net_0700), .T(net_0701), .O(net_0115));

  // D0X9Y8.MAIN.IO_E[0]
  assign net_0530 = pin_P71;
  wire D0X9Y8_MAIN_IO_E_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y8_MAIN_IO_E_0__input (.PAD_IN(pin_P71), .IK(net_0002), .Q(D0X9Y8_MAIN_IO_E_0__input_q));
  assign net_0703 = D0X9Y8_MAIN_IO_E_0__input_q;
  wire D0X9Y8_MAIN_IO_E_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y8_MAIN_IO_E_0__output_ff (.D(net_0598), .OK(net_0000), .Q(D0X9Y8_MAIN_IO_E_0__output_q));
  (* XC3000_LOC="D0X9Y8.MAIN", XC3000_BEL="IO_E[0]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y8_MAIN_IO_E_0__driver (.O(net_0598), .T(~(net_0597)), .PAD(pin_P71));

  // D0X9Y8.MAIN.IO_E[1]
  assign net_0588 = pin_P70;
  wire D0X9Y8_MAIN_IO_E_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y8_MAIN_IO_E_1__input (.PAD_IN(pin_P70), .IK(net_0002), .Q(D0X9Y8_MAIN_IO_E_1__input_q));
  assign net_0704 = D0X9Y8_MAIN_IO_E_1__input_q;
  wire D0X9Y8_MAIN_IO_E_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y8_MAIN_IO_E_1__output_ff (.D(net_0698), .OK(net_0000), .Q(D0X9Y8_MAIN_IO_E_1__output_q));
  (* XC3000_LOC="D0X9Y8.MAIN", XC3000_BEL="IO_E[1]", XC3000_SLEW="FAST" *)
  XC3000_IOB_DRIVER D0X9Y8_MAIN_IO_E_1__driver (.O(net_0698), .T(~(net_0699)), .PAD(pin_P70));

  // D0X9Y9.MAIN.CLB; MODE=FG
  wire D0X9Y9_MAIN_CLB_f, D0X9Y9_MAIN_CLB_g, D0X9Y9_MAIN_CLB_m, D0X9Y9_MAIN_CLB_qx, D0X9Y9_MAIN_CLB_qy, D0X9Y9_MAIN_CLB_dx, D0X9Y9_MAIN_CLB_dy;
  (* XC3000_LOC="D0X9Y9.MAIN", XC3000_BEL="CLB" *)
  XC3000_FGM #(.INIT_F(16'b0000000000000000), .INIT_G(16'b0000000000000000)) D0X9Y9_MAIN_CLB_lut (
    .A(net_0004), .F2(D0X9Y9_MAIN_CLB_qy), .F3(D0X9Y9_MAIN_CLB_qy), .F4(net_0531),
    .G2(D0X9Y9_MAIN_CLB_qy), .G3(D0X9Y9_MAIN_CLB_qy), .G4(net_0531), .E(net_0531), .F(D0X9Y9_MAIN_CLB_f), .G(D0X9Y9_MAIN_CLB_g), .M(D0X9Y9_MAIN_CLB_m));
  assign D0X9Y9_MAIN_CLB_dx = D0X9Y9_MAIN_CLB_f;
  assign D0X9Y9_MAIN_CLB_dy = D0X9Y9_MAIN_CLB_g;
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X9Y9_MAIN_CLB_ffx (.D(D0X9Y9_MAIN_CLB_dx), .K(net_0057), .EC(net_0004), .RD(net_0705), .Q(D0X9Y9_MAIN_CLB_qx));
  XC3000_CLB_FF #(.EC_USED(0), .RD_USED(0)) D0X9Y9_MAIN_CLB_ffy (.D(D0X9Y9_MAIN_CLB_dy), .K(net_0057), .EC(net_0004), .RD(net_0705), .Q(D0X9Y9_MAIN_CLB_qy));
  assign net_0715 = D0X9Y9_MAIN_CLB_qx;
  assign net_0716 = D0X9Y9_MAIN_CLB_qy;

  (* XC3000_LOC="D0X9Y9.MAIN", XC3000_BEL="TBUF[0]" *)
  XC3000_TBUF D0X9Y9_MAIN_TBUF_0_ (.I(net_0707), .T(net_0713), .O(net_0016));

  (* XC3000_LOC="D0X9Y9.MAIN", XC3000_BEL="TBUF[1]" *)
  XC3000_TBUF D0X9Y9_MAIN_TBUF_1_ (.I(net_0017), .T(net_0714), .O(net_0123));

  (* XC3000_LOC="D0X9Y9.MAIN", XC3000_BEL="TBUF_E[0]" *)
  XC3000_TBUF D0X9Y9_MAIN_TBUF_E_0_ (.I(net_0711), .T(net_0713), .O(net_0016));

  (* XC3000_LOC="D0X9Y9.MAIN", XC3000_BEL="TBUF_E[1]" *)
  XC3000_TBUF D0X9Y9_MAIN_TBUF_E_1_ (.I(net_0712), .T(net_0713), .O(net_0123));

  // D0X9Y9.MAIN.IO_E[0]
  assign net_0717 = pin_P73;
  wire D0X9Y9_MAIN_IO_E_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y9_MAIN_IO_E_0__input (.PAD_IN(pin_P73), .IK(net_0002), .Q(D0X9Y9_MAIN_IO_E_0__input_q));
  assign net_0719 = D0X9Y9_MAIN_IO_E_0__input_q;
  wire D0X9Y9_MAIN_IO_E_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y9_MAIN_IO_E_0__output_ff (.D(net_0708), .OK(net_0000), .Q(D0X9Y9_MAIN_IO_E_0__output_q));
  pullup(pin_P73);
  (* XC3000_LOC="D0X9Y9.MAIN", XC3000_BEL="IO_E[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y9_MAIN_IO_E_0__driver (.O(net_0708), .T(1'b1), .PAD(pin_P73));

  // D0X9Y9.MAIN.IO_E[1]
  assign net_0718 = pin_P72;
  wire D0X9Y9_MAIN_IO_E_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y9_MAIN_IO_E_1__input (.PAD_IN(pin_P72), .IK(net_0002), .Q(D0X9Y9_MAIN_IO_E_1__input_q));
  assign net_0720 = D0X9Y9_MAIN_IO_E_1__input_q;
  wire D0X9Y9_MAIN_IO_E_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y9_MAIN_IO_E_1__output_ff (.D(net_0004), .OK(net_0000), .Q(D0X9Y9_MAIN_IO_E_1__output_q));
  pullup(pin_P72);
  (* XC3000_LOC="D0X9Y9.MAIN", XC3000_BEL="IO_E[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y9_MAIN_IO_E_1__driver (.O(net_0004), .T(1'b1), .PAD(pin_P72));

  // D0X9Y9.MAIN.IO_N[0]
  assign net_0721 = pin_P76;
  wire D0X9Y9_MAIN_IO_N_0__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y9_MAIN_IO_N_0__input (.PAD_IN(pin_P76), .IK(net_0002), .Q(D0X9Y9_MAIN_IO_N_0__input_q));
  assign net_0723 = D0X9Y9_MAIN_IO_N_0__input_q;
  wire D0X9Y9_MAIN_IO_N_0__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y9_MAIN_IO_N_0__output_ff (.D(net_0601), .OK(net_0015), .Q(D0X9Y9_MAIN_IO_N_0__output_q));
  pullup(pin_P76);
  (* XC3000_LOC="D0X9Y9.MAIN", XC3000_BEL="IO_N[0]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y9_MAIN_IO_N_0__driver (.O(net_0601), .T(1'b1), .PAD(pin_P76));

  // D0X9Y9.MAIN.IO_N[1]
  assign net_0722 = pin_P75;
  wire D0X9Y9_MAIN_IO_N_1__input_q;
  XC3000_IOB_INPUT_LATCH D0X9Y9_MAIN_IO_N_1__input (.PAD_IN(pin_P75), .IK(net_0002), .Q(D0X9Y9_MAIN_IO_N_1__input_q));
  assign net_0724 = D0X9Y9_MAIN_IO_N_1__input_q;
  wire D0X9Y9_MAIN_IO_N_1__output_q;
  XC3000_IOB_OUTPUT_FF D0X9Y9_MAIN_IO_N_1__output_ff (.D(net_0710), .OK(net_0015), .Q(D0X9Y9_MAIN_IO_N_1__output_q));
  pullup(pin_P75);
  (* XC3000_LOC="D0X9Y9.MAIN", XC3000_BEL="IO_N[1]", XC3000_SLEW="SLOW" *)
  XC3000_IOB_DRIVER D0X9Y9_MAIN_IO_N_1__driver (.O(net_0710), .T(1'b1), .PAD(pin_P75));
endmodule
