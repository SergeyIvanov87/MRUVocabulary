#ifndef TRANSLATOR_PLUGIN_INTERFACE_H
#define TRANSLATOR_PLUGIN_INTERFACE_H
#include <memory>
#include <stdlib.h>

#include "interfaces/PluginInterface.h"

struct SharedDecodedData;
struct ITranslatorPlugin// : virtual public IPlugin
{
    virtual ~ITranslatorPlugin() noexcept {};

    virtual long long translate(PluginCtxPtr &translator_ctx, SessionPtr &in_decoder_session, SessionPtr &out_translator_session) = 0;

    using cstr_ptr = std::unique_ptr<char, decltype(&free)>;
    virtual cstr_ptr sharedCtx2CStr(PluginCtxPtr &translator_ctx, SessionPtr& in_translator_session) const = 0;
};
#endif //TRANSLATOR_PLUGIN_INTERFACE_H
