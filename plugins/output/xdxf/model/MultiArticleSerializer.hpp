#ifndef MULTI_ARTICLE_FB2_SERIALIZER_HPP
#define MULTI_ARTICLE_FB2_SERIALIZER_HPP

#include <txml/include/engine/FormatSerializerBase.hpp>

#include <txml/applications/xdxf/serializer/to_fb2.hpp>
#include <txml/applications/xdxf/xdxf.hpp>
#include <txml/applications/fb2/fb2.hpp>

#include "model/MultiArticle.hpp"

template<class Stream>
struct MultiArticleToFB2 : public txml::FormatSerializerBase<MultiArticleToFB2<Stream>, txml::StaticCheckUnscopedElement,
                                                 MultiArticle, xdxf::KeyPhrase, xdxf::Transcription,
                                                              TranslationContainer, TranslationElement, LanguageTitle,
                                                                    xdxf::TextElement>
{
    MultiArticleToFB2(Stream &stream) :
        out(stream),
        default_fb2_serializer(stream)
    {
    }


    template<class Element, class Tracer>
    void serialize_impl(const Element& val, Tracer tracer)
    {
        default_fb2_serializer.serialize_impl(val, tracer);
    }

    template<class Tracer>
    void serialize_impl(const MultiArticle& val, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", MultiArticle::class_name());

        out << "<" << fb2::Section::class_name() << ">\n";
        val.make_format_serialize(*this, tracer);
        out << "</" << fb2::Section::class_name() << ">\n";
    }

    template<class Tracer>
    void serialize_impl(const TranslationContainer& val, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", TranslationContainer::class_name());
        out << "<title>";
        val.make_format_serialize(*this, tracer);
        out << "</title>\n";
        out << "<empty-line/>\n";
    }

    template<class Tracer>
    void serialize_impl(const TranslationElement& val, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", TranslationElement::class_name());
        out << "<" << fb2::Section::class_name() << ">\n";
        val.make_format_serialize(*this, tracer);
        out << "</" << fb2::Section::class_name() << ">\n";
        out << "<empty-line/>\n";
    }

    template<class Tracer>
    void serialize_impl(const LanguageTitle& val, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", LanguageTitle::class_name());
        out << "<title>";
        val.make_format_serialize(*this, tracer);
        out << "</title>\n";
        out << "<empty-line/>\n";
    }

    Stream &out;
private:
    xdxf::ToFB2<Stream> default_fb2_serializer;
};
#endif //MULTI_ARTICLE_FB2_SERIALIZER_HPP
