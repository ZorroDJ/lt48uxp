/* Display/search cache only. Operation owners independently reopen and validate
 * their source resources. No cached flag authorizes hardware execution. */
#ifdef G_OS_UNIX
#include <dlfcn.h>
#endif

static gboolean catalogue_hash_file(GChecksum *sum, const char *path,
    GCancellable *cancel) {
    GFile *file=g_file_new_for_path(path);
    GFileInputStream *in=g_file_read(file,cancel,NULL);
    g_object_unref(file);
    if(!in)return FALSE;
    unsigned char buffer[65536];gssize n;
    while((n=g_input_stream_read(G_INPUT_STREAM(in),buffer,sizeof buffer,cancel,NULL))>0)
        g_checksum_update(sum,buffer,(gsize)n);
    g_object_unref(in);return n==0;
}
static gint catalogue_path_compare(gconstpointer a,gconstpointer b) {
    return g_strcmp0(*(char *const *)a,*(char *const *)b);
}
static gboolean catalogue_hash_directory(GChecksum *sum,const char *path,
    gboolean objects,GCancellable *cancel) {
    GDir *dir=g_dir_open(path,0,NULL);if(!dir)return FALSE;
    GPtrArray *names=g_ptr_array_new_with_free_func(g_free);const char *name;
    while((name=g_dir_read_name(dir)))g_ptr_array_add(names,g_strdup(name));
    g_dir_close(dir);g_ptr_array_sort(names,catalogue_path_compare);
    gboolean ok=TRUE;
    for(guint i=0;ok&&i<names->len;i++) {
        name=g_ptr_array_index(names,i);
        char *child=g_build_filename(path,name,NULL);
        gboolean sub=g_file_test(child,G_FILE_TEST_IS_DIR);
        if(sub&&g_file_test(child,G_FILE_TEST_IS_SYMLINK)){g_free(child);ok=FALSE;break;}
        if(sub && (objects || !strcmp(name,"objects"))) {
            ok=catalogue_hash_directory(sum,child,TRUE,cancel);
        } else if(!sub && (objects || g_str_has_suffix(name,".rez") ||
                  !strcmp(name,"manifest.lt48") || g_str_has_suffix(name,".bin") ||
                  g_str_has_suffix(name,".exe") || g_str_has_suffix(name,".EXE") ||
                  g_str_has_suffix(name,".hex") || g_str_has_suffix(name,".HEX") ||
                  g_str_has_suffix(name,".BIN"))) {
            g_checksum_update(sum,(const guchar *)name,strlen(name)+1);
            ok=catalogue_hash_file(sum,child,cancel);
        }
        g_free(child);
    }
    g_ptr_array_unref(names);return ok;
}
static char *catalogue_fingerprint(const char *path,GCancellable *cancel) {
    if(!path||!*path)return NULL;
    GChecksum *sum=g_checksum_new(G_CHECKSUM_SHA256);
    const char *schema="lt48-gtk-index-v1:" __DATE__ ":" __TIME__;
    g_checksum_update(sum,(const guchar *)schema,strlen(schema)+1);
    char *canonical=g_canonicalize_filename(path,NULL);
    g_checksum_update(sum,(const guchar *)canonical,strlen(canonical)+1);
    gboolean ok=catalogue_hash_directory(sum,path,FALSE,cancel);
    g_free(canonical);
    /* Bind support decisions to the actual native library image, not just its
     * human version. On platforms without image discovery, disable reuse. */
#ifdef G_OS_UNIX
    Dl_info info;
    if(!dladdr((void *)(uintptr_t)lt48_version_string,&info)||!info.dli_fname)
        ok=FALSE;
    else if(ok)ok=catalogue_hash_file(sum,info.dli_fname,cancel);
#else
    ok=FALSE;
#endif
    char library[4096];lt48_status st=lt48_new_device_lib_default_path(library,sizeof library);
    if(ok&&st==LT48_OK) {
        char *manifest=g_build_filename(library,"library.json",NULL);
        g_checksum_update(sum,(const guchar *)library,strlen(library)+1);
        ok=catalogue_hash_file(sum,manifest,cancel);g_free(manifest);
        /* Include actual record content, so edits with unchanged manifest
         * text cannot leave the catalogue index looking current. */
        const char *dirs[]={"devices","corrections","applets","bitstreams","bundles"};
        for(size_t i=0;ok&&i<G_N_ELEMENTS(dirs);i++) {
            char *directory=g_build_filename(library,dirs[i],NULL);
            if(g_file_test(directory,G_FILE_TEST_IS_DIR))
                ok=catalogue_hash_directory(sum,directory,TRUE,cancel);
            g_free(directory);
        }
    } else if(st!=LT48_NOT_FOUND)ok=FALSE;
    char *result=ok?g_strdup(g_checksum_get_string(sum)):NULL;
    g_checksum_free(sum);return result;
}

