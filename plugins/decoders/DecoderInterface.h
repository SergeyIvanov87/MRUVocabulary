#ifndef DECODER_PLUGIN_INTERFACE_H
#define DECODER_PLUGIN_INTERFACE_H

#include "interfaces/PluginInterface.h"

struct SharedDecodedData;
struct IDecoderPlugin// : virtual public IPlugin
{
    virtual ~IDecoderPlugin() noexcept {};

    virtual long long decodeData(PluginCtxPtr &ctx, size_t size) = 0;
    virtual SharedDecodedData* getSharedCtx(PluginCtxPtr &ctx) const = 0;
};
#endif //DECODER_PLUGIN_INTERFACE_H
