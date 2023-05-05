#ifndef DECODER_PLUGIN_INTERFACE_H
#define DECODER_PLUGIN_INTERFACE_H

#include "interfaces/PluginInterface.h"

struct IDecoderPlugin// : virtual public IPlugin
{
    virtual ~IDecoderPlugin() noexcept {};

    virtual long long decodeData(PluginCtxPtr &ctx, size_t size, SessionPtr &sess) = 0;
};
#endif //DECODER_PLUGIN_INTERFACE_H
