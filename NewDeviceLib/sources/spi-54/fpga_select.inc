        /* Exact D1ac source typo: SO16 W419 with BGA P5e. Reuse the
         * fitted BGA W41a from this same digest-bound source library. */
        if(sr_binding==4 && closure->definition==0x1ac && op.operation_id==30) {
            if(op.wave!=0x419 || code.wave_type!=4){status=LT48_SAFETY_REJECTED;goto fail;}
            TRY(lt48_rez_find_resource(r->rez,"W41a",&index));
            TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        }
