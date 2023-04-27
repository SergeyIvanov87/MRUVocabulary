#ifndef TRANSLATOR_PLUGIN_WRAPPER_H
#define TRANSLATOR_PLUGIN_WRAPPER_H
#include <plugins/translators/TranslatorInterface.h>
#include <plugins/translators/TranslatorExport.h>
#include "PluginHolder.h"

struct SharedDecodedData;
struct SharedTranslatedData;
class TranslatorPluginWrapper : public ITranslatorPlugin,
                                public PluginHolder
{
public:
    using TranslatorPluginPtr = std::shared_ptr<TranslatorPluginWrapper>;

    TranslatorPluginWrapper(void *handle);
    TranslatorPluginWrapper(TranslatorPluginWrapper &&src) = default;
    TranslatorPluginWrapper(PluginHolder &&src);
    ~TranslatorPluginWrapper() noexcept;

    long long translate(PluginCtxPtr &translator_ctx, SharedDecodedData &decoder_ctx) override;
    SharedTranslatedData* getSharedCtx(PluginCtxPtr &ctx) const override;
    ITranslatorPlugin::cstr_ptr sharedCtx2CStr(SharedTranslatedData &ctx) const override;

    static TranslatorPluginPtr loadPlugin(const std::string &fileName);
private:
    translate_data_ptr translate_data_function;
    get_translated_ctx_ptr get_shared_ctx_function;
    shared_ctx_2_cstr_ptr  shared_ctx_2_cstr_function;

};
#endif //OUTPUT_PLUGIN_WRAPPER_H
