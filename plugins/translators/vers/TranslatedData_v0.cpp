#include "translators/vers/TranslatedData_v0.h"

namespace v0
{
std::shared_ptr<::TranslatedDataStructure> TranslatedData::getDataStructure() const
{
    return data;
}

inline void TranslatedData::insert(const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article)
{
    auto &comment = article->node_or<xdxf::Comment>("0");

    std::string &cur_val = comment->value();
    size_t cur_repeatition_count = std::stoull(cur_val);
    cur_repeatition_count += repeat_num;
    cur_val = std::to_string(cur_repeatition_count);

    if (! data.local_dictionary.empty())
    {
        auto &last_word = data.local_dictionary.back();
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

    data.local_dictionary.emplace_back(std::forward_as_tuple(word, Articles({{std::string(""), article}})));
    (void)repeat_num;
}

inline void TranslatedData::insert(const std::string& word, std::optional<xdxf::XDXFArticle> article)
{
    data.local_dictionary.emplace_back(std::forward_as_tuple(word, Articles({{std::string(""), article}})));
}

inline void TranslatedData::insert(const std::string& word, size_t repeat_num, const std::string &lang_to, std::optional<xdxf::XDXFArticle> article)
{
    auto &comment = article->node_or<xdxf::Comment>("0");

    std::string &cur_val = comment->value();
    size_t cur_repeatition_count = std::stoull(cur_val);
    cur_repeatition_count += repeat_num;
    cur_val = std::to_string(cur_repeatition_count);

    if (!data.local_dictionary.empty())
    {
        auto &last_word = data.local_dictionary.back();
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

    data.local_dictionary.emplace_back(std::forward_as_tuple(word, Articles({{lang_to, article}})));

    (void)repeat_num;
}
}
