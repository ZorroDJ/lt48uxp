#include "property_sheet.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct property_binding {
    lt48_property_control *control;
    GtkWidget *widget;
} property_binding;

typedef struct property_sheet_state {
    GtkWindow *window;
    GtkWindow *help_window;
    lt48_property_model *model;
    lt48_gtk_property_commit_func commit;
    gpointer commit_data;
    GPtrArray *bindings;
    GtkWidget *status;
} property_sheet_state;

static char *format_number(uint32_t value, uint8_t radix) {
    if (radix == 16u) return g_strdup_printf("%X", value);
    if (radix == 8u) return g_strdup_printf("%o", value);
    if (radix == 2u) {
        char buffer[33];
        size_t position = sizeof(buffer) - 1u;
        buffer[position] = '\0';
        do {
            buffer[--position] = (char)('0' + (value & 1u));
            value >>= 1u;
        } while (value != 0u);
        return g_strdup(buffer + position);
    }
    return g_strdup_printf("%u", value);
}

static gboolean parse_number(
    const char *text,
    const lt48_property_number *number,
    uint32_t *out_value
) {
    char *end = NULL;
    guint64 value;
    if (text == NULL || text[0] == '\0') return FALSE;
    errno = 0;
    value = g_ascii_strtoull(text, &end, number->radix);
    if (errno == ERANGE || end == text || end == NULL || *end != '\0' ||
        value > UINT32_MAX || value < number->minimum ||
        value > number->maximum)
        return FALSE;
    *out_value = (uint32_t)value;
    return TRUE;
}

static void property_sheet_destroyed(GtkWidget *widget, gpointer user_data) {
    property_sheet_state *state = user_data;
    (void)widget;
    g_ptr_array_free(state->bindings, TRUE);
    g_free(state);
}

static void property_sheet_cancel(GtkButton *button, gpointer user_data) {
    property_sheet_state *state = user_data;
    (void)button;
    gtk_window_destroy(state->window);
}

static void property_sheet_help(GtkButton *button, gpointer user_data) {
    property_sheet_state *state = user_data;
    (void)button;
    if (state->help_window != NULL) {
        gtk_window_set_transient_for(state->help_window, state->window);
        gtk_window_present(state->help_window);
    }
}

static void property_sheet_accept(GtkButton *button, gpointer user_data) {
    property_sheet_state *state = user_data;
    size_t index;
    uint32_t *values;
    (void)button;
    values = g_new(uint32_t, state->bindings->len);
    for (index = 0u; index < state->bindings->len; ++index) {
        property_binding *binding = g_ptr_array_index(state->bindings, index);
        lt48_property_control *control = binding->control;
        uint32_t value;
        if (control->kind == LT48_PROPERTY_NUMBER) {
            if (!parse_number(
                    gtk_editable_get_text(GTK_EDITABLE(binding->widget)),
                    &control->metadata.number, &value)) {
                char *message = g_strdup_printf(
                    "%s must be between %u and %u in radix %u.",
                    control->label, control->metadata.number.minimum,
                    control->metadata.number.maximum,
                    control->metadata.number.radix);
                gtk_label_set_text(GTK_LABEL(state->status), message);
                gtk_widget_grab_focus(binding->widget);
                g_free(message);
                g_free(values);
                return;
            }
        } else if (control->kind == LT48_PROPERTY_CHECKBOX) {
            value = gtk_check_button_get_active(
                GTK_CHECK_BUTTON(binding->widget)) ? 1u : 0u;
        } else {
            value = gtk_drop_down_get_selected(GTK_DROP_DOWN(binding->widget));
            if (value >= control->metadata.choice.count) {
                gtk_label_set_text(
                    GTK_LABEL(state->status), "Select a value for every option.");
                gtk_widget_grab_focus(binding->widget);
                g_free(values);
                return;
            }
        }
        values[index] = value;
    }
    for (index = 0u; index < state->bindings->len; ++index) {
        property_binding *binding = g_ptr_array_index(state->bindings, index);
        binding->control->value = values[index];
    }
    g_free(values);
    if (state->commit != NULL)
        state->commit(state->model, state->commit_data);
    gtk_window_destroy(state->window);
}

