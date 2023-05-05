#include <stdio.h>
#include <stdarg.h>

#include "XDXFPlugin.h"
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

#include "translators/vers/TranslatedDataStructure_v0.h"
#include "decoders/vers/PluginDecodedData.h"

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

void __attribute__((constructor)) initPlugin(void)
{
    printf("Library is initialized\n");
}

void __attribute__ ((destructor)) cleanPlugin(void)
{
    printf("Library is exited\n");
}

static void check_ctx(plugin_ctx_t* ctx)
{
    if(!ctx or !ctx->data)
    {
        std::string ret("CTX is empty in plugin: ");
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

    if(ctx->version != XDXF_DICTIONARY_CURRENT_VERSION)
    {
        std::string ret("CTX version is mismatched: ");
        ret += std::to_string(ctx->version);
        ret += std::string(". Expected: ") + std::to_string(XDXF_DICTIONARY_CURRENT_VERSION);
        perror(ret.c_str());
        abort();
    }
}


namespace plugin_inner_op
{
    static void insert_data(v0::TranslatedDataStructure &out_data, const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article);
    static void dump_data(const v0::TranslatedDataStructure &out_data, std::ostream &out);
}

template<class ...Args>
static void insert_data(int version, ISharedTranslatedData &out_data, Args&&...args)
{
    if (version == 0)
    {
        plugin_inner_op::insert_data(dynamic_cast<v0::TranslatedDataStructure&>(out_data), std::forward<Args>(args)...);
    }
    else
    {
        abort();
    }
}

template<class ...Args>
static void dump_data(int version, const ISharedTranslatedData &out_data, Args&&...args)
{
    if (version == 0)
    {
        plugin_inner_op::dump_data(dynamic_cast<const v0::TranslatedDataStructure&>(out_data), std::forward<Args>(args)...);
    }
    else
    {
        abort();
    }
}


plugin_ctx_t* INIT_PLUGIN_FUNC(const u_int8_t *data, size_t size)
{
    plugin_ctx_t *ctx = (plugin_ctx_t*)calloc(1, sizeof(plugin_ctx_t));
    ctx->version = XDXF_DICTIONARY_CURRENT_VERSION;
    ctx->id = NAME_PLUGIN_FUNC();




    xdxf_dictionary_context_v0 *inner_ctx = new xdxf_dictionary_context_v0;
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

        const std::vector<std::string> &xdxf_pathes = path->getValue();
        if (xdxf_pathes.empty())
        {
            ctx->err = -1;
            std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. '"
                      << XDXFTranslatorPath::class_name() <<"' no any path provided. Please set at list one"
                      << std::endl;
            return ctx;
        }

        const std::string &xdxf_path = xdxf_pathes.front();
        FILE* file = fopen(xdxf_path.c_str(), "r");
        if(!file)
        {
            ctx->err = errno;
            std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. '"
                      << XDXFTranslatorPath::class_name() <<"' no XDXF dictionary at: "
                      << xdxf_path << std::endl;
            return ctx;
        }
        fseek(file, 0, SEEK_END);
        ctx->data_size = ftell(file);
        ctx->data_offset = 0;
        rewind(file);
        ctx->err = 0;
        fclose(file);
        inner_ctx->filePath = xdxf_path;

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

    std::locale::global(std::locale(""));
    try
    {
        txml::StdoutTracer std_tracer;
        txml::EmptyTracer empty_tracer;

        inner_ctx->xml_reader = std::make_unique<txml::TextReaderWrapper>(inner_ctx->filePath);
        int depth;
        while(inner_ctx->xml_reader->read())
        {
            depth = inner_ctx->xml_reader->get_depth();
            std::optional<xdxf::XDXFArticle> art;
            if (log_level >= eLogLevel::DEBUG_LEVEL)
            {
                art = txml::XMLCreator::try_create<xdxf::XDXFArticle>(*inner_ctx->xml_reader,
                                                                      std_tracer);
            }
            else
            {
                art = txml::XMLCreator::try_create<xdxf::XDXFArticle>(*inner_ctx->xml_reader,
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
            std::string name = art->value<xdxf::KeyPhrase>().value();
            std::transform(name.begin(), name.end(), name.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            inner_ctx->dictionary.insert({name, art});
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return ctx;
    }

    // finalize inner ctx creation
    ctx->data = reinterpret_cast<void*>(inner_ctx);
    return ctx;
}

bool SET_PARAM_PLUGIN_FUNC(plugin_ctx_t* ctx, const char* param_name, ...)
{
    check_ctx(ctx);
    xdxf_dictionary_context_v0 *inner_ctx = reinterpret_cast<xdxf_dictionary_context_v0*>(ctx->data);

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
    xdxf_dictionary_context_v0 *inner_ctx = reinterpret_cast<xdxf_dictionary_context_v0*>(ctx->data);

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
        delete reinterpret_cast<xdxf_dictionary_context_v0*>(ctx->data);
    }
    free(ctx);
}

session_t* ALLOCATE_SESSION_FUNC(plugin_ctx_t* global_ctx, const u_int8_t *data, size_t size)
{
    session_t *ctx = (session_t*)calloc(1, sizeof(session_t));
    ctx->version = XDXF_DICTIONARY_CURRENT_VERSION;
    ctx->id = NAME_PLUGIN_FUNC();
    if (ctx->version == 0)
    {
        ctx->data = new v0::TranslatedDataStructure;
    }
    else
    {
        throw std::runtime_error("Cannot create TranslatedDataStructure, version is unsupported: " + std::to_string(ctx->version));
    }
    return ctx;
}

void RELEASE_SESSION_FUNC(plugin_ctx_t*, session_t* ctx)
{
    if (ctx)
    {
        //check_session_ctx(ctx);
        delete reinterpret_cast<ISharedTranslatedData*>(ctx->data);;
        ctx->data = nullptr;
    }
    ctx->version = -1;
    ctx->id = nullptr;
    free(ctx);
}

template<class Tracer>
static void translate(int version, size_t freq, const std::string &word,
                      xdxf_dictionary_context_v0 *plugin_ctx_instance, ISharedTranslatedData* inner_ctx, size_t &found_num, Tracer& tracer)
{
    std::string lower_case_word = word;
    std::transform(word.begin(), word.end(), lower_case_word.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    if (auto it = plugin_ctx_instance->dictionary.find(lower_case_word); it != plugin_ctx_instance->dictionary.end())
    {
        const auto &dict_word = it->second->value<xdxf::KeyPhrase>().value();
        insert_data(version, *inner_ctx, dict_word, freq, it->second);
        found_num++;
    }
    else
    {
        if (log_level >= eLogLevel::INFO_LEVEL)
        {
            tracer.trace(word, " - ", freq, " is not found in: ",
                             plugin_ctx_instance->filePath);
        }
    }
}

long long TRANSLATE_PLUGIN_FUNC(plugin_ctx_t* translator_ctx, session_t* in_decoder_sess,
                                session_t *out_translator_session)
{
    check_ctx(translator_ctx);


    txml::StdoutTracer std_tracer;
    size_t found_num = 0;

    if (!in_decoder_sess || !in_decoder_sess->data)
    {
        return found_num;
    }

    SharedDecodedData *in_decoder_ctx = nullptr;
    if(in_decoder_sess->version == 0)
    {
        in_decoder_ctx = reinterpret_cast<SharedDecodedData *>(in_decoder_sess->data);
    }
    else
    {
        abort();
    }

    ISharedTranslatedData *inner_ctx = reinterpret_cast<ISharedTranslatedData*>(out_translator_session->data);
    xdxf_dictionary_context_v0 *ctx = reinterpret_cast<xdxf_dictionary_context_v0*>(translator_ctx->data);
    if (order == "direct")
    {
        for(auto word_pair = in_decoder_ctx->counts.begin(); word_pair != in_decoder_ctx->counts.end(); ++word_pair)
        {
            translate(translator_ctx->version, word_pair->first, word_pair->second, ctx, inner_ctx, found_num, std_tracer);
        }
    }
    else
    {
        for(auto word_pair = in_decoder_ctx->counts.rbegin(); word_pair != in_decoder_ctx->counts.rend(); ++word_pair)
        {
            translate(translator_ctx->version, word_pair->first, word_pair->second, ctx, inner_ctx, found_num, std_tracer);
        }
    }

    std_tracer.trace("Translated: ", found_num, " from: ", in_decoder_ctx->words.size());
    return found_num;
}

char *SHARED_CTX_2_STRING_FUNC(plugin_ctx_t* in_translator_ctx, session_t* in_translator_session)
{
    if (!in_translator_ctx || in_translator_ctx->data || !in_translator_session || !in_translator_session->data)
    {
        return nullptr;
    }

    ISharedTranslatedData *inner_ctx = reinterpret_cast<ISharedTranslatedData*>(in_translator_session->data);
    xdxf_dictionary_context_v0 *ctx = reinterpret_cast<xdxf_dictionary_context_v0*> (in_translator_ctx->data);

    std::stringstream ss;
    dump_data(in_translator_session->version, *inner_ctx, ss);

    const std::string &str = ss.str();
    char *ret = (char*)malloc(str.size() + 1);
    memcpy(ret, str.data(), str.size());
    ret[str.size()]='\0';
    return ret;
}

const char* NAME_PLUGIN_FUNC()
{
    return XDXF_DICTIONARY_PLUGIN_NAME;
}


namespace plugin_inner_op
{
static void insert_data(v0::TranslatedDataStructure &out_data, const std::string& word, size_t repeat_num, std::optional<xdxf::XDXFArticle> article)
{
    auto &comment = article->node_or<xdxf::Comment>("0");

    std::string &cur_val = comment->value();
    size_t cur_repeatition_count = std::stoull(cur_val);
    cur_repeatition_count += repeat_num;
    cur_val = std::to_string(cur_repeatition_count);

    if (! out_data.local_dictionary.empty())
    {
        auto &last_word = out_data.local_dictionary.back();
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

    out_data.local_dictionary.emplace_back(std::forward_as_tuple(word,
                                           v0::TranslatedDataStructure::Articles({{std::string(""), article}})));
    (void)repeat_num;
}


static void dump_data(const v0::TranslatedDataStructure &out_data, std::ostream &out)
{
    out << "XDXF Translated words: " << out_data.local_dictionary.size() << std::endl;
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
