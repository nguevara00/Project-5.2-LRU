//
// Created by Ali A. Kooshesh on 5/9/21.
//

#ifndef HASHTABLESOPENADDRESSING_HASHTABLEDICTIONARY_HPP
#define HASHTABLESOPENADDRESSING_HASHTABLEDICTIONARY_HPP

#include<vector>
#include<string>
#include <cstdint>

class HashTableDictionary {

    enum ELEMENT_STATUS {AVAILABLE, DELETED, USED};

public:
    enum PROBE_TYPE {SINGLE, DOUBLE};

    HashTableDictionary( std::size_t tableSize_,
        PROBE_TYPE probeType, bool doCompact=false, double compactionTriggerRate=0.95);



    bool insert( const std::string& v );
    bool member( const std::string& v );
    bool remove( const std::string& v);
    [[nodiscard]] bool empty() const;
    [[nodiscard]] std::size_t size() const;
    void printStats() const;
    void printCounts();

    void printMask();
    void printMask(ELEMENT_STATUS status);
    void printBeforeAndAfterCompactionMaps();
    void printActiveDeleteMap();

    void clear();
    std::string csvStats();
    static std::string csvStatsHeader();


private:
    std::size_t  TABLE_SIZE;
    PROBE_TYPE probeType;

    std::vector<std::string> hashTable;
    std::vector<ELEMENT_STATUS> hashTableMask;

    std::vector<char> beforeCompaction, afterCompaction;

    std::size_t primaryHashFunction( const std::string&  v );
    std::size_t secondaryHashFunction( const std::string&  v );
    std::size_t memberHelper( const std::string& v );
    [[nodiscard]] double effectiveLoadFactor() const;

    void compactTable();

    double compactionTriggerEffectiveRate = 0.95;

    bool shouldCompact = false;

    std::int64_t numLookups = 0;
    std::int64_t numDeletes = 0;
    std::int64_t numInserts = 0;

    int numCompactions = 0;

    std::int64_t numHits = 0;
    std::int64_t numMisses = 0;
    std::int64_t numFullScans = 0;

    std::int64_t totalProbes = 0;

    std::int64_t numberOfActive = 0;
    std::int64_t numberOfTombstones = 0;
    std::int64_t maxTombstones = 0;

    std::int64_t maxValuesInTable = 0;
};


#endif //HASHTABLESOPENADDRESSING_HASHTABLEDICTIONARY_HPP
