/* Source-defined boot/security fields remain separate from the memory image. */
static unsigned usb51_fields(unsigned flags){(void)flags;return 10;}
static const char *usb51_label(unsigned flags,unsigned field) {
    (void)flags;static const char *names[]={"Bootloader jump bit","X2 hardware bit (1 = standard clock)","Oscillator (0 disabled, 1 0–8, 2 0–16, 3 0–32 MHz)","Lock level (0–3)","Status byte (hex)","Boot vector (hex)","P1_CF (hex)","P3_CF (hex)","P4_CF (hex)","Software security byte (hex)"};
    return field<10?names[field]:"Invalid field";
}
typedef struct {app_widgets *widgets;GtkWindow *window;GtkWidget *enable[16],*value[16],*error;unsigned count,flags;size_t device;uint64_t revision;} usb51_editor;
static gboolean usb51_parse_field(unsigned flags,unsigned field,const char *text,unsigned char *out) {
    if(field>=usb51_fields(flags) || !text || !*text)return FALSE;
    int hex=field>=4;
    if(hex && strlen(text)>2)return FALSE;
    for(const char *p=text;*p;p++)if(hex?!g_ascii_isxdigit(*p):!g_ascii_isdigit(*p))return FALSE;
    char *end;guint64 n=g_ascii_strtoull(text,&end,hex?16:10);
    if(*end || n>(field<2?1u:field<4?3u:255u))return FALSE;
    *out=(unsigned char)n;return TRUE;
}
static char *usb51_configuration_message(unsigned flags,const unsigned char values[10],gboolean edited) {
    GString *message=g_string_new(edited?"Configuration written and verified.\n":"Configuration read.\n");
    for(unsigned i=0;i<usb51_fields(flags);i++)g_string_append_printf(message,i>flags?"%s: %02X\n":"%s: %u\n",usb51_label(flags,i),values[i]);
    g_string_append(message,"Cleanup completed.");return g_string_free(message,FALSE);
}
static void usb51_editor_action(GtkButton *button,gpointer data) {
    usb51_editor *e=data;app_widgets *w=e->widgets;
    if(!w->has_selected_device || e->device!=w->selected_device_index || e->revision!=w->operation_input_revision){gtk_window_destroy(e->window);return;}
    unsigned mask=0;unsigned char edit[12]={0};
    if(g_object_get_data(G_OBJECT(button),"apply"))for(unsigned i=0;i<e->count;i++) {
        if(!gtk_check_button_get_active(GTK_CHECK_BUTTON(e->enable[i])))continue;
        const char *text=gtk_editable_get_text(GTK_EDITABLE(e->value[i]));
        if(!usb51_parse_field(e->flags,i,text,edit+2+i)) {
            gtk_label_set_text(GTK_LABEL(e->error),"Invalid selected value. Flags 0/1; oscillator and lock level 0–3; byte fields 00–FF hexadecimal.");return;
        }
        mask|=1u<<i;
    }
    edit[0]=(unsigned char)mask;edit[1]=(unsigned char)(mask>>8);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(w->operation_dropdown),9);
    memcpy(w->usb51_configuration,edit,sizeof(edit));gtk_window_destroy(e->window);
    w->read_after_prepare=0;start_operation_preparation(w,LT48_OPERATION_CONFIGURATION);
}
static void usb51_editor_open(app_widgets *w) {
    lt48_catalogue_entry device;
    if(lt48_installation_get_device(w->installation,w->selected_device_index,&device)!=LT48_OK)return;
    usb51_editor *e=g_new0(usb51_editor,1);e->flags=3;e->count=usb51_fields(e->flags);e->widgets=w;e->device=w->selected_device_index;e->revision=w->operation_input_revision;
    e->window=GTK_WINDOW(gtk_window_new());gtk_window_set_title(e->window,"AT89C5130/C5131 configuration");
    gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);gtk_window_set_default_size(e->window,650,650);
    GtkWidget *scroll=gtk_scrolled_window_new(),*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);
    gtk_window_set_child(e->window,scroll);gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),box);
    GtkWidget *intro=gtk_label_new("Read current settings, or check the fields to change. Unchecked fields are preserved.\nThese are new values, not a live read. Chip erase is required to reduce protection.\nLocked chips reject boot-setting changes. Byte fields use hexadecimal 00–FF. Code and secondary memory regions are in the memory buffer.");gtk_label_set_wrap(GTK_LABEL(intro),TRUE);gtk_box_append(GTK_BOX(box),intro);
    for(unsigned i=0;i<e->count;i++) {
        GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);gtk_box_append(GTK_BOX(box),row);
        e->enable[i]=gtk_check_button_new_with_label(usb51_label(e->flags,i));gtk_box_append(GTK_BOX(row),e->enable[i]);
        e->value[i]=gtk_entry_new();gtk_widget_set_hexpand(e->value[i],TRUE);gtk_editable_set_text(GTK_EDITABLE(e->value[i]),i<=e->flags?"0":i==e->flags+2?"3F":"FF");gtk_box_append(GTK_BOX(row),e->value[i]);
    }
    e->error=gtk_label_new("");gtk_label_set_wrap(GTK_LABEL(e->error),TRUE);gtk_box_append(GTK_BOX(box),e->error);
    GtkWidget *read=gtk_button_new_with_label("Prepare configuration read"),*apply=gtk_button_new_with_label("Prepare selected changes");
    gtk_box_append(GTK_BOX(box),read);gtk_box_append(GTK_BOX(box),apply);g_object_set_data(G_OBJECT(apply),"apply",GINT_TO_POINTER(1));
    g_signal_connect(read,"clicked",G_CALLBACK(usb51_editor_action),e);g_signal_connect(apply,"clicked",G_CALLBACK(usb51_editor_action),e);
    g_object_set_data_full(G_OBJECT(e->window),"usb51-editor",e,g_free);gtk_window_present(e->window);
}
