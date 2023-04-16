#include <dlfcn.h>
#include <string>

#include <Framework/Utils/StringUtils.h>
#include "pluginWrapper/OutputPluginWrapper.h"


OutputPluginWrapper::OutputPluginWrapper(void *handle):
    PluginHolder(handle)
{
}

OutputPluginWrapper::OutputPluginWrapper(PluginHolder &&src) :
    PluginHolder(std::move(src))
{
}

OutputPluginWrapper::~OutputPluginWrapper() noexcept
{
}

long long OutputPluginWrapper::writeTranslatedData(PluginCtxPtr &ctx, SharedTranslatedData &translated_ctx)
{
    auto ret = (*write_decoded_data_function)(ctx.get(), &translated_ctx);
    return ret;
}

OutputPluginWrapper::OutputPluginPtr OutputPluginWrapper::loadPlugin(const std::string &fileName)
{
    PluginHolder::PluginHolderPtr base = PluginHolder::loadPlugin(fileName);
    OutputPluginPtr plugin = std::make_shared<OutputPluginWrapper>(std::move(*base));
    base.reset();

    write_translated_data_ptr write_decoded_function = plugin->exportFunction<write_translated_data_ptr>(WRITE_TRANSLATED_DATA_PLUGIN_FUNC_STR);
    plugin->write_decoded_data_function = write_decoded_function;
    return plugin;
}
