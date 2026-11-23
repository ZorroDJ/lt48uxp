#include <labtool48/labtool48.h>
#include "internal.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct lt48_test_vectors {
    char *rows;
    char *clipboard;
    char *undo;
    size_t pin_count;
    size_t row_count;
    size_t current_row;
    size_t undo_row;
    int has_undo;
};

static int valid_document(const lt48_test_vectors *vectors) {
    return vectors != NULL && vectors->pin_count != 0u &&
           vectors->pin_count <= 48u && vectors->rows != NULL &&
           vectors->clipboard != NULL && vectors->undo != NULL;
}

int lt48_test_vector_symbol_valid(char symbol) {
    static const char allowed[] = "01BCDFHKLNPUXZ";
    unsigned char value = (unsigned char)symbol;
    if (value > 0x7fu) return 0;
    return strchr(allowed, toupper(value)) != NULL;
}

static lt48_status normalize_row(
    const lt48_test_vectors *vectors, const char *symbols, char *destination
) {
    size_t index;
    if (!valid_document(vectors) || symbols == NULL || destination == NULL)
        return LT48_INVALID_ARGUMENT;
    for (index = 0u; index < vectors->pin_count; ++index) {
        unsigned char value = (unsigned char)symbols[index];
        if (value == 0u || !lt48_test_vector_symbol_valid((char)value))
            return LT48_FORMAT_ERROR;
        destination[index] = (char)toupper(value);
    }
    if (symbols[vectors->pin_count] != '\0') return LT48_FORMAT_ERROR;
    return LT48_OK;
}

lt48_status lt48_test_vectors_create(
    size_t pin_count, lt48_test_vectors **out_vectors
) {
    lt48_test_vectors *vectors;
    if (out_vectors == NULL || pin_count == 0u || pin_count > 48u)
        return LT48_INVALID_ARGUMENT;
    *out_vectors = NULL;
    vectors = calloc(1u, sizeof(*vectors));
    if (vectors == NULL) return LT48_NO_MEMORY;
    vectors->rows = malloc(pin_count);
    vectors->clipboard = malloc(pin_count);
    vectors->undo = malloc(pin_count);
    if (vectors->rows == NULL || vectors->clipboard == NULL ||
        vectors->undo == NULL) {
        lt48_test_vectors_close(vectors);
        return LT48_NO_MEMORY;
    }
    vectors->pin_count = pin_count;
    vectors->row_count = 1u;
    memset(vectors->rows, 'N', pin_count);
    memset(vectors->clipboard, 'N', pin_count);
    memset(vectors->undo, 'N', pin_count);
    *out_vectors = vectors;
    return LT48_OK;
}

void lt48_test_vectors_close(lt48_test_vectors *vectors) {
    if (vectors == NULL) return;
    free(vectors->undo);
    free(vectors->clipboard);
    free(vectors->rows);
    free(vectors);
}

lt48_status lt48_test_vectors_get_info(
    const lt48_test_vectors *vectors, lt48_test_vector_info *out_info
) {
    if (!valid_document(vectors) || out_info == NULL)
        return LT48_INVALID_ARGUMENT;
    out_info->pin_count = vectors->pin_count;
    out_info->row_count = vectors->row_count;
    out_info->current_row = vectors->current_row;
    out_info->has_undo = vectors->has_undo;
    return LT48_OK;
}

static lt48_status copy_text(
    const lt48_test_vectors *vectors, const char *source,
    char *destination, size_t destination_size
) {
    if (!valid_document(vectors) || source == NULL || destination == NULL)
        return LT48_INVALID_ARGUMENT;
    if (destination_size <= vectors->pin_count) return LT48_BUFFER_TOO_SMALL;
    memcpy(destination, source, vectors->pin_count);
    destination[vectors->pin_count] = '\0';
    return LT48_OK;
}

lt48_status lt48_test_vectors_get_row(
    const lt48_test_vectors *vectors, size_t row,
    char *destination, size_t destination_size
) {
    if (!valid_document(vectors)) return LT48_INVALID_ARGUMENT;
    if (row >= vectors->row_count) return LT48_NOT_FOUND;
    return copy_text(
        vectors, vectors->rows + row * vectors->pin_count,
        destination, destination_size);
}

