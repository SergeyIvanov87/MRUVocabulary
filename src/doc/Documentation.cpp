#include "doc/manual.h"
#include "Documentation.h"

const Documentation &Documentation::instance()
{
    static const Documentation instance;
    return instance;
}

std::string Documentation::help(int argc, const char *argv[]) const
{
    std::string ret;
    if(argc <= 1 or !argv)
    {
        return help_full();
    }

    ret = "TODO: check args";
    return ret;
}

std::string Documentation::help_full() const
{
    return std::string(description) + manual;
}
