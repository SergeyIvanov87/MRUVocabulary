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
#include <txml/applications/xdxf/xdxf.hpp>
#include "TranslatorSharedData.h"
#include "TranslatorSharedDataImpl.h"

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

    if(ctx->version != XDXF_DICTIONARY_CURRENT_VERSION)
    {
        std::string ret("CTX version is mismatched: ");
        ret += std::to_string(ctx->version);
        ret += std::string(". Expected: ") + std::to_string(XDXF_DICTIONARY_CURRENT_VERSION);
        perror(ret.c_str());
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

        std::string xdxf_path = path->getValue();
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
            const std::string& name = art->value<xdxf::KeyPhrase>().value();
            inner_ctx->dictionary.insert({name, art});
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return ctx;
    }
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

template<class Tracer>
void translate(size_t freq, const std::string &word, xdxf_dictionary_context_v0* inner_ctx, size_t &found_num, Tracer& tracer)
{
    if (auto it = inner_ctx->dictionary.find(word); it != inner_ctx->dictionary.end())
    {
        if (!inner_ctx->shared_data_ptr)
        {
            inner_ctx->shared_data_ptr.reset(new SharedTranslatedData);
        }
        inner_ctx->shared_data_ptr->impl->insert(word, freq, it->second);
        found_num++;
    }
    else
    {
        if (log_level >= eLogLevel::DEBUG_LEVEL)
        {
            tracer.trace(word, " - ", freq, " is not found in: ",
                             inner_ctx->filePath);
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

    xdxf_dictionary_context_v0 *inner_ctx = reinterpret_cast<xdxf_dictionary_context_v0*>(translator_ctx->data);
    if (order == "direct")
    {
        for(auto word_pair = decoder_ctx->counts.begin(); word_pair != decoder_ctx->counts.end(); ++word_pair)
        {
            translate(word_pair->first, word_pair->second, inner_ctx, found_num, std_tracer);
        }
    }
    else
    {
        for(auto word_pair = decoder_ctx->counts.rbegin(); word_pair != decoder_ctx->counts.rend(); ++word_pair)
        {
            translate(word_pair->first, word_pair->second, inner_ctx, found_num, std_tracer);
        }
    }

    std_tracer.trace("Translated: ", found_num, " from: ", decoder_ctx->words.size());
    return found_num;
}

shared_translated_data_t* GET_SHARED_TRANSLATED_CTX_FUNC(plugin_ctx_t* translator_ctx)
{
    check_ctx(translator_ctx);
    xdxf_dictionary_context_v0 *inner_ctx = reinterpret_cast<xdxf_dictionary_context_v0*>(translator_ctx->data);
    return inner_ctx->shared_data_ptr.get();
}

char *SHARED_CTX_2_STRING_FUNC(shared_translated_data_t* translated_ctx)
{
    if (!translated_ctx)
    {
        return nullptr;
    }


    std::stringstream ss;
    translated_ctx->dump(ss);

    const std::string &str = ss.str();
    char *ret = (char*)malloc(str.size());
    memcpy(ret, str.data(), str.size());
    return ret;
}

const char* NAME_PLUGIN_FUNC()
{
    return XDXF_DICTIONARY_PLUGIN_NAME;
}

inline void SharedTranslatedDataImpl::dump(std::ostream &out) const
{
    out << "Translated words: " << local_dictionary.size() << std::endl;
    for (const auto& val : local_dictionary)
    {
        std::get<1>(val)->xml_serialize(out);
        out << std::endl;
    }
}

void SharedTranslatedData::dump(std::ostream &out) const
{
    impl->dump(out);
}
