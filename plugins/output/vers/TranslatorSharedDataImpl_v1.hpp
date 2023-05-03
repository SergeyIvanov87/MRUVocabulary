#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "ISharedTranslatedData.h"

namespace v0
{
    struct TranslatedDataStructure;
}

class MultiArticle;
namespace v1
{
struct SharedOutputDataImpl final : public ISharedTranslatedData
{
    using Article = std::shared_ptr<MultiArticle>;
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


    void format_serialize(std::ostream &out, const std::string &format,txml::EmptyTracer &tracer) const;
    void format_serialize(std::ostream &out, const std::string &format, txml::StdoutTracer &tracer) const;
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
