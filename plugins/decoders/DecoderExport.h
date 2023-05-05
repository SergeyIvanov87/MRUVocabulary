#ifndef DECODER_PLUGIN_INTERFACE_EXPORT_H
#define DECODER_PLUGIN_INTERFACE_EXPORT_H
#include "BasePluginExports.h"

#define DECODE_PLUGIN_FUNC                   decode
#define DECODE_PLUGIN_FUNC_STR               "decode"


EXTERN_BEGIN
long long        DECODE_PLUGIN_FUNC             (plugin_ctx_t* ctx, size_t size, session_t* out_session);

typedef long long (*decode_ptr) (plugin_ctx_t* ctx, size_t size, session_t* out_session);
EXTERN_END

#endif //DECODER_PLUGIN_INTERFACE_EXPORT_H
