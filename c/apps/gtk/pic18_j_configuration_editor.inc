/* J configuration is embedded in the final eight physical Flash bytes. */
#include "../../src/pic18_j.h"
#include "../../../NewDeviceLib/sources/pic-common/pic18_j_property.h"
typedef struct {
 app_widgets *widgets;lt48_pic18_j_owner *owner;lt48_pic18_j_property *property;
 size_t device;uint64_t revision;
} pic18_j_configuration_editor;
static void pic18_j_configuration_editor_free(gpointer data) {
 pic18_j_configuration_editor *e=data;lt48_pic18_j_property_close(e->property);lt48_pic18_j_owner_close(e->owner);g_free(e);
}
static void pic18_j_configuration_committed(lt48_property_model *model,gpointer data) {
 pic18_j_configuration_editor *e=data;app_widgets *w=e->widgets;
 if(!w->has_selected_device||w->selected_device_index!=e->device||w->operation_input_revision!=e->revision||!w->device_buffer) {
  gtk_label_set_text(GTK_LABEL(w->read_preparation),"Configuration edit discarded because the selected device or buffer changed.");return;
 }
 size_t size=lt48_buffer_size(w->device_buffer);unsigned char *stage=g_memdup2(lt48_buffer_data(w->device_buffer),size);
 lt48_status st=model==lt48_pic18_j_property_model(e->property)?lt48_pic18_j_property_commit(e->property,stage,size):LT48_INVALID_ARGUMENT;
 if(st==LT48_OK)st=lt48_buffer_write(w->device_buffer,0,stage,size);
 g_free(stage);
 if(st==LT48_OK){invalidate_operation_preparation(w,0);gtk_label_set_text(GTK_LABEL(w->read_preparation),"Configuration updated in the buffer. Program applies these settings to the selected device.");}
 else {char *message=g_strdup_printf("Configuration buffer edit failed: %s",lt48_status_string(st));gtk_label_set_text(GTK_LABEL(w->read_preparation),message);g_free(message);}
}
/* Caller owns the returned hidden sheet; construction consumes owner. */
static GtkWindow *pic18_j_configuration_sheet(app_widgets *w,lt48_pic18_j_owner *owner) {
 pic18_j_configuration_editor *e=g_new0(pic18_j_configuration_editor,1);
 *e=(pic18_j_configuration_editor){w,owner,NULL,w->selected_device_index,w->operation_input_revision};
 lt48_status st=w->device_buffer?lt48_pic18_j_property_create(lt48_pic18_j_owner_options(owner),lt48_buffer_data(w->device_buffer),lt48_buffer_size(w->device_buffer),&e->property):LT48_INVALID_ARGUMENT;
 GtkWindow *sheet=NULL;
 if(st==LT48_OK)sheet=lt48_gtk_property_sheet_new(w->window,w->help_window,lt48_pic18_j_property_model(e->property),pic18_j_configuration_committed,e);
 if(!sheet){gtk_label_set_text(GTK_LABEL(w->read_preparation),"J-series configuration needs a complete device buffer with valid settings.");pic18_j_configuration_editor_free(e);return NULL;}
 g_object_set_data_full(G_OBJECT(sheet),"pic18-j-configuration-editor",e,pic18_j_configuration_editor_free);return sheet;
}
static gboolean pic18_j_configuration_editor_open(app_widgets *w) {
 lt48_pic18_j_owner *owner=NULL;
 if(lt48_pic18_j_owner_open(w->installation,w->selected_device_index,&owner)!=LT48_OK)return FALSE;
 GtkWindow *sheet=pic18_j_configuration_sheet(w,owner);if(sheet)gtk_window_present(sheet);return TRUE;
}
