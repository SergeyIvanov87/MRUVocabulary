#include "vers/TranslatorSharedDataImpl_v0.hpp"
#include "vers/TranslatorSharedDataImpl_v1.hpp"

#include "model/MultiArticle.hpp"
#include "xdxf/Formatter.hpp"
#include "model/MultiArticleSerializer.hpp"
#include "../translators/vers/TranslatedDataStructure_v0.h"

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
            assert(!std::get<1>(val).empty());
            std::get<1>(val).begin()->second->format_serialize(format_out, tracer);
            //out << std::endl;
        }
    }
    else if (format == "fb2")
    {
        xdxf::ToFB2 format_out(out);
        for (const auto& val : src.local_dictionary)
        {
            assert(!std::get<1>(val).empty());
            std::get<1>(val).begin()->second->format_serialize(format_out, tracer);
            //out << std::endl;
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
