#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "model/MultiArticle.hpp"

#include "../IncomingTranslatorDataInterface.h"

namespace v0
{
    struct TranslatedDataStructure;
}

namespace v1
{
struct SharedOutputDataImpl final : public ISharedTranslatedData
{
    using Article = std::optional<MultiArticle>;
    using Articles = Article;
    using translated_tuple_t = std::tuple<std::string/*word*/, Article>;

    SharedOutputDataImpl() = default;
    SharedOutputDataImpl(ISharedTranslatedData&);
    SharedOutputDataImpl(const SharedOutputDataImpl&) = default;
    SharedOutputDataImpl(SharedOutputDataImpl&&) = default;
    SharedOutputDataImpl& operator=(const SharedOutputDataImpl&) = default;
    SharedOutputDataImpl& operator=(SharedOutputDataImpl&&) = default;

    // conv
    SharedOutputDataImpl& operator=(const v0::TranslatedDataStructure&);
    ~SharedOutputDataImpl() override;
    // std::vector to save the order of words
    std::vector<translated_tuple_t> local_dictionary;

   /* void insert(const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article);
    void insert(const std::string& word, std::optional<xdxf::XDXFArticle> article) override;
    void insert(const std::string& word, size_t repeat_num, const std::string &lang_to, std::optional<xdxf::XDXFArticle> article);

    size_t size() const override;

    void dump(std::ostream &out) const override;

    template<class Formatter>
    void format_dump(Formatter &out) const;
    */
};
}
