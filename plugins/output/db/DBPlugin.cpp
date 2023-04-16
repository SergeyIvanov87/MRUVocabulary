#include <stdio.h>
#include <stdarg.h>

#include "DBPlugin.h"
#include "interfaces/PluginCtx.h"
#include <fstream>
#include <cassert>
#include <list>
#include <string.h>
#include <error.h>
#include "common/base_command_parser/RegexFilterParam.h"
#include "common/base_command_parser/CommandArguments.hpp"

void __attribute__((constructor)) initPlugin(void)
{
    printf("DB Plugin is initialized\n");
}

void __attribute__ ((destructor)) cleanPlugin(void)
{
    printf("DB Plugin is exited\n");
}

static void check_ctx(plugin_ctx_t* ctx)
{
    if(!ctx or !ctx->data)
    {
        std::string ret("CTX is empty in plugin: ");
        ret += NAME_PLUGIN_FUNC();
        perror(ret.c_str());
        abort();
    }

    if(strncmp(ctx->id,  NAME_PLUGIN_FUNC(), strlen(NAME_PLUGIN_FUNC())))
    {
        std::string ret("CTX type invalid: ");
        ret += ctx->id;
        ret += std::string(". Expected: ") + NAME_PLUGIN_FUNC();
        perror(ret.c_str());
        abort();
    }

    if(ctx->version != DB_CURRENT_VERSION)
    {
        std::string ret("CTX version is mismatched: ");
        ret += std::to_string(ctx->version);
        ret += std::string(". Expected: ") + std::to_string(DB_CURRENT_VERSION);
        perror(ret.c_str());
        abort();
    }
}


plugin_ctx_t* INIT_PLUGIN_FUNC(const u_int8_t *data, size_t size)
{
    plugin_ctx_t *ctx = (plugin_ctx_t*)calloc(1, sizeof(plugin_ctx_t));
    ctx->version = DB_CURRENT_VERSION;
    ctx->id = NAME_PLUGIN_FUNC();


    db_context_v0 *inner_ctx = new db_context_v0;

    //TODO inner_ctx->connectionPool = std::make_unique<DBConnectionPool>();

    inner_ctx->property_holder = CommandArgumentsBase::create<CommandArgumentsBase,
                                                              RegexFilterValue>();

    ctx->data = reinterpret_cast<void*>(inner_ctx);
    return ctx;
}

bool SET_PARAM_PLUGIN_FUNC(plugin_ctx_t* ctx, const char* param_name, ...)
{
    check_ctx(ctx);
    db_context_v0 *inner_ctx = reinterpret_cast<db_context_v0*>(ctx->data);

    int argc = 1;
    va_list valist;
    va_start(valist, param_name);
    std::string value;
    for (int i = 0; i < 1; i++)
    {
        value.assign(va_arg(valist, const char*));
    }
    va_end(valist);

    const char *argv[] {param_name, value.c_str()};
    try
    {
        inner_ctx->property_holder->makeParamsExtract(argc, argv);
    }
    catch(const std::exception &ex)
    {
        printf("%s - error: %s", __FUNCTION__, ex.what());
        return false;
    }
    return true;
}

bool GET_PARAM_PLUGIN_FUNC(plugin_ctx_t* ctx, const char* param_name, ...)
{
    return false;
}


bool SET_TYPED_PARAMS_PLUGIN_FUNC(plugin_ctx_t* ctx, int argc, const void *data[])
{
    check_ctx(ctx);
    db_context_v0 *inner_ctx = reinterpret_cast<db_context_v0*>(ctx->data);

    try
    {
        inner_ctx->property_holder->makeParamsDeserialize(argc, data);
    }
    catch(const std::exception &ex)
    {
        printf("%s - error: %s", __FUNCTION__, ex.what());
        return false;
    }
    return true;
}


long long WRITE_TRANSLATED_DATA_PLUGIN_FUNC(plugin_ctx_t* ctx, shared_translated_data_t *translated_ctx)
{
    printf("%s - NOT IMPLEMENTED", __FUNCTION__);
    (void)ctx;
    (void)translated_ctx;
    return 0;
}

void RELEASE_PLUGIN_FUNC(plugin_ctx_t* ctx)
{
    if (ctx)
    {
        check_ctx(ctx);
        delete reinterpret_cast<db_context_v0*>(ctx->data);
    }
    free(ctx);
}

const char* NAME_PLUGIN_FUNC()
{
    return DB_PLUGIN_NAME;
}
