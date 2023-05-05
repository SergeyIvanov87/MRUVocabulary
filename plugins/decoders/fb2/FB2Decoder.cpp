#include <stdio.h>
#include <stdarg.h>

#include "FB2Decoder.h"
#include "interfaces/PluginCtx.h"
#include <fstream>
#include <regex>
#include <cassert>
#include <list>
#include <string.h>
#include <error.h>
#include "common/base_command_parser/RegexFilterParam.h"
#include "common/base_command_parser/FilterWords.hpp"
#include "common/base_command_parser/RegexFilterWords.hpp"
#include "common/base_command_parser/LogLevel.h"
#include "common/base_command_parser/CommandArguments.hpp"

#include <txml/applications/fb2/fb2.hpp>

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

    if(ctx->version != FB2_DECODER_CURRENT_VERSION)
    {
        std::string ret("CTX version is mismatched: ");
        ret += std::to_string(ctx->version);
        ret += std::string(". Expected: ") + std::to_string(FB2_DECODER_CURRENT_VERSION);
        perror(ret.c_str());
        abort();
    }
}

std::list<std::string> tokenize(const std::string &data, std::string rgx_str, size_t& elapsed)
{
    (void)elapsed;

    std::list<std::string> elems;
    std::regex rgx (rgx_str);

    std::sregex_token_iterator iter(data.begin(), data.end(), rgx, -1);
    std::sregex_token_iterator end;

    while (iter != end)
    {
        elems.push_back(*iter);
        ++iter;
    }

    return elems;
}


plugin_ctx_t* INIT_PLUGIN_FUNC(const u_int8_t *data, size_t size)
{
    plugin_ctx_t *ctx = (plugin_ctx_t*)calloc(1, sizeof(plugin_ctx_t));
    ctx->version = FB2_DECODER_CURRENT_VERSION;
    ctx->id = NAME_PLUGIN_FUNC();


    FILE* file = fopen((const char*)data, "r");
    if(!file)
    {
        ctx->err = errno;
        return ctx;
    }
    fseek(file, 0, SEEK_END);
    ctx->data_size = ftell(file);
    ctx->data_offset = 0;
    rewind(file);
    ctx->err = 0;
    fclose(file);

    fb2_context_v0 *inner_ctx = new fb2_context_v0;
    inner_ctx->filePath = (const char*)data;
    inner_ctx->property_holder = CommandArgumentsBase::create<CommandArgumentsBase,
                                                              ParamCount,
                                                              RegexFilterValue,
                                                              FilterWordsValue,
                                                              RegexFilterWordsValue,
                                                              LogLevel>();
    /*
    try
    {
        //get param count
        if (size < sizeof(ParamCount::value_t))
        {
            ctx->err = -1;
            std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. data size is too small, expected: "
                      << sizeof(ParamCount::value_t) <<", got: "
                      << size << std::endl;
            return ctx;
        }

        //deserialzie arguments
        std::istringstream inbuf(std::string((const char*)data, size));

        //args count
        auto paramCount = ValueBase::unpack<ParamCount>(inbuf);
        if (!paramCount or paramCount->getValue() < 1)
        {
            ctx->err = -1;
            std::cerr << NAME_PLUGIN_FUNC() << " invalid parameters. Set '"
                      << ParamCount::class_name() <<"' at least" << std::endl;
            return ctx;
        }
        size_t params = paramCount->getValue();

        //skip such symbols
        auto regex_filter = ValueBase::unpack<RegexFilterValue>(inbuf);
        if (regex_filter and !regex_filter->getValue().empty())
        {
            inner_ctx->property_holder->set<RegexFilterValue>(regex_filter);
        }
        params--;

        //words to skip
        if (params != 0)
        {
            auto filtered_words = ValueBase::unpack<FilterWordsValue>(inbuf);
            if (filtered_words and !filtered_words->getValue().empty())
            {
                inner_ctx->property_holder->set<FilterWordsValue>(filtered_words);
            }
            params--;
        }

        //regex words to skip
        if (params != 0)
        {
            auto filtered_regex_words = ValueBase::unpack<RegexFilterWordsValue>(inbuf);
            if (filtered_regex_words and !filtered_regex_words->getValue().empty())
            {
                inner_ctx->property_holder->set<RegexFilterWordsValue>(filtered_regex_words);
            }
            params--;
        }

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
    catch(const std::exception& ex)
    {
        ctx->err = -1;
        std::cerr << NAME_PLUGIN_FUNC() << " Got exception durign arguments parsing: "
                  << ex.what() << std::endl;
        return ctx;
    }*/
    ctx->data = reinterpret_cast<void*>(inner_ctx);
    return ctx;
}

