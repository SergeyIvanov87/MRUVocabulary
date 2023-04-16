#ifndef PLUGIN_FORMATTER
#define PLUGIN_FORMATTER

#include <iostream>
#include "FormatMapper.h"

#include <txml/applications/fb2/fb2.hpp>
#include <txml/applications/xdxf/xdxf.hpp>
//#include <applications/xdxf/XDXFArticle.hpp>
//#include <applications/fb2/body/Section.hpp>
//#include <applications/fb2/FB2TextElement.hpp>

using InFormatXDXF = FormatList<xdxf::XDXFArticle, xdxf::KeyPhrase, xdxf::Comment, xdxf::Transcription, xdxf::TextElement>;
using OutFormatFb2 = FormatList<fb2::Section, fb2::Paragraph, fb2::Paragraph, fb2::Paragraph, fb2::Paragraph>;


struct ToXDXF : public FormatMapper<ToXDXF, InFormatXDXF, InFormatXDXF>
{
    using base = FormatMapper<ToXDXF, InFormatXDXF, InFormatXDXF>;
    using out_list_t = typename base::out_list_t;

    ToXDXF(std::ostream &stream) :
        out(stream)
    {
    }

    template<class Tracer>
    void map_impl(const xdxf::XDXFArticle& val, std::integral_constant<int, 0> vl, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", xdxf::XDXFArticle::class_name());

        out << "<" << xdxf::XDXFArticle::class_name() << ">";
        val.format_serialize(*this, tracer);
        out << "</" << xdxf::XDXFArticle::class_name() << ">\n";
    }

    template<class Tracer>
    void map_impl(const xdxf::KeyPhrase& val, std::integral_constant<int, 1> vl, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", xdxf::KeyPhrase::class_name());
        out << "<" << xdxf::KeyPhrase::class_name() << ">";
        val.format_serialize(*this, tracer);
        out << "</" << xdxf::KeyPhrase::class_name() << ">\n";
    }


    template<class Tracer>
    void map_impl(const xdxf::Comment& val, std::integral_constant<int, 2> vl, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", xdxf::Comment::class_name());
        val.xml_serialize(out);
    }

    template<class Tracer>
    void map_impl(const xdxf::Transcription& val, std::integral_constant<int, 3> vl, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", xdxf::Transcription::class_name());
        out << "<" << xdxf::Transcription::class_name() << ">";
        val.format_serialize_request(*this, tracer);
        out << "</" << xdxf::Transcription::class_name() << ">\n";
    }

    template<class Tracer>
    void map_impl(const xdxf::TextElement& val, std::integral_constant<int, 4> vl, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", xdxf::TextElement::class_name());
        val.xml_serialize(out, tracer);
    }

    template<class In, int InIndex, class Tracer>
    void map_impl(const In& val, std::integral_constant<int, InIndex> vl, Tracer tracer)
    {
        using mapped_type = std::tuple_element_t<InIndex,
                                    typename out_list_t::impl_t>;


        tracer.trace(__FUNCTION__, " - ", mapped_type::class_name());
        out << "<" << mapped_type::class_name() << ">";
        val.format_dump(*this, tracer);
        out << "</" << mapped_type::class_name() << ">\n";
    }

    std::ostream &out;
};

struct ToFB2 : public FormatMapper<ToFB2, InFormatXDXF, OutFormatFb2>
{
    ToFB2(std::ostream &stream) :
        out(stream)
    {
    }


    template<class Tracer>
    void map_impl(const xdxf::XDXFArticle& val, std::integral_constant<int, 0> vl, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", xdxf::XDXFArticle::class_name());

        out << "<" << fb2::Section::class_name() << ">\n";
        val.format_serialize(*this, tracer);
        out << "</" << fb2::Section::class_name() << ">\n";
    }

    template<class Tracer>
    void map_impl(const xdxf::KeyPhrase& val, std::integral_constant<int, 1> vl, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", xdxf::KeyPhrase::class_name());
        out << "<title>";
        val.format_serialize(*this, tracer);
        out << "</title>\n";
        out << "<empty-line/>\n";
    }

    template<class Tracer>
    void map_impl(const xdxf::Comment& val, std::integral_constant<int, 2> vl, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", xdxf::Comment::class_name());
        out << "<" << fb2::Section::class_name() << ">\n";
        out << "<title>";

        out << "<p><emphasis>";
        out << "used: " << val.value() << " times";
        out << "</emphasis></p>";

        out << "</title>\n";
        out << "</" << fb2::Section::class_name() << ">\n";
    }

    template<class Tracer>
    void map_impl(const xdxf::Transcription& val, std::integral_constant<int, 3> vl, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", xdxf::Transcription::class_name());
        out << "<empty-line/>\n";
        out << "<" << fb2::Section::class_name() << ">\n";
        out << "<title>";

        val.format_serialize(*this, tracer);

        out << "</title>\n";
        out << "</" << fb2::Section::class_name() << ">\n";
    }

    template<class Tracer>
    void map_impl(const xdxf::TextElement& val, std::integral_constant<int, 4> vl, Tracer tracer)
    {
        tracer.trace(__FUNCTION__, " - ", xdxf::TextElement::class_name());
        out << "<" << fb2::Paragraph::class_name() << ">";
//        val.xml_serialize(out, tracer);


        //each \n or \r is on new paragraph
        std::stringstream ss;
        val.xml_serialize(ss, tracer);
        const std::string& text = ss.str();
        std::string::size_type start_paragraph_pos = 0, end_paragraph_pos = text.size();
        while((end_paragraph_pos = text.find_first_of("\n\r", start_paragraph_pos)) != std::string::npos)
        {
            out.write(text.data() + start_paragraph_pos, end_paragraph_pos - start_paragraph_pos);
            out << "</" << fb2::Paragraph::class_name() << ">\n";

            out << "<empty-line/>\n";

            start_paragraph_pos = text.find_first_not_of("\n\r", end_paragraph_pos + 1);
            if (start_paragraph_pos == std::string::npos)//< text.size())
            {
                break;
            }

            out << "<" << fb2::Paragraph::class_name() << ">";
        }

        if( start_paragraph_pos != std::string::npos)
        {
            if (end_paragraph_pos == std::string::npos)
            {
                end_paragraph_pos = text.size();
            }
            out.write(text.data() + start_paragraph_pos, end_paragraph_pos - start_paragraph_pos);
        }
        out << "</" << fb2::Paragraph::class_name() << ">\n";
    }

    template<class In, int InIndex, class Tracer>
    void map_impl(const In& val, std::integral_constant<int, InIndex> vl, Tracer tracer)
    {
        using mapped_type = std::tuple_element_t<InIndex,
                                    typename out_list_t::impl_t>;

        out << "<" << mapped_type::class_name() << ">";
        val.format_dump(*this, tracer);
        out << "</" << mapped_type::class_name() << ">\n";
    }
    std::ostream &out;
};
#endif //PLUGIN_FORMATTER
