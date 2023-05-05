#ifndef TRANSLATOR_PLUGIN_INTERFACE_EXPORT_H
#define TRANSLATOR_PLUGIN_INTERFACE_EXPORT_H
#include "BasePluginExports.h"


#define TRANSLATE_PLUGIN_FUNC                translate
#define TRANSLATE_PLUGIN_FUNC_STR            "translate"

#define SHARED_CTX_2_STRING_FUNC             shared_ctx_2_cstr
#define SHARED_CTX_2_STRING_FUNC_STR        "shared_ctx_2_cstr"

EXTERN_BEGIN
long long                       TRANSLATE_PLUGIN_FUNC                  (plugin_ctx_t* in_translator_ctx, session_t* in_decoder_session, session_t *out_translator_session);
char*                           SHARED_CTX_2_STRING_FUNC               (plugin_ctx_t* in_translator_ctx, session_t* in_translator_session);

typedef long long                    (*translate_data_ptr)             (plugin_ctx_t* in_translator_ctx, session_t* in_decoder_session, session_t *out_translator_session);
typedef char*                        (*shared_ctx_2_cstr_ptr)          (plugin_ctx_t* in_translator_ctx, session_t* in_translator_session);
EXTERN_END

#endif //TRANSLATOR_PLUGIN_INTERFACE_EXPORT_H
