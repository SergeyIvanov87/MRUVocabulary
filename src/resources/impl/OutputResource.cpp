#include <iostream>
#include <fstream>
#include <Framework/ResourcesFramework/ResourcesFramework.h>
#include <Framework/SerializeFramework/Interfaces/ISerializable.h>

#include "resources/OutputResource.h"

OutputResource::OutputResource(OutputPluginWrapper &&impl) :
    Base(),
    ISerializable<OutputResource>(),
    OutputPluginWrapper(std::move(impl))
{
}

OutputResource::~OutputResource() noexcept
{
}

template<class Tracer>
OutputResource::ResourcesMap
OutputResource::loadResourcesImpl(const std::string &resourceFilePath, Tracer tracer)
{
    ResourcesMap instances;
    try
    {
        OutputPluginPtr plugin_ptr = OutputPluginWrapper::loadPlugin(resourceFilePath);
        tracer.trace("Loaded: ", plugin_ptr->getName());

        std::shared_ptr<OutputResource> plugin (new OutputResource(std::move(*plugin_ptr)));
        instances.insert({plugin->getName(), plugin});
    }
    catch(const std::exception &ex)
    {
        tracer.trace("Cannot load: ", ex.what());
    }

    return instances;
}

template
OutputResource::ResourcesMap OutputResource::loadResourcesImpl<Tracer<EmptyTracerImpl>>(const std::string &resourcePath, Tracer<EmptyTracerImpl> tracer);
template
OutputResource::ResourcesMap OutputResource::loadResourcesImpl<Tracer<SSTracerImpl>>(const std::string &resourcePath, Tracer<SSTracerImpl> tracer);