static GBytes *catalogue_index_get(lt48_installation *x,const char *fingerprint,
    GCancellable *cancel,gboolean *cache_hit) {
    size_t count=lt48_installation_device_count(x);
    if(cache_hit)*cache_hit=FALSE;
    if(count>1000000u)return NULL;
    char *directory=g_build_filename(g_get_user_cache_dir(),"labtool48","catalogue",NULL);
    char *name=fingerprint?g_strconcat(fingerprint,".idx",NULL):NULL;
    char *path=name?g_build_filename(directory,name,NULL):NULL;
    g_free(name);
    /* Fixed-size, bounded format. Reject malformed, obsolete or edited data. */
    char *contents=NULL;gsize size=0;GStatBuf statbuf;
    if(path&&g_stat(path,&statbuf)==0&&statbuf.st_size==(gint64)(80+count*2)&&
       g_file_get_contents(path,&contents,&size,NULL)&&size==80+count*2&&
       !memcmp(contents,"LT48IDX1",8)&&!memcmp(contents+8,fingerprint,64)) {
        char *digest=g_compute_checksum_for_data(G_CHECKSUM_SHA256,
            (const guchar *)contents+80,count*2);
        /* Header includes an eight-byte payload digest prefix for accidental
         * corruption detection; it is not an authenticity credential. */
        gboolean valid=!memcmp(contents+72,digest,8);g_free(digest);
        guint16 *flags=g_new(guint16,count?count:1);
        for(size_t i=0;valid&&i<count;i++) {
            flags[i]=(guint8)contents[80+2*i]|((guint16)(guint8)contents[81+2*i]<<8);
            if(flags[i]&~0x1e1u)valid=FALSE;
        }
        g_free(contents);contents=NULL;
        if(valid) {
            if(cache_hit)*cache_hit=TRUE;
            g_free(path);g_free(directory);
            return g_bytes_new_take(flags,count*sizeof *flags);
        }
        g_free(flags);
    }
    g_free(contents);
    guint16 *flags=g_new0(guint16,count?count:1);
    for(size_t i=0;i<count;i++) {
        if(cancel&&g_cancellable_is_cancelled(cancel)){g_free(flags);g_free(path);g_free(directory);return NULL;}
        lt48_catalogue_entry e;lt48_operation_support support[LT48_OPERATION_KIND_COUNT];
        size_t n;const char *profile=NULL;int complete=0;
        if(lt48_installation_get_device(x,i,&e)!=LT48_OK)continue;
        if(lt48_installation_support_summary(x,i,support,G_N_ELEMENTS(support),&n,&profile,&complete)!=LT48_OK)continue;
        if(complete)flags[i]=1u|memory_filter_mask(x,i,&e,profile);
        if(e.device_class==2000u) {
            lt48_microwire_resources *m=NULL;
            if(lt48_microwire_open(x,i,&m)==LT48_OK&&!lt48_microwire_is_nvram(m))flags[i]|=1u<<5;
            lt48_microwire_close(m);
        }
    }
    if (path) {
        char *current=catalogue_fingerprint(lt48_installation_path(x),cancel);
        gboolean unchanged=current&&!strcmp(current,fingerprint);g_free(current);
        if(!unchanged){g_free(flags);g_free(path);g_free(directory);return NULL;}
    }
    if(path&&g_mkdir_with_parents(directory,0700)==0) {
        guchar *raw=g_malloc0(80+count*2);
        memcpy(raw,"LT48IDX1",8);memcpy(raw+8,fingerprint,64);
        for(size_t i=0;i<count;i++){raw[80+2*i]=(guchar)flags[i];raw[81+2*i]=(guchar)(flags[i]>>8);}
        char *digest=g_compute_checksum_for_data(G_CHECKSUM_SHA256,raw+80,count*2);
        memcpy(raw+72,digest,8);g_free(digest);
        g_file_set_contents_full(path,(const char *)raw,80+count*2,
            G_FILE_SET_CONTENTS_CONSISTENT,0600,NULL);g_free(raw);
    }
    g_free(path);g_free(directory);
    return g_bytes_new_take(flags,count*sizeof *flags);
}
