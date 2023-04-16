#include <iostream>
#include "translators/TranslatorSharedData.h"
#include "translators/TranslatorSharedDataImpl.h"

SharedTranslatedData::SharedTranslatedData() :
    impl(new SharedTranslatedDataImpl)
{
}

SharedTranslatedData::SharedTranslatedData(const SharedTranslatedData &src) :
    impl(new SharedTranslatedDataImpl(*src.impl))
{
}

SharedTranslatedData::~SharedTranslatedData() = default;
