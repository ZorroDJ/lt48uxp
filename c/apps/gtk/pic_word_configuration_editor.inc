/* Shared buffer editor for configuration regions exposed by the C owner. */
typedef struct {
 app_widgets *widgets;GtkWindow *window;GtkWidget **entries;
 lt48_pic_resources *resources;size_t count,device;
 uint64_t revision,buffer_revision;
} pic_word_editor;
static void pic_word_editor_free(gpointer data){
 pic_word_editor *e=data;lt48_pic_close(e->resources);g_free(e->entries);g_free(e);
}
static gboolean pic_word_editor_commit(pic_word_editor *e){
 app_widgets *w=e->widgets;
 if(!w->has_selected_device||w->selected_device_index!=e->device||
    w->operation_input_revision!=e->revision||w->buffer_revision!=e->buffer_revision||
    !w->device_buffer||lt48_buffer_size(w->device_buffer)!=lt48_pic_capacity(e->resources))return FALSE;
 size_t size=lt48_buffer_size(w->device_buffer);unsigned char *image=g_memdup2(lt48_buffer_data(w->device_buffer),size);
 unsigned encoding_mask=lt48_pic_configuration_encoding_mask(e->resources);
 lt48_status status=LT48_OK;
 for(size_t i=0;i<e->count&&status==LT48_OK;i++){
  lt48_pic_configuration_word word;status=lt48_pic_configuration_word_info(e->resources,i,&word);
  const char *text=gtk_editable_get_text(GTK_EDITABLE(e->entries[i]));char *end=NULL;
  if(!text||!*text){status=LT48_INVALID_ARGUMENT;break;}
  unsigned long value=strtoul(text,&end,16);
  if(*end||value>0xffff||(value>encoding_mask&&value!=0xffff)){status=LT48_INVALID_ARGUMENT;break;}
  if(status!=LT48_OK)break;
  value=(value&word.value_mask)|(encoding_mask^word.value_mask);
  image[word.image_offset]=(unsigned char)value;image[word.image_offset+1]=(unsigned char)(value>>8);
 }
 if(status==LT48_OK)status=lt48_pic_validate(e->resources,LT48_OPERATION_PROGRAM,0,size,image,size);
 if(status==LT48_OK)status=lt48_buffer_write(w->device_buffer,0,image,size);
 g_free(image);
 if(status!=LT48_OK){gtk_label_set_text(GTK_LABEL(w->read_preparation),"Configuration rejected; check hexadecimal values, protection encoding and full device image.");return FALSE;}
 w->buffer_revision++;invalidate_operation_preparation(w,0);
 gtk_label_set_text(GTK_LABEL(w->read_preparation),"Configuration updated in the buffer. Program writes these values; no hardware operation has run.");
 return TRUE;
}
static void pic_word_editor_apply(GtkButton *button,gpointer data){
 (void)button;pic_word_editor *e=data;
 if(pic_word_editor_commit(e))gtk_window_destroy(e->window);
}
static GtkWindow *pic_word_configuration_sheet(app_widgets *w,lt48_pic_resources *r){
 size_t n=lt48_pic_configuration_word_count(r);
 if(!n||n>64||!w->device_buffer||lt48_buffer_size(w->device_buffer)!=lt48_pic_capacity(r)){lt48_pic_close(r);return NULL;}
 pic_word_editor *e=g_new0(pic_word_editor,1);
 e->widgets=w;e->resources=r;e->count=n;e->device=w->selected_device_index;
 e->revision=w->operation_input_revision;e->buffer_revision=w->buffer_revision;
 e->entries=g_new0(GtkWidget *,n);e->window=GTK_WINDOW(gtk_window_new());
 gtk_window_set_title(e->window,"PIC configuration words");gtk_window_set_transient_for(e->window,w->window);gtk_window_set_modal(e->window,TRUE);
 GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);gtk_window_set_child(e->window,box);
 gtk_box_append(GTK_BOX(box),gtk_label_new("Edit hexadecimal configuration words according to the selected device's datasheet. Program applies them from the buffer; Secure changes protection separately."));
 for(size_t i=0;i<n;i++){
  lt48_pic_configuration_word word;
  if(lt48_pic_configuration_word_info(r,i,&word)!=LT48_OK){pic_word_editor_free(e);gtk_window_destroy(GTK_WINDOW(gtk_widget_get_root(box)));return NULL;}
  char *label=g_strdup_printf("Word %zu · image byte %04X · mask %04X",i+1,word.image_offset,word.value_mask);
  gtk_box_append(GTK_BOX(box),gtk_label_new(label));g_free(label);
  e->entries[i]=gtk_entry_new();gtk_entry_set_max_length(GTK_ENTRY(e->entries[i]),4);
  const unsigned char *p=lt48_buffer_data(w->device_buffer)+word.image_offset;
  char text[5];g_snprintf(text,sizeof(text),"%04X",p[0]|(unsigned)p[1]<<8);
  gtk_editable_set_text(GTK_EDITABLE(e->entries[i]),text);gtk_box_append(GTK_BOX(box),e->entries[i]);
 }
 GtkWidget *apply=gtk_button_new_with_label("Apply to buffer");gtk_box_append(GTK_BOX(box),apply);
 g_signal_connect(apply,"clicked",G_CALLBACK(pic_word_editor_apply),e);
 g_object_set_data_full(G_OBJECT(e->window),"pic-word-editor",e,pic_word_editor_free);
 return e->window;
}
static gboolean pic_word_configuration_editor_open(app_widgets *w){
 lt48_pic_resources *r=NULL;if(lt48_pic_open(w->installation,w->selected_device_index,&r)!=LT48_OK)return FALSE;
 GtkWindow *sheet=pic_word_configuration_sheet(w,r);
 if(sheet)gtk_window_present(sheet);
 else gtk_label_set_text(GTK_LABEL(w->read_preparation),"PIC configuration editing requires a complete device buffer.");
 return TRUE;
}
