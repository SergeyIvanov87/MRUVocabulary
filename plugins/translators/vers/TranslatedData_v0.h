#ifndef TRANSLATED_DATA_V0_H
#define TRANSLATED_DATA_V0_H

#include "translators/vers/TranslatedDataStructure_v0.h"
#include "translators/PluginTranslatedDataInterface.h"

namespace v0
{
struct TranslatedData : public ISharedTranslatedData;
{
    ~TranslatedData() = default override;

    std::shared_ptr<::TranslatedDataStructure> getDataStructure() const override;
    void insert(const std::string& word, std::optional<xdxf::XDXFArticle> article) override;
    void dump(std::ostream &out) const override;

    void insert(const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article);
    void insert(const std::string& word, size_t repeat_num, const std::string &lang_to, std::optional<xdxf::XDXFArticle> article);
private:
    std::shared_ptr<TranslatedDataStructure> data;
};
}
#endif
