#ifndef PLUGIN_TRANSLATED_DATA_H
#define PLUGIN_TRANSLATED_DATA_H
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <ostream>

struct ISharedTranslatedData;
struct SharedTranslatedData
{
    SharedTranslatedData(int version);
    SharedTranslatedData(const SharedTranslatedData &src);
    ~SharedTranslatedData();

    int getVersion() const;
    ISharedTranslatedData &getImpl();
    const ISharedTranslatedData &getImpl() const;

private:
    int version;
    std::unique_ptr<ISharedTranslatedData> impl;
};

#endif
