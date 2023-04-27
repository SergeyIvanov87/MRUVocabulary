#include <exception>
#include <iostream>
#include "../IncomingTranslatorData.h"


#include "../IncomingTranslatorDataInterface.h"
#include "xdxf/vers/TranslatorSharedDataImpl_v0.hpp"
#include "xdxf/vers/TranslatorSharedDataImpl_v1.hpp"

template<class ...Args>
static void init_impl(int version, std::unique_ptr<ISharedTranslatedData> &impl, Args&&...args)
{
    if (version == 0)
    {
        impl.reset(new v0::SharedOutputDataImpl(std::forward<Args>(args)...));
    }
    else if (version == 1)
    {
        impl.reset(new v1::SharedOutputDataImpl(std::forward<Args>(args)...));
    }
    else
    {
        throw std::runtime_error("Cannot create SharedTranslatedData, version is unsupported: " + std::to_string(version));
    }
}

SharedTranslatedData::SharedTranslatedData(int vers) :
    version(vers)
{
    init_impl(version, impl);
}

SharedTranslatedData::SharedTranslatedData(const SharedTranslatedData &src) :
    version(src.version)
{
    init_impl(version, impl, *src.impl);
}

SharedTranslatedData::~SharedTranslatedData() = default;