lt48_status lt48_test_vectors_get_clipboard(
    const lt48_test_vectors *vectors,
    char *destination, size_t destination_size
) {
    if (!valid_document(vectors)) return LT48_INVALID_ARGUMENT;
    return copy_text(
        vectors, vectors->clipboard, destination, destination_size);
}

lt48_status lt48_test_vectors_set_row(
    lt48_test_vectors *vectors, size_t row, const char *symbols
) {
    char normalized[48];
    char *target;
    lt48_status status;
    if (!valid_document(vectors) || row >= vectors->row_count)
        return LT48_INVALID_ARGUMENT;
    status = normalize_row(vectors, symbols, normalized);
    if (status != LT48_OK) return status;
    target = vectors->rows + row * vectors->pin_count;
    if (memcmp(target, normalized, vectors->pin_count) == 0) return LT48_OK;
    memcpy(vectors->undo, target, vectors->pin_count);
    vectors->undo_row = row;
    vectors->has_undo = 1;
    memcpy(target, normalized, vectors->pin_count);
    return LT48_OK;
}

lt48_status lt48_test_vectors_set_current(
    lt48_test_vectors *vectors, size_t row
) {
    if (!valid_document(vectors) || row >= vectors->row_count)
        return LT48_INVALID_ARGUMENT;
    vectors->current_row = row;
    return LT48_OK;
}

lt48_status lt48_test_vectors_insert(lt48_test_vectors *vectors) {
    char *expanded;
    size_t offset;
    if (!valid_document(vectors) || vectors->row_count == SIZE_MAX)
        return LT48_INVALID_ARGUMENT;
    if (vectors->row_count > SIZE_MAX / vectors->pin_count - 1u)
        return LT48_NO_MEMORY;
    expanded = realloc(
        vectors->rows, (vectors->row_count + 1u) * vectors->pin_count);
    if (expanded == NULL) return LT48_NO_MEMORY;
    vectors->rows = expanded;
    offset = vectors->current_row * vectors->pin_count;
    memmove(vectors->rows + offset + vectors->pin_count,
            vectors->rows + offset,
            (vectors->row_count - vectors->current_row) * vectors->pin_count);
    memcpy(vectors->rows + offset, vectors->clipboard, vectors->pin_count);
    ++vectors->row_count;
    vectors->has_undo = 0;
    return LT48_OK;
}

lt48_status lt48_test_vectors_copy(lt48_test_vectors *vectors) {
    if (!valid_document(vectors) || vectors->current_row >= vectors->row_count)
        return LT48_INVALID_ARGUMENT;
    memcpy(vectors->clipboard,
           vectors->rows + vectors->current_row * vectors->pin_count,
           vectors->pin_count);
    return LT48_OK;
}

lt48_status lt48_test_vectors_cut(lt48_test_vectors *vectors) {
    size_t offset;
    if (!valid_document(vectors) || vectors->row_count <= 1u ||
        vectors->current_row >= vectors->row_count)
        return LT48_INVALID_ARGUMENT;
    (void)lt48_test_vectors_copy(vectors);
    offset = vectors->current_row * vectors->pin_count;
    memmove(vectors->rows + offset, vectors->rows + offset + vectors->pin_count,
            (vectors->row_count - vectors->current_row - 1u) *
                vectors->pin_count);
    --vectors->row_count;
    if (vectors->current_row >= vectors->row_count)
        vectors->current_row = vectors->row_count - 1u;
    vectors->has_undo = 0;
    return LT48_OK;
}

lt48_status lt48_test_vectors_undo(lt48_test_vectors *vectors) {
    if (!valid_document(vectors) || !vectors->has_undo ||
        vectors->undo_row >= vectors->row_count)
        return LT48_NOT_FOUND;
    memcpy(vectors->rows + vectors->undo_row * vectors->pin_count,
           vectors->undo, vectors->pin_count);
    vectors->current_row = vectors->undo_row;
    vectors->has_undo = 0;
    return LT48_OK;
}

/* Pinned 13.10 host 00440160: retain its three separate drive passes and
 * two bias captures. No direct hardware mapping is implied by these codes. */
