#ifndef TRANSLATOR_PLUGIN_INTERFACE_H
#define TRANSLATOR_PLUGIN_INTERFACE_H
#include <memory>
#include <stdlib.h>

#include "interfaces/PluginInterface.h"

struct SharedTranslatedData;
struct SharedDecodedData;
struct ITranslatorPlugin// : virtual public IPlugin
{
    virtual ~ITranslatorPlugin() noexcept {};

    virtual long long translate(PluginCtxPtr &translator_ctx, SharedDecodedData &decoder_ctx) = 0;
    virtual SharedTranslatedData* getSharedCtx(PluginCtxPtr &ctx) const = 0;

    using cstr_ptr = std::unique_ptr<char, decltype(&free)>;
    virtual cstr_ptr sharedCtx2CStr(SharedTranslatedData &ctx) const = 0;
};
#endif //TRANSLATOR_PLUGIN_INTERFACE_H
