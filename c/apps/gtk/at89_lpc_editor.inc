/* Generated configuration editor. */
/* Source-defined boot/security fields remain separate from the memory image. */
static unsigned lpc_fields(unsigned flags){(void)flags;return 7;}
static const char *lpc_label(unsigned flags,unsigned field) {
    (void)flags;static const char *names[]={"CLKR / CPU clock rate (0–1)","BOV / brownout voltage selection (0–1)","PRHI / port reset high (0–1)","RPD / reset pin disabled (0–1)","WDTE / watchdog enabled (0–1)","Oscillator selection (0–4)","Security level (0–2)"};
    return field<7?names[field]:"Invalid field";
}
typedef struct {app_widgets *widgets;GtkWindow *window;GtkWidget *enable[16],*value[16],*error;unsigned count,flags;size_t device;uint64_t revision;} lpc_editor;
static gboolean lpc_parse_field(unsigned flags,unsigned field,const char *text,unsigned char *out) {
    if(field>=lpc_fields(flags) || !text || !*text)return FALSE;
    static const unsigned radix[]={10,10,10,10,10,10,10},maximum[]={1,1,1,1,1,4,2};
    int hex=radix[field]==16;
    if(strlen(text)>(hex?2u:3u))return FALSE;
    for(const char *p=text;*p;p++)if(hex?!g_ascii_isxdigit(*p):!g_ascii_isdigit(*p))return FALSE;
    char *end;guint64 n=g_ascii_strtoull(text,&end,radix[field]);
    if(*end || n>maximum[field])return FALSE;
    *out=(unsigned char)n;return TRUE;
}
static char *lpc_configuration_message(unsigned flags,const unsigned char values[7],gboolean edited) {
    GString *message=g_string_new(edited?"Configuration written and verified.\n":"Configuration read.\n");
    for(unsigned i=0;i<lpc_fields(flags);i++)g_string_append_printf(message,i>flags?"%s: %02X\n":"%s: %u\n",lpc_label(flags,i),values[i]);
    g_string_append(message,"Cleanup completed.");return g_string_free(message,FALSE);
}
static void lpc_editor_action(GtkButton *button,gpointer data) {
    lpc_editor *e=data;app_widgets *w=e->widgets;
    if(!w->has_selected_device || e->device!=w->selected_device_index || e->revision!=w->operation_input_revision){gtk_window_destroy(e->window);return;}
    unsigned mask=0;unsigned char edit[8]={0};
    if(g_object_get_data(G_OBJECT(button),"apply"))for(unsigned i=0;i<e->count;i++) {
        if(!gtk_check_button_get_active(GTK_CHECK_BUTTON(e->enable[i])))continue;
        const char *text=gtk_editable_get_text(GTK_EDITABLE(e->value[i]));
        if(!lpc_parse_field(e->flags,i,text,edit+1+i)) {
            gtk_label_set_text(GTK_LABEL(e->error),"Enter a value within the range shown in the field label.");return;
        }
        mask|=1u<<i;
    }
    edit[0]=(unsigned char)mask;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(w->operation_dropdown),9);
    memcpy(w->lpc_configuration,edit,sizeof(edit));gtk_window_destroy(e->window);
    w->read_after_prepare=0;start_operation_preparation(w,LT48_OPERATION_CONFIGURATION);
}
static void lpc_editor_open(app_widgets *w) {
    lt48_catalogue_entry device;
    if(lt48_installation_get_device(w->installation,w->selected_device_index,&device)!=LT48_OK)return;
    lpc_editor *e=g_new0(lpc_editor,1);e->flags=6;e->count=lpc_fields(e->flags);e->widgets=w;e->device=w->selected_device_index;e->revision=w->operation_input_revision;
    e->window=GTK_WINDOW(gtk_window_new());gtk_window_set_title(e->window,"Philips/NXP 85/87LPC configuration");
    gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);gtk_window_set_default_size(e->window,650,650);
    GtkWidget *scroll=gtk_scrolled_window_new(),*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);
    gtk_window_set_child(e->window,scroll);gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),box);
    GtkWidget *intro=gtk_label_new("Read settings, or check fields to change. Unchecked fields are preserved. Values below are proposed changes, not a live read.\nOTP bits can only change from 1 to 0. No electrical erase. Flags are raw UCFG1 bits.\nOscillator: 0 external clock, 1 internal RC, 2 low-frequency crystal, 3 medium-frequency crystal, 4 high-frequency crystal.\nSecurity: 0 unlocked, 1 programming disabled, 2 programming and verification disabled. Settings remain separate from the code/customer image.");gtk_label_set_wrap(GTK_LABEL(intro),TRUE);gtk_box_append(GTK_BOX(box),intro);
    static const char *defaults[]={"1","1","1","1","1","0","0"};
    for(unsigned i=0;i<e->count;i++) {
        GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);gtk_box_append(GTK_BOX(box),row);
        e->enable[i]=gtk_check_button_new_with_label(lpc_label(e->flags,i));gtk_box_append(GTK_BOX(row),e->enable[i]);
        e->value[i]=gtk_entry_new();gtk_widget_set_hexpand(e->value[i],TRUE);gtk_editable_set_text(GTK_EDITABLE(e->value[i]),defaults[i]);gtk_box_append(GTK_BOX(row),e->value[i]);
    }
    e->error=gtk_label_new("");gtk_label_set_wrap(GTK_LABEL(e->error),TRUE);gtk_box_append(GTK_BOX(box),e->error);
    GtkWidget *read=gtk_button_new_with_label("Prepare configuration read"),*apply=gtk_button_new_with_label("Prepare selected changes");
    gtk_box_append(GTK_BOX(box),read);gtk_box_append(GTK_BOX(box),apply);g_object_set_data(G_OBJECT(apply),"apply",GINT_TO_POINTER(1));
    g_signal_connect(read,"clicked",G_CALLBACK(lpc_editor_action),e);g_signal_connect(apply,"clicked",G_CALLBACK(lpc_editor_action),e);
    g_object_set_data_full(G_OBJECT(e->window),"lpc-editor",e,g_free);gtk_window_present(e->window);
}
