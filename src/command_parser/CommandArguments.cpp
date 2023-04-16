#if 0
#include <iostream>
#include <stdexcept>
#include "command_parser/SourceValue.h"
#include "command_parser/DestinationValue.h"
#include "command_parser/DestinationFormat.h"

#include "doc/Documentation.h"
#include "command_parser/CommandArgumentsBase.h"

std::unique_ptr<CommandArgumentsBase> CommandArgumentsBase::create(int argc, char *argv[])
{
    std::unique_ptr<CommandArgumentsBase> args(new CommandArgumentsBase);
    int argc_tmp = argc;
    int arg_offset = 1;
    argc_tmp -= 1;
    Filters f = CommandArgumentsBase::prepareFilters<SourceValue, DestinationValue, DestinationFormat>();

    while(argc_tmp and !f.empty())
    {
        if(auto it = f.find(argv[arg_offset]); it != f.end())
        {
            args->storage.emplace(it->first, it->second(argc_tmp, arg_offset, argv));
            f.erase(it);
        }
        else
        {
            throw std::runtime_error(makeString("Invalid argument: ",
                                                Documentation::instance().help(argc_tmp, argv + arg_offset)));
        }
    }

    return args;
}
#endif
