#include <stdio.h>
#include <stdarg.h>

#include "XDXFOutputPlugin.h"
#include "interfaces/PluginCtx.h"
#include <fstream>
#include <regex>
#include <cassert>
#include <list>
#include <string.h>
#include <error.h>
#include "common/base_command_parser/DestinationValue.h"
#include "common/base_command_parser/DestinationFormat.h"
#include "common/base_command_parser/LogLevel.h"
#include "common/base_command_parser/CommandArguments.hpp"

#include "Formatter.hpp"

#include "vers/TranslatorSharedDataImpl_v1.hpp"
#include "vers/TranslatorSharedDataImpl_v0.hpp"

#include "xdxf/model/MultiArticle.hpp"

static eLogLevel log_level {eLogLevel::ERROR_LEVEL};

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

    if(ctx->version != XDXF_OUTPUT_CURRENT_VERSION)
    {
        std::string ret("CTX version is mismatched: ");
        ret += std::to_string(ctx->version);
        ret += std::string(". Expected: ") + std::to_string(XDXF_OUTPUT_CURRENT_VERSION);
        perror(ret.c_str());
        abort();
    }
}

namespace plugin_inner_op
{
    inline void insert_data(v0::OutputSessionCtx &data, const std::string &word, const std::optional<xdxf::XDXFArticle> &art);
    inline void insert_data(v1::OutputSessionCtx &data, const std::string &word, const std::optional<MultiArticle> &art);

    template<class Formatter>
    inline void format_dump(const v0::OutputSessionCtx &data, Formatter &out, const std::string &format);

    template<class Formatter>
    inline void format_dump(const v1::OutputSessionCtx &data, Formatter &out, const std::string &format);
}


inline void insert_data(int version, ISharedTranslatedData &data, const std::string &word, const std::optional<MultiArticle> &art)
{
    if (version == 0)
    {
        abort();
    }
    else if(version == 1)
    {
        plugin_inner_op::insert_data(dynamic_cast<v1::OutputSessionCtx&>(data), word, art);
    }
    else
    {
        abort();
    }
}

inline void insert_data(int version, ISharedTranslatedData &data, const std::string &word, const std::optional<xdxf::XDXFArticle> &art)
{
    if (version == 0)
    {
        plugin_inner_op::insert_data(dynamic_cast<v0::OutputSessionCtx&>(data), word, art);
    }
    else if(version == 1)
    {
        abort();
    }
    else
    {
        abort();
    }
}

template<class Formatter>
inline void format_dump(int version, const ISharedTranslatedData &data, Formatter &out, const std::string &format)
{
    if (version == 0)
    {
        plugin_inner_op::format_dump(dynamic_cast<const v0::OutputSessionCtx&>(data), out, format);
    }
    else if(version == 1)
    {
        plugin_inner_op::format_dump(dynamic_cast<const v1::OutputSessionCtx&>(data), out, format);
    }
    else
    {
        abort();
    }
}

plugin_ctx_t* INIT_PLUGIN_FUNC(const u_int8_t *data, size_t size)
{
    plugin_ctx_t *ctx = (plugin_ctx_t*)calloc(1, sizeof(plugin_ctx_t));
    ctx->version = XDXF_OUTPUT_CURRENT_VERSION;
    ctx->id = NAME_PLUGIN_FUNC();




    xdxf_output_context_v0 *inner_ctx = new xdxf_output_context_v0;
    inner_ctx->property_holder = CommandArgumentsBase::create<CommandArgumentsBase,
                                                              ParamCount,
                                                              DestinationValue,
                                                              DestinationFormat,
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
        auto path = ValueBase::unpack<DestinationValue>(inbuf);
        if (!path or path->getValue().empty())
        {
            ctx->err = -1;
            std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. '"
                      << DestinationValue::class_name() <<"' is empty" << std::endl;
            return ctx;
        }
        params--;
        std::string xdxf_path = path->getValue();
        inner_ctx->filePath = xdxf_path;

        //format
        auto format = ValueBase::unpack<DestinationFormat>(inbuf);
        if (!format or format->getValue().empty())
        {
            ctx->err = -1;
            std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. '"
                      << DestinationFormat::class_name() <<"' is empty" << std::endl;
            return ctx;
        }
        inner_ctx->fileFormat = format->getValue();
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

    FILE* file = fopen(inner_ctx->filePath.c_str(), "r");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        ctx->data_size = ftell(file);
        ctx->data_offset = 0;
        rewind(file);
        ctx->err = 0;
        fclose(file);
    }
    else
    {
        ctx->data = reinterpret_cast<void*>(inner_ctx);
        return ctx;
    }

    try
    {
        txml::StdoutTracer std_tracer;
        txml::EmptyTracer empty_tracer;

        // TODO
        // TRY Parse, if format throws exception - upgrade SharedTranslatedData version
        // and try again with MultiArticle
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
                    std_tracer << "skip: no_key_phrase\n";
                }
                else
                {
                    empty_tracer << "skip: no_key_phrase\n";
                }
                continue;
            }

            const std::string& name = art->value<xdxf::KeyPhrase>().value();
            if(!inner_ctx->translated_data_ptr)
            {
                // TODO inner_ctx->translated_data_ptr.reset(new SharedTranslatedData(ctx->version));
                abort(); //TODO
            }
            // insert_data(ctx->version, inner_ctx->translated_data_ptr->getImpl(), name, art);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return ctx;
    }

    //check result
    if (!inner_ctx->translated_data_ptr || ctx->data_size != 0)
    {
        // cannot parse file with XDXFArticle model
        ctx->err = -1;
        std::cerr << NAME_PLUGIN_FUNC() << " Cannot parse existing file. Try different plugin" << std::endl;
        delete inner_ctx;
        return ctx;
    }

    ctx->data = reinterpret_cast<void*>(inner_ctx);
    return ctx;
}

