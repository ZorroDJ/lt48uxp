/* Configuration and user signature remain separate from the code buffer. */
static const char *lp21_labels[]={"CS0 programmed","CS1 programmed","SUT0 programmed","SUT1 programmed",
    "Reset pin as GPIO","Brownout disabled","On-chip debug enabled","ISP disabled except at power-on",
    "User-signature writes enabled","Quasi-bidirectional ports after reset","Reserved interface fuse (0 only)",
    "Lock level (0–2)","Oscillator trim (hex)"};
typedef struct {app_widgets *widgets;GtkWindow *window;GtkWidget *enable[14],*value[14],*error;size_t device;uint64_t revision;} lp21_editor;
static gboolean lp21_parse_field(unsigned field,const char *text,unsigned char *out) {
    if(field>12 || !text || !*text)return FALSE;
    for(const char *p=text;*p;p++)if(field==12?!g_ascii_isxdigit(*p):!g_ascii_isdigit(*p))return FALSE;
    char *end;guint64 n=g_ascii_strtoull(text,&end,field==12?16:10);
    if(*end || n>(field==12?255:field==11?2:field==10?0:1))return FALSE;
    *out=(unsigned char)n;return TRUE;
}
static gboolean lp21_parse_signature(const char *text,unsigned char out[64]) {
    unsigned n=0;int high=-1;if(!text)return FALSE;
    for(const char *p=text;*p;p++) {
        if(g_ascii_isspace(*p))continue;
        int digit=g_ascii_xdigit_value(*p);if(digit<0)return FALSE;
        if(high<0)high=digit;
        else {if(n>=64)return FALSE;out[n++]=(unsigned char)(high*16+digit);high=-1;}
    }
    return n==64 && high<0;
}
static char *lp21_configuration_message(const unsigned char values[77],gboolean edited) {
    GString *message=g_string_new(edited?"Configuration written and verified.\n":"Configuration read.\n");
    for(unsigned i=0;i<13;i++)g_string_append_printf(message,i==12?"%s: %02X\n":"%s: %u\n",lp21_labels[i],values[i]);
    g_string_append(message,"User signature (64 bytes):\n");
    for(unsigned i=0;i<64;i++)g_string_append_printf(message,"%02X%c",values[13+i],i%16==15?'\n':' ');
    g_string_append(message,"Cleanup completed.");return g_string_free(message,FALSE);
}
static void lp21_editor_action(GtkButton *button,gpointer data) {
    lp21_editor *e=data;app_widgets *w=e->widgets;
    if(!w->has_selected_device || e->device!=w->selected_device_index || e->revision!=w->operation_input_revision){gtk_window_destroy(e->window);return;}
    unsigned mask=0;unsigned char edit[79]={0};
    if(g_object_get_data(G_OBJECT(button),"apply"))for(unsigned i=0;i<14;i++) {
        if(!gtk_check_button_get_active(GTK_CHECK_BUTTON(e->enable[i])))continue;
        const char *text=gtk_editable_get_text(GTK_EDITABLE(e->value[i]));
        if(i==13?!lp21_parse_signature(text,edit+15):!lp21_parse_field(i,text,edit+2+i)) {
            gtk_label_set_text(GTK_LABEL(e->error),"Invalid selected value. Flags 0/1; reserved fuse 0; lock 0–2; trim 00–FF; signature exactly 64 hex bytes.");return;
        }
        mask|=1u<<i;
    }
    edit[0]=(unsigned char)mask;edit[1]=(unsigned char)(mask>>8);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(w->operation_dropdown),9);
    memcpy(w->lp21_configuration,edit,sizeof(edit));gtk_window_destroy(e->window);
    w->read_after_prepare=0;start_operation_preparation(w,LT48_OPERATION_CONFIGURATION);
}
static void lp21_editor_open(app_widgets *w) {
    lp21_editor *e=g_new0(lp21_editor,1);e->widgets=w;e->device=w->selected_device_index;e->revision=w->operation_input_revision;
    e->window=GTK_WINDOW(gtk_window_new());gtk_window_set_title(e->window,"AT89LP configuration and user signature");
    gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);gtk_window_set_default_size(e->window,650,650);
    GtkWidget *scroll=gtk_scrolled_window_new(),*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);
    gtk_window_set_child(e->window,scroll);gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),box);
    GtkWidget *intro=gtk_label_new("Read current settings, or check the fields to change. Unchecked fields are preserved.\nThese are new values, not a live read. Chip erase is required to reduce protection.\nLocked chips reject fuse/signature changes. Clock selection CS1=1, CS0=0 is reserved.");gtk_label_set_wrap(GTK_LABEL(intro),TRUE);gtk_box_append(GTK_BOX(box),intro);
    for(unsigned i=0;i<14;i++) {
        GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);gtk_box_append(GTK_BOX(box),row);
        e->enable[i]=gtk_check_button_new_with_label(i==13?"Replace 64-byte user signature":lp21_labels[i]);gtk_box_append(GTK_BOX(row),e->enable[i]);
        e->value[i]=gtk_entry_new();gtk_widget_set_hexpand(e->value[i],TRUE);gtk_editable_set_text(GTK_EDITABLE(e->value[i]),i==13?"":i==12?"00":"0");gtk_box_append(GTK_BOX(row),e->value[i]);
    }
    e->error=gtk_label_new("");gtk_label_set_wrap(GTK_LABEL(e->error),TRUE);gtk_box_append(GTK_BOX(box),e->error);
    GtkWidget *read=gtk_button_new_with_label("Prepare configuration read"),*apply=gtk_button_new_with_label("Prepare selected changes");
    gtk_box_append(GTK_BOX(box),read);gtk_box_append(GTK_BOX(box),apply);g_object_set_data(G_OBJECT(apply),"apply",GINT_TO_POINTER(1));
    g_signal_connect(read,"clicked",G_CALLBACK(lp21_editor_action),e);g_signal_connect(apply,"clicked",G_CALLBACK(lp21_editor_action),e);
    g_object_set_data_full(G_OBJECT(e->window),"lp21-editor",e,g_free);gtk_window_present(e->window);
}
