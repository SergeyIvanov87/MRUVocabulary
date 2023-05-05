#ifndef DECODER_RESOURCE_H
#define DECODER_RESOURCE_H
#include "pluginWrapper/DecoderPluginWrapper.h"

class DecoderResource :
            public Resources::IBaseResource<DecoderResource>,
            public ISerializableIntrusive<DecoderResource>,
            public DecoderPluginWrapper
{
public:
    using Base = Resources::IBaseResource<DecoderResource>;
    using Base::ResourcesMap;

    ~DecoderResource() noexcept;

    //IBaseResource
    constexpr static const char *getResourceTypeDescriptionImpl()
    {
        return "DecoderResource";
    }

    template<class Tracer = Tracer<EmptyTracerImpl>>
    static ResourcesMap loadResourcesImpl(const std::string &resourceFilePath, Tracer tracer);
private:
    DecoderResource(DecoderPluginWrapper &&impl);
};
#endif  //DECODER_RESOURCE_H