bool SET_PARAM_PLUGIN_FUNC(plugin_ctx_t* ctx, const char* param_name, ...)
{
    check_ctx(ctx);
    fb2_context_v0 *inner_ctx = reinterpret_cast<fb2_context_v0*>(ctx->data);

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
    fb2_context_v0 *inner_ctx = reinterpret_cast<fb2_context_v0*>(ctx->data);

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

long long DECODE_PLUGIN_FUNC(plugin_ctx_t* ctx, size_t size)
{
    check_ctx(ctx);
    fb2_context_v0 *inner_ctx = reinterpret_cast<fb2_context_v0*>(ctx->data);
    FILE* file = fopen(inner_ctx->filePath.c_str(), "r");
    if(!file)
    {
        ctx->err = errno;
        return errno;
    }

    fseek(file, 0, SEEK_END);
    long cur_offset = ftell(file);
    if(cur_offset == -1)
    {
        ctx->err = errno;
        return errno;
    }

    if(ctx->data_size != static_cast<size_t>(cur_offset))
    {
        //TODO file is changed!
        ctx->data_size = cur_offset;

        if(ctx->data_offset > ctx->data_size)
        {
            abort();
        }
        assert(false && "content changed");
    }


    fseek(file, ctx->data_offset, SEEK_SET);

    auto log = inner_ctx->property_holder->get<LogLevel>();
    if (log)
    {
        log_level = log->getValue();
    }

    std::string regex{};
    auto property_regex = inner_ctx->property_holder->get<RegexFilterValue>();
    if (!property_regex)
    {
        regex = "[\\s\\t\\r\\n,\\.\\:\\!\\?\\;\\'\\&\\%\\#\\$\\@\\~\\(\\)\\{\\}\\[\\]\\-\\+\\=]+";
    }
    else
    {
        regex = property_regex->getValue();
    }

    std::set<std::string> filter_words;
    auto filter_list = inner_ctx->property_holder->get<FilterWordsValue>();
    if (filter_list)
    {
        filter_words = filter_list->getValue();
    }

    std::unique_ptr<std::regex> words_regex_ptr;
    auto regex_filter_list = inner_ctx->property_holder->get<RegexFilterWordsValue>();
    if (regex_filter_list)
    {
        const auto& regex_filter_words = regex_filter_list->getValue();
        std::string reg;
        for(const auto& word : regex_filter_words)
        {
            reg +=  (word + "|");
        }

        if (!reg.empty())
        {
            reg.pop_back();
        }

        words_regex_ptr.reset(new std::regex(reg));
    }


    //////
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
            std::optional<fb2::FictionBook> art;
            if (log_level >= eLogLevel::DEBUG_LEVEL)
            {
                art = txml::XMLCreator::try_create<fb2::FictionBook>(*inner_ctx->xml_reader,
                                                                     std_tracer);
            }
            else
            {
                art = txml::XMLCreator::try_create<fb2::FictionBook>(*inner_ctx->xml_reader,
                                                                     empty_tracer);
            }

            if (!art)
            {
                continue;
            }

            if (!art->has_value<fb2::Body>())
            {
                if (log_level >= eLogLevel::DEBUG_LEVEL)
                {
                    std_tracer << indent(depth) << "skip: body\n";
                }
                else
                {
                    empty_tracer << indent(depth) << "skip: body\n";
                }
                continue;
            }

            const auto &section_container = art->value<fb2::Body>().value();
            for (const auto& section : section_container)
            {
                const auto& paragraphs = section->value();
                for (const auto & paragraph : paragraphs)
                {
                    try
                    {
                        size_t elapsed = 0;
                        auto words = tokenize(paragraph->value(), regex, elapsed);
                        //TODO make counting
                        for(auto &word : words)
                        {
                            //TODO ASCII
                            word.erase(std::remove_if(word.begin(), word.end(), [](char s){ return !::isascii(static_cast<int>(s));}), word.end());
                            //TODO make unordered case-insensitive
                            std::transform(word.begin(), word.end(), word.begin(),
                                            [](unsigned char c){ return std::tolower(c); });

                            if (!std::regex_search(word, *words_regex_ptr) )
                            {

                                if (filter_words.find(word) == filter_words.end())
                                {
                                    inner_ctx->decoded.insert(word);
                                }
                            }
                        }
                    }
                    catch(const std::exception& ex)
                    {
                        //TODO image maybe o other tag
                    }
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 0;
    }
    return 0;
    /*


    size_t elapsed = 0;
    {
        auto words = tokenize(data, regex, elapsed);
        ctx->data_offset -= elapsed;

        //TODO make counting
        for(auto &word : words)
        {
            //TODO ASCII
            std::erase_if(word, [](char s){ return !::isascii(static_cast<int>(s));});
            //TODO make unordered case-insensitive
            std::transform(word.begin(), word.end(), word.begin(),
                   [](unsigned char c){ return std::tolower(c); });

            if (!std::regex_search(word, *words_regex_ptr) )
            {

                if (filter_words.find(word) == filter_words.end())
                {
                    inner_ctx->decoded.insert(word);
                }
            }
        }
    }

//printf("data size: %zu, offset: %zu\n", ctx->data_size, ctx->data_offset);
    ctx->err = 0;
    fclose(file);
    return ctx->data_size - ctx->data_offset;
    */
}

void RELEASE_PLUGIN_FUNC(plugin_ctx_t* ctx)
{
    if (ctx)
    {
        check_ctx(ctx);
        delete reinterpret_cast<fb2_context_v0*>(ctx->data);
    }
    free(ctx);
}

session_t* ALLOCATE_SESSION_FUNC(plugin_ctx_t* ctx, const u_int8_t *data, size_t size)
{

    return nullptr;
}

void RELEASE_SESSION_FUNC(session_t* ctx)
{
    if (ctx)
    {
    }
    free(ctx);
}
const char* NAME_PLUGIN_FUNC()
{
    return FB2_DECODER_PLUGIN_NAME;
}

void* GET_SHARED_PLUGIN_CTX_FUNC(plugin_ctx_t* ctx)
{
    check_ctx(ctx);
    fb2_context_v0 *inner_ctx = reinterpret_cast<fb2_context_v0*>(ctx->data);
    return &inner_ctx->decoded;
}
