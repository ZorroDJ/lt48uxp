/* Exact source-bound compiler words, including the proved selector zero.
 * This never enables general missing-pin fallback for other definitions. */
static lt48_status mx26aone_compile_erase(const lt48_wave_code*code,lt48_compiled_wave**out){
 static const unsigned char source[]={8,0,0x69,75,0x58,5,1,0x46,5,11};
 static const unsigned char words[]={14,0,77,0,75,0,67,0,0,0,1,0,0x46,5,11,0};
 if(code->wave_type||code->data_size!=sizeof source||memcmp(code->data,source,sizeof source))return LT48_SAFETY_REJECTED;
 return lt48_compiled_word_table_internal(words,sizeof words,out);
}
