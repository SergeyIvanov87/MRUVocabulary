#ifndef PLUGIN_CTX_H
#define PLUGIN_CTX_H
#include "BasePluginDeclarations.h"

EXTERN_BEGIN
typedef struct plugin_ctx
{
    int version;
    const char *id;
    /** TODO move to PluginPrivate session ctx **/
    size_t data_size;
    size_t data_offset;
    int err;
    void *data;
} plugin_ctx_t;


typedef struct session
{
    int version;
    const char *id;
    void *data;
} session_t;

EXTERN_END


#endif //PLUGIN_CTX_H
