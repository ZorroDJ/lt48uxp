/* Source-defined boot/security fields remain separate from the memory image. */
static unsigned lp51_basic_fields(unsigned flags){(void)flags;return 9;}
static const char *lp51_basic_label(unsigned flags,unsigned field) {
    (void)flags;static const char *names[]={"Compatibility mode flag (0 = compatible)","ISP disable","User signature programming enable","Tristate ports flag","In-application programming enable","R1 flag (0 = enabled)","Oscillator select (0–3)","Start-up time (0–3)","Lock level (0–3)"};
    return field<9?names[field]:"Invalid field";
}
typedef struct {app_widgets *widgets;GtkWindow *window;GtkWidget *enable[16],*value[16],*error;unsigned count,flags;size_t device;uint64_t revision;} lp51_basic_editor;
static gboolean lp51_basic_parse_field(unsigned flags,unsigned field,const char *text,unsigned char *out) {
    if(field>=lp51_basic_fields(flags) || !text || !*text)return FALSE;
    int hex=0;
    if(hex && strlen(text)>2)return FALSE;
    for(const char *p=text;*p;p++)if(hex?!g_ascii_isxdigit(*p):!g_ascii_isdigit(*p))return FALSE;
    char *end;guint64 n=g_ascii_strtoull(text,&end,hex?16:10);
    if(*end || n>(field<6?1u:3u))return FALSE;
    *out=(unsigned char)n;return TRUE;
}
static char *lp51_basic_configuration_message(unsigned flags,const unsigned char values[9],gboolean edited) {
    GString *message=g_string_new(edited?"Configuration written and verified.\n":"Configuration read.\n");
    for(unsigned i=0;i<lp51_basic_fields(flags);i++)g_string_append_printf(message,"%s: %u\n",lp51_basic_label(flags,i),values[i]);
    g_string_append(message,"Cleanup completed.");return g_string_free(message,FALSE);
}
static void lp51_basic_editor_action(GtkButton *button,gpointer data) {
    lp51_basic_editor *e=data;app_widgets *w=e->widgets;
    if(!w->has_selected_device || e->device!=w->selected_device_index || e->revision!=w->operation_input_revision){gtk_window_destroy(e->window);return;}
    unsigned mask=0;unsigned char edit[11]={0};
    if(g_object_get_data(G_OBJECT(button),"apply"))for(unsigned i=0;i<e->count;i++) {
        if(!gtk_check_button_get_active(GTK_CHECK_BUTTON(e->enable[i])))continue;
        const char *text=gtk_editable_get_text(GTK_EDITABLE(e->value[i]));
        if(!lp51_basic_parse_field(e->flags,i,text,edit+2+i)) {
            gtk_label_set_text(GTK_LABEL(e->error),"Flags must be 0 or 1; oscillator, start-up and lock level must be 0–3.");return;
        }
        mask|=1u<<i;
    }
    edit[0]=(unsigned char)mask;edit[1]=(unsigned char)(mask>>8);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(w->operation_dropdown),9);
    memcpy(w->lp51_basic_configuration,edit,sizeof(edit));gtk_window_destroy(e->window);
    w->read_after_prepare=0;start_operation_preparation(w,LT48_OPERATION_CONFIGURATION);
}
static void lp51_basic_editor_open(app_widgets *w) {
    lt48_catalogue_entry device;
    if(lt48_installation_get_device(w->installation,w->selected_device_index,&device)!=LT48_OK)return;
    lp51_basic_editor *e=g_new0(lp51_basic_editor,1);e->flags=6;e->count=lp51_basic_fields(e->flags);e->widgets=w;e->device=w->selected_device_index;e->revision=w->operation_input_revision;
    e->window=GTK_WINDOW(gtk_window_new());gtk_window_set_title(e->window,"AT89LP51/52 configuration");
    gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);gtk_window_set_default_size(e->window,650,650);
    GtkWidget *scroll=gtk_scrolled_window_new(),*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);
    gtk_window_set_child(e->window,scroll);gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),box);
    GtkWidget *intro=gtk_label_new("Read current settings, or check the fields to change. Unchecked fields are preserved.\nThese are new values, not a live read. Chip erase is required to reduce protection.\nLocked chips reject fuse changes. Flags use the original software encoding. Oscillator: 0 internal, 1 external clock, 2 low-speed crystal, 3 high-speed crystal. Start-up: 0 shortest through 3 longest. Lock: 0 unlocked through 3 strongest. The buffer contains code, 256-byte data flash, then 256-byte user signature.");gtk_label_set_wrap(GTK_LABEL(intro),TRUE);gtk_box_append(GTK_BOX(box),intro);
    for(unsigned i=0;i<e->count;i++) {
        GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);gtk_box_append(GTK_BOX(box),row);
        e->enable[i]=gtk_check_button_new_with_label(lp51_basic_label(e->flags,i));gtk_box_append(GTK_BOX(row),e->enable[i]);
        e->value[i]=gtk_entry_new();gtk_widget_set_hexpand(e->value[i],TRUE);gtk_editable_set_text(GTK_EDITABLE(e->value[i]),"0");gtk_box_append(GTK_BOX(row),e->value[i]);
    }
    e->error=gtk_label_new("");gtk_label_set_wrap(GTK_LABEL(e->error),TRUE);gtk_box_append(GTK_BOX(box),e->error);
    GtkWidget *read=gtk_button_new_with_label("Prepare configuration read"),*apply=gtk_button_new_with_label("Prepare selected changes");
    gtk_box_append(GTK_BOX(box),read);gtk_box_append(GTK_BOX(box),apply);g_object_set_data(G_OBJECT(apply),"apply",GINT_TO_POINTER(1));
    g_signal_connect(read,"clicked",G_CALLBACK(lp51_basic_editor_action),e);g_signal_connect(apply,"clicked",G_CALLBACK(lp51_basic_editor_action),e);
    g_object_set_data_full(G_OBJECT(e->window),"lp51_basic-editor",e,g_free);gtk_window_present(e->window);
}
