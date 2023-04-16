#ifndef DOCUMENTATION_H
#define DOCUMENTATION_H
#include <string>
#include <Framework/Utils/StringUtils.h>

struct Documentation
{
    static const Documentation &instance();
    std::string help(int argc, const char *argv[]) const;
    std::string help_full() const;
private:
    Documentation() = default;
};
#endif //DOCUMENTATION_H
