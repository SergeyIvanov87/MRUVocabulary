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

struct ISharedTranslatedData
{
    virtual ~ISharedTranslatedData() {};

    virtual void insert(const std::string& word, std::optional<xdxf::XDXFArticle> article) = 0;
    virtual size_t size() const = 0;
    virtual void dump(std::ostream &out) const = 0;
};


struct TranslatedData : public ISharedTranslatedData
{
    using Article = std::optional<xdxf::XDXFArticle>;
    using Articles = std::map<std::string/*lang*/, Article>;
    using translated_tuple_t = std::tuple<std::string/*word*/, Articles>;

    ~TranslatedData() = default;
    // std::vector to save the order of words
    std::vector<translated_tuple_t> local_dictionary;
    void insert(const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article);
    void insert(const std::string& word, std::optional<xdxf::XDXFArticle> article) override;
    void insert(const std::string& word, size_t repeat_num, const std::string &lang_to, std::optional<xdxf::XDXFArticle> article);
    void dump(std::ostream &out) const override;
};

inline void TranslatedData::insert(const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article)
{
    auto &comment = article->node_or<xdxf::Comment>("0");

    std::string &cur_val = comment->value();
    size_t cur_repeatition_count = std::stoull(cur_val);
    cur_repeatition_count += repeat_num;
    cur_val = std::to_string(cur_repeatition_count);

    if (! local_dictionary.empty())
    {
        auto &last_word = local_dictionary.back();
        if (std::get<0>(last_word) == word)
        {
            auto &articles = std::get<1>(last_word);
            assert(!articles.empty());
            if (auto it = articles.find(""); it != articles.end())
            {
                it->second = *article;
            }
            else
            {
                articles.emplace("", article);
            }
            return;
        }
    }

    local_dictionary.emplace_back(std::forward_as_tuple(word, Articles({{std::string(""), article}})));
    (void)repeat_num;
}

inline void TranslatedData::insert(const std::string& word, std::optional<xdxf::XDXFArticle> article)
{
    local_dictionary.emplace_back(std::forward_as_tuple(word, Articles({{std::string(""), article}})));
}

inline void TranslatedData::insert(const std::string& word, size_t repeat_num, const std::string &lang_to, std::optional<xdxf::XDXFArticle> article)
{
    auto &comment = article->node_or<xdxf::Comment>("0");

    std::string &cur_val = comment->value();
    size_t cur_repeatition_count = std::stoull(cur_val);
    cur_repeatition_count += repeat_num;
    cur_val = std::to_string(cur_repeatition_count);

    if (! local_dictionary.empty())
    {
        auto &last_word = local_dictionary.back();
        if (std::get<0>(last_word) == word)
        {
            auto &articles = std::get<1>(last_word);
            assert(!articles.empty());
            if (auto it = articles.find(lang_to); it != articles.end())
            {
                it->second = *article;
            }
            else
            {
                articles.emplace(lang_to, article);
            }
            return;
        }
    }

    local_dictionary.emplace_back(std::forward_as_tuple(word, Articles({{lang_to, article}})));

    (void)repeat_num;
}

#endif
