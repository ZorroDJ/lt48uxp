/* Configuration remains separate; memory image includes user signature. */
static const char *lp_large_labels[]={"CS0 programmed","CS1 programmed","SUT0 programmed","SUT1 programmed",
    "Reset pin as GPIO","Brownout disabled","On-chip debug enabled","ISP disabled except at power-on",
    "User-signature writes enabled","Quasi-bidirectional ports after reset","Reserved interface fuse (0 only)",
    "In-application programming enabled","Lock level (0–2)"};
typedef struct {app_widgets *widgets;GtkWindow *window;GtkWidget *enable[13],*value[13],*error;size_t device;uint64_t revision;} lp_large_editor;
static gboolean lp_large_parse_field(unsigned field,const char *text,unsigned char *out) {
    if(field>12 || !text || !*text)return FALSE;
    for(const char *p=text;*p;p++)if(!g_ascii_isdigit(*p))return FALSE;
    char *end;guint64 n=g_ascii_strtoull(text,&end,10);
    if(*end || n>(field==12?2:field==10?0:1))return FALSE;
    *out=(unsigned char)n;return TRUE;
}
static char *lp_large_configuration_message(const unsigned char values[13],gboolean edited) {
    GString *message=g_string_new(edited?"Configuration written and verified.\n":"Configuration read.\n");
    for(unsigned i=0;i<13;i++)g_string_append_printf(message,"%s: %u\n",lp_large_labels[i],values[i]);
    g_string_append(message,"Analog/signature page preserved. Cleanup completed.");return g_string_free(message,FALSE);
}
static void lp_large_editor_action(GtkButton *button,gpointer data) {
    lp_large_editor *e=data;app_widgets *w=e->widgets;
    if(!w->has_selected_device || e->device!=w->selected_device_index || e->revision!=w->operation_input_revision){gtk_window_destroy(e->window);return;}
    unsigned mask=0;unsigned char edit[15]={0};
    if(g_object_get_data(G_OBJECT(button),"apply"))for(unsigned i=0;i<13;i++) {
        if(!gtk_check_button_get_active(GTK_CHECK_BUTTON(e->enable[i])))continue;
        const char *text=gtk_editable_get_text(GTK_EDITABLE(e->value[i]));
        if(!lp_large_parse_field(i,text,edit+2+i)) {
            gtk_label_set_text(GTK_LABEL(e->error),"Invalid selected value. Flags 0/1; reserved fuse 0; lock level 0–2.");return;
        }
        mask|=1u<<i;
    }
    edit[0]=(unsigned char)mask;edit[1]=(unsigned char)(mask>>8);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(w->operation_dropdown),9);
    memcpy(w->lp_large_configuration,edit,sizeof(edit));gtk_window_destroy(e->window);
    w->read_after_prepare=0;start_operation_preparation(w,LT48_OPERATION_CONFIGURATION);
}
static void lp_large_editor_open(app_widgets *w) {
    lp_large_editor *e=g_new0(lp_large_editor,1);e->widgets=w;e->device=w->selected_device_index;e->revision=w->operation_input_revision;
    e->window=GTK_WINDOW(gtk_window_new());gtk_window_set_title(e->window,"AT89LP428/828/3240/6440 configuration");
    gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);gtk_window_set_default_size(e->window,650,650);
    GtkWidget *scroll=gtk_scrolled_window_new(),*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);
    gtk_window_set_child(e->window,scroll);gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),box);
    GtkWidget *intro=gtk_label_new("Read current settings, or check the fields to change. Unchecked fields are preserved.\nThese are new values, not a live read. Chip erase is required to reduce protection.\nLocked chips reject fuse changes. Code, data and user signature are in the memory buffer; the analog page is preserved.");gtk_label_set_wrap(GTK_LABEL(intro),TRUE);gtk_box_append(GTK_BOX(box),intro);
    for(unsigned i=0;i<13;i++) {
        GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);gtk_box_append(GTK_BOX(box),row);
        e->enable[i]=gtk_check_button_new_with_label(lp_large_labels[i]);gtk_box_append(GTK_BOX(row),e->enable[i]);
        e->value[i]=gtk_entry_new();gtk_widget_set_hexpand(e->value[i],TRUE);gtk_editable_set_text(GTK_EDITABLE(e->value[i]),"0");gtk_box_append(GTK_BOX(row),e->value[i]);
    }
    e->error=gtk_label_new("");gtk_label_set_wrap(GTK_LABEL(e->error),TRUE);gtk_box_append(GTK_BOX(box),e->error);
    GtkWidget *read=gtk_button_new_with_label("Prepare configuration read"),*apply=gtk_button_new_with_label("Prepare selected changes");
    gtk_box_append(GTK_BOX(box),read);gtk_box_append(GTK_BOX(box),apply);g_object_set_data(G_OBJECT(apply),"apply",GINT_TO_POINTER(1));
    g_signal_connect(read,"clicked",G_CALLBACK(lp_large_editor_action),e);g_signal_connect(apply,"clicked",G_CALLBACK(lp_large_editor_action),e);
    g_object_set_data_full(G_OBJECT(e->window),"lp_large-editor",e,g_free);gtk_window_present(e->window);
}