static GtkWidget *property_page_new(
    property_sheet_state *state,
    lt48_property_page *page
) {
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *scroll = gtk_scrolled_window_new();
    size_t index;
    int row = 0;
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_widget_set_margin_top(grid, 12);
    gtk_widget_set_margin_bottom(grid, 12);
    gtk_widget_set_margin_start(grid, 12);
    gtk_widget_set_margin_end(grid, 12);
    gtk_widget_set_hexpand(grid, TRUE);
    for (index = 0u; index < page->control_count; ++index) {
        lt48_property_control *control = &page->controls[index];
        property_binding *binding = g_new0(property_binding, 1);
        GtkWidget *editor;
        binding->control = control;
        if (control->kind == LT48_PROPERTY_NUMBER) {
            GtkWidget *label = gtk_label_new(control->label);
            char *value = format_number(
                control->value, control->metadata.number.radix);
            editor = gtk_entry_new();
            gtk_editable_set_text(GTK_EDITABLE(editor), value);
            gtk_entry_set_input_purpose(
                GTK_ENTRY(editor), GTK_INPUT_PURPOSE_NUMBER);
            gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
            gtk_widget_set_hexpand(editor, TRUE);
            gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), editor, 1, row, 1, 1);
            g_free(value);
        } else if (control->kind == LT48_PROPERTY_CHECKBOX) {
            editor = gtk_check_button_new_with_label(control->label);
            gtk_check_button_set_active(
                GTK_CHECK_BUTTON(editor), control->value != 0u);
            gtk_grid_attach(GTK_GRID(grid), editor, 0, row, 2, 1);
        } else {
            GtkWidget *label = gtk_label_new(control->label);
            GtkStringList *items = gtk_string_list_new(NULL);
            size_t choice_index;
            for (choice_index = 0u;
                 choice_index < control->metadata.choice.count;
                 ++choice_index)
                gtk_string_list_append(
                    items, control->metadata.choice.labels[choice_index]);
            editor = gtk_drop_down_new(G_LIST_MODEL(items), NULL);
            gtk_drop_down_set_selected(
                GTK_DROP_DOWN(editor), control->value);
            gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
            gtk_widget_set_hexpand(editor, TRUE);
            gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), editor, 1, row, 1, 1);
            /* gtk_drop_down_new() takes full ownership of its model. */
        }
        gtk_accessible_update_property(
            GTK_ACCESSIBLE(editor), GTK_ACCESSIBLE_PROPERTY_LABEL,
            control->label, -1);
        binding->widget = editor;
        g_ptr_array_add(state->bindings, binding);
        ++row;
    }
    if (page->control_count == 0u) {
        GtkWidget *empty = gtk_label_new("This page has no editable values.");
        gtk_label_set_xalign(GTK_LABEL(empty), 0.0F);
        gtk_grid_attach(GTK_GRID(grid), empty, 0, 0, 2, 1);
    }
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);
    return scroll;
}

GtkWindow *lt48_gtk_property_sheet_new(
    GtkWindow *parent,
    GtkWindow *help_window,
    lt48_property_model *model,
    lt48_gtk_property_commit_func commit,
    gpointer user_data
) {
    property_sheet_state *state;
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *notebook;
    GtkWidget *actions;
    GtkWidget *spacer;
    GtkWidget *help;
    GtkWidget *cancel;
    GtkWidget *accept;
    size_t page_index;
    if (lt48_property_model_validate(model, NULL) != LT48_OK) return NULL;
    state = g_new0(property_sheet_state, 1);
    state->model = model;
    state->help_window = help_window;
    state->commit = commit;
    state->commit_data = user_data;
    state->bindings = g_ptr_array_new_with_free_func(g_free);
    window = gtk_window_new();
    state->window = GTK_WINDOW(window);
    gtk_window_set_title(state->window, model->title);
    gtk_window_set_transient_for(state->window, parent);
    gtk_window_set_destroy_with_parent(state->window, TRUE);
    gtk_window_set_modal(state->window, TRUE);
    gtk_window_set_default_size(state->window, 640, 480);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    notebook = gtk_notebook_new();
    gtk_widget_set_vexpand(notebook, TRUE);
    for (page_index = 0u; page_index < model->page_count; ++page_index) {
        GtkWidget *page = property_page_new(state, &model->pages[page_index]);
        GtkWidget *label = gtk_label_new(model->pages[page_index].title);
        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, label);
    }
    state->status = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(state->status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(state->status), TRUE);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    help = gtk_button_new_with_label("Help");
    cancel = gtk_button_new_with_label("Cancel");
    accept = gtk_button_new_with_label("OK");
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_widget_set_sensitive(help, help_window != NULL);
    gtk_box_append(GTK_BOX(actions), help);
    gtk_box_append(GTK_BOX(actions), spacer);
    gtk_box_append(GTK_BOX(actions), cancel);
    gtk_box_append(GTK_BOX(actions), accept);
    gtk_widget_set_margin_top(box, 10);
    gtk_widget_set_margin_bottom(box, 10);
    gtk_widget_set_margin_start(box, 10);
    gtk_widget_set_margin_end(box, 10);
    gtk_box_append(GTK_BOX(box), notebook);
    gtk_box_append(GTK_BOX(box), state->status);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_window_set_child(state->window, box);
    g_signal_connect(help, "clicked", G_CALLBACK(property_sheet_help), state);
    g_signal_connect(cancel, "clicked", G_CALLBACK(property_sheet_cancel), state);
    g_signal_connect(accept, "clicked", G_CALLBACK(property_sheet_accept), state);
    g_signal_connect(window, "destroy", G_CALLBACK(property_sheet_destroyed), state);
    return state->window;
}
