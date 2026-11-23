/* Values are explicit file/buffer data, never represented as a live RTC read. */
typedef struct {
 app_widgets *widgets;GtkWindow *window;GtkWidget *entry[16],*message;
 size_t device,count;uint64_t revision;int kind;
} timekeeper_editor;
static void timekeeper_apply(GtkButton *button,gpointer context){
 (void)button;timekeeper_editor *e=context;app_widgets*w=e->widgets;
 if(!w->has_selected_device||w->selected_device_index!=e->device||w->operation_input_revision!=e->revision){gtk_window_destroy(e->window);return;}
 unsigned char data[16]={0};
 for(size_t i=0;i<e->count;i++){
  const char*t=gtk_editable_get_text(GTK_EDITABLE(e->entry[i]));char*end;unsigned long v=strtoul(t,&end,16);
  if(!*t||*end||v>255){gtk_label_set_text(GTK_LABEL(e->message),"Enter one hexadecimal byte in every register field.");return;}data[i]=(unsigned char)v;
 }
 lt48_status s=lt48_timekeeper_validate_registers(e->kind,data,e->count);
 if(s!=LT48_OK){gtk_label_set_text(GTK_LABEL(e->message),"Invalid register bits: check BCD/calendar ranges, alarm masks, reserved bits and ST W/FT. DS1486 INTA and square-wave cannot both be enabled.");return;}
 memcpy(w->rtc_registers,data,e->count);w->rtc_register_count=e->count;
 gtk_drop_down_set_selected(GTK_DROP_DOWN(w->operation_dropdown),9);
 gtk_window_destroy(e->window);w->read_after_prepare=0;start_operation_preparation(w,LT48_OPERATION_CONFIGURATION);
}
static unsigned char timekeeper_bcd(int n){return (unsigned char)((n/10)*16+n%10);}
static void timekeeper_now(GtkButton *button,gpointer context){
 (void)button;timekeeper_editor*e=context;GDateTime*t=g_date_time_new_now_local();
 unsigned char data[16]={0};
 for(size_t i=0;i<e->count;i++)data[i]=(unsigned char)strtoul(gtk_editable_get_text(GTK_EDITABLE(e->entry[i])),NULL,16);
 if(e->kind==3||e->kind==4||e->kind==6){
  unsigned char *all=data;if(e->kind==6){data[1]=timekeeper_bcd(g_date_time_get_year(t)/100);}
  unsigned off=e->kind==6?8:0;
  unsigned char *d=all+off;
  d[0]&=e->kind==4?0:0x3f;d[1]=timekeeper_bcd(g_date_time_get_second(t));d[2]=timekeeper_bcd(g_date_time_get_minute(t));d[3]=timekeeper_bcd(g_date_time_get_hour(t));d[4]=(unsigned char)g_date_time_get_day_of_week(t);d[5]=timekeeper_bcd(g_date_time_get_day_of_month(t));d[6]=timekeeper_bcd(g_date_time_get_month(t));d[7]=timekeeper_bcd(g_date_time_get_year(t)%100);
 }else if(e->kind==5){
  data[0]=timekeeper_bcd(g_date_time_get_second(t));data[2]=timekeeper_bcd(g_date_time_get_minute(t));data[4]=timekeeper_bcd(g_date_time_get_hour(t));data[6]=g_date_time_get_day_of_week(t);data[7]=timekeeper_bcd(g_date_time_get_day_of_month(t));data[8]=timekeeper_bcd(g_date_time_get_month(t));data[9]=timekeeper_bcd(g_date_time_get_year(t)%100);data[10]=0x20;data[11]=2;data[1]=data[3]=data[5]=0xc0;
 }else{
  data[0]=0;data[1]=timekeeper_bcd(g_date_time_get_second(t));data[2]=timekeeper_bcd(g_date_time_get_minute(t));data[4]=timekeeper_bcd(g_date_time_get_hour(t));data[6]=(unsigned char)g_date_time_get_day_of_week(t);data[8]=timekeeper_bcd(g_date_time_get_day_of_month(t));data[9]=(data[9]&0x40)|timekeeper_bcd(g_date_time_get_month(t));data[10]=timekeeper_bcd(g_date_time_get_year(t)%100);data[11]=(data[11]&0x7c)|0x80;
 }
 for(size_t i=0;i<e->count;i++){char v[3];g_snprintf(v,sizeof v,"%02X",data[i]);gtk_editable_set_text(GTK_EDITABLE(e->entry[i]),v);}
 g_date_time_unref(t);gtk_label_set_text(GTK_LABEL(e->message),"Local time copied into fields; nothing has been written. Weekday convention: Monday=1. Adjust if your equipment uses another convention.");
}
static void timekeeper_editor_open(app_widgets*w){
 lt48_small_eprom_resources*r=NULL;if(lt48_small_eprom_open(w->installation,w->selected_device_index,&r)!=LT48_OK)return;
 int kind=lt48_small_eprom_is_timekeeper(r);uint32_t base=0;size_t count=0;lt48_timekeeper_region(r,&base,&count);lt48_small_eprom_close(r);if(!kind)return;
 static const char*dallas[]={"Hundredths (BCD)","Seconds (BCD)","Minutes (BCD)","Minute alarm / mask","Hours / 12-hour mode","Hour alarm / mask","Weekday (1–7)","Weekday alarm / mask","Date (BCD)","Month / oscillator stop / square-wave disable","Year (BCD, 00–99)","Command: TE, interrupt controls; flags read-only","Watchdog hundredths (BCD)","Watchdog seconds (BCD)"};
 static const char*st[]={"Control: W=0, R, calibration sign/magnitude","Seconds / oscillator stop (bit 7)","Minutes (BCD)","Hours (24-hour BCD)","Weekday (1–7); frequency-test must be zero","Date (BCD)","Month (BCD)","Year (BCD, 00–99)"};
 timekeeper_editor*e=g_new0(timekeeper_editor,1);e->widgets=w;e->device=w->selected_device_index;e->revision=w->operation_input_revision;e->kind=kind;e->count=count;
 e->window=GTK_WINDOW(gtk_window_new());gtk_window_set_title(e->window,"Timekeeper clock and controls");gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);
 GtkWidget*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,6);gtk_window_set_child(e->window,box);
 GtkWidget*intro=gtk_label_new("Explicit hexadecimal register values from the buffer, not a live read.\nRAM Program and Erase preserve these registers. Read the chip first to back them up.\nClock Configuration writes only these fields; pending Dallas alarm flags cannot be restored.");gtk_box_append(GTK_BOX(box),intro);
 GtkWidget*grid=gtk_grid_new();gtk_grid_set_column_spacing(GTK_GRID(grid),12);gtk_box_append(GTK_BOX(box),grid);
 int known=0;
 if(w->device_buffer&&lt48_buffer_size(w->device_buffer)>=base+e->count)lt48_buffer_range_known(w->device_buffer,base,e->count,&known);
 unsigned char defaults[16]={0,0,0,0x80,0,0x80,1,0x81,1,0x41,0,0x8c,0,0};
 if(kind==3||kind==4||kind==6){unsigned char v[8]={0,0,0,0,1,1,1,0};memset(defaults,0,16);memcpy(defaults+(kind==6?8:0),v,8);if(kind==6){defaults[1]=0x20;defaults[5]=defaults[6]=1;}}
 if(kind==5){unsigned char v[14]={0,0xc0,0,0xc0,0,0xc0,1,1,1,0,0x20,2,0,0};memcpy(defaults,v,14);}
 static const char*mux[]={"Seconds","Seconds alarm","Minutes","Minutes alarm","Hours / PM","Hours alarm","Weekday","Date","Month","Year","A: normal divider / rate (bank preserved)","B: mode / interrupts (SET must be zero)","C: status (read-only; reading clears flags)","D: battery status (read-only)"};
 static const char*ext[]={"Flags (read-only; read acknowledges alarms)","Century (BCD)","Alarm seconds / RPT1","Alarm minutes / RPT2","Alarm hours / RPT3","Alarm date / RPT4 / RPT5","Alarm month / AFE / ABE","Watchdog"};
 for(size_t i=0;i<e->count;i++){
  char label[160],value[3];g_snprintf(label,sizeof label,"%04X  %s",base+(unsigned)i,kind==5?mux[i]:kind==6&&i<8?ext[i]:(kind==3||kind==4||kind==6)?(kind==4&&i==0?"Control: W=0, R; low six bits zero":st[i-(kind==6?8:0)]):dallas[i]);
  gtk_grid_attach(GTK_GRID(grid),gtk_label_new(label),0,(int)i,1,1);
  e->entry[i]=gtk_entry_new();gtk_entry_set_max_length(GTK_ENTRY(e->entry[i]),2);gtk_editable_set_width_chars(GTK_EDITABLE(e->entry[i]),3);
  g_snprintf(value,sizeof value,"%02X",known?lt48_buffer_data(w->device_buffer)[base+i]:defaults[i]);gtk_editable_set_text(GTK_EDITABLE(e->entry[i]),value);gtk_grid_attach(GTK_GRID(grid),e->entry[i],1,(int)i,1,1);if((kind==5&&i>=12)||(kind==6&&i==0))gtk_editable_set_editable(GTK_EDITABLE(e->entry[i]),FALSE);
 }
 e->message=gtk_label_new(known?"Loaded from current buffer. Review the timestamp before restoring it.":"No known clock bytes in the buffer: shown values are defaults, not device contents.");gtk_label_set_wrap(GTK_LABEL(e->message),TRUE);gtk_box_append(GTK_BOX(box),e->message);
 GtkWidget*now=gtk_button_new_with_label("Use current local time");gtk_box_append(GTK_BOX(box),now);g_signal_connect(now,"clicked",G_CALLBACK(timekeeper_now),e);
 GtkWidget*apply=gtk_button_new_with_label("Prepare clock-register write");gtk_box_append(GTK_BOX(box),apply);g_signal_connect(apply,"clicked",G_CALLBACK(timekeeper_apply),e);
 g_object_set_data_full(G_OBJECT(e->window),"timekeeper-editor",e,g_free);gtk_window_present(e->window);
}
