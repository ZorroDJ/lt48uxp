#ifndef LABTOOL48_GTK_PROPERTY_SHEET_H
#define LABTOOL48_GTK_PROPERTY_SHEET_H

#include <gtk/gtk.h>
#include <labtool48/labtool48.h>

typedef void (*lt48_gtk_property_commit_func)(
    lt48_property_model *model,
    gpointer user_data
);

/*
 * Build a modal, transactional editor for a validated property model.
 * `model` and its borrowed strings/arrays must outlive the returned window.
 * Values are copied back and `commit` is called only after a successful OK.
 */
GtkWindow *lt48_gtk_property_sheet_new(
    GtkWindow *parent,
    GtkWindow *help_window,
    lt48_property_model *model,
    lt48_gtk_property_commit_func commit,
    gpointer user_data
);

#endif
