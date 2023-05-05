#include <stdio.h>
#include <stdarg.h>

#include "TxtDecoder.h"
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
#include "common/base_command_parser/CommandArguments.hpp"

#include "vers/PluginDecodedData.h"

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

    if(ctx->version != TXT_DECODER_CURRENT_VERSION)
    {
        std::string ret("CTX version is mismatched: ");
        ret += std::to_string(ctx->version);
        ret += std::string(". Expected: ") + std::to_string(TXT_DECODER_CURRENT_VERSION);
        perror(ret.c_str());
        abort();
    }
}

static void check_session_ctx(session_t* sctx)
{
    if(!sctx or !sctx->data)
    {
        std::string ret("session is empty in plugin: ");
        ret += NAME_PLUGIN_FUNC();
        perror(ret.c_str());
        abort();
    }

    if (!sctx->id)
    {
        std::string ret("session id is unknown, plugin: ");
        ret += NAME_PLUGIN_FUNC();
        perror(ret.c_str());
        abort();
    }

    //TODO version check
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
    ctx->version = TXT_DECODER_CURRENT_VERSION;
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

    txt_context_v0 *inner_ctx = new txt_context_v0;
    inner_ctx->filePath = (const char*)data;
    inner_ctx->property_holder = CommandArgumentsBase::create<CommandArgumentsBase,
                                                              RegexFilterValue,
                                                              FilterWordsValue,
                                                              RegexFilterWordsValue>();
    ctx->data = reinterpret_cast<void*>(inner_ctx);
    return ctx;
}

bool SET_PARAM_PLUGIN_FUNC(plugin_ctx_t* ctx, const char* param_name, ...)
{
    check_ctx(ctx);
    txt_context_v0 *inner_ctx = reinterpret_cast<txt_context_v0*>(ctx->data);

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
    txt_context_v0 *inner_ctx = reinterpret_cast<txt_context_v0*>(ctx->data);

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

long long DECODE_PLUGIN_FUNC(plugin_ctx_t* ctx, size_t size, session_t *session_ctx)
{
    check_ctx(ctx);
    check_session_ctx(session_ctx);

    txt_context_v0 *inner_ctx = reinterpret_cast<txt_context_v0*>(ctx->data);

    SharedDecodedData *decoded_data_ptr = nullptr;
    if (session_ctx->version == 0)
    {
        decoded_data_ptr = reinterpret_cast<SharedDecodedData*>(session_ctx->data);
    }

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

    size_t bytes_to_read = std::min(size, ctx->data_size - ctx->data_offset);
    size_t chunk_size = std::min<size_t>(bytes_to_read, 4096);
    std::string data(bytes_to_read, '\0');
    size_t tmp = 0;

//printf("btr: %zu, chunk_size: %zu\n", bytes_to_read, chunk_size);
//fflush(stdout);

    while(tmp < bytes_to_read)
    {
        int ret = fread(&data.at(tmp), std::min(chunk_size, bytes_to_read - tmp), 1, file);
        if(ret < 1)
        {
            printf("feof\n");
            if(!feof(file))
            {
                abort();
            }
            break;
        }
        tmp += chunk_size;
  //      printf("loop btr: %zu, tmp size: %zu\n", bytes_to_read, tmp);
        fflush(stdout);
    }
    ctx->data_offset = ftell(file);

//printf("string data size: %zu\n", data.size());

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

    size_t elapsed = 0;
    {
        auto words = tokenize(data, regex, elapsed);
        ctx->data_offset -= elapsed;

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
                    decoded_data_ptr->insert(word);
                }
            }
        }
    }

//printf("data size: %zu, offset: %zu\n", ctx->data_size, ctx->data_offset);
    ctx->err = 0;
    fclose(file);
    return ctx->data_size - ctx->data_offset;
}

void RELEASE_PLUGIN_FUNC(plugin_ctx_t* ctx)
{
    if (ctx)
    {
        check_ctx(ctx);
        delete reinterpret_cast<txt_context_v0*>(ctx->data);
    }
    free(ctx);
}

session_t* ALLOCATE_SESSION_FUNC(plugin_ctx_t* global_ctx, const u_int8_t *data, size_t size)
{
    session_t *ctx = (session_t*)calloc(1, sizeof(session_t));
    ctx->version = TXT_DECODER_CURRENT_VERSION;
    ctx->id = NAME_PLUGIN_FUNC();
    // TODO
    if (ctx->version == 0)
    {
        ctx->data = new SharedDecodedData;
    }
    else
    {
        throw std::runtime_error("Cannot create SharedDecodedData, version is unsupported: " + std::to_string(ctx->version));
    }
    return ctx;
}

void RELEASE_SESSION_FUNC(plugin_ctx_t*, session_t* ctx)
{
    if (ctx)
    {
        check_session_ctx(ctx);
        delete reinterpret_cast<SharedDecodedData*>(ctx->data);;
        ctx->data = nullptr;
    }
    ctx->version = -1;
    ctx->id = nullptr;
    free(ctx);
}

const char* NAME_PLUGIN_FUNC()
{
    return TXT_DECODER_PLUGIN_NAME;
}
