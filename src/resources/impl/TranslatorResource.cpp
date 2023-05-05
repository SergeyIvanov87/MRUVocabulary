#include <iostream>
#include <fstream>
#include <Framework/ResourcesFramework/ResourcesFramework.h>
#include <Framework/SerializeFramework/Interfaces/ISerializable.h>

#include "resources/TranslatorResource.h"

TranslatorResource::TranslatorResource(TranslatorPluginWrapper &&impl) :
    Base(),
    ISerializableIntrusive<TranslatorResource>(),
    TranslatorPluginWrapper(std::move(impl))
{
}

TranslatorResource::~TranslatorResource() noexcept
{
}

template<class Tracer>
TranslatorResource::ResourcesMap
TranslatorResource::loadResourcesImpl(const std::string &resourceFilePath, Tracer tracer)
{
    ResourcesMap instances;
    try
    {
        TranslatorPluginPtr plugin_ptr = TranslatorPluginWrapper::loadPlugin(resourceFilePath);
        tracer.trace("Loaded: ", plugin_ptr->getName());

        std::shared_ptr<TranslatorResource> plugin (new TranslatorResource(std::move(*plugin_ptr)));
        instances.insert({plugin->getName(), plugin});
    }
    catch(const std::exception &ex)
    {
        tracer.trace("Cannot load: ", ex.what());
    }

    return instances;
}

template
TranslatorResource::ResourcesMap TranslatorResource::loadResourcesImpl<Tracer<EmptyTracerImpl>>(const std::string &resourcePath, Tracer<EmptyTracerImpl> tracer);
template
TranslatorResource::ResourcesMap TranslatorResource::loadResourcesImpl<Tracer<SSTracerImpl>>(const std::string &resourcePath, Tracer<SSTracerImpl> tracer);
template
TranslatorResource::ResourcesMap TranslatorResource::loadResourcesImpl<Tracer<Stdout>>(const std::string &resourcePath, Tracer<Stdout> tracer);
