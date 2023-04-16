#ifndef DECODER_PLUGIN_INTERFACE_EXPORT_H
#define DECODER_PLUGIN_INTERFACE_EXPORT_H
#include "BasePluginExports.h"

#define DECODE_PLUGIN_FUNC                   decode
#define DECODE_PLUGIN_FUNC_STR               "decode"

#define GET_SHARED_PLUGIN_CTX_FUNC           get_shared_ctx
#define GET_SHARED_PLUGIN_CTX_FUNC_STR       "get_shared_ctx"

EXTERN_BEGIN
long long        DECODE_PLUGIN_FUNC             (plugin_ctx_t* ctx, size_t size);
void*            GET_SHARED_PLUGIN_CTX_FUNC     (plugin_ctx_t*);

typedef long long (*decode_ptr) (plugin_ctx_t* ctx, size_t size);
typedef void* (*get_shared_ctx_ptr)(plugin_ctx_t* ctx);
EXTERN_END

#endif //DECODER_PLUGIN_INTERFACE_EXPORT_H