lt48_status lt48_vector_cycle_internal(const char *row,size_t pins,unsigned x_mode,
    const lt48_vector_cycle_io *io,void *context,lt48_vector_cycle_result *result) {
    if(!row||!pins||pins>48||x_mode>2||!io||!io->drive||!io->control||!io->sample||!result)
        return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<pins;i++)
        if(!row[i]||!strchr("01BCDFHKLNPUXZ",row[i]))return LT48_INVALID_ARGUMENT;
    if(row[pins])return LT48_INVALID_ARGUMENT;
    lt48_vector_cycle_result pending={0};unsigned char first[48]={0};
    memcpy(pending.actual,row,pins);memset(pending.errors,' ',pins);
    unsigned base=(unsigned)((48-pins)/2)*4;
    lt48_status status;
#define VECTOR_TRY(call) do {status=(call);if(status!=LT48_OK)return status;} while(0)
    for(unsigned pass=0;pass<3;pass++)for(size_t i=0;i<pins;i++) {
        unsigned code=base+(unsigned)i*4;char c=row[i];
        if(pass==0) {
            if(c=='C')VECTOR_TRY(io->drive(context,code));
            else if(c=='K')VECTOR_TRY(io->drive(context,code+1));
            else if(c=='H'||c=='L'||c=='Z')VECTOR_TRY(io->drive(context,code+2));
            else if(c=='X')VECTOR_TRY(io->drive(context,code+x_mode));
        } else if(pass==1) {
            if(c=='0')VECTOR_TRY(io->drive(context,code));
            else if(c=='1')VECTOR_TRY(io->drive(context,code+1));
            else if(c=='F')VECTOR_TRY(io->drive(context,code+2));
        } else {
            if(c=='C'||c=='D') {
                VECTOR_TRY(io->drive(context,code+1));VECTOR_TRY(io->drive(context,code));
            } else if(c=='K'||c=='U') {
                VECTOR_TRY(io->drive(context,code));VECTOR_TRY(io->drive(context,code+1));
            }
        }
    }
    VECTOR_TRY(io->control(context,0x11));VECTOR_TRY(io->control(context,0x0f));
    for(size_t i=0;i<pins;i++)if(row[i]=='H'||row[i]=='L'||row[i]=='Z')
        VECTOR_TRY(io->sample(context,base+(unsigned)i*4,&first[i]));
    VECTOR_TRY(io->control(context,0x11));VECTOR_TRY(io->control(context,0));
    for(size_t i=0;i<pins;i++)if(row[i]=='H'||row[i]=='L'||row[i]=='Z') {
        unsigned char second=0;VECTOR_TRY(io->sample(context,base+(unsigned)i*4,&second));
        pending.actual[i]=first[i]!=second?'Z':second?'H':'L';
        if(pending.actual[i]!=row[i]){pending.errors[i]='^';pending.mismatch_count++;}
    }
#undef VECTOR_TRY
    *result=pending;return LT48_OK;
}

/* 13.10 CDrvUSB vtable 00789f98: +00 (496270) queues [1,value],
 * +0c (496360) queues [2,value], +78 (497850 -> 4975c0) flushes
 * then samples with command [04,pin]. Existing plan execution checks ACKs. */
