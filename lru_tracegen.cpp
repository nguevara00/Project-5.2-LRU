#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <cstdlib>

#include "utils/TraceConfig.hpp"

static const std::string WORD_LIST_PATH = "20980712_uniq_words.txt";

void loadFirst4NWords(std::size_t n, std::vector<std::string>& uniqueWords) {
    std::ifstream in(WORD_LIST_PATH);
    if (!in.is_open()) {
        std::cerr << "Failed to open word list file: " << WORD_LIST_PATH << std::endl;
        std::exit(1);
    }

    uniqueWords.clear();
    uniqueWords.reserve(4 * n);

    std::string line;
    while (uniqueWords.size() < 4 * n && std::getline(in, line)) {
        if (!line.empty())
            uniqueWords.push_back(line);
    }

    if (uniqueWords.size() < 4 * n) {
        std::cerr << "Word list does not contain enough lines for N = " << n << std::endl;
        std::exit(1);
    }
}

void buildAccessBag(const std::vector<std::string>& uniqueWords,
    std::size_t n,
    std::vector<std::string>& bag)
{
    bag.clear();
    bag.reserve(12 * n);

    // First N words: appear once
    for (std::size_t i = 0; i < n; ++i)
        bag.push_back(uniqueWords[i]);

    // Second N words: appear five times
    for (std::size_t i = n; i < 2 * n; ++i)
        for (int r = 0; r < 5; ++r)
            bag.push_back(uniqueWords[i]);

    // Third N words: appear three times
    for (std::size_t i = 2 * n; i < 3 * n; ++i)
        for (int r = 0; r < 3; ++r)
            bag.push_back(uniqueWords[i]);

    // Fourth N words: appear three times
    for (std::size_t i = 3 * n; i < 4 * n; ++i)
        for (int r = 0; r < 3; ++r)
            bag.push_back(uniqueWords[i]);

    if (bag.size() != 12 * n) {
        std::cerr << "Internal error: bag size is " << bag.size()
            << " but expected " << (12 * n) << std::endl;
        std::exit(1);
    }
}

void generateTrace(unsigned int seed,
    std::size_t n,
    TraceConfig& config,
    std::mt19937& rng)
{
    std::string outputFileName = config.makeTraceFileName(seed, n);
    std::cout << "Generating LRU trace: " << outputFileName << std::endl;

    std::ofstream out(outputFileName);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << outputFileName << std::endl;
        std::exit(1);
    }

    out << config.profileName << " " << n << " " << seed << "\n";

    std::vector<std::string> uniqueWords;
    loadFirst4NWords(n, uniqueWords);

    std::vector<std::string> bag;
    buildAccessBag(uniqueWords, n, bag);


    std::shuffle(bag.begin(), bag.end(), rng);

    std::list<std::string> lruList;
    std::unordered_map<std::string, std::list<std::string>::iterator> residentMap;

    for (const std::string& w : bag) {

        auto it = residentMap.find(w);

        if (it != residentMap.end()) {
            lruList.splice(lruList.begin(), lruList, it->second);
            out << "I " << w << "\n";
        }
        else {
            if (residentMap.size() < n) {
                lruList.push_front(w);
                residentMap[w] = lruList.begin();
                out << "I " << w << "\n";
            }
            else {
                const std::string& victim = lruList.back();
                out << "E " << victim << "\n";

                residentMap.erase(victim);
                lruList.pop_back();

                lruList.push_front(w);
                residentMap[w] = lruList.begin();

                out << "I " << w << "\n";
            }
        }
    }
}

int main() {
    TraceConfig config("lru_profile");

    for (unsigned seed : config.seeds) {
        std::mt19937 rng(seed);

        for (std::size_t n : config.Ns) {
            generateTrace(seed, n, config, rng);
        }
    }

    return 0;
}
