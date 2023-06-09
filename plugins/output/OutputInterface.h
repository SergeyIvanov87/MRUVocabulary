#ifndef OUTPUT_PLUGIN_INTERFACE_H
#define OUTPUT_PLUGIN_INTERFACE_H

#include "interfaces/PluginInterface.h"

struct IOutputPlugin// : public IPlugin
{
    virtual ~IOutputPlugin() noexcept {};

    virtual long long writeTranslatedData(PluginCtxPtr &ctx, SessionPtr &decoder_ctx) = 0;
};
#endif //OUTPUT_PLUGIN_INTERFACE_H
