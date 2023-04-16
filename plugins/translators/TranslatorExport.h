#ifndef TRANSLATOR_PLUGIN_INTERFACE_EXPORT_H
#define TRANSLATOR_PLUGIN_INTERFACE_EXPORT_H
#include "BasePluginExports.h"


#define TRANSLATE_PLUGIN_FUNC                translate
#define TRANSLATE_PLUGIN_FUNC_STR            "translate"
#define GET_SHARED_TRANSLATED_CTX_FUNC       get_translated_ctx
#define GET_SHARED_TRANSLATED_CTX_FUNC_STR   "get_translated_ctx"

#define SHARED_CTX_2_STRING_FUNC             shared_ctx_2_cstr
#define SHARED_CTX_2_STRING_FUNC_STR        "shared_ctx_2_cstr"

EXTERN_BEGIN
typedef struct SharedDecodedData shared_decoded_data_t;
typedef struct SharedTranslatedData shared_translated_data_t;
long long                       TRANSLATE_PLUGIN_FUNC                  (plugin_ctx_t* translator_ctx, shared_decoded_data_t* decoder_ctx);
shared_translated_data_t*       GET_SHARED_TRANSLATED_CTX_FUNC         (plugin_ctx_t* translator_ctx);
char*                           SHARED_CTX_2_STRING_FUNC               (shared_translated_data_t* translated_ctx);

typedef long long                    (*translate_data_ptr)             (plugin_ctx_t* translator_ctx, shared_decoded_data_t* decoder_ctx);
typedef shared_translated_data_t*    (*get_translated_ctx_ptr)         (plugin_ctx_t* translator_ctx);
typedef char*                        (*shared_ctx_2_cstr_ptr)          (shared_translated_data_t* translated_ctx);
EXTERN_END

#endif //TRANSLATOR_PLUGIN_INTERFACE_EXPORT_H
