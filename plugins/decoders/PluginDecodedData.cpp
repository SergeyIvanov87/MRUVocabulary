#include "decoders/PluginDecodedData.h"

void SharedDecodedData::insert(const std::string& word)
{
    if (auto it = words.find(word); it != words.end())
    {
        const auto &[count, dict_word] = *it->second;
        counts.erase(it->second);
        it->second = counts.emplace(count + 1, dict_word);
    }
    else
    {
        auto count_it = counts.emplace(1, word);
        words.emplace(word, count_it);
    }
}

/*
void SharedDecodedData::dump(std::ostream &out) const
{
    for(auto count_pair = counts.rbegin(); count_pair != counts.rend(); ++count_pair)
    {
        out << count_pair->second << "\tcount: "  << count_pair->first << std::endl;
    }
}
*/
