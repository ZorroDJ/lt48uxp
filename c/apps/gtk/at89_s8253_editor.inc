#include "n79w79_labels.inc"
#include "numicro81_editor.inc"
#include "nuvoton30_editor.inc"
#include "n79w79_21_editor.inc"
#include "sstmx16_editor.inc"
#include "xmega7_editor.inc"
#include "nuvoton8_editor.inc"
#include "../../../NewDeviceLib/sources/cc254x5/editor.inc"
#include "../../../NewDeviceLib/sources/stm8s4/editor.inc"
#include "msp43076_editor.inc"
#include "class2018_38_editor.inc"
/* Source-defined boot/security fields remain separate from the memory image. */
static unsigned s8253_fields(unsigned flags){if(n79_fields(flags))return n79_fields(flags);if(flags>=25000){switch(flags){case 25009:case 25073:return 7;case 25059:case 25074:return 6;case 25131:return 5;case 25016:case 25204:return 4;default:return 0;}}return flags==22?4:flags==21?6:flags==20?3:flags>=15?flags-10:flags==2?2:5;}
static const char *s8253_label(unsigned flags,unsigned field) {
    if(n79_fields(flags))return n79_label(flags,field);
    if(flags>=25000){
        if(flags==25009){static const char*n[]={"Lock Bit Active","MOVC Inhibit","Encryption Enable","P2 Reboot Enable","P4 Reboot Enable","EEPROM Protect","Crystal more than 24MHz"};return field<s8253_fields(flags)?n[field]:"Invalid field";}
        if(flags==25016){static const char*n[]={"Lock Bit","MOVC Inhibit","Encryption Enable","Osillator Control"};return field<s8253_fields(flags)?n[field]:"Invalid field";}
        if(flags==25059){static const char*n[]={"Lock Bit Active","MOVC Inhibit","Encryption Enable","6 Clock Enable.","P2.6 P2.7 are as reboot mode","P4.3 is as reboot mode"};return field<s8253_fields(flags)?n[field]:"Invalid field";}
        if(flags==25073){static const char*n[]={"Lock Bit Active","MOVC Inhibit","Encryption Enable","6 Clock Enable.","P2.6 P2.7 are as reboot mode","P4.3 is as reboot mode","Crystal more than 24MHz"};return field<s8253_fields(flags)?n[field]:"Invalid field";}
        if(flags==25074){static const char*n[]={"Lock Bit Active","MOVC Inhibit","Encryption Enable","P2.6 P2.7 are as reboot mode","P4.3 is as reboot mode","Crystal more than 24MHz"};return field<s8253_fields(flags)?n[field]:"Invalid field";}
        if(flags==25131){static const char*n[]={"Lock Bit Active","MOVC Inhibit","P2.6 P2.7 are as reboot mode","P4.3 is as reboot mode","Crystal more than 24MHz"};return field<s8253_fields(flags)?n[field]:"Invalid field";}
        if(flags==25204){static const char*n[]={"Lock Bit","MOVC Inhibit","6 Clock Enable.","Osillator Control"};return field<s8253_fields(flags)?n[field]:"Invalid field";}
        return "Invalid field";
    }
    if(flags==22){static const char*n[]={"Watchdog enabled","Watchdog period B2 programmed (0)","Watchdog period B3 programmed (0)","Power-low supervisor enabled"};return field<4?n[field]:"Invalid field";}
    if(flags==21){static const char*n[]={"Lock Bit","MOVC Inhibit","CBS=0","Noise Reduction enabled","6 Clock Enable","Crystal Select 24MHz (half gain)"};return field<6?n[field]:"Invalid field";}
    if(flags==20){static const char*n[]={"Serial Safe Mode","Secure Mode","Lock Mode"};return field<3?n[field]:"Invalid field";}
    if(flags>=15){static const char *five[]={"SC0","Lock Bit 1","Lock Bit 2","Lock Bit 3","Clock Double(6 Clock)"};static const char *six[]={"SC0","SC1","Lock Bit 1","Lock Bit 2","Lock Bit 3","Clock Double(6 Clock)"};return field<s8253_fields(flags)?(flags==15?five[field]:six[field]):"Invalid field";}
    if(flags==2)return field==0?"Lock Bit":field==1?"Clock Double(6 Clock)":"Invalid field";
    static const char *names[]={"SPI programming enable", "x2 clock enable", "User signature programming enable", "External clock enable", "Lock level (0–3)"};
    return field<5?names[field]:"Invalid field";
}
typedef struct {app_widgets *widgets;GtkWindow *window;GtkWidget *enable[16],*value[16],*error;unsigned count,flags;size_t device;uint64_t revision;} s8253_editor;
static gboolean s8253_parse_field(unsigned flags,unsigned field,const char *text,unsigned char *out) {
    if(field>=s8253_fields(flags) || !text || !*text)return FALSE;
    int hex=0;
    if(hex && strlen(text)>2)return FALSE;
    for(const char *p=text;*p;p++)if(hex?!g_ascii_isxdigit(*p):!g_ascii_isdigit(*p))return FALSE;
    char *end;guint64 n=g_ascii_strtoull(text,&end,hex?16:10);
    if(*end || n>(flags>=15||flags==2||field<4?1u:3u))return FALSE;
    *out=(unsigned char)n;return TRUE;
}
static char *s8253_configuration_message(unsigned flags,const unsigned char *values,gboolean edited) {
    GString *message=g_string_new(edited?"Configuration written and verified.\n":"Configuration read.\n");
    for(unsigned i=0;i<s8253_fields(flags);i++) {
        if(flags==2 && i==1 && values[0])g_string_append(message,"Clock Double(6 Clock): unavailable while locked\n");
        else g_string_append_printf(message,"%s: %u\n",s8253_label(flags,i),values[i]);
    }
    g_string_append(message,"Cleanup completed.");return g_string_free(message,FALSE);
}
static void s8253_editor_action(GtkButton *button,gpointer data) {
    s8253_editor *e=data;app_widgets *w=e->widgets;
    if(!w->has_selected_device || e->device!=w->selected_device_index || e->revision!=w->operation_input_revision){gtk_window_destroy(e->window);return;}
    unsigned mask=0;unsigned shift=n79_fields(e->flags)?2:1;unsigned char edit[18]={0};
    if(g_object_get_data(G_OBJECT(button),"apply"))for(unsigned i=0;i<e->count;i++) {
        if(!gtk_check_button_get_active(GTK_CHECK_BUTTON(e->enable[i])))continue;
        const char *text=gtk_editable_get_text(GTK_EDITABLE(e->value[i]));
        if(!s8253_parse_field(e->flags,i,text,edit+shift+i)) {
            gtk_label_set_text(GTK_LABEL(e->error),"Flags must be 0 or 1; lock level must be 0–3.");return;
        }
        mask|=1u<<i;
    }
    edit[0]=(unsigned char)mask;if(shift==2)edit[1]=(unsigned char)(mask>>8);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(w->operation_dropdown),9);
    if(e->flags==22)memcpy(w->mcu51_configuration,edit,5);else if(e->flags==20)memcpy(w->mcu51_configuration,edit,4);else memcpy(w->s8253_configuration,edit,sizeof(edit));gtk_window_destroy(e->window);
    w->read_after_prepare=0;start_operation_preparation(w,LT48_OPERATION_CONFIGURATION);
}
static void s8253_editor_open(app_widgets *w) {
    if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"stm8s4-nine-operation")){s8_editor_open(w);return;}
    if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"cc254x5-nine-operation")){c25_editor_open(w);return;}
    if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"nuvoton8-nine-operation")){nv8_editor_open(w);return;}
    if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"xmega7-nine-operation")){x7_editor_open(w);return;}
    if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"msp43076-nine-operation")){msp430_editor_open(w);return;}
    if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"sstmx16-nine-operation")){sm16_editor_open(w);return;}
    if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"n79w79_21-nine-operation")){nw21_editor_open(w);return;}
    if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"nuvoton30-nine-operation")){nuvoton_editor_open(w);return;}
    if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"numicro81-nine-operation")){numicro_editor_open(w);return;}
    lt48_catalogue_entry device;
    if(lt48_installation_get_device(w->installation,w->selected_device_index,&device)!=LT48_OK)return;
    s8253_editor *e=g_new0(s8253_editor,1);e->flags=!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"p89v51-nine-operation")?2:4;if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"sst89rd-nine-operation")){lt48_s8253_resources*r=NULL;if(lt48_s8253_open(w->installation,w->selected_device_index,&r)!=LT48_OK){g_free(e);return;}e->flags=r->fields+10;lt48_s8253_close(r);}if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"legacy14-aduc-nine-operation"))e->flags=20;if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"w78d24-nine-operation"))e->flags=21;if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"w78bank147-nine-operation") || !strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"n79w79-nine-operation")){lt48_s8253_resources*r=NULL;if(lt48_s8253_open(w->installation,w->selected_device_index,&r)!=LT48_OK){g_free(e);return;}e->flags=r->configuration_id;lt48_s8253_close(r);}if(!strcmp(lt48_installation_complete_operation_profile(w->installation,w->selected_device_index),"legacy-mcu20-options-eight-operation"))e->flags=22;e->count=s8253_fields(e->flags);e->widgets=w;e->device=w->selected_device_index;e->revision=w->operation_input_revision;
    e->window=GTK_WINDOW(gtk_window_new());gtk_window_set_title(e->window,e->flags==22?"W78E354 watchdog and supervisor options":e->flags>=25000?"Winbond/Nuvoton configuration":e->flags==21?"W78D configuration":e->flags==20?"ADuC824 security configuration":e->flags>=15?"SST RD/RD2 configuration":e->flags==2?"P89V/LV51 configuration":"AT89S8253 configuration");
    gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);gtk_window_set_default_size(e->window,650,650);
    GtkWidget *scroll=gtk_scrolled_window_new(),*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);
    gtk_window_set_child(e->window,scroll);gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),box);
    GtkWidget *intro=gtk_label_new(e->flags==22?"Read or program four option bits. Values are active-low: 1 means the physical option bit is programmed to zero. Unchecked fields are preserved. Clearing a programmed flag needs chip erase. Watchdog period B3/B2=00,01,10,11 gives 2^19,2^21,2^23,2^24 oscillator periods. These options do not lock code.":n79_fields(e->flags)?"Read or change source-defined oscillator, device options and protection. Unchecked fields are preserved. Direct option bits program from 1 to 0; active protection bits program from 0 to 1. Erase is required to reverse these changes. Program and Overprogram erase all memory regions first and restore non-lock options. Secure sets Flash Lock.":e->flags>=25000?"Read or change the source-defined flags. Unchecked fields are preserved; clearing a programmed flag requires chip erase. Program and Overprogram erase all banks first and restore non-lock options after verification. The memory image concatenates main code banks and 4 KiB secondary Flash. Secure sets Lock Bit.":e->flags==21?"Read or change six one-way configuration flags. Unchecked fields are preserved; chip erase clears them. Program and Overprogram erase both Flash regions first, then restore non-lock options. The file contains main code followed by 2 KiB secondary Flash, without a gap. Secure sets Lock Bit and preserves other options.":e->flags==20?"Read or apply Serial Safe Mode, Secure Mode and Lock Mode flags. Only 0-to-1 changes are accepted. Unchecked flags are preserved. Chip erase clears code, EEPROM and security. Secure sets all three flags.":e->flags>=15?"SC, lock and clock flags use 0/1 values. Only 0 to 1 changes are programmable; chip erase clears them and both Flash regions. Unchecked fields are preserved. Read uses a concatenated main plus 8 KiB secondary Flash image; use the original package adapter.":e->flags==2?"Lock Bit and Clock Double use 0/1 values. Only changes from 0 to 1 can be programmed; chip erase is required to clear them. Unchecked fields are preserved. Chip erase also clears the ISP/IAP boot block. The 72 KiB image contains code, FF padding to 64 KiB, and the 8 KiB boot block.":"Read current settings, or check the fields to change. Unchecked fields are preserved.\nThese are new values, not a live read. Chip erase is required to reduce protection.\nLocked chips reject fuse changes. Flags: 1 enabled, 0 disabled. Lock: 0 unlocked through 3 strongest. The buffer contains 12288 bytes of code, 2048 bytes of EEPROM, then the 64-byte user signature. Erase clears all three regions and preserves fuse settings.");gtk_label_set_wrap(GTK_LABEL(intro),TRUE);gtk_box_append(GTK_BOX(box),intro);
    for(unsigned i=0;i<e->count;i++) {
        GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);gtk_box_append(GTK_BOX(box),row);
        e->enable[i]=gtk_check_button_new_with_label(s8253_label(e->flags,i));gtk_box_append(GTK_BOX(row),e->enable[i]);
        e->value[i]=gtk_entry_new();gtk_widget_set_hexpand(e->value[i],TRUE);gtk_editable_set_text(GTK_EDITABLE(e->value[i]),"0");gtk_box_append(GTK_BOX(row),e->value[i]);
    }
    e->error=gtk_label_new("");gtk_label_set_wrap(GTK_LABEL(e->error),TRUE);gtk_box_append(GTK_BOX(box),e->error);
    GtkWidget *read=gtk_button_new_with_label("Prepare configuration read"),*apply=gtk_button_new_with_label("Prepare selected changes");
    gtk_box_append(GTK_BOX(box),read);gtk_box_append(GTK_BOX(box),apply);g_object_set_data(G_OBJECT(apply),"apply",GINT_TO_POINTER(1));
    g_signal_connect(read,"clicked",G_CALLBACK(s8253_editor_action),e);g_signal_connect(apply,"clicked",G_CALLBACK(s8253_editor_action),e);
    g_object_set_data_full(G_OBJECT(e->window),"s8253-editor",e,g_free);gtk_window_present(e->window);
}
