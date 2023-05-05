#ifndef TRANSLATOR_PLUGIN_WRAPPER_H
#define TRANSLATOR_PLUGIN_WRAPPER_H
#include <plugins/translators/TranslatorInterface.h>
#include <plugins/translators/TranslatorExport.h>
#include "PluginHolder.h"

struct SharedDecodedData;
class TranslatorPluginWrapper : public ITranslatorPlugin,
                                public PluginHolder
{
public:
    using TranslatorPluginPtr = std::shared_ptr<TranslatorPluginWrapper>;

    TranslatorPluginWrapper(void *handle);
    TranslatorPluginWrapper(TranslatorPluginWrapper &&src) = default;
    TranslatorPluginWrapper(PluginHolder &&src);
    ~TranslatorPluginWrapper() noexcept;

    long long translate(PluginCtxPtr &translator_ctx, SharedDecodedData &in_decoder_session, SessionPtr& out_translator_session) override;
    ITranslatorPlugin::cstr_ptr sharedCtx2CStr(PluginCtxPtr &translator_ctx, SessionPtr& in_translator_session) const override;

    static TranslatorPluginPtr loadPlugin(const std::string &fileName);
private:
    translate_data_ptr translate_data_function;
    shared_ctx_2_cstr_ptr  shared_ctx_2_cstr_function;

};
#endif //OUTPUT_PLUGIN_WRAPPER_H
