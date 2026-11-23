/* Generated audited profile metadata. */
if(entry.device_class==2018u){
    lt48_c5114_resources *r=NULL;
    if(lt48_c5114_open(installation,device_index,&r)==LT48_OK){
      for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation==1 || items[i].operation==2 || items[i].operation==3 || items[i].operation==4 || items[i].operation==5 || items[i].operation==6 || items[i].operation==7 || items[i].operation==8 || items[i].operation==10)){
        items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at89_c5114_source_audited";items[i].reason="Exact C5114 source resources; 4-KiB code operations, restored settings after erase, independent margin/configuration readback and failure cleanup; hardware qualification pending";
      }
      lt48_c5114_close(r);return LT48_OK;
    }
  }
if(entry.device_class==2018u){
    lt48_c5132_resources *r=NULL;
    if(lt48_c5132_open(installation,device_index,&r)==LT48_OK){
      for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation==1 || items[i].operation==2 || items[i].operation==3 || items[i].operation==4 || items[i].operation==5 || items[i].operation==6 || items[i].operation==7 || items[i].operation==8 || items[i].operation==10)){
        items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at89_c5132_source_audited";items[i].reason="Exact C5132 source resources; 64-KiB code plus 4-KiB boot operations, restored extra row and settings after erase, independent margin/configuration readback and failure cleanup; hardware qualification pending";
      }
      lt48_c5132_close(r);return LT48_OK;
    }
  }
if(entry.device_class==2010u){
    lt48_lpc_resources *r=NULL;
    if(lt48_lpc_open(installation,device_index,&r)==LT48_OK){
      for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation==1 || items[i].operation==2 || items[i].operation==3 || items[i].operation==4 || items[i].operation==5 || items[i].operation==6 || items[i].operation==8 || items[i].operation==10)){
        items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="philips_lpc_source_audited";items[i].reason="Exact seven LPC source definitions; sparse code/customer data, OTP guards, configuration and security, independent margin readback and cleanup; hardware pending";
      }
      lt48_lpc_close(r);return LT48_OK;
    }
  }
