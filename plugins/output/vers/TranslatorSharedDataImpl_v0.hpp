#ifndef TRANSLATOR_SHARED_DATA_IMPL_V0_HPP
#define TRANSLATOR_SHARED_DATA_IMPL_V0_HPP
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "ISharedTranslatedData.h"
#include <txml/applications/xdxf/xdxf_fwd.h>

#include "../translators/vers/TranslatedDataStructure_v0.h"
namespace v0
{
    using OutputSessionCtx = TranslatedDataStructure;

    void format_serialize(const OutputSessionCtx& data, std::ostream &out, const std::string &format,txml::EmptyTracer &tracer);
    void format_serialize(const OutputSessionCtx& data, std::ostream &out, const std::string &format, txml::StdoutTracer &tracer);

/*struct OutputSessionCtx final : public ISharedTranslatedData
{
    using Article = std::optional<xdxf::XDXFArticle>;
    using Articles = std::map<std::string, Article>;
    using translated_tuple_t = std::tuple<std::string, Articles>;

    OutputSessionCtx() = default;
    OutputSessionCtx(ISharedTranslatedData&);
    OutputSessionCtx(const OutputSessionCtx&) = default;
    OutputSessionCtx(OutputSessionCtx&&) = default;
    OutputSessionCtx& operator=(const OutputSessionCtx&) = default;
    OutputSessionCtx& operator=(OutputSessionCtx&&) = default;

    ~OutputSessionCtx() override;
    // std::vector to save the order of words
    std::vector<translated_tuple_t> local_dictionary;

    /*void insert(const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article);
    void insert(const std::string& word, std::optional<xdxf::XDXFArticle> article) override;
    void insert(const std::string& word, size_t repeat_num, const std::string &lang_to, std::optional<xdxf::XDXFArticle> article);

    size_t size() const override;

    void dump(std::ostream &out) const override;
    template<class Formatter>
    void format_dump(Formatter &out) const;
    * /
};*/
}
#endif // TRANSLATOR_SHARED_DATA_IMPL_V0_HPP
