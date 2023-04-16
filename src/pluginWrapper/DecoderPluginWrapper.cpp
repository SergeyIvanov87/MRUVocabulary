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

long long DecoderPluginWrapper::decodeData(PluginCtxPtr &ctx, size_t size)
{
    auto ret = (*decode_function)(ctx.get(), size);
    return ret;
}

SharedDecodedData* DecoderPluginWrapper::getSharedCtx(PluginCtxPtr &ctx) const
{
    return reinterpret_cast<SharedDecodedData*>((*get_shared_ctx)(ctx.get()));
}

DecoderPluginWrapper::DecoderPluginPtr DecoderPluginWrapper::loadPlugin(const std::string &fileName)
{
    PluginHolder::PluginHolderPtr base = PluginHolder::loadPlugin(fileName);
    DecoderPluginPtr plugin = std::make_shared<DecoderPluginWrapper>(std::move(*base));
    base.reset();

    decode_ptr decode_function = plugin->exportFunction<decode_ptr>(DECODE_PLUGIN_FUNC_STR);
    get_shared_ctx_ptr ctx_shared = plugin->exportFunction<get_shared_ctx_ptr>(GET_SHARED_PLUGIN_CTX_FUNC_STR);
    plugin->decode_function = decode_function;
    plugin->get_shared_ctx = ctx_shared;
    return plugin;
}
