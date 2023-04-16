#include <iostream>
#include "translators/TranslatorSharedData.h"
#include "xdxf/TranslatorSharedDataImpl.h"

SharedTranslatedData::SharedTranslatedData() :
    impl(new SharedTranslatedDataImpl)
{
}

SharedTranslatedData::SharedTranslatedData(const SharedTranslatedData &src) :
    impl(new SharedTranslatedDataImpl(*src.impl))
{
}

SharedTranslatedData::~SharedTranslatedData() = default;

void SharedTranslatedData::dump(std::ostream &out) const
{
    impl->dump(out);
}
