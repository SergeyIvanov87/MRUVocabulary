#include "vers/TranslatorSharedDataImpl_v0.hpp"
#include "vers/TranslatorSharedDataImpl_v1.hpp"

#include "model/MultiArticle.hpp"
#include "xdxf/Formatter.hpp"
#include "model/MultiArticleSerializer.hpp"

namespace v1
{
OutputSessionCtx::OutputSessionCtx(ISharedTranslatedData &src)
{
    try
    {
        v0::TranslatedDataStructure &casted_src = dynamic_cast<v0::TranslatedDataStructure&>(src);
        *this  = casted_src;

    }
    catch(const std::bad_cast &ex)
    {
        throw std::runtime_error(std::string("Cannot initialize v1::SharedOutputData implementation by an unproper version. Error: ") + ex.what());
    }
}

OutputSessionCtx::~OutputSessionCtx() = default;

// conv
OutputSessionCtx& OutputSessionCtx::operator= (const v0::TranslatedDataStructure& src)
{
    for (const auto &local_dict_pair : src.local_dictionary)
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
            if (! multi_article->has_value<TranslationContainer>())
            {
                multi_article->node_or<TranslationContainer>();
            }
            auto &tr_container = multi_article->value<TranslationContainer>();

            xdxf::TextElement text{std::string(lang)};
            LanguageTitle lang_title(text);
            TranslationElement al {lang_title, article->value<xdxf::TextElement>()};
            tr_container.value().emplace_back(al);
        }

        local_dictionary.emplace_back(word, std::make_shared<MultiArticle>(*multi_article));
    }

    return *this;
}

template<class Formatter>
void format_serialize_impl(const OutputSessionCtx& src,const std::string &format, std::ostream& out, Formatter &tracer)
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

void OutputSessionCtx::format_serialize(std::ostream &out, const std::string &format,txml::EmptyTracer &tracer) const
{
    format_serialize_impl(*this, format, out, tracer);
}
void OutputSessionCtx::format_serialize(std::ostream &out, const std::string &format, txml::StdoutTracer &tracer) const
{
    format_serialize_impl(*this, format, out, tracer);
}
}




namespace v0
{
template<class Formatter>
void format_serialize_impl(const OutputSessionCtx& src,const std::string &format, std::ostream& out, Formatter &tracer)
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


void format_serialize(const OutputSessionCtx& data, std::ostream &out, const std::string &format, txml::StdoutTracer &tracer)
{
    format_serialize_impl(data, format, out, tracer);
}

void format_serialize(const OutputSessionCtx& data, std::ostream &out, const std::string &format,txml::EmptyTracer &tracer)
{
    format_serialize_impl(data, format, out, tracer);
}
}
