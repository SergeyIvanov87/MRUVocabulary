#include <iostream>
#include <fstream>
#include <Framework/ResourcesFramework/ResourcesFramework.h>
#include <Framework/SerializeFramework/Interfaces/ISerializable.h>

#include "resources/Settings.h"

#include "common/base_command_parser/RegexFilterParam.h"
#include "common/base_command_parser/FilterWords.hpp"
#include "common/base_command_parser/RegexFilterWords.hpp"
#include "common/base_command_parser/DBServiceParam.h"
#include "common/base_command_parser/DBAuthParam.h"
#include "common/base_command_parser/LogLevel.h"
#include "common/base_command_parser/TranslationOrder.h"
#include "common/base_command_parser/XDXFValue.h"
#include "common/base_command_parser/CommandArguments.hpp"

Settings::~Settings()
{

}

bool Settings::serializeImpl(std::ostream &out)
{
    //TODO
    serializeParams(out, true);
    return true;
}

bool Settings::deserializeImpl(std::istream &in)
{
    //TODO
    bool ret = false;
    deserializeParams(in, ret);
    if(!ret)
    {
        std::cerr << "Error in: " << __PRETTY_FUNCTION__ << std::endl;
        abort();
    }
    return true;
}

template<class Tracer>
Settings::ResourcesMap Settings::loadResourcesImpl(const std::string &resourcePath, Tracer tracer)
{
    ResourcesMap instances;
    std::ifstream file;
    file.open(resourcePath, std::ifstream::in);
    if(!file.good())
    {
        return instances;
    }

    char line[1024];
    std::vector<std::string> params;

    while(file.getline(line, 1024))
    {
        //name
        const char *tmp = Utils::get_next_char_if(line, ::isspace);
        if(!tmp)
        {
            throw std::runtime_error(makeString("Invalid content in settings file: ", resourcePath,
                                                ". Bad setting line: ", line, ". Separator is missed"));
        }
        std::string name (line, tmp - line);

        if (name.empty())
        {
            continue;
        }

        //value
        tmp = Utils::get_next_char_not_if(tmp, ::isspace);
        std::string value(tmp);

        //remember to create plain array
        params.push_back(std::move(name));
        params.push_back(std::move(value));
    }

    //Global
    std::shared_ptr<Settings> global =
                    CommandArgumentsBase::create_with_filters<Settings,
                                                              RegexFilterValue,
                                                              FilterWordsValue,
                                                              RegexFilterWordsValue,
                                                              DBHostParamValue,
                                                              DBPortParamValue,
                                                              DBUserParamValue,
                                                              DBPwdParamValue,
                                                              LogLevel,
                                                              XDXFTranslatorPath,
                                                              TranslationOrder>();

    int argc = params.size();
    //const char *argv[argc]{};
    std::vector<const char*> argv(argc, nullptr);
    std::transform(params.begin(), params.end(), argv.begin(), [] (const std::string& str) -> const char*
    {
        return str.data();
    });


    global->makeParamsExtract(argc, argv.data());
    instances["GlobalSettings"] = global;

    return instances;
}

template
Settings::ResourcesMap Settings::loadResourcesImpl<Tracer<EmptyTracerImpl>>(const std::string &resourcePath, Tracer<EmptyTracerImpl> tracer);
template
Settings::ResourcesMap Settings::loadResourcesImpl<Tracer<SSTracerImpl>>(const std::string &resourcePath, Tracer<SSTracerImpl> tracer);
