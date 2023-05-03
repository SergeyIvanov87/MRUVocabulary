#include <stdio.h>
#include <stdarg.h>

#include "MultiXDXFPlugin.h"
#include "interfaces/PluginCtx.h"
#include <fstream>
#include <regex>
#include <cassert>
#include <list>
#include <string.h>
#include <error.h>
#include "common/base_command_parser/XDXFValue.h"
#include "common/base_command_parser/LogLevel.h"
#include "common/base_command_parser/TranslationOrder.h"
#include "common/base_command_parser/CommandArguments.hpp"
#include <txml/applications/xdxf/xdxf.hpp>
#include "SharedTranslatedData.h"

#include "translators/vers/TranslatedDataStructure_v0.h"
#include "decoders/PluginDecodedData.h"

struct indent {
  int depth_;
  explicit indent(int depth): depth_(depth) {};
};

std::ostream & operator<<(std::ostream & o, indent const & in)
{
  for(int i = 0; i != in.depth_; ++i)
  {
    o << "  ";
  }
  return o;
}


static eLogLevel log_level {eLogLevel::ERROR_LEVEL};
static std::string order("direct");

using avp_map = std::vector<std::pair<std::string, std::string>>;
static void extract_dictionary_attributes(const txml::TextReaderWrapper& reader,
                                          avp_map &lang_from, avp_map &lang_to)
{
    char *path = reinterpret_cast<char*>(xmlTextReaderBaseUri(reader.m_reader));
    std::string attribute_name  = reader.get_name();
    std::string attribute_value = reader.get_value();
    std::transform(attribute_value.begin(), attribute_value.end(), attribute_value.begin(),
                   [](unsigned char c){ return std::toupper(c); });
    if(attribute_name == "lang_from")
    {
        lang_from.push_back(std::make_pair<std::string, std::string>(path, std::move(attribute_value)));
    }
    if (attribute_name == "lang_to")
    {
        lang_to.push_back(std::make_pair<std::string, std::string>(path, std::move(attribute_value)));
    }

    free(path);
}

void __attribute__((constructor)) initPlugin(void)
{
    printf("MultiXDXF Plugin is initialized\n");
}

void __attribute__ ((destructor)) cleanPlugin(void)
{
    printf("MultiXDXF Plugin is exited\n");
}

static void check_ctx(plugin_ctx_t* ctx)
{
    if(!ctx or !ctx->data)
    {
        std::string ret("CTX is emty in plugin: ");
        ret += NAME_PLUGIN_FUNC();
        perror(ret.c_str());
        abort();
    }

    if(strncmp(ctx->id,  NAME_PLUGIN_FUNC(), strlen(NAME_PLUGIN_FUNC())))
    {
        std::string ret("CTX type invalid: ");
        ret += ctx->id;
        ret += std::string(". Expected: ") + NAME_PLUGIN_FUNC();
        perror(ret.c_str());
        abort();
    }

    if(ctx->version != MULTI_XDXF_DICTIONARY_CURRENT_VERSION)
    {
        std::string ret("CTX version is mismatched: ");
        ret += std::to_string(ctx->version);
        ret += std::string(". Expected: ") + std::to_string(MULTI_XDXF_DICTIONARY_CURRENT_VERSION);
        perror(ret.c_str());
        abort();
    }
}


namespace plugin_inner_op
{
    static void insert_data(v0::TranslatedDataStructure &out_data, const std::string& word, size_t repeat_num, const std::string& lang_to, std::optional<xdxf::XDXFArticle> article);
    static void dump_data(const v0::TranslatedDataStructure &out_data, std::ostream &out);
}

template<class ...Args>
void insert_data(int version, ISharedTranslatedData &out_data, Args&&...args)
{
    if (version == 0 || version == 1)
    {
        plugin_inner_op::insert_data(dynamic_cast<v0::TranslatedDataStructure&>(out_data), std::forward<Args>(args)...);
    }
    else
    {
        abort();
    }
}

