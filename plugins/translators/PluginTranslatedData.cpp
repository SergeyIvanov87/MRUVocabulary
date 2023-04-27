#include <exception>
#include <iostream>

#include "translators/PluginTranslatedData.h"
#include "translators/PluginTranslatedDataInterface.h"
#include "translators/vers/TranslatedDataStructure_v0.h"

template<class ...Args>
static void init_impl(int version, std::unique_ptr<ISharedTranslatedData> &impl, Args&&...args)
{
    if (version == 0)
    {
        impl.reset(new v0::TranslatedDataStructure(std::forward<Args>(args)...));
    }
    else
    {
        throw std::runtime_error("Cannot create TranslatedDataStructure, version is unsupported: " + std::to_string(version));
    }
}

SharedTranslatedData::SharedTranslatedData(int vers) :
    version(vers)
{
    init_impl(version, impl);
}

SharedTranslatedData::SharedTranslatedData(const SharedTranslatedData &src) :
    version(src.getVersion())
{
    init_impl(version, impl, src.getImpl());
}

SharedTranslatedData::~SharedTranslatedData() = default;

int SharedTranslatedData::getVersion() const
{
    return version;
}

ISharedTranslatedData &SharedTranslatedData::getImpl()
{
    return const_cast<ISharedTranslatedData &>(static_cast<const SharedTranslatedData*>(this)->getImpl());
}

const ISharedTranslatedData &SharedTranslatedData::getImpl() const
{
    assert(impl);
    return *impl;
}
