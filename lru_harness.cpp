#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <utility>


#include "Operations.hpp"
#include "RunResults.hpp"
#include "RunMetaData.hpp"
#include "HashTableDictionary.hpp"
#include "utils/TraceConfig.hpp"


// Adapted run_trace_ops for HashTableDictionary

template<class Impl>
RunResult run_trace_ops(Impl& ht,
    RunResult& runResult,
    const std::vector<Operation>& ops) {
    // Count ops only Insert/Erase
    for (const auto& op : ops) {
        switch (op.tag) {
        case OpCode::Insert: ++runResult.inserts;
            break;
        case OpCode::Erase: ++runResult.erases;
            break;
        }
    }
    // One untimed run

    ht.clear();
    std::cout << "Starting the throw-away run for N = " << runResult.run_meta_data.N << std::endl;
    for (const auto& op : ops) {
        if (op.tag == OpCode::Insert) {
            ht.insert(op.key);
        }
        if (op.tag == OpCode::Erase) {
            ht.remove(op.key);
        }
    }

    using clock = std::chrono::steady_clock;
    const int numTrials = 7;

    std::vector<std::int64_t> trials_ns;
    trials_ns.reserve(numTrials);

    for (int i = 0; i < numTrials; ++i) {
        ht.clear();
        std::cout << "Run " << i << " for N = " << runResult.run_meta_data.N << std::endl;
        auto t0 = clock::now();
        for (const auto& op : ops) {
            if (op.tag == OpCode::Insert) {
                ht.insert(op.key);
            }
            if (op.tag == OpCode::Erase) {
                ht.remove(op.key);
            }
        }
        auto t1 = clock::now();
        trials_ns.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
    }

    const size_t mid = trials_ns.size() / 2;     // the median of 0..numTrials
    std::nth_element(trials_ns.begin(), trials_ns.begin() + mid, trials_ns.end());
    runResult.elapsed_ns = trials_ns[mid];

    return runResult;
}

// Assumes OpCode/Op (with the two arguments) are defined

// The first line of the header must contain:  <profile> <N> <seed>
// After the header: blank lines and lines starting with '#' are okay
// and will be ignored.
// Adapted for LRU traces (I/E)

bool load_trace_strict_header(const std::string& path, RunMetaData& runMeta, std::vector<Operation>& out_operations) {
    out_operations.clear();

    std::ifstream in(path);
    if (!in.is_open())
        return false;

    // --- read FIRST line as header
    std::string header;
    if (!std::getline(in, header))
        return false;

    // Look for a non-while-space character
    const auto first = header.find_first_not_of(" \t\r\n");
    // Since this is the first line, we don't expect it to be blank
    // or start with a comment.
    if (first == std::string::npos || header[first] == '#')
        return false;

    // Create a string stream so that we can read the profile name,
    // N, and the seed more easily.
    std::istringstream hdr(header);
    if (!(hdr >> runMeta.profile >> runMeta.N >> runMeta.seed))
        return false;

    // --- read ops, allowing comments/blank lines AFTER the header ---
    std::string line;
    while (std::getline(in, line)) {
        const auto opCodeIdx = line.find_first_not_of(" \t\r\n");
        if (opCodeIdx == std::string::npos || line[opCodeIdx] == '#')
            continue; // skip blank and comment lines.

        std::istringstream iss(line.substr(opCodeIdx));
        std::string tok, key;
        if (!(iss >> tok))
            continue;

        if (!(iss >> key))
            return false;

        if (tok == "I") {
            out_operations.emplace_back(OpCode::Insert, key);
        }
        else if (tok == "E") {
            out_operations.emplace_back(OpCode::Erase, key);
        }
        else {
            return false; // unknown token
        }
    }

    return true;
}


