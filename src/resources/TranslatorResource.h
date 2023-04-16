#ifndef TRANSLATOR_RESOURCE_H
#define TRANSLATOR_RESOURCE_H
#include "pluginWrapper/TranslatorPluginWrapper.h"

class TranslatorResource :
            public Resources::IBaseResource<TranslatorResource>,
            public ISerializable<TranslatorResource>,
            public TranslatorPluginWrapper
{
public:
    using Base = Resources::IBaseResource<TranslatorResource>;
    using Base::ResourcesMap;

    enum
    {
        isSerializableSupport = false,
        isDumpObjectSupport = false
    };

    ~TranslatorResource() noexcept;

    //IBaseResource
    constexpr static const char *getResourceTypeDescriptionImpl()
    {
        return "TranslatorResource";
    }

    template<class Tracer = Tracer<EmptyTracerImpl>>
    static ResourcesMap loadResourcesImpl(const std::string &resourceFilePath, Tracer tracer);
private:
    TranslatorResource(TranslatorPluginWrapper &&impl);
};
#endif  //OUTPUT_RESOURCE_H
