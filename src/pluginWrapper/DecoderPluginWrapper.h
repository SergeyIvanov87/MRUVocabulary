#ifndef DECODER_PLUGIN_WRAPPER_H
#define DECODER_PLUGIN_WRAPPER_H
#include <plugins/decoders/DecoderInterface.h>
#include <plugins/decoders/DecoderExport.h>
#include "plugins/decoders/PluginDecodedData.h"
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

    long long decodeData(PluginCtxPtr &ctx, size_t size) override;
    SharedDecodedData* getSharedCtx(PluginCtxPtr &ctx) const override;

    static DecoderPluginPtr loadPlugin(const std::string &fileName);
private:
    decode_ptr decode_function;
    get_shared_ctx_ptr get_shared_ctx;

};
#endif //DECODER_PLUGIN_WRAPPER_H
