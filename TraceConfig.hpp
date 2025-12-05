//
// Created by Ali Kooshesh on 11/1/25.
//

#ifndef HASH_TABLE_TRACECONFIG_HPP
#define HASH_TABLE_TRACECONFIG_HPP

#include<string>
#include<vector>
#include<cassert>
#include<iostream>

struct TraceConfig {

    explicit TraceConfig(const std::string &pName):profileName(pName) {
        // Generates N = 2^10, 2^11, ..., 2^20
        constexpr int start_exp = 10, end_exp = 20;
        for (int exp = start_exp; exp <= end_exp; exp++)
            Ns.push_back(1 << exp);

    }

    std::vector<unsigned> seeds = {23};  // only one seed to get started.
    std::vector<unsigned> Ns;
    std::string traceDirectory = "traceFiles"; // awkward!
    std::string profileName;

    std::string makeTraceFileName(const unsigned int seed, const unsigned n) {
        assert(profileName != "");
        return traceDirectory + "/" +
                profileName + "/" +
                profileName +  "_N_" + std::to_string(n) + "_S_" + std::to_string(seed) + ".trace";
    }
};


#endif //HASH_TABLE_TRACECONFIG_HPP
