#ifndef PLUGIN_TRANSLATED_DATA_IMPL_H
#define PLUGIN_TRANSLATED_DATA_IMPL_H
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include <string>
#include <ostream>

#include <txml/applications/xdxf/xdxf_fwd.h>

struct SharedTranslatedDataImpl
{
    using translated_tuple_t = std::tuple<std::string, std::optional<xdxf::XDXFArticle>>;
    std::vector<translated_tuple_t> local_dictionary;
    void insert(const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article);
    void insert(const std::string& word, std::optional<xdxf::XDXFArticle> article);

    void dump(std::ostream &out) const;
    template<class Formatter>
    void format_dump(Formatter &out) const;
};

inline void SharedTranslatedDataImpl::insert(const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article)
{
    auto &comment = article->node_or<xdxf::Comment>("0");
    std::string &cur_val = comment->value();
    size_t cur_repeatition_count = std::stoull(cur_val);
    cur_repeatition_count += repeat_num;
    cur_val = std::to_string(cur_repeatition_count);

    local_dictionary.emplace_back(word, article);
    (void)repeat_num;
}

inline void SharedTranslatedDataImpl::insert(const std::string& word, std::optional<xdxf::XDXFArticle> article)
{
    local_dictionary.emplace_back(word, article);
}

inline void SharedTranslatedDataImpl::dump(std::ostream &out) const
{
    for (const auto& val : local_dictionary)
    {
        std::get<1>(val)->xml_serialize(out);
        out << std::endl;
    }
}
#endif
