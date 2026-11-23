/* One-shot source configuration choices; no values are claimed to be live. */
static const char *zilog_option_names[]={"ROM protection","RAM protection","Disable autolatch","Port 0 pull-ups","Port 1 pull-ups","Port 2 pull-ups","RC oscillator","32 kHz oscillator","Permanent watchdog"};
typedef struct {app_widgets*w;GtkWindow*window;GtkWidget*select[9];size_t device;uint64_t revision;} zilog_editor;
static void zilog_editor_prepare(GtkButton*b,gpointer data){
 zilog_editor*e=data;app_widgets*w=e->w;
 if(!w->has_selected_device||w->selected_device_index!=e->device||w->operation_input_revision!=e->revision){gtk_window_destroy(e->window);return;}
 unsigned mask=0;if(g_object_get_data(G_OBJECT(b),"apply"))for(unsigned i=0;i<9;i++)if(gtk_check_button_get_active(GTK_CHECK_BUTTON(e->select[i])))mask|=1u<<i;
 gtk_drop_down_set_selected(GTK_DROP_DOWN(w->operation_dropdown),9);
 memset(w->rb2_configuration,0,sizeof w->rb2_configuration);w->rb2_configuration[0]=mask;w->rb2_configuration[1]=mask>>8;w->rb2_configuration[2]=mask;w->rb2_configuration[3]=mask>>8;
 gtk_window_destroy(e->window);w->read_after_prepare=0;start_operation_preparation(w,LT48_OPERATION_CONFIGURATION);
}
static void zilog_editor_open(app_widgets*w){
 zilog_editor*e=g_new0(zilog_editor,1);e->w=w;e->device=w->selected_device_index;e->revision=w->operation_input_revision;e->window=GTK_WINDOW(gtk_window_new());gtk_window_set_title(e->window,"Zilog MUZE OTP options");gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);
 GtkWidget*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);gtk_window_set_child(e->window,box);gtk_box_append(GTK_BOX(box),gtk_label_new("Read current options, or select options to program permanently.\nUnselected and reserved cells are preserved. OTP options cannot be cleared.\nROM protection prevents later raw code Read. Choices below are not a live read."));
 for(unsigned i=0;i<9;i++){e->select[i]=gtk_check_button_new_with_label(zilog_option_names[i]);gtk_box_append(GTK_BOX(box),e->select[i]);}
 GtkWidget*read=gtk_button_new_with_label("Prepare option read"),*apply=gtk_button_new_with_label("Prepare permanent selected options");gtk_box_append(GTK_BOX(box),read);gtk_box_append(GTK_BOX(box),apply);g_object_set_data(G_OBJECT(apply),"apply",GINT_TO_POINTER(1));g_signal_connect(read,"clicked",G_CALLBACK(zilog_editor_prepare),e);g_signal_connect(apply,"clicked",G_CALLBACK(zilog_editor_prepare),e);g_object_set_data_full(G_OBJECT(e->window),"zilog-editor",e,g_free);gtk_window_present(e->window);
}
