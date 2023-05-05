#include <dlfcn.h>
#include <string>

#include <Framework/Utils/StringUtils.h>
#include "pluginWrapper/DecoderPluginWrapper.h"


DecoderPluginWrapper::DecoderPluginWrapper(void *handle):
    PluginHolder(handle)
{
}
/*
DecoderPluginWrapper::DecoderPluginWrapper(DecoderPluginWrapper &&src)
{
}
*/
DecoderPluginWrapper::DecoderPluginWrapper(PluginHolder &&src) :
    PluginHolder(std::move(src))
{
}

DecoderPluginWrapper::~DecoderPluginWrapper() noexcept
{
}

long long DecoderPluginWrapper::decodeData(PluginCtxPtr &ctx, size_t size, SessionPtr &sess)
{
    auto ret = (*decode_function)(ctx.get(), size, sess.get());
    return ret;
}

DecoderPluginWrapper::DecoderPluginPtr DecoderPluginWrapper::loadPlugin(const std::string &fileName)
{
    PluginHolder::PluginHolderPtr base = PluginHolder::loadPlugin(fileName);
    DecoderPluginPtr plugin = std::make_shared<DecoderPluginWrapper>(std::move(*base));
    base.reset();

    decode_ptr decode_function = plugin->exportFunction<decode_ptr>(DECODE_PLUGIN_FUNC_STR);
    plugin->decode_function = decode_function;
    return plugin;
}
