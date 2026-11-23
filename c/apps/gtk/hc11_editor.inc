/* Uses the existing one-shot configuration snapshot; no persistent hardware defaults. */
typedef struct {app_widgets *widgets;GtkWindow *window;GtkWidget *enable[4],*value[4];size_t device;uint64_t revision;} hc11_editor;
static void hc11_editor_prepare(GtkButton *button,gpointer data){
 hc11_editor *e=data;app_widgets *w=e->widgets;
 if(!w->has_selected_device||w->selected_device_index!=e->device||w->operation_input_revision!=e->revision){gtk_window_destroy(e->window);return;}
 unsigned char cfg[8]={0};gboolean apply=GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button),"hc11-apply"));
 for(unsigned i=0;i<4&&apply;i++){if(gtk_check_button_get_active(GTK_CHECK_BUTTON(e->enable[i])))cfg[0]|=1u<<i;cfg[i+1]=gtk_check_button_get_active(GTK_CHECK_BUTTON(e->value[i]));}
 gtk_drop_down_set_selected(GTK_DROP_DOWN(w->operation_dropdown),9);memcpy(w->rb2_configuration,cfg,8);gtk_window_destroy(e->window);w->read_after_prepare=0;start_operation_preparation(w,LT48_OPERATION_CONFIGURATION);
}
static void hc11_editor_open(app_widgets *w){
 hc11_editor *e=g_new0(hc11_editor,1);e->widgets=w;e->device=w->selected_device_index;e->revision=w->operation_input_revision;
 e->window=GTK_WINDOW(gtk_window_new());gtk_window_set_title(e->window,"HC11 CONFIG register");gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);
 GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);gtk_window_set_child(e->window,box);
 gtk_box_append(GTK_BOX(box),gtk_label_new("Read CONFIG, or select fields to change. Unselected bits are preserved.\nValues below are choices, not a live read. NOSEC can only be set to 1.\nBootstrap on an already secured mask erases EEPROM, RAM and CONFIG."));
 static const char *names[]={"EEON: EEPROM enable","ROMON: mask ROM enable","NOCOP: watchdog disable","NOSEC: disable optional security"};
 for(unsigned i=0;i<4;i++){GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);gtk_box_append(GTK_BOX(box),row);e->enable[i]=gtk_check_button_new_with_label(names[i]);e->value[i]=gtk_check_button_new_with_label("Set to 1");gtk_box_append(GTK_BOX(row),e->enable[i]);gtk_box_append(GTK_BOX(row),e->value[i]);if(i==3){gtk_check_button_set_active(GTK_CHECK_BUTTON(e->value[i]),TRUE);gtk_widget_set_sensitive(e->value[i],FALSE);}}
 GtkWidget *read=gtk_button_new_with_label("Prepare CONFIG read"),*apply=gtk_button_new_with_label("Prepare selected CONFIG changes");gtk_box_append(GTK_BOX(box),read);gtk_box_append(GTK_BOX(box),apply);g_object_set_data(G_OBJECT(apply),"hc11-apply",GINT_TO_POINTER(1));g_signal_connect(read,"clicked",G_CALLBACK(hc11_editor_prepare),e);g_signal_connect(apply,"clicked",G_CALLBACK(hc11_editor_prepare),e);g_object_set_data_full(G_OBJECT(e->window),"hc11-editor",e,g_free);gtk_window_present(e->window);
}
