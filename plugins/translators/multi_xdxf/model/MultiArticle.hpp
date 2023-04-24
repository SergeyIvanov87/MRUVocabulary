#ifndef TRANSLATION_MULTI_ARTICLE_HPP
#define TRANSLATION_MULTI_ARTICLE_HPP
#include <txml/applications/xdxf/xdxf.hpp>

class LanguageTitle : public txml::XMLNode<LanguageTitle, TextElement>
{
public:
    using base = txml::XMLNode<LanguageTitle, TextElement>;
    using value_t = std::string;

    static constexpr std::string_view class_name()
    {
        return "lang";
    }

    static constexpr txml::TextReaderWrapper::NodeType class_node_type()
    {
        return txml::TextReaderWrapper::NodeType::Element;
    };

    const value_t &value() const
    {
        const auto& val = base::node<TextElement>();
        if (!val)
        {
            throw std::runtime_error(std::string(Transcription::class_name()) + " - no value");
        }
        return val->value();
    }
};

class TranslationElement : public txml::XMLNode<TranslationElement, LanguageTitle, xdxf::TextElement>
{
    public:
    using base = txml::XMLNode<TranslationElement, LanguageTitle, xdxf::TextElement>

    static constexpr std::string_view class_name()
    {
        return "translation_unit";
    }

    static constexpr txml::TextReaderWrapper::NodeType class_node_type()
    {
        return txml::TextReaderWrapper::NodeType::Element;
    };
};


class TranslationContainer : public txml::XMLArray<TranslationContainer, TranslationElement>
{
public:
    using base_t = txml::XMLArray<TranslationContainer, TranslationElement>;

    static constexpr std::string_view class_name()
    {
        return "translation_container";
    }

    static constexpr txml::TextReaderWrapper::NodeType class_node_type()
    {
        return txml::TextReaderWrapper::NodeType::Element;
    }
};

class MultiArticle : public txml::XMLNode<MultiArticle, xdxf::KeyPhrase, xdxf::Transcription, TranslationContainer>
{
public:
    using base_t = txml::XMLNode<MultiArticle, xdxf::KeyPhrase, xdxf::Transcription, TranslationContainer>;

    static constexpr std::string_view class_name()
    {
        return "ar";
    }

    static constexpr txml::TextReaderWrapper::NodeType class_node_type()
    {
        return txml::TextReaderWrapper::NodeType::Element;
    }
};
#endif //TRANSLATION_MULTI_ARTICLE_HPP
