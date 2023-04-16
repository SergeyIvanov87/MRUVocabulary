#ifndef PLUGIN_DECODED_DATA_H
#define PLUGIN_DECODED_DATA_H
#include <map>
#include <unordered_map>
#include <string>
#include <ostream>

struct SharedDecodedData
{
    using MRU = std::multimap<size_t, std::string>;
    using counts_it = typename MRU::iterator;
    std::unordered_map<std::string, counts_it> words;
    MRU counts;

    void insert(const std::string &word);
    void dump(std::ostream &out) const
    {
        for(auto count_pair = counts.rbegin(); count_pair != counts.rend(); ++count_pair)
        {
            out << count_pair->second << "\tcount: "  << count_pair->first << std::endl;
        }
    }
};

#endif