void find_trace_files_or_die(const std::string& dir,
    const std::string& profile_prefix,
    std::vector<std::string>& out_files) {
    namespace fs = std::filesystem;
    out_files.clear();

    std::error_code ec;
    fs::path p(dir);

    if (!fs::exists(p, ec)) {
        std::cerr << "Error: directory '" << dir << "' does not exist";
        if (ec) std::cerr << " (" << ec.message() << ")";
        std::cerr << "\n";
        std::exit(1);
    }
    if (!fs::is_directory(p, ec)) {
        std::cerr << "Error: path '" << dir << "' is not a directory";
        if (ec) std::cerr << " (" << ec.message() << ")";
        std::cerr << "\n";
        std::exit(1);
    }

    fs::directory_iterator it(p, ec);
    if (ec) {
        std::cerr << "Error: cannot iterate directory '" << dir << "': "
            << ec.message() << "\n";
        std::exit(1);
    }

    const std::string suffix = ".trace";
    for (const auto& entry : it) {
        if (!entry.is_regular_file(ec)) {
            if (ec) {
                std::cerr << "Error: is_regular_file failed for '"
                    << entry.path().string() << "': " << ec.message() << "\n";
                std::exit(1);
            }
            continue;
        }

        const std::string name = entry.path().filename().string();
        const bool has_prefix = (name.rfind(profile_prefix, 0) == 0);
        const bool has_suffix = name.size() >= suffix.size() &&
            name.compare(name.size() - suffix.size(),
                suffix.size(), suffix) == 0;

        if (has_prefix && has_suffix) {
            out_files.push_back(entry.path().string());
        }
    }

    std::sort(out_files.begin(), out_files.end()); // stable order for reproducibility
}

std::size_t tableSizeForN(std::size_t N) {
    static const std::vector<std::pair<std::size_t, std::size_t>> N_and_primes = {
        {1024,     1279},
        {2048,     2551},
        {4096,     5101},
        {8192,    10273},
        {16384,   20479},
        {32768,   40849},
        {65536,   81931},
        {131072,  163861},
        {262144,  327739},
        {524288,  655243},
        {1048576, 1310809}
    };

    for (auto item : N_and_primes) {
        if (item.first == N)
            return item.second;
    }

    std::cerr << "Unable to find table size for N = " << N << std::endl;
    std::exit(1);
}

int main() {
    std::cout << "[LRU_HARNESS] starting\n"; //remove this later
    const auto profileName = std::string("lru_profile");
    const auto traceDir    = std::string("../../../traceFiles/") + profileName;

    std::vector<std::string> traceFiles;
    find_trace_files_or_die(traceDir, profileName, traceFiles);
 
    if (traceFiles.size() == 0) {
        std::cerr << "No trace files found.\n";
        exit(1);
    }

    std::cout << RunResult::csv_header()
        << ","
        << HashTableDictionary::csvStatsHeader()
        << std::endl;

    for (auto traceFile : traceFiles) {
        const auto pos = traceFile.find_last_of("/\\");
        auto traceFileBaseName = (pos == std::string::npos) ? traceFile : traceFile.substr(pos + 1);

        std::vector<Operation> operations;
        RunMetaData run_meta_data;
        load_trace_strict_header(traceFile, run_meta_data, operations);

        {
            RunResult r(run_meta_data);
            HashTableDictionary ht(tableSizeForN(run_meta_data.N), HashTableDictionary::DOUBLE, true);
            r.impl       = "hash_map_double";
            r.trace_path = traceFileBaseName;
            run_trace_ops(ht, r, operations);
            std::cout << r.to_csv_row()
                << ","
                << ht.csvStats()
                << std::endl;
        }

        {
            RunResult r(run_meta_data);
            HashTableDictionary ht(tableSizeForN(run_meta_data.N), HashTableDictionary::SINGLE, true);
            r.impl       = "hash_map_single";
            r.trace_path = traceFileBaseName;
            run_trace_ops(ht, r, operations);
            std::cout << r.to_csv_row()
                << ","
                << ht.csvStats()
                << std::endl;
        }
    }

    return 0;
}