long long WRITE_TRANSLATED_DATA_PLUGIN_FUNC(plugin_ctx_t* ctx, session_t *translated_ctx)
{
    if (!translated_ctx)
    {
        return 0;
    }

    check_ctx(ctx);
    xdxf_output_context_v0 *inner_ctx = reinterpret_cast<xdxf_output_context_v0*>(ctx->data);

    inner_ctx->merge(translated_ctx);

    std::stringstream ss;
    if (inner_ctx->fileFormat != "fb2")
    {
        ss << R"dict_header(<?xml version="1.0" encoding="UTF-8" ?>
              <!DOCTYPE xdxf SYSTEM "http://xdxf.sourceforge.net/xdxf_lousy.dtd">
              <xdxf lang_from="ENG" lang_to="RUS" format="visual">
              <full_name>)dict_header";
        ss << inner_ctx->filePath << "</full_name>\n<description>TODO</description>\n";

        //    if (log_level >= eLogLevel::DEBUG_LEVEL)
        //    {
            //ToXDXF out(ss);
            format_dump(ctx->version, *inner_ctx->translated_data_ptr, ss, inner_ctx->fileFormat);
        //    }
        //    else
        //    {
        //        inner_ctx->translated_data_ptr->dump(ss, empty_tracer);
        //    }

        ss << "</xdxf>";
    }
    else
    {
        ss << R"dict_header(<FictionBook xmlns="http://www.gribuser.ru/xml/fictionbook/2.0" xmlns:l="http://www.w3.org/1999/xlink">
<description></description>
<body>)dict_header";
        //xdxf::ToFB2 out(ss);
        format_dump(ctx->version, *inner_ctx->translated_data_ptr, ss, inner_ctx->fileFormat);

        ss << R"dict_footer(
</body>
</FictionBook>)dict_footer";
    }

    const std::string &str = ss.str();
    FILE* file = fopen(inner_ctx->filePath.c_str(), "w");
    if(!file)
    {
        ctx->err = errno;
        std::cerr << NAME_PLUGIN_FUNC() << " Cannot open file to append: " << inner_ctx->filePath
                  << ". Error: " << strerror(errno) << std::endl;
        return 0;
    }

    fprintf(file, "%s", str.c_str());
    return 0;
}

void xdxf_output_context_v0::merge(session_t *new_data)
{
    if (!new_data)
    {
        return;
    }

    if (!translated_data_ptr)
    {
        if (new_data->version == 0)
        {
            translated_data_ptr.reset(new v0::OutputSessionCtx(*reinterpret_cast<ISharedTranslatedData*>(new_data->data)));
        }
        else if (new_data->version == 1)
        {
            translated_data_ptr.reset(new v1::OutputSessionCtx(*reinterpret_cast<ISharedTranslatedData*>(new_data->data)));
        }
        else
        {
            throw std::runtime_error("Cannot create OutputSessionCtx, version is unsupported: " + std::to_string(new_data->version));
        }
    }

    //TODO
}

bool SET_PARAM_PLUGIN_FUNC(plugin_ctx_t* ctx, const char* param_name, ...)
{
    check_ctx(ctx);
    xdxf_output_context_v0 *inner_ctx = reinterpret_cast<xdxf_output_context_v0*>(ctx->data);

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
    xdxf_output_context_v0 *inner_ctx = reinterpret_cast<xdxf_output_context_v0*>(ctx->data);

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
        delete reinterpret_cast<xdxf_output_context_v0*>(ctx->data);
    }
    free(ctx);
}

session_t* ALLOCATE_SESSION_FUNC(plugin_ctx_t* ctx, const u_int8_t *data, size_t size)
{
    return nullptr;
}

void RELEASE_SESSION_FUNC(plugin_ctx_t*, session_t* ctx)
{
    if (ctx)
    {
    }
    free(ctx);
}

const char* NAME_PLUGIN_FUNC()
{
    return XDXF_OUTPUT_PLUGIN_NAME;
}

namespace plugin_inner_op
{
inline void insert_data(v0::OutputSessionCtx &data, const std::string &word, const std::optional<xdxf::XDXFArticle> &article)
{
    data.local_dictionary.emplace_back(std::forward_as_tuple(word, v0::OutputSessionCtx::Articles{{"", article}}));
}

inline void insert_data(v1::OutputSessionCtx &data, const std::string &word, const std::optional<MultiArticle> &art)
{
}

template<class Formatter>
inline void format_dump(const v0::OutputSessionCtx &data, Formatter &out, const std::string &format)
{
    txml::StdoutTracer std_tracer;
    txml::EmptyTracer empty_tracer;

    if (log_level >= eLogLevel::DEBUG_LEVEL)
    {
        v0::format_serialize(data, out, format, std_tracer);
    }
    else
    {
        v0::format_serialize(data, out, format, empty_tracer);
    }
}

template<class Formatter>
inline void format_dump(const v1::OutputSessionCtx &data, Formatter &out, const std::string &format)
{
    txml::StdoutTracer std_tracer;
    txml::EmptyTracer empty_tracer;

    if (log_level >= eLogLevel::DEBUG_LEVEL)
    {
        data.format_serialize(out, format, std_tracer);
    }
    else
    {
        data.format_serialize(out, format, empty_tracer);
    }
}
}
