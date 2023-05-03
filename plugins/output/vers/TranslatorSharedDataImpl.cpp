#include "vers/TranslatorSharedDataImpl_v0.hpp"
#include "vers/TranslatorSharedDataImpl_v1.hpp"

#include "model/MultiArticle.hpp"
#include "xdxf/Formatter.hpp"
#include "model/MultiArticleSerializer.hpp"

namespace v1
{

SharedOutputDataImpl::SharedOutputDataImpl(ISharedTranslatedData &src)
{
    try
    {
        // v0 ???
        v0::TranslatedDataStructure &casted_src = reinterpret_cast<v0::TranslatedDataStructure&>(src);
        for (const auto &local_dict_pair : casted_src.local_dictionary)
        {
            std::optional<MultiArticle> multi_article;
            const auto &[word, articles] = local_dict_pair;
            if (!articles.empty())
            {
                multi_article = std::make_optional<MultiArticle>();

            }
            for (const auto &articles_pair : articles)
            {
                const auto &[lang, article] = articles_pair;

                // set keyphrase if doesn't esixt
                auto &key_phrase = multi_article->node_or<xdxf::KeyPhrase>(article->value<xdxf::KeyPhrase>());
                // set transcription if doesn't esixt
                if (article->has_value<xdxf::Transcription>())
                {
                    if (! multi_article->has_value<xdxf::Transcription>())
                    {
                        multi_article->insert<xdxf::Transcription>(article->value<xdxf::Transcription>());
                    }
                }

                // put translation into container
                auto &tr_container = multi_article->node_or<TranslationContainer>();

                xdxf::TextElement text{std::string(lang)};
                LanguageTitle lang_title(text);
                TranslationElement al {lang_title, article->value<xdxf::TextElement>()};
                tr_container->value().emplace_back(al);
            }

            local_dictionary.emplace_back(word, std::make_shared<MultiArticle>(*multi_article));
        }

        abort();
    }
    catch(const std::bad_cast &ex)
    {
        try
        {
            v0::SharedOutputDataImpl &casted_src = reinterpret_cast<v0::SharedOutputDataImpl&>(src);
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
    return *this;
}

template<class Formatter>
void format_serialize_impl(const SharedOutputDataImpl& src,const std::string &format, std::ostream& out, Formatter &tracer)
{
    if (format == "xdxf")
    {
        /* TODO
        ToXDXF format_out(out);
        for (const auto& val : src.local_dictionary)
        {
            std::get<1>(val)->format_serialize(format_out, tracer);
            //out << std::endl;
        }
        */
    }
    else if (format == "fb2")
    {
        MultiArticleToFB2 format_out(out);
        for (const auto& val : src.local_dictionary)
        {
            std::get<1>(val)->format_serialize(format_out, tracer);
            //out << std::endl;
        }
    }
}

void SharedOutputDataImpl::format_serialize(std::ostream &out, const std::string &format,txml::EmptyTracer &tracer) const
{
    format_serialize_impl(*this, format, out, tracer);
}
void SharedOutputDataImpl::format_serialize(std::ostream &out, const std::string &format, txml::StdoutTracer &tracer) const
{
    format_serialize_impl(*this, format, out, tracer);
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





namespace v0
{
template<class Formatter>
void format_serialize_impl(const SharedOutputDataImpl& src,const std::string &format, std::ostream& out, Formatter &tracer)
{
    if (format == "xdxf")
    {
        ToXDXF format_out(out);
        for (const auto& val : src.local_dictionary)
        {
            const auto &[word, volume] = val;
            assert(!volume.empty());
            for (const auto &lang_article : volume)
            {
                lang_article.second->format_serialize(format_out, tracer);
            }
        }
    }
    else if (format == "fb2")
    {
        xdxf::ToFB2 format_out(out);
        for (const auto& val : src.local_dictionary)
        {
            const auto &[word, volume] = val;
            assert(!volume.empty());
            for (const auto &lang_article : volume)
            {
                lang_article.second->format_serialize(format_out, tracer);
            }
        }
    }
}


void format_serialize(const SharedOutputDataImpl& data, std::ostream &out, const std::string &format, txml::StdoutTracer &tracer)
{
    format_serialize_impl(data, format, out, tracer);
}

void format_serialize(const SharedOutputDataImpl& data, std::ostream &out, const std::string &format,txml::EmptyTracer &tracer)
{
    format_serialize_impl(data, format, out, tracer);
}
}
