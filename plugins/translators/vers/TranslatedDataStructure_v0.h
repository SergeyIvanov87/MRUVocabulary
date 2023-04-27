#ifndef TRANSLATED_DATA_STRUCTURE_V0_H
#define TRANSLATED_DATA_STRUCTURE_V0_H

#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>


#include <txml/applications/xdxf/xdxf_fwd.h>
#include "translators/PluginTranslatedDataInterface.h"

namespace v0
{
struct TranslatedDataStructure : public ISharedTranslatedData
{
    using Word = std::string;
    using Language = std::string;
    using Article = std::optional<xdxf::XDXFArticle>;
    using Articles = std::map<Language, Article>;
    using translated_tuple_t = std::tuple<Word, Articles>;

    TranslatedDataStructure() = default;
    TranslatedDataStructure(const ISharedTranslatedData&);
    TranslatedDataStructure& operator=(const TranslatedDataStructure&) = default;
    TranslatedDataStructure& operator=(TranslatedDataStructure&&) = default;
    std::vector<translated_tuple_t> local_dictionary;
};


inline TranslatedDataStructure::TranslatedDataStructure(const ISharedTranslatedData &src)
{
    if (this == &src)
    {
        return;
    }

    try
    {
        const TranslatedDataStructure &casted_src = dynamic_cast<const TranslatedDataStructure&>(src);
        *this = casted_src;
    }
    catch(const std::bad_cast &ex)
    {
        throw std::runtime_error(std::string("Cannot initialize v0::TranslatedDataStructure implementation by an unproper version. Error: ") + ex.what());
    }
}
}
#endif // TRANSLATED_DATA_STRUCTURE_V0_H
