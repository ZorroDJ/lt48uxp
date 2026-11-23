/* Buffer-only PIC18 settings through the source-verified portable C API. */
#include "pic18_j_configuration_editor.inc"
#include "../../../NewDeviceLib/drafts/pic-population/pic18_k80_ui.h"
#include "../../../NewDeviceLib/drafts/pic-population/pic18_k80_properties.h"
static lt48_status pic18_configuration_document_open(const lt48_installation *x,size_t index,
 const lt48_device_spec *spec,lt48_property_document **document,lt48_property_model **authored) {
 *authored=NULL;
 lt48_status st=lt48_installation_open_device_property_document(x,index,LT48_PROPERTY_SHEET_CONFIGURATION,document);
 if(st==LT48_NOT_FOUND&&spec->configuration_set==25288&&
    lt48_pic18_k80_configuration_binding(spec->definition_resource,spec->family_code,spec->pinmap)) {
  *authored=lt48_pic18_k80_properties();return *authored?LT48_OK:LT48_NO_MEMORY;
 }
 return st;
}
typedef struct {
    app_widgets *widgets;lt48_pic18_resources *resources;size_t device;uint64_t revision;
    uint16_t configuration_set;
} pic18_configuration_editor;
static int pic18_configuration_key(const char *key,uint16_t set,
    lt48_runtime_configuration_value *value) {
    if(!key||!value)return 0;
    unsigned id,a,b;int end=0;
    if(sscanf(key,"configuration.%u.checkbox.%u.%u%n",&id,&a,&b,&end)==3&&
       key[end]==0&&id==set&&a<=255&&b<32) {
        *value=(lt48_runtime_configuration_value){-1,0,(uint16_t)((a<<8)|b),0};return 1;
    }
    end=0;
    if(sscanf(key,"configuration.%u.choice.%u%n",&id,&a,&end)==2&&key[end]==0&&id==set&&a<=65535) {
        *value=(lt48_runtime_configuration_value){-1,1,(uint16_t)a,0};return 1;
    }
    end=0;
    if(sscanf(key,"configuration.%u.number.%u%n",&id,&a,&end)==2&&key[end]==0&&id==set&&a<=65535) {
        *value=(lt48_runtime_configuration_value){-1,2,(uint16_t)a,0};return 1;
    }return 0;
}
static void pic18_configuration_editor_free(gpointer data) {
    pic18_configuration_editor *e=data;lt48_pic18_close(e->resources);g_free(e);
}
static void pic18_configuration_committed(lt48_property_model *model,gpointer data) {
    pic18_configuration_editor *e=data;app_widgets *w=e->widgets;
    if(!w->has_selected_device||w->selected_device_index!=e->device||w->operation_input_revision!=e->revision||!w->device_buffer) {
        gtk_label_set_text(GTK_LABEL(w->read_preparation),"Configuration edit discarded because the selected device or buffer changed.");return;
    }
    size_t count=0;for(size_t p=0;p<model->page_count;p++)count+=model->pages[p].control_count;
    if(count>1024)return;
    lt48_runtime_configuration_value *edits=g_new0(lt48_runtime_configuration_value,count?count:1);
    size_t n=0;lt48_status st=LT48_OK;
    for(size_t p=0;st==LT48_OK&&p<model->page_count;p++)for(size_t c=0;c<model->pages[p].control_count;c++) {
        lt48_property_control *control=model->pages[p].controls+c;
        if(!pic18_configuration_key(control->key,e->configuration_set,edits+n)){st=LT48_UNSUPPORTED;break;}
        edits[n++].value=control->value;
    }
    size_t size=lt48_buffer_size(w->device_buffer);unsigned char *stage=g_memdup2(lt48_buffer_data(w->device_buffer),size);
    if(st==LT48_OK)st=lt48_pic18_configuration_edit(e->resources,stage,size,edits,n);
    if(st==LT48_OK)st=lt48_buffer_write(w->device_buffer,0,stage,size);
    g_free(stage);g_free(edits);
    if(st==LT48_OK){invalidate_operation_preparation(w,0);gtk_label_set_text(GTK_LABEL(w->read_preparation),
        "Configuration and user IDs updated in the buffer. Program applies these settings to the selected device.");}
    else {char *message=g_strdup_printf("Configuration buffer edit failed: %s",lt48_status_string(st));
        gtk_label_set_text(GTK_LABEL(w->read_preparation),message);g_free(message);}
}
static gboolean pic18_configuration_editor_open(app_widgets *w) {
    if(pic18_j_configuration_editor_open(w))return TRUE;
    lt48_pic18_resources *r=NULL;
    if(lt48_pic18_open(w->installation,w->selected_device_index,&r)!=LT48_OK)return FALSE;
    size_t count=0;lt48_status st=w->device_buffer?lt48_pic18_configuration_values(r,
        lt48_buffer_data(w->device_buffer),lt48_buffer_size(w->device_buffer),NULL,0,&count):LT48_INVALID_ARGUMENT;
    if(st==LT48_UNSUPPORTED){lt48_pic18_close(r);return FALSE;}
    lt48_property_document *document=NULL;lt48_property_model *authored=NULL;lt48_device_spec spec;
    if(st==LT48_BUFFER_TOO_SMALL)st=lt48_installation_resolve_device(w->installation,w->selected_device_index,&spec);
    if(st==LT48_OK)st=pic18_configuration_document_open(w->installation,w->selected_device_index,&spec,&document,&authored);
    lt48_runtime_configuration_value *values=g_new0(lt48_runtime_configuration_value,count?count:1);
    if(st==LT48_OK)st=lt48_pic18_configuration_values(r,lt48_buffer_data(w->device_buffer),lt48_buffer_size(w->device_buffer),values,count,&count);
    lt48_property_model *model=authored?authored:document?lt48_property_document_model(document):NULL;
    size_t controls=0;if(model)for(size_t p=0;p<model->page_count;p++)controls+=model->pages[p].control_count;
    if(st==LT48_OK&&(controls!=count||count>1024))st=LT48_FORMAT_ERROR;
    unsigned char seen[1024]={0};
    for(size_t p=0;st==LT48_OK&&p<model->page_count;p++)for(size_t c=0;c<model->pages[p].control_count;c++) {
        lt48_property_control *control=model->pages[p].controls+c;lt48_runtime_configuration_value key;
        if(!pic18_configuration_key(control->key,spec.configuration_set,&key)){st=LT48_UNSUPPORTED;break;}
        size_t j=0;for(;j<count;j++)if(values[j].category==key.category&&values[j].field==key.field)break;
        if(j==count||j>=1024||seen[j]){st=LT48_UNSUPPORTED;break;}seen[j]=1;
        if((key.category==0&&control->kind!=LT48_PROPERTY_CHECKBOX)||
           (key.category==1&&control->kind!=LT48_PROPERTY_CHOICE)||
           (key.category==2&&control->kind!=LT48_PROPERTY_NUMBER)){st=LT48_FORMAT_ERROR;break;}
        control->value=values[j].value;
        if(control->kind==LT48_PROPERTY_CHOICE&&control->value>=control->metadata.choice.count)control->value=(uint32_t)control->metadata.choice.count;
    }
    g_free(values);
    if(st!=LT48_OK){char *message=g_strdup_printf("Configuration editor needs a full device buffer and imported settings metadata: %s",lt48_status_string(st));
        gtk_label_set_text(GTK_LABEL(w->read_preparation),message);g_free(message);lt48_property_document_close(document);lt48_pic18_k80_properties_free(authored);lt48_pic18_close(r);return TRUE;}
    pic18_configuration_editor *e=g_new0(pic18_configuration_editor,1);
    *e=(pic18_configuration_editor){w,r,w->selected_device_index,w->operation_input_revision,spec.configuration_set};
    GtkWindow *sheet=lt48_gtk_property_sheet_new(w->window,w->help_window,model,pic18_configuration_committed,e);
    if(!sheet){gtk_label_set_text(GTK_LABEL(w->read_preparation),"Configuration metadata failed portable-model validation.");
        pic18_configuration_editor_free(e);lt48_property_document_close(document);lt48_pic18_k80_properties_free(authored);return TRUE;}
    g_object_set_data_full(G_OBJECT(sheet),"pic18-configuration-editor",e,pic18_configuration_editor_free);
    g_signal_connect(sheet,"destroy",G_CALLBACK(configuration_document_destroyed),document);
    if(authored)g_object_set_data_full(G_OBJECT(sheet),"pic18-authored-properties",authored,lt48_pic18_k80_properties_free);
    gtk_window_present(sheet);return TRUE;
}
