#ifndef CUSTOM_RESOURCE_LOADERS_H
#define CUSTOM_RESOURCE_LOADERS_H
extern const char system_info_dummy_path[];

#include <Framework/ResourcesFramework/IBaseResource.h>
#include <Framework/Utils/StringUtils.h>
#include <Framework/SerializeFramework/Interfaces/ISerializable.h>


#include "resources/Settings.h"
#include "resources/DecoderResource.h"
#include "resources/OutputResource.h"
#include "resources/TranslatorResource.h"



//Resource Traits For Resources
#include <Framework/ResourcesFramework/ResourceTraits.h>
template<>
struct ResourcesTraits<Settings>
{
   static constexpr const char* getResourcePath() noexcept {return "./";}
   static constexpr bool hasAssetsPath() noexcept {return true;}
   static constexpr const char *getResourceRegexFilter() noexcept {return ".*\\..*";}
};

template<>
struct ResourcesTraits<DecoderResource>
{
   static constexpr const char* getResourcePath() noexcept {return "plugins";}
   static constexpr bool hasAssetsPath() noexcept {return true;}
   static constexpr const char *getResourceRegexFilter() noexcept {return ".*\\.so";}
};

template<>
struct ResourcesTraits<OutputResource>
{
   static constexpr const char* getResourcePath() noexcept {return "plugins";}
   static constexpr bool hasAssetsPath() noexcept {return true;}
   static constexpr const char *getResourceRegexFilter() noexcept {return ".*\\.so";}
};

template<>
struct ResourcesTraits<TranslatorResource>
{
   static constexpr const char* getResourcePath() noexcept {return "plugins";}
   static constexpr bool hasAssetsPath() noexcept {return true;}
   static constexpr const char *getResourceRegexFilter() noexcept {return ".*\\.so";}
};
#endif //CUSTOM_RESOURCE_LOADERS_H
