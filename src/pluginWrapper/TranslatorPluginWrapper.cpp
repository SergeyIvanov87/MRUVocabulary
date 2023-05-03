#include <dlfcn.h>
#include <string>

#include "decoders/PluginDecodedData.h"
#include "pluginWrapper/TranslatorPluginWrapper.h"


TranslatorPluginWrapper::TranslatorPluginWrapper(void *handle):
    PluginHolder(handle)
{
}

TranslatorPluginWrapper::TranslatorPluginWrapper(PluginHolder &&src) :
    PluginHolder(std::move(src))
{
}

TranslatorPluginWrapper::~TranslatorPluginWrapper() noexcept
{
}

long long TranslatorPluginWrapper::translate(PluginCtxPtr &translator_ctx,
                                             SharedDecodedData &decoder_ctx)
{
    auto ret = (*translate_data_function)(translator_ctx.get(), &decoder_ctx);
    return ret;
}

SharedTranslatedData* TranslatorPluginWrapper::getSharedCtx(PluginCtxPtr &ctx) const
{
    return reinterpret_cast<SharedTranslatedData*>((*get_shared_ctx_function)(ctx.get()));
}

ITranslatorPlugin::cstr_ptr TranslatorPluginWrapper::sharedCtx2CStr(SharedTranslatedData &ctx) const
{
    return ITranslatorPlugin::cstr_ptr((*shared_ctx_2_cstr_function)(&ctx), &free);
}

TranslatorPluginWrapper::TranslatorPluginPtr TranslatorPluginWrapper::loadPlugin(const std::string &fileName)
{
    PluginHolder::PluginHolderPtr base = PluginHolder::loadPlugin(fileName);
    TranslatorPluginPtr plugin = std::make_shared<TranslatorPluginWrapper>(std::move(*base));
    base.reset();

    translate_data_ptr translate_data_function = plugin->exportFunction<translate_data_ptr>(TRANSLATE_PLUGIN_FUNC_STR);
    plugin->translate_data_function = translate_data_function;

    get_translated_ctx_ptr get_shared_ctx_function = plugin->exportFunction<get_translated_ctx_ptr>(GET_SHARED_TRANSLATED_CTX_FUNC_STR);
    plugin->get_shared_ctx_function = get_shared_ctx_function;

    shared_ctx_2_cstr_ptr shared_ctx_2_cstr_function = plugin->exportFunction<shared_ctx_2_cstr_ptr>(SHARED_CTX_2_STRING_FUNC_STR);
    plugin->shared_ctx_2_cstr_function = shared_ctx_2_cstr_function;
    return plugin;
}