template<class ...Args>
static void dump_data(int version, const ISharedTranslatedData &data, Args&&...args)
{
    if (version == 0 || version == 1)
    {
        plugin_inner_op::dump_data(dynamic_cast<const v0::TranslatedDataStructure&>(data), std::forward<Args>(args)...);
    }
    else
    {
        abort();
    }
}



plugin_ctx_t* INIT_PLUGIN_FUNC(const u_int8_t *data, size_t size)
{
    plugin_ctx_t *ctx = (plugin_ctx_t*)calloc(1, sizeof(plugin_ctx_t));
    ctx->version = MULTI_XDXF_DICTIONARY_CURRENT_VERSION;
    ctx->id = NAME_PLUGIN_FUNC();




    multi_xdxf_dictionary_context_v0 *inner_ctx = new multi_xdxf_dictionary_context_v0;
    inner_ctx->property_holder = CommandArgumentsBase::create<CommandArgumentsBase,
                                                              ParamCount,
                                                              XDXFTranslatorPath,
                                                              TranslationOrder,
                                                              LogLevel>();
    try
    {
        //get param count
        if (size < /*sizeof(ParamCount::class_name()) + */sizeof(ParamCount::value_t))
        {
            ctx->err = -1;
            std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. data size is too small, expected: "
                      << /*sizeof(ParamCount::class_name()) + */sizeof(ParamCount::value_t) <<", got: "
                      << size << std::endl;
            return ctx;
        }

        std::istringstream inbuf(std::string((const char*)data, size));
        auto paramCount = ValueBase::unpack<ParamCount>(inbuf);

        if (!paramCount or paramCount->getValue() < 1)
        {
            ctx->err = -1;
            std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. Set '"
                      << ParamCount::class_name() <<"' at least" << std::endl;
            return ctx;
        }
        size_t params = paramCount->getValue();

        //path
        auto path = ValueBase::unpack<XDXFTranslatorPath>(inbuf);
        if (!path or path->getValue().empty())
        {
            ctx->err = -1;
            std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. '"
                      << XDXFTranslatorPath::class_name() <<"' is empty" << std::endl;
            return ctx;
        }
        params--;

        const std::vector<std::string> &multi_xdxf_pathes = path->getValue();
        if (multi_xdxf_pathes.empty())
        {
            ctx->err = -1;
            std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. '"
                      << XDXFTranslatorPath::class_name() <<"' no any path provided. Please set at list one"
                      << std::endl;
            return ctx;
        }

        size_t dict_count = 0;
        for (const std::string &multi_xdxf_path : multi_xdxf_pathes)
        {
            FILE* file = fopen(multi_xdxf_path.c_str(), "r");
            if(!file)
            {
                ctx->err = errno;
                std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. '"
                          << XDXFTranslatorPath::class_name() <<"' no XDXF dictionary at: "
                          << multi_xdxf_path << std::endl;
                return ctx;
            }
            fseek(file, 0, SEEK_END);
            ctx->data_size += ftell(file);
            rewind(file);
            ctx->err = 0;
            fclose(file);

            // remember dict path
            inner_ctx->filePathes.emplace(std::to_string(++dict_count), multi_xdxf_path);
        }
        ctx->data_offset = 0;

        //order
        if (params != 0)
        {
            auto order_param = ValueBase::unpack<TranslationOrder>(inbuf);
            if (order_param)
            {
                order = order_param->getValue();
            }
        }
        params--;

        //log
        if (params != 0)
        {
            auto log = ValueBase::unpack<LogLevel>(inbuf);
            if (log)
            {
                log_level = log->getValue();
            }
        }
        params--;
    }
    catch(const std::exception &ex)
    {
        ctx->err = -1;
        std::cerr << NAME_PLUGIN_FUNC() << " Got exception durign arguments parsing: "
                  << ex.what() << std::endl;
        return ctx;
    }

    // iterate over all dictionary files and collect articles
    std::locale::global(std::locale(""));
    try
    {
        txml::StdoutTracer std_tracer;
        txml::EmptyTracer empty_tracer;

        static const std::string unknown_target_lang("--UNKNOWN--");
        avp_map dictionary_lang_from;
        avp_map dictionary_lang_to;
        size_t dictionaries_loaded_count = 0;
        for (const auto &val : inner_ctx->filePathes)
        {
            auto dictionary_xml_pointer = std::make_unique<txml::TextReaderWrapper>(val.second);
            dictionaries_loaded_count++;
            int depth = 0;

            // parse xdxf header
            const size_t nodes_before_quit_from_header_parsing = 5;
            size_t nodes_counter = 0;
            while (dictionary_xml_pointer->read() && nodes_counter < nodes_before_quit_from_header_parsing)
            {
                auto node_type = dictionary_xml_pointer->get_node_type();
                auto node_name = dictionary_xml_pointer->get_name();
                if (node_name == "xdxf" && node_type == txml::TextReaderWrapper::NodeType::Element)
                {
                    if (dictionary_xml_pointer->has_attributes())
                    {
                        if(dictionary_xml_pointer->move_to_first_attribute())
                        {
                            extract_dictionary_attributes(*dictionary_xml_pointer, dictionary_lang_from, dictionary_lang_to);
                        }

                        while (dictionary_xml_pointer->move_to_next_attribute())
                        {
                            extract_dictionary_attributes(*dictionary_xml_pointer, dictionary_lang_from, dictionary_lang_to);
                        }
                    }
                    break;
                }
                nodes_counter++;
            }

            // analyze attributes
            // 1) lang_to
            if (dictionaries_loaded_count != dictionary_lang_to.size())
            {
                // put '--UNKNOWN--' lang
                dictionary_lang_to.emplace_back(val.second, unknown_target_lang);
            }

            // 2) lang_from
            auto begin_lang_from = dictionary_lang_from.begin();
            auto rbegin_lang_from = dictionary_lang_from.rbegin();
            if ((begin_lang_from->second != rbegin_lang_from->second))
            {
                ctx->err = -1;
                std::cerr << NAME_PLUGIN_FUNC() << " invalid dictionaries set.\nAttributes 'lang_from' are expected to be equal for all dictionaries.\n"
                          << "The first contains: \"" << begin_lang_from->second << "\" which conflicts with \"" << rbegin_lang_from->second << "\" from dictionary with path: "
                          << rbegin_lang_from->first << std::endl;
                std::cerr << "Total list:" << std::endl;
                for (auto v : dictionary_lang_from)
                {
                    std::cerr << v.first << " <- from <- " << v.second << std::endl;
                }
                return ctx;
            }
            // parse articles

            assert(!dictionary_lang_to.empty());
            auto &lang_to_el = dictionary_lang_to.back();
            const std::string &lang_to = lang_to_el.second;
            auto &target_dictionary = inner_ctx->multidictionary[lang_to];
            while(dictionary_xml_pointer->read())
            {
                depth = dictionary_xml_pointer->get_depth();
                std::optional<xdxf::XDXFArticle> art;
                if (log_level >= eLogLevel::DEBUG_LEVEL)
                {
                    art = txml::XMLCreator::try_create<xdxf::XDXFArticle>(*dictionary_xml_pointer,
                                                                          std_tracer);
                }
                else
                {
                    art = txml::XMLCreator::try_create<xdxf::XDXFArticle>(*dictionary_xml_pointer,
                                                                          empty_tracer);
                }

                if (!art)
                {
                    continue;
                }

                if (!art->has_value<xdxf::KeyPhrase>())
                {
                    if (log_level >= eLogLevel::DEBUG_LEVEL)
                    {
                        std_tracer << indent(depth) << "skip: no_key_phrase\n";
                    }
                    else
                    {
                        empty_tracer << indent(depth) << "skip: no_key_phrase\n";
                    }
                    continue;
                }
                const std::string& name = art->value<xdxf::KeyPhrase>().value();

                target_dictionary.insert({name, art});
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return ctx;
    }

    // finalize innder ctx creation
    if (!inner_ctx->shared_data_ptr)
    {
        inner_ctx->shared_data_ptr.reset(new SharedTranslatedData(ctx->version));
    }
    ctx->data = reinterpret_cast<void*>(inner_ctx);
    return ctx;
}

bool SET_PARAM_PLUGIN_FUNC(plugin_ctx_t* ctx, const char* param_name, ...)
{
    check_ctx(ctx);
    multi_xdxf_dictionary_context_v0 *inner_ctx = reinterpret_cast<multi_xdxf_dictionary_context_v0*>(ctx->data);

    int argc = 1;
    va_list valist;
    va_start(valist, param_name);
    std::string value;
    for (int i = 0; i < 1; i++)
    {
        value.assign(va_arg(valist, const char*));
    }
    va_end(valist);

    const char *argv[] {param_name, value.c_str()};
    try
    {
        inner_ctx->property_holder->makeParamsExtract(argc, argv);
    }
    catch(const std::exception &ex)
    {
        printf("%s - error: %s", __FUNCTION__, ex.what());
        return false;
    }
    return true;
}

bool GET_PARAM_PLUGIN_FUNC(plugin_ctx_t* ctx, const char* param_name, ...)
{
    return false;
}


bool SET_TYPED_PARAMS_PLUGIN_FUNC(plugin_ctx_t* ctx, int argc, const void *data[])
{
    check_ctx(ctx);
    multi_xdxf_dictionary_context_v0 *inner_ctx = reinterpret_cast<multi_xdxf_dictionary_context_v0*>(ctx->data);

    try
    {
        inner_ctx->property_holder->makeParamsDeserialize(argc, data);
    }
    catch(const std::exception &ex)
    {
        printf("%s - error: %s", __FUNCTION__, ex.what());
        return false;
    }
    return true;
}

void RELEASE_PLUGIN_FUNC(plugin_ctx_t* ctx)
{
    if (ctx)
    {
        check_ctx(ctx);
        delete reinterpret_cast<multi_xdxf_dictionary_context_v0*>(ctx->data);
    }
    free(ctx);
}

void RELEASE_SHARED_CTX_FUNC(shared_ctx_t* ctx)
{
    if (ctx)
    {
        // TODO check_ctx(ctx);
        // TODO #delete reinterpret_cast<multi_xdxf_dictionary_context_v0*>(ctx->data);
    }
    free(ctx);
}

template<class Tracer>
void translate(int version, size_t freq, const std::string &word, multi_xdxf_dictionary_context_v0* inner_ctx, size_t &found_num, Tracer& tracer)
{
    for (const auto &lang_dict_pair: inner_ctx->multidictionary)
    {
        if (auto it = lang_dict_pair.second.find(word); it != lang_dict_pair.second.end())
        {
            insert_data(version, inner_ctx->shared_data_ptr->getImpl(), word, freq, lang_dict_pair.first, it->second);
            found_num++;
        }
        else
        {
            if (log_level >= eLogLevel::INFO_LEVEL)
            {
                tracer.trace(word, " - ", freq, " is not found in dictionary: ", lang_dict_pair.first);
            }
        }
    }
}

long long TRANSLATE_PLUGIN_FUNC(plugin_ctx_t* translator_ctx, shared_decoded_data_t* decoder_ctx)
{
    check_ctx(translator_ctx);


    txml::StdoutTracer std_tracer;
    size_t found_num = 0;

    if (!decoder_ctx)
    {
        return found_num;
    }

    multi_xdxf_dictionary_context_v0 *inner_ctx = reinterpret_cast<multi_xdxf_dictionary_context_v0*>(translator_ctx->data);
    if (order == "direct")
    {
        for(auto word_pair = decoder_ctx->counts.begin(); word_pair != decoder_ctx->counts.end(); ++word_pair)
        {
            translate(translator_ctx->version, word_pair->first, word_pair->second, inner_ctx, found_num, std_tracer);
        }
    }
    else
    {
        for(auto word_pair = decoder_ctx->counts.rbegin(); word_pair != decoder_ctx->counts.rend(); ++word_pair)
        {
            translate(translator_ctx->version, word_pair->first, word_pair->second, inner_ctx, found_num, std_tracer);
        }
    }

    std_tracer.trace("Translated: ", found_num, " from: ", decoder_ctx->words.size());
    return found_num;
}

shared_translated_data_t* GET_SHARED_TRANSLATED_CTX_FUNC(plugin_ctx_t* translator_ctx)
{
    check_ctx(translator_ctx);
    multi_xdxf_dictionary_context_v0 *inner_ctx = reinterpret_cast<multi_xdxf_dictionary_context_v0*>(translator_ctx->data);
    return inner_ctx->shared_data_ptr.get();
}

char *SHARED_CTX_2_STRING_FUNC(shared_translated_data_t* translated_ctx)
{
    if (!translated_ctx)
    {
        return nullptr;
    }


    std::stringstream ss;
    dump_data(translated_ctx->getVersion(), translated_ctx->getImpl(), ss);

    const std::string &str = ss.str();
    char *ret = (char*)malloc(str.size() + 1);
    memcpy(ret, str.data(), str.size());
    ret[str.size()]='\0';
    return ret;
}

const char* NAME_PLUGIN_FUNC()
{
    return MULTI_XDXF_DICTIONARY_PLUGIN_NAME;
}


namespace plugin_inner_op
{
static void insert_data(v0::TranslatedDataStructure &out_data, const std::string& word, size_t repeat_num, const std::string &lang_to, std::optional<xdxf::XDXFArticle> article)
{
    auto &comment = article->node_or<xdxf::Comment>("0");

    std::string &cur_val = comment->value();
    size_t cur_repeatition_count = std::stoull(cur_val);
    cur_repeatition_count += repeat_num;
    cur_val = std::to_string(cur_repeatition_count);

    if (!out_data.local_dictionary.empty())
    {
        auto &last_word = out_data.local_dictionary.back();
        if (std::get<0>(last_word) == word)
        {
            auto &articles = std::get<1>(last_word);
            assert(!articles.empty());
            if (auto it = articles.find(lang_to); it != articles.end())
            {
                it->second = *article;
                if (log_level >= eLogLevel::DEBUG_LEVEL)
                {
                    std::cout << "language: " << lang_to << "  found, for word: " << word << std::endl;
                }
            }
            else
            {
                if (log_level >= eLogLevel::DEBUG_LEVEL)
                {
                    std::cout << "update language: " << lang_to << " for word: " << word << std::endl;
                }
                articles.emplace(lang_to, article);
            }
            return;
        }
        else
        {
            if (log_level >= eLogLevel::DEBUG_LEVEL)
            {
                std::cout << "found a new word: " << word << ", language: " << lang_to << std::endl;
            }
        }
    }

    out_data.local_dictionary.emplace_back(std::forward_as_tuple(word,
                                           v0::TranslatedDataStructure::Articles({{lang_to, article}})));

    (void)repeat_num;
}

static void dump_data(const v0::TranslatedDataStructure &out_data, std::ostream &out)
{
    out << "MultiXDXF Translated words: " << out_data.local_dictionary.size() << std::endl;
    for (const auto& val : out_data.local_dictionary)
    {
        const auto &[word, volume] = val;
        assert(!volume.empty());
        out << "languages: " << volume.size() << std::endl;
        for (const auto &lang_article : volume)
        {
            out << lang_article.first << std::endl;
            lang_article.second->xml_serialize(out);
            out << std::endl;
        }
    }
}

}
