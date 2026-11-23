typedef struct {
 app_widgets *widgets;operation_prepare_task *prepared;size_t device;
 uint64_t revision,buffer_revision;
} pic18_j_confirmation;
static gboolean pic18_j_confirmation_needed(app_widgets *w) {
 if(!w->prepared_operation||w->prepared_operation->pic18_authorization)return FALSE;
 unsigned op=w->prepared_operation->operation;
 if(op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_OVER_PROGRAM&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_SECURE)return FALSE;
 lt48_pic18_resources *r=NULL;gboolean needed=lt48_pic18_open(w->installation,w->selected_device_index,&r)==LT48_OK&&lt48_pic18_is_j_series(r);
 lt48_pic18_close(r);return needed;
}
static gboolean pic18_j_confirmation_grant(pic18_j_confirmation *c,gboolean accepted) {
 app_widgets *w=c->widgets;
 if(!accepted||!w->has_selected_device||w->selected_device_index!=c->device||w->operation_input_revision!=c->revision||w->buffer_revision!=c->buffer_revision||!w->prepared_operation||w->prepared_operation!=c->prepared)return FALSE;
 w->prepared_operation->pic18_authorization=LT48_PIC18_ERASE_AUTHORIZED;return TRUE;
}
static void pic18_j_confirmation_response(GObject *source,GAsyncResult *result,gpointer data) {
 pic18_j_confirmation *c=data;GError *error=NULL;int choice=gtk_alert_dialog_choose_finish(GTK_ALERT_DIALOG(source),result,&error);g_clear_error(&error);
 app_widgets *w=c->widgets;gboolean granted=pic18_j_confirmation_grant(c,choice==1);g_free(c);
 if(granted)execute_prepared_operation(w);
 else gtk_label_set_text(GTK_LABEL(w->read_preparation),"Erase/rewrite cancelled; a changed selection or buffer requires fresh preparation.");
}
static void pic18_j_ask_confirmation(app_widgets *w) {
 pic18_j_confirmation *c=g_new0(pic18_j_confirmation,1);*c=(pic18_j_confirmation){w,w->prepared_operation,w->selected_device_index,w->operation_input_revision,w->buffer_revision};
 GtkAlertDialog *dialog=gtk_alert_dialog_new("Allow whole-chip erase for %s?",lt48_operation_kind_name(w->prepared_operation->operation));
 const char *buttons[]={"Cancel","Allow erase/rewrite",NULL};
 gtk_alert_dialog_set_detail(dialog,w->prepared_operation->operation==LT48_OPERATION_PROGRAM?"Program replaces the entire Flash image, including configuration. Existing contents will be erased.":w->prepared_operation->operation==LT48_OPERATION_ERASE?"Erase clears the entire Flash image and removes code protection.":"This operation reads a stable backup, erases the whole chip and rewrites the image. Secure preserves code and other configuration while enabling protection. Protected code cannot be recovered. A completed backup is retained even if rewriting fails.");
 gtk_alert_dialog_set_buttons(dialog,buttons);gtk_alert_dialog_set_default_button(dialog,0);gtk_alert_dialog_set_cancel_button(dialog,0);gtk_alert_dialog_choose(dialog,w->window,NULL,pic18_j_confirmation_response,c);g_object_unref(dialog);
}