typedef struct vector_transport {
    lt48_protocol_plan *plan;
    const lt48_protocol_transport *api;
    void *context;
    size_t executed;
} vector_transport;
static lt48_status vector_transport_pending(vector_transport *t,lt48_runtime_capture *capture,size_t *count) {
    size_t total=lt48_protocol_plan_transaction_count(t->plan),completed=0;
    lt48_status s=lt48_protocol_plan_execute_range(t->plan,t->executed,total-t->executed,
        t->api,t->context,capture,capture?1:0,count,&completed);
    t->executed+=completed;return s;
}
static lt48_status vector_transport_drive(void *ctx,unsigned code) {
    vector_transport *t=ctx;
    if(code>255)return LT48_INVALID_ARGUMENT;
    return lt48_protocol_plan_pair_internal(t->plan,2,(uint8_t)code);
}
static lt48_status vector_transport_control(void *ctx,unsigned value) {
    vector_transport *t=ctx;
    if(value>255)return LT48_INVALID_ARGUMENT;
    return lt48_protocol_plan_pair_internal(t->plan,1,(uint8_t)value);
}
static lt48_status vector_transport_sample(void *ctx,unsigned code,unsigned char *value) {
    vector_transport *t=ctx;lt48_runtime_capture capture={0};size_t count=0;
    if(code>255)return LT48_INVALID_ARGUMENT;
    lt48_status s=lt48_protocol_plan_sample_pin_internal(t->plan,(uint8_t)code,0);
    if(s==LT48_OK)s=vector_transport_pending(t,&capture,&count);
    if(s==LT48_OK&&(count!=1||capture.length!=1))s=LT48_PROTOCOL_ERROR;
    if(s==LT48_OK)*value=capture.values[0]&1;
    return s;
}
lt48_status lt48_vector_cycle_transport_internal(const char *row,size_t pins,unsigned x_mode,
    const lt48_protocol_transport *api,void *context,lt48_vector_cycle_result *result) {
    if(!api||!api->write||!api->read||!result)return LT48_INVALID_ARGUMENT;
    vector_transport t={0};t.api=api;t.context=context;
    lt48_status s=lt48_protocol_plan_create(&t.plan);
    if(s!=LT48_OK)return s;
    const lt48_vector_cycle_io io={vector_transport_drive,vector_transport_control,vector_transport_sample};
    lt48_vector_cycle_result pending;
    s=lt48_vector_cycle_internal(row,pins,x_mode,&io,&t,&pending);
    if(s==LT48_OK)s=lt48_protocol_plan_flush(t.plan);
    size_t captures=0;
    if(s==LT48_OK)s=vector_transport_pending(&t,NULL,&captures);
    if(s==LT48_OK)*result=pending;
    lt48_protocol_plan_close(t.plan);return s;
}

/* Original 0043ff80 with 0043f960/0043f8f0/0043f9f0/0043fa60.
 * Pin arguments are source pin-map codes (zero-based ZIF index times four).
 * Caller must bind the source, reset resources and permitted supply range. */
lt48_status lt48_vector_power_internal(const unsigned char *ground,size_t grounds,
    const unsigned char *supply,size_t supplies,unsigned centivolts,int on,
    lt48_vector_packet_fn send,void *context) {
    if(!send||grounds>48||supplies>48||(grounds&&!ground)||(supplies&&!supply)||
       centivolts>600||(on!=0&&on!=1))return LT48_INVALID_ARGUMENT;
    unsigned char occupied[48]={0};
    for(unsigned group=0;group<2;group++) {
        const unsigned char *pins=group?supply:ground;size_t count=group?supplies:grounds;
        for(size_t i=0;i<count;i++) {
            if(pins[i]>188||(pins[i]&3)||occupied[pins[i]/4])return LT48_INVALID_ARGUMENT;
            occupied[pins[i]/4]=1;
        }
    }
    /* Preserve the original float round-trip before DAC quantization. */
    float volts=(float)((double)centivolts/100.0);
    unsigned scaled=(unsigned)((double)volts*100.0);
    unsigned char dac=(unsigned char)(scaled?scaled/10+13:0);
    lt48_status status;unsigned char bytes[4];
#define POWER_SEND(n) do{status=send(context,bytes,(n));if(status!=LT48_OK)return status;}while(0)
    if(!on)for(unsigned channel=1;channel<=2;channel++) {
        bytes[0]=0x10;bytes[1]=(unsigned char)channel;bytes[2]=0;POWER_SEND(3);
    }
    for(size_t i=0;i<grounds;i++) {
        unsigned pin=ground[i]/4;size_t n=0;
        /* 00768d14 marks the three relay-capable ground locations. */
        if(pin==14||pin==23||pin==33){bytes[n++]=0x1d;bytes[n++]=(unsigned char)(pin*2+on);}
        bytes[n++]=0x1e;bytes[n++]=(unsigned char)(pin*2+on);POWER_SEND(n);
    }
    for(size_t i=0;i<supplies;i++) {
        unsigned pin=supply[i]/4;
        bytes[0]=(unsigned char)((pin&1)?0x20:0x21);
        bytes[1]=(unsigned char)(pin*2+on);POWER_SEND(2);
    }
    if(on) {
        for(unsigned channel=1;channel<=2;channel++) {
            bytes[0]=0x10;bytes[1]=(unsigned char)channel;bytes[2]=dac;POWER_SEND(3);
        }
        bytes[0]=0x4f;bytes[1]=100;bytes[2]=0;POWER_SEND(3);
    }
#undef POWER_SEND
    return LT48_OK;
}
