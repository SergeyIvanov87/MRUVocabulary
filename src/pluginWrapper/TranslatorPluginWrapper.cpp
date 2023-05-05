#include <dlfcn.h>
#include <string>

#include "decoders/vers/PluginDecodedData.h"
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
                                             SharedDecodedData &in_decoder_session,
                                             SessionPtr& out_translator_session)
{
    auto ret = (*translate_data_function)(translator_ctx.get(), &in_decoder_session, out_translator_session.get());
    return ret;
}

ITranslatorPlugin::cstr_ptr TranslatorPluginWrapper::sharedCtx2CStr(PluginCtxPtr &translator_ctx, SessionPtr& in_translator_session) const
{
    return ITranslatorPlugin::cstr_ptr((*shared_ctx_2_cstr_function)(translator_ctx.get(), in_translator_session.get()), &free);
}

TranslatorPluginWrapper::TranslatorPluginPtr TranslatorPluginWrapper::loadPlugin(const std::string &fileName)
{
    PluginHolder::PluginHolderPtr base = PluginHolder::loadPlugin(fileName);
    TranslatorPluginPtr plugin = std::make_shared<TranslatorPluginWrapper>(std::move(*base));
    base.reset();

    translate_data_ptr translate_data_function = plugin->exportFunction<translate_data_ptr>(TRANSLATE_PLUGIN_FUNC_STR);
    plugin->translate_data_function = translate_data_function;

    shared_ctx_2_cstr_ptr shared_ctx_2_cstr_function = plugin->exportFunction<shared_ctx_2_cstr_ptr>(SHARED_CTX_2_STRING_FUNC_STR);
    plugin->shared_ctx_2_cstr_function = shared_ctx_2_cstr_function;
    return plugin;
}
