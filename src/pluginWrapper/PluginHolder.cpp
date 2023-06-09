#include <dlfcn.h>
#include <string>

#include <Framework/Utils/StringUtils.h>
#include "pluginWrapper/PluginHolder.h"

PluginHolder::PluginHolder(void *handle) :
    dl_handle(handle)
{
}

PluginHolder::PluginHolder(PluginHolder &&src) :
    dl_handle(std::move(src.dl_handle)),
    init_function(std::move(src.init_function)),
    set_typed_params_function(std::move(src.set_typed_params_function)),
    releases_plugin_function(std::move(src.releases_plugin_function)),
    allocate_new_session_function(std::move(src.allocate_new_session_function)),
    releases_shared_ctx_function(std::move(src.releases_shared_ctx_function)),
    name(std::move(src.name))
{
    src.dl_handle = nullptr;
}

PluginHolder::~PluginHolder() noexcept
{
    if (dl_handle)
    {
        dlclose(dl_handle);
    }
}

const std::string &PluginHolder::getName() const
{
    return name;
}

PluginHolder::PluginCtxPtr PluginHolder::initPluginCtx(const u_int8_t *data, size_t size)
{
    PluginHolder::PluginCtxPtr ctx((*init_function)(data, size), releases_plugin_function);

    return ctx;
}

PluginHolder::SessionPtr PluginHolder::allocateSession(PluginCtxPtr &ctx, const u_int8_t *data, size_t size)
{
    PluginHolder::SessionPtr sess((*allocate_new_session_function)(ctx.get(), data, size), [&](session_t* sess_ctx)
        {
            //TODO swap nullptr on ctx, which must become member of PluginHolder
            this->releases_shared_ctx_function(nullptr, sess_ctx);
        });

    return sess;
}

bool PluginHolder::setParam(PluginCtxPtr &ctx, const ValueBase& param, const void *data)
{
    int argc = 2;
    const void *argv[]
    {
        param.name(),
        data
    };
    return (*set_typed_params_function)(ctx.get(), argc, argv);
}

void PluginHolder::exportImpl(void **exported, const std::string &name) const
{
    *exported = dlsym(dl_handle, name.c_str());
    if (!*exported)
    {
        throw std::runtime_error(makeString("Cannot bind plugin export by name: \"", name,
                                            "\". Error: ", dlerror()));
    }
}

PluginHolder::PluginHolderPtr PluginHolder::loadPlugin(const std::string &filename)
{
    std::string rel_path = std::string("./") + filename;
    void *dl_handle = dlopen(rel_path.c_str(), RTLD_NOW|RTLD_GLOBAL);
    if (!dl_handle)
    {
        throw std::runtime_error(makeString("Cannot load plugin: ", filename, ". Error: ", dlerror()));
    }

    dlerror();    /* Clear any existing error */

    PluginHolderPtr plugin = std::make_shared<PluginHolder>(dl_handle);

    try
    {
        init_ptr init_function = plugin->exportFunction<init_ptr>(INIT_PLUGIN_FUNC_STR);
        /*
        *(void**)(&init_function) = dlsym(dl_handle, INIT_PLUGIN_FUNC_STR);
        if (!init_function)
        {
            throw std::runtime_error(makeString("Cannot bind plugin function: \"", INIT_PLUGIN_FUNC_STR,
                                                "\" in plugin: ", filename, ". Error: ", dlerror()));
        }*/

        set_typed_params_ptr set_param_function = plugin->exportFunction<set_typed_params_ptr>(SET_TYPED_PARAMS_PLUGIN_FUNC_STR);
        release_plugin_ptr releases_plugin_function = plugin->exportFunction<release_plugin_ptr>(RELEASE_PLUGIN_FUNC_STR);

        alloc_session_ptr allocate_new_session_function = plugin->exportFunction<alloc_session_ptr>(ALLOCATE_SESSION_FUNC_STR);
        release_session_ptr releases_shared_ctx_function = plugin->exportFunction<release_session_ptr>(RELEASE_SESSION_FUNC_STR);
        name_ptr name = plugin->exportFunction<name_ptr>(NAME_PLUGIN_FUNC_STR);

        plugin->init_function = init_function;
        plugin->set_typed_params_function = set_param_function;
        plugin->releases_plugin_function = releases_plugin_function;
        plugin->allocate_new_session_function = allocate_new_session_function;
        plugin->releases_shared_ctx_function = releases_shared_ctx_function;
        plugin->name = (*name)();
    }
    catch(const std::exception &ex)
    {
        std::string text("Plugin: ");
        text = text + filename + " - " + ex.what();
        throw std::runtime_error(text);
    }

    return plugin;
}
