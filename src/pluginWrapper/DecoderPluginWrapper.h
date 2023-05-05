#ifndef DECODER_PLUGIN_WRAPPER_H
#define DECODER_PLUGIN_WRAPPER_H
#include <plugins/decoders/DecoderInterface.h>
#include <plugins/decoders/DecoderExport.h>
#include "plugins/decoders/vers/PluginDecodedData.h"
#include "PluginHolder.h"

class DecoderPluginWrapper : public IDecoderPlugin,
                             public PluginHolder
{
public:

    using DecoderPluginPtr = std::shared_ptr<DecoderPluginWrapper>;

    DecoderPluginWrapper(void *handle);
    DecoderPluginWrapper(DecoderPluginWrapper &&src) = default;
    DecoderPluginWrapper(PluginHolder &&src);
    ~DecoderPluginWrapper() noexcept;

    long long decodeData(PluginCtxPtr &ctx, size_t size, SessionPtr &sess) override;

    static DecoderPluginPtr loadPlugin(const std::string &fileName);
private:
    decode_ptr decode_function;

};
#endif //DECODER_PLUGIN_WRAPPER_H
