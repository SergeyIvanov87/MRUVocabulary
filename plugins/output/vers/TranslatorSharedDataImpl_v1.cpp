#include "xdxf/vers/TranslatorSharedDataImpl_v0.hpp"
#include "xdxf/vers/TranslatorSharedDataImpl_v1.hpp"

namespace v1
{

SharedOutputDataImpl::SharedOutputDataImpl(ISharedTranslatedData &src)
{
    if (this == &src)
    {
        return;
    }

    try
    {
        SharedOutputDataImpl &casted_src = dynamic_cast<SharedOutputDataImpl&>(src);
        *this = casted_src;
    }
    catch(const std::bad_cast &ex)
    {
        try
        {
            v0::SharedOutputDataImpl &casted_src = dynamic_cast<v0::SharedOutputDataImpl&>(src);
            *this = casted_src;
        }
        catch(const std::bad_cast &ex)
        {
            throw std::runtime_error(std::string("Cannot initialize v1::SharedOutputData implementation by an unproper version. Error: ") + ex.what());
        }
    }
}

SharedOutputDataImpl::~SharedOutputDataImpl() = default;

// conv
SharedOutputDataImpl& SharedOutputDataImpl::operator= (const v0::TranslatedDataStructure& src)
{

}
/*
inline void SharedOutputDataImpl::insert(const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article)
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

    local_dictionary.emplace_back(std::forward_as_tuple(word, Articles{{"", article}}));
    (void)repeat_num;
}

inline void SharedOutputDataImpl::insert(const std::string& word, std::optional<xdxf::XDXFArticle> article)
{
    local_dictionary.emplace_back(std::forward_as_tuple(word, Articles{{"", article}}));
}

inline void SharedOutputDataImpl::insert(const std::string& word, size_t repeat_num, const std::string &lang_to, std::optional<xdxf::XDXFArticle> article)
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

    local_dictionary.emplace_back(std::forward_as_tuple(word, Articles{{lang_to, article}}));

    (void)repeat_num;
}

inline size_t SharedOutputDataImpl::size() const
{
    return local_dictionary.size();
}

inline void SharedOutputDataImpl::dump(std::ostream &out) const
{
    for (const auto& val : local_dictionary)
    {
        for(const auto& dict: std::get<1>(val))
        {
            out << dict.first << std::endl;
            dict.second->xml_serialize(out);
            out << std::endl;
        }
    }
}*/
}
