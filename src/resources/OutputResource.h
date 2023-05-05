#ifndef OUTPUT_RESOURCE_H
#define OUTPUT_RESOURCE_H
#include "pluginWrapper/OutputPluginWrapper.h"

class OutputResource :
            public Resources::IBaseResource<OutputResource>,
            public ISerializableIntrusive<OutputResource>,
            public OutputPluginWrapper
{
public:
    using Base = Resources::IBaseResource<OutputResource>;
    using Base::ResourcesMap;

     ~OutputResource() noexcept;

    //IBaseResource
    constexpr static const char *getResourceTypeDescriptionImpl()
    {
        return "OutputResource";
    }

    template<class Tracer = Tracer<EmptyTracerImpl>>
    static ResourcesMap loadResourcesImpl(const std::string &resourceFilePath, Tracer tracer);
private:
    OutputResource(OutputPluginWrapper &&impl);
};
#endif  //OUTPUT_RESOURCE_H
