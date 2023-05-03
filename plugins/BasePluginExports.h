#include "BasePluginDeclarations.h"

#ifndef BASE_PLUGIN_INTERFACE_H
#define BASE_PLUGIN_INTERFACE_H

#define INIT_PLUGIN_FUNC                     init
#define INIT_PLUGIN_FUNC_STR                 "init"
#define SET_PARAM_PLUGIN_FUNC                 set_param
#define SET_PARAM_PLUGIN_FUNC_STR            "set_param"
#define GET_PARAM_PLUGIN_FUNC                 get_param
#define GET_PARAM_PLUGIN_FUNC_STR            "get_param"
#define SET_TYPED_PARAMS_PLUGIN_FUNC          set_typed_params
#define SET_TYPED_PARAMS_PLUGIN_FUNC_STR     "set_typed_params"
#define GET_TYPED_PARAMS_PLUGIN_FUNC          get_typed_params
#define GET_TYPED_PARAMS_PLUGIN_FUNC_STR     "get_typed_params"

#define RELEASE_PLUGIN_FUNC                  release_plugin
#define RELEASE_PLUGIN_FUNC_STR              "release_plugin"

#define ALLOCATE_SESSION_FUNC                alloc_session
#define ALLOCATE_SESSION_FUNC_STR            "alloc_session"
#define RELEASE_SHARED_CTX_FUNC              release_shared_ctx
#define RELEASE_SHARED_CTX_FUNC_STR          "release_shared_ctx"
#define NAME_PLUGIN_FUNC                     name
#define NAME_PLUGIN_FUNC_STR                 "name"

#include <cstddef>
#include <stdlib.h>

EXTERN_BEGIN
typedef struct plugin_ctx plugin_ctx_t;
typedef struct shared_ctx shared_ctx_t;

plugin_ctx_t*    INIT_PLUGIN_FUNC               (const u_int8_t *data, size_t size);
bool             SET_PARAM_PLUGIN_FUNC          (plugin_ctx_t* ctx, const char* param_name, ...);
bool             GET_PARAM_PLUGIN_FUNC          (plugin_ctx_t* ctx, const char* param_name, ...);

bool             SET_TYPED_PARAMS_PLUGIN_FUNC   (plugin_ctx_t* ctx, int argc, const void *data[]);
bool             GET_TYPED_PARAMS_PLUGIN_FUNC   (plugin_ctx_t* ctx, int argc, const void *data[]);
void             RELEASE_PLUGIN_FUNC            (plugin_ctx_t* ctx);

shared_ctx_t*    ALLOCATE_SESSION_FUNC          (plugin_ctx_t* ctx, const u_int8_t *data, size_t size);
void             RELEASE_SHARED_CTX_FUNC        (shared_ctx_t* ctx);

const char*      NAME_PLUGIN_FUNC               ();


typedef plugin_ctx_t* (*init_ptr)(const u_int8_t *data, size_t size);
typedef shared_ctx_t* (*alloc_session_ptr)(plugin_ctx_t* ctx, const u_int8_t *data, size_t size);

typedef bool (*set_param_ptr)(plugin_ctx_t* ctx, const char* param_name, ...);
typedef bool (*get_param_ptr)(plugin_ctx_t* ctx, const char* param_name, ...);
typedef bool (*set_typed_params_ptr)(plugin_ctx_t* ctx, int argc, const void *data[]);
typedef bool (*get_typed_params_ptr)(plugin_ctx_t* ctx, int argc, const void *data[]);

typedef void (*release_plugin_ptr)(plugin_ctx_t* ctx);
typedef void (*release_session_ptr)(shared_ctx_t* ctx);
typedef const char* (*name_ptr)();
EXTERN_END
#endif
