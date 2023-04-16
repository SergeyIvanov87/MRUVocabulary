#include <iostream>
#include <fstream>
#include <Framework/ResourcesFramework/ResourcesFramework.h>
#include <Framework/SerializeFramework/Interfaces/ISerializable.h>

#include "resources/DecoderResource.h"

DecoderResource::DecoderResource(DecoderPluginWrapper &&impl) :
    Base(),
    ISerializable<DecoderResource>(),
    DecoderPluginWrapper(std::move(impl))
{
}

DecoderResource::~DecoderResource() noexcept
{
}

template<class Tracer>
DecoderResource::ResourcesMap
DecoderResource::loadResourcesImpl(const std::string &resourceFilePath, Tracer tracer)
{
    ResourcesMap instances;
    try
    {
        DecoderPluginPtr plugin_ptr = DecoderPluginWrapper::loadPlugin(resourceFilePath);
        tracer.trace("Loaded: ", plugin_ptr->getName());

        std::shared_ptr<DecoderResource> plugin (new DecoderResource(std::move(*plugin_ptr)));
        instances.insert({plugin->getName(), plugin});
    }
    catch(const std::exception &ex)
    {
        tracer.trace("Cannot load: ", ex.what());
    }

    return instances;
}

template
DecoderResource::ResourcesMap DecoderResource::loadResourcesImpl<Tracer<EmptyTracerImpl>>(const std::string &resourcePath, Tracer<EmptyTracerImpl> tracer);
template
DecoderResource::ResourcesMap DecoderResource::loadResourcesImpl<Tracer<SSTracerImpl>>(const std::string &resourcePath, Tracer<SSTracerImpl> tracer);
