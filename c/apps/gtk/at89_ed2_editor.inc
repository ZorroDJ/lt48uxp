/* Source-defined boot/security fields remain separate from the memory image. */
static unsigned ed2_fields(unsigned flags){return flags==4?8:7;}
static const char *ed2_label(unsigned flags,unsigned field) {
    static const char *ed[]={"XRAM config bit", "Bootloader jump bit", "X2 hardware bit (1 = standard clock)"};
    static const char *id[]={"XRAM config bit", "Oscillator bit", "Bootloader jump bit", "X2 hardware bit (1 = standard clock)"};
    static const char *cc[]={"Bootloader jump bit", "X2 hardware bit (1 = standard clock)"};
    static const char *numbers[]={"Status byte (hex)","Boot vector (hex)","Software security byte (hex)","Extra byte (hex)"};
    if(field<flags)return flags==2?cc[field]:flags==4?id[field]:ed[field];
    return field==flags?"Lock level (0–3)":field<ed2_fields(flags)?numbers[field-flags-1]:"Invalid field";
}
typedef struct {app_widgets *widgets;GtkWindow *window;GtkWidget *enable[16],*value[16],*error;unsigned count,flags;size_t device;uint64_t revision;} ed2_editor;
static gboolean ed2_parse_field(unsigned flags,unsigned field,const char *text,unsigned char *out) {
    if(flags<2 || flags>4 || field>=ed2_fields(flags) || !text || !*text)return FALSE;
    int hex=field>flags;
    if(hex && strlen(text)>2)return FALSE;
    for(const char *p=text;*p;p++)if(hex?!g_ascii_isxdigit(*p):!g_ascii_isdigit(*p))return FALSE;
    char *end;guint64 n=g_ascii_strtoull(text,&end,hex?16:10);
    if(*end || n>(field<flags?1u:field==flags?3u:255u))return FALSE;
    *out=(unsigned char)n;return TRUE;
}
static char *ed2_configuration_message(unsigned flags,const unsigned char values[8],gboolean edited) {
    GString *message=g_string_new(edited?"Configuration written and verified.\n":"Configuration read.\n");
    for(unsigned i=0;i<ed2_fields(flags);i++)g_string_append_printf(message,i>flags?"%s: %02X\n":"%s: %u\n",ed2_label(flags,i),values[i]);
    g_string_append(message,"Cleanup completed.");return g_string_free(message,FALSE);
}
static void ed2_editor_action(GtkButton *button,gpointer data) {
    ed2_editor *e=data;app_widgets *w=e->widgets;
    if(!w->has_selected_device || e->device!=w->selected_device_index || e->revision!=w->operation_input_revision){gtk_window_destroy(e->window);return;}
    unsigned mask=0;unsigned char edit[9]={0};
    if(g_object_get_data(G_OBJECT(button),"apply"))for(unsigned i=0;i<e->count;i++) {
        if(!gtk_check_button_get_active(GTK_CHECK_BUTTON(e->enable[i])))continue;
        const char *text=gtk_editable_get_text(GTK_EDITABLE(e->value[i]));
        if(!ed2_parse_field(e->flags,i,text,edit+1+i)) {
            gtk_label_set_text(GTK_LABEL(e->error),"Invalid selected value. Flags 0/1; lock level 0–3; byte fields 00–FF hexadecimal.");return;
        }
        mask|=1u<<i;
    }
    edit[0]=(unsigned char)mask;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(w->operation_dropdown),9);
    memcpy(w->ed2_configuration,edit,sizeof(edit));gtk_window_destroy(e->window);
    w->read_after_prepare=0;start_operation_preparation(w,LT48_OPERATION_CONFIGURATION);
}
static void ed2_editor_open(app_widgets *w) {
    lt48_catalogue_entry device;
    if(lt48_installation_get_device(w->installation,w->selected_device_index,&device)!=LT48_OK)return;
    ed2_editor *e=g_new0(ed2_editor,1);e->flags=device.device_type>=0x14b?2:device.device_type==0x14a?4:3;e->count=ed2_fields(e->flags);e->widgets=w;e->device=w->selected_device_index;e->revision=w->operation_input_revision;
    e->window=GTK_WINDOW(gtk_window_new());gtk_window_set_title(e->window,"AT89C51ED2/ID2/CC03 configuration");
    gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);gtk_window_set_default_size(e->window,650,650);
    GtkWidget *scroll=gtk_scrolled_window_new(),*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);
    gtk_window_set_child(e->window,scroll);gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),box);
    GtkWidget *intro=gtk_label_new("Read current settings, or check the fields to change. Unchecked fields are preserved.\nThese are new values, not a live read. Chip erase is required to reduce protection.\nLocked chips reject boot-setting changes. Byte fields use hexadecimal 00–FF. Code and secondary memory regions are in the memory buffer.");gtk_label_set_wrap(GTK_LABEL(intro),TRUE);gtk_box_append(GTK_BOX(box),intro);
    for(unsigned i=0;i<e->count;i++) {
        GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);gtk_box_append(GTK_BOX(box),row);
        e->enable[i]=gtk_check_button_new_with_label(ed2_label(e->flags,i));gtk_box_append(GTK_BOX(row),e->enable[i]);
        e->value[i]=gtk_entry_new();gtk_widget_set_hexpand(e->value[i],TRUE);gtk_editable_set_text(GTK_EDITABLE(e->value[i]),i<=e->flags?"0":i==e->flags+2?"FC":"FF");gtk_box_append(GTK_BOX(row),e->value[i]);
    }
    e->error=gtk_label_new("");gtk_label_set_wrap(GTK_LABEL(e->error),TRUE);gtk_box_append(GTK_BOX(box),e->error);
    GtkWidget *read=gtk_button_new_with_label("Prepare configuration read"),*apply=gtk_button_new_with_label("Prepare selected changes");
    gtk_box_append(GTK_BOX(box),read);gtk_box_append(GTK_BOX(box),apply);g_object_set_data(G_OBJECT(apply),"apply",GINT_TO_POINTER(1));
    g_signal_connect(read,"clicked",G_CALLBACK(ed2_editor_action),e);g_signal_connect(apply,"clicked",G_CALLBACK(ed2_editor_action),e);
    g_object_set_data_full(G_OBJECT(e->window),"ed2-editor",e,g_free);gtk_window_present(e->window);
}
