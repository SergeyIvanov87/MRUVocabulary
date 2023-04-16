#ifndef PLUGIN_TRANSLATED_DATA_H
#define PLUGIN_TRANSLATED_DATA_H
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <ostream>

struct SharedTranslatedDataImpl;
struct SharedTranslatedData
{
    SharedTranslatedData();
    SharedTranslatedData(const SharedTranslatedData &src);
    ~SharedTranslatedData();

    std::unique_ptr<SharedTranslatedDataImpl> impl;

    void dump(std::ostream &out) const;
};

#endif
