#include <iostream>
#include <iterator>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <Framework/ResourcesFramework/ResourcesFramework.hpp>
#include <Framework/Utils/LogTracer.h>
#include "doc/Documentation.h"
#include "command_parser/MainCommandArguments.h"
#include "common/base_command_parser/RegexFilterParam.h"
#include "common/base_command_parser/FilterWords.hpp"
#include "common/base_command_parser/RegexFilterWords.hpp"
#include "common/base_command_parser/DBServiceParam.h"
#include "common/base_command_parser/DBAuthParam.h"
#include "common/base_command_parser/XDXFValue.h"
#include "common/base_command_parser/LogLevel.h"
#include "common/base_command_parser/TranslationOrder.h"
#include "resources/ResourceFramework.h"

#include "output/FormatMapper.h"
int main(int argc, const char *argv[])
{
    //TODO
/*
    using in_t = FormatList<int, std::string, std::vector<int>, double>;
    std::cout << "int: " << in_t:: template index<int>();
    std::cout << "double: " << in_t:: template index<double>();
    return 1;
*/
    if(argc == 1)
    {
        std::cerr << Documentation::instance().help(argc, argv) << std::endl;
        return -1;
    }

    SSTracerImpl ss('\t');
    Tracer<SSTracerImpl> tracer(ss);
    try
    {
        const char *home_path = nullptr;
        if ((home_path = getenv("HOME")) == nullptr)
        {
            home_path = getpwuid(getuid())->pw_dir;
        }
        std::string settings_path(home_path);
        settings_path += "/.vocabulary";
        ResourcesFramework::initializeResourceHolder(settings_path, "dumps", tracer);
        std::cout << "Tracer: \n" << tracer.getManagedTracer().str() << std::endl;
    }
    catch(const std::exception &ex)
    {
        std::cerr << "Cannot initialize ResourcesFramework, error:\n" << ex.what()
                  << "\nTrace:\n" << tracer.getManagedTracer().str() << std::endl;
        return -1;
    }

    // create command parser and extract command line arguments
    std::unique_ptr<MainCommandArguments> args;
    try
    {
        args = CommandArgumentsBase::create<MainCommandArguments,
                                            SourceValue,
                                            DestinationValue,
                                            DestinationFormat>();
        args->makeParamsExtract(argc - 1, const_cast<const char**>(argv + 1));
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return -1;
    }

    /* Extract COMMAND arguments */

    // SRC
    auto src_val = args->get<SourceValue>();
    if (!src_val)
    {
        std::cerr << SourceValue::class_name() << " is empty!" << std::endl;
        return -1;
    }

    // DST
    auto dst_val = args->get<DestinationValue>();
    if (!dst_val)
    {
        std::cerr << DestinationValue::class_name() << " is empty!" << std::endl;
        return -1;
    }

    // DST Format
    auto dst_format_val = args->get<DestinationFormat>();
    if (!dst_format_val)
    {
        dst_format_val.reset( new DestinationFormat("fb2"));
        std::cout << DestinationValue::class_name() << " is empty. Use \":" <<  dst_format_val->getValue() << "\"" << std::endl;
        return -1;
    }


    /* Load application settings */

    std::cout << "Load " << Settings::getResourceTypeDescription() << " \"GlobalSettings\"" << std::endl;
    auto settings = ResourcesFramework::Instance()->getResourcePtr<Settings>("GlobalSettings");

    // LOG
    auto logLevel = settings->get<LogLevel>();
    if (logLevel)
    {
        std::cout << LogLevel::class_name() << ": " << logLevel->getValue() << std::endl;
    }

    // Translation printfout order
    auto order = settings->get<TranslationOrder>();
    if (!order)
    {
        const char *argv_[] = {TranslationOrder::class_name(), "direct"};
        int argc_ = 2;
        int arg_offset = 0;
        order = TranslationOrder::create(argc_, arg_offset, argv_);

        std::cout << TranslationOrder::class_name() << " is empty. set default: " << order->getValue() << std::endl;
    }
    else
    {
        std::cout << TranslationOrder::class_name() << ": " << order->getValue() << std::endl;
    }

    // FilteredOut words REGEX
    auto regex = settings->get<RegexFilterValue>();
    if (!regex)
    {
        std::cerr << RegexFilterValue::class_name() << " is empty!" << std::endl;
    }
    else
    {
        std::cout << RegexFilterValue::class_name() << ": " << regex->getValue() << std::endl;
    }

    // Filter REGEX
    auto filter_words = settings->get<FilterWordsValue>();
    if (!filter_words)
    {
        std::cout << FilterWordsValue::class_name() << " is empty!" << std::endl;
    }
    else
    {
        std::cout << "Filter list: " << std::endl;
        for (const auto& str : filter_words->getValue())
        {
            std::cout << str << std::endl;
        }
    }

    auto regex_filter_words = settings->get<RegexFilterWordsValue>();
    if (!regex_filter_words)
    {
        std::cerr << RegexFilterWordsValue::class_name() << " is empty!" << std::endl;
    }
    else
    {
        std::cout << "Regex Filter list: " << std::endl;
        for (const auto& str : regex_filter_words->getValue())
        {
            std::cout << str << std::endl;
        }
    }

    // DB
    auto dbhost = settings->get<DBHostParamValue>();
    if (!dbhost)
    {
        std::cerr << DBHostParamValue::class_name() << " is empty!" << std::endl;
        return -1;
    }
    std::cout << DBHostParamValue::class_name() << ": " << dbhost->getValue() << std::endl;

    auto dbport = settings->get<DBPortParamValue>();
    if (!dbport)
    {
        std::cerr << DBPortParamValue::class_name() << " is empty!" << std::endl;
        return -1;
    }
    std::cout << DBPortParamValue::class_name() << ": " << dbport->getValue() << std::endl;

    auto dbuser = settings->get<DBUserParamValue>();
    if (!dbuser)
    {
        std::cerr << DBUserParamValue::class_name() << " is empty!" << std::endl;
        return -1;
    }
    std::cout << DBUserParamValue::class_name() << ": " << dbuser->getValue() << std::endl;

    auto dbpwd = settings->get<DBPwdParamValue>();
    if (!dbpwd)
    {
        std::cerr << DBPwdParamValue::class_name() << " is empty!" << std::endl;
        return -1;
    }
    std::cout << DBPwdParamValue::class_name() << ": " << dbpwd->getValue() << std::endl;


    auto xdxf_path = settings->get<XDXFTranslatorPath>();
    if (!xdxf_path)
    {
        std::cerr << XDXFTranslatorPath::class_name() << " is empty!" << std::endl;
        return -1;
    }
    std::cout << XDXFTranslatorPath::class_name() << ": ";
    std::copy(xdxf_path->getValue().begin(), xdxf_path->getValue().end(), std::ostream_iterator<std::string>(std::cout, ", "));
    std::cout << std::endl;

    /* PLUGINS */

    // load plugin TXT
    auto *txtPlugin = ResourcesFramework::Instance()->getResourceInstancePtr<DecoderResource>("txt");
    if (!txtPlugin)
    {
        std::cerr << "Cannot load " << DecoderResource::getResourceTypeDescription() << " \"txt\"" << std::endl;
        return -1;
    }

    // Init decoders plugins
    const std::string& srcPath = src_val->getValue();
    PluginHolder::PluginCtxPtr txt_ctx = txtPlugin->initPluginCtx(reinterpret_cast<const u_int8_t*>(srcPath.c_str()),
                                                              srcPath.size());
    if (regex)
    {
        txtPlugin->setTypedParam(txt_ctx, *regex);
    }

    if (filter_words)
    {
        txtPlugin->setTypedParam(txt_ctx, *filter_words);
    }

    if (regex_filter_words)
    {
        txtPlugin->setTypedParam(txt_ctx, *regex_filter_words);
    }


    std::stringstream ss_packer;
    std::string packer_string;

    // load plugin DB
    auto *dbPlugin = ResourcesFramework::Instance()->getResourceInstancePtr<OutputResource>("DB");
    if (dbPlugin)
    {
        pack_values(ss_packer, dbhost, dbport, dbuser, dbpwd, logLevel);

        packer_string = ss_packer.str();
        PluginHolder::PluginCtxPtr dbctx =
                dbPlugin->initPluginCtx(reinterpret_cast<const u_int8_t*>(packer_string.c_str()),
                                                                      packer_string.size());

        (void)dbctx;
    }

    // load plugin XDXF output
    auto *outputPlugin = ResourcesFramework::Instance()->getResourceInstancePtr<OutputResource>("xdxf_output");
    if (!outputPlugin)
    {
        std::cerr << "Cannot load " << OutputResource::getResourceTypeDescription() << " \"xdxf_output\"" << std::endl;
        return -1;
    }

    const std::string& dstPath = dst_val ? dst_val->getValue() : "output.xdxf";

    std::stringstream().swap(ss_packer);
    pack_values(ss_packer, dst_val, dst_format_val, logLevel);
    packer_string = ss_packer.str();

    //Init output plugin
    PluginHolder::PluginCtxPtr output_ctx = outputPlugin->initPluginCtx(reinterpret_cast<const u_int8_t*>(packer_string.c_str()),
                                                              packer_string.size());




    // load plugin TRANSLATOR
    TranslatorResource *translatorPlugin = nullptr;
    if (xdxf_path->getValue().size() > 1)
    {
        translatorPlugin = ResourcesFramework::Instance()->getResourceInstancePtr<TranslatorResource>("multi_xdxf");
    }
    else
    {
        translatorPlugin = ResourcesFramework::Instance()->getResourceInstancePtr<TranslatorResource>("xdxf");
    }
    std::stringstream().swap(ss_packer);
    pack_values(ss_packer, xdxf_path, order, logLevel);
    packer_string = ss_packer.str();

    //Init translator plugin
    PluginHolder::PluginCtxPtr xdxfctx =
            translatorPlugin->initPluginCtx(reinterpret_cast<const u_int8_t*>(packer_string.c_str()),
                                                                              packer_string.size());

    /* RUN */

    size_t source_size = txt_ctx->data_size;
    long long elapsedBytes = 0;
    do
    {
        elapsedBytes = txtPlugin->decodeData(txt_ctx, source_size / 3);
        std::cout << "elapsedBytes: " << elapsedBytes << std::endl;

        //shared_ctx->dump(std::cout);
    } while(elapsedBytes);

    SharedDecodedData* decoded_session = txtPlugin->getSharedCtx(txt_ctx);
    if(!decoded_session)
    {
        std::cerr << "No shared txt_ctx" << std::endl;
        return -1;
    }

    PluginHolder::SharedCtxPtr translator_session = translatorPlugin->allocateSession(xdxfctx, nullptr, 0);
    if (!translator_session)
    {
        std::cerr << "Cannot create translator session. Exit" << std::endl;
        return -1;
    }
    size_t translated_num = translatorPlugin->translate(xdxfctx, *decoded_session, translator_session);
    std::cout << "translated count: " << translated_num << std::endl;
    //auto str_ptr = translatorPlugin->sharedCtx2CStr(xdxfctx, translator_session);
    //std::cout << str_ptr.get() << "\nDump finished\n" << std::endl;
    outputPlugin->writeTranslatedData(output_ctx, translator_session);
}

/*
(postponed) 1) hide plugin_ctx into PluginWrapper members
initPluginCtx might be reinmplemented in a way that it will produce a new class PluginWrapperInstance, which carry on this plugin_ctx as its data inside
(postponed) 2) If we took this consideration - that it doesnt need to pass  plugin_ctx as member functions argument in PluginWrapperInstance
3) But it|s  exactly shared_ctx_t what is necessary to pass in these methods
4) In this interpretation shared_ctx_t must be transformed into session_t. So we separate plugi initialization step and plugin excuting step
5) So shared_ctx free should be renamred to session_free and a new plugin fucntios as session_alloc should be added
6) plugin_ctx must not bring any pointer on shared_ctx anymore!
(postponed) 7) remove IPlugin
*/
