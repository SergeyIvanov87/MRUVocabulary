#ifndef RESOURCES_FRAMEWORK_SPECS_H
#define RESOURCES_FRAMEWORK_SPECS_H

#include <Framework/ResourcesFramework/ResourcesFramework.h>
#include "ResourceLoader.h"
using ResourcesFramework = Resources::ResourcesFrameworkFactory<Settings,
                                                                DecoderResource,
                                                                OutputResource,
                                                                TranslatorResource>;

#endif //RESOURCES_FRAMEWORK_SPECS_H
