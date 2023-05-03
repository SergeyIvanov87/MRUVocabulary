#ifndef PLUGIN_CTX_H
#define PLUGIN_CTX_H
#include "BasePluginDeclarations.h"

EXTERN_BEGIN
typedef struct plugin_ctx
{
    int version;
    const char *id;
    size_t data_size;
    size_t data_offset;
    int err;
    void *data;
} plugin_ctx_t;


typedef struct shared_ctx
{
    int version;
    const char *id;
    void *data;
} shared_ctx_t;

EXTERN_END


#endif //PLUGIN_CTX_H
