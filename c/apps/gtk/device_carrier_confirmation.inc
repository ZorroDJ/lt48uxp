typedef struct {
    app_widgets *widgets;
    operation_prepare_task *prepared;
    size_t device;
    uint64_t revision, buffer_revision;
} device_carrier_confirmation;

static gboolean device_carrier_confirmation_needed(app_widgets *w) {
    lt48_device_carrier_info info;
    return w->has_selected_device && w->prepared_operation &&
        !w->prepared_operation->carrier_authorization &&
        lt48_installation_device_carrier_info(w->installation,
            w->selected_device_index, &info) == LT48_OK && info.requires_confirmation;
}

static gboolean device_carrier_confirmation_grant(device_carrier_confirmation *c,
                                                  gboolean accepted) {
    app_widgets *w = c->widgets;
    if (!accepted || !device_carrier_confirmation_needed(w) ||
        w->selected_device_index != c->device ||
        w->operation_input_revision != c->revision ||
        w->buffer_revision != c->buffer_revision ||
        w->prepared_operation != c->prepared) return FALSE;
    w->prepared_operation->carrier_authorization = 1;
    return TRUE;
}

static void device_carrier_confirmation_response(GObject *source, GAsyncResult *result,
                                                gpointer data) {
    device_carrier_confirmation *c = data;
    GError *error = NULL;
    int choice = gtk_alert_dialog_choose_finish(GTK_ALERT_DIALOG(source), result, &error);
    g_clear_error(&error);
    app_widgets *w = c->widgets;
    gboolean granted = device_carrier_confirmation_grant(c, choice == 1);
    g_free(c);
    if (granted) execute_prepared_operation(w);
    else gtk_label_set_text(GTK_LABEL(w->read_preparation),
        "Carrier confirmation cancelled; changed device or buffer requires fresh preparation.");
}

static void device_carrier_ask_confirmation(app_widgets *w) {
    lt48_device_carrier_info info;
    if (lt48_installation_device_carrier_info(w->installation,
            w->selected_device_index, &info) != LT48_OK) return;
    device_carrier_confirmation *c = g_new0(device_carrier_confirmation, 1);
    *c = (device_carrier_confirmation){w, w->prepared_operation,
        w->selected_device_index, w->operation_input_revision, w->buffer_revision};
    GtkAlertDialog *dialog = gtk_alert_dialog_new("Confirm %s programming carrier", info.physical_package);
    const char *buttons[] = {"Cancel", "Carrier wiring checked", NULL};
    gtk_alert_dialog_set_detail(dialog, info.note);
    gtk_alert_dialog_set_buttons(dialog, buttons);
    gtk_alert_dialog_set_default_button(dialog, 0);
    gtk_alert_dialog_set_cancel_button(dialog, 0);
    gtk_alert_dialog_choose(dialog, w->window, NULL, device_carrier_confirmation_response, c);
    g_object_unref(dialog);
}
