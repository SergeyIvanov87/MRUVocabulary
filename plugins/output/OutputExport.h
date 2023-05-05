#ifndef OUTPUT_PLUGIN_INTERFACE_EXPORT_H
#define OUTPUT_PLUGIN_INTERFACE_EXPORT_H
#include "BasePluginExports.h"

#define WRITE_TRANSLATED_DATA_PLUGIN_FUNC              write_translated_data
#define WRITE_TRANSLATED_DATA_PLUGIN_FUNC_STR          "write_translated_data"

EXTERN_BEGIN
long long        WRITE_TRANSLATED_DATA_PLUGIN_FUNC             (plugin_ctx_t* ctx, session_t *translated_ctx);

typedef long long (*write_translated_data_ptr)                 (plugin_ctx_t* ctx, session_t *translated_ctx);

EXTERN_END

#endif //OUTPUT_PLUGIN_INTERFACE_EXPORT_H
