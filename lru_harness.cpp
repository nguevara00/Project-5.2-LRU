#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <chrono>

#include "Operations.hpp"
#include "RunResults.hpp"
#include "RunMetaData.hpp"
#include "HashTableDictionary.hpp"
#include "utils/TraceConfig.hpp"

// ================================================================
// run_trace_ops: warm-up + 7 timed runs, returns median elapsed_ns
// ================================================================
template<class Impl>
RunResult run_trace_ops(Impl& ht,
    RunResult& runResult,
    const std::vector<Operation>& ops)
{
    using clock = std::chrono::steady_clock;

    // Warm-up (untimed)
    ht.clear();
    for (const auto& op : ops) {
        if (op.tag == OpCode::Insert) {
            ht.insert(op.key);
        }
        else if (op.tag == OpCode::Erase) {
            ht.remove(op.key);
        }
    }

    // Timed trials
    const int numTrials = 7;
    std::vector<std::int64_t> trials_ns;
    trials_ns.reserve(numTrials);

    for (int trial = 0; trial < numTrials; ++trial) {

        ht.clear();

        auto t0 = clock::now();
        for (const auto& op : ops) {
            if (op.tag == OpCode::Insert) {
                ht.insert(op.key);
            }
            else if (op.tag == OpCode::Erase) {
                ht.remove(op.key);
            }
        }
        auto t1 = clock::now();

        trials_ns.push_back(
            std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()
        );
    }

    // Median
    const size_t mid = trials_ns.size() / 2;
    std::nth_element(trials_ns.begin(),
        trials_ns.begin() + mid,
        trials_ns.end());
    runResult.elapsed_ns = trials_ns[mid];

    return runResult;
}

// ================================================================
// Parse trace: header "<profile> <N> <seed>"
// Then lines: I key   or   E key
// ================================================================
bool load_trace_strict_header(const std::string& path,
    RunMetaData& runMeta,
    std::vector<Operation>& out_operations)
{
    out_operations.clear();

    std::ifstream in(path);
    if (!in.is_open())
        return false;

    // Header
    std::string header;
    if (!std::getline(in, header))
        return false;

    std::istringstream hdr(header);
    if (!(hdr >> runMeta.profile >> runMeta.N >> runMeta.seed))
        return false;

    // Opcodes
    std::string line;
    while (std::getline(in, line)) {

        const auto first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos || line[first] == '#')
            continue;

        std::istringstream iss(line.substr(first));
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
            return false;
        }
    }

    return true;
}

// ================================================================
// Find trace files
// ================================================================
void find_trace_files_or_die(const std::string& dir,
    const std::string& profile_prefix,
    std::vector<std::string>& out_files)
{
    namespace fs = std::filesystem;

    out_files.clear();

    fs::path p(dir);
    std::error_code ec;

    if (!fs::exists(p, ec) || !fs::is_directory(p, ec)) {
        std::cerr << "Error: trace directory '" << dir << "' not found.\n";
        std::exit(1);
    }

    const std::string suffix = ".trace";

    for (const auto& entry : fs::directory_iterator(p)) {
        if (!entry.is_regular_file(ec))
            continue;

        const std::string name = entry.path().filename().string();

        bool has_prefix = (name.rfind(profile_prefix, 0) == 0);
        bool has_suffix = name.size() >= suffix.size() &&
            name.compare(name.size() - suffix.size(),
                suffix.size(), suffix) == 0;

        if (has_prefix && has_suffix) {
            out_files.push_back(entry.path().string());
        }
    }

    std::sort(out_files.begin(), out_files.end());
}

// ================================================================
// N -> M table size mapping
// ================================================================
std::size_t tableSizeForN(std::size_t N)
{
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

    std::cerr << "Error: invalid N = " << N << "\n";
    std::exit(1);
}

int main()
{
    const std::string profileName = "lru_profile";
    const std::string traceDir = "traceFiles/" + profileName;

    std::vector<std::string> traceFiles;
    find_trace_files_or_die(traceDir, profileName, traceFiles);

    if (traceFiles.empty()) {
        std::cerr << "No trace files found.\n";
        return 1;
    }

    std::cout << RunResult::csv_header()
        << ","
        << HashTableDictionary::csvStatsHeader()
        << std::endl;

    for (const auto& traceFile : traceFiles) {

        const auto pos = traceFile.find_last_of("/\\");
        const std::string base =
            (pos == std::string::npos) ? traceFile : traceFile.substr(pos + 1);

        std::vector<Operation> operations;
        RunMetaData meta;

        if (!load_trace_strict_header(traceFile, meta, operations)) {
            std::cerr << "Error: failed to parse " << traceFile << "\n";
            continue;
        }

        long inserts = 0;
        long erases = 0;
        for (const auto& op : operations) {
            if (op.tag == OpCode::Insert) ++inserts;
            else if (op.tag == OpCode::Erase) ++erases;
        }

        // DOUBLE probing
        {
            RunResult r(meta);
            r.impl = "hash_map_double";
            r.trace_path = base;
            r.inserts = inserts;
            r.erases = erases;

            HashTableDictionary ht(tableSizeForN(meta.N),
                HashTableDictionary::DOUBLE,
                true);

            run_trace_ops(ht, r, operations);

            std::cout << r.to_csv_row()
                << ","
                << ht.csvStats()
                << std::endl;
        }

        // SINGLE probing
        {
            RunResult r(meta);
            r.impl = "hash_map_single";
            r.trace_path = base;
            r.inserts = inserts;
            r.erases = erases;

            HashTableDictionary ht(tableSizeForN(meta.N),
                HashTableDictionary::SINGLE,
                true);

            run_trace_ops(ht, r, operations);

            std::cout << r.to_csv_row()
                << ","
                << ht.csvStats()
                << std::endl;
        }
    }

    return 0;
}
