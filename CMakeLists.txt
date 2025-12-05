cmake_minimum_required(VERSION 3.17)
project(HashTablesOpenAddressing)

set(CMAKE_CXX_STANDARD 17)

# ------------------------------
# Common source files
# ------------------------------
set(HASHTABLE_SRCS
    HashTableDictionary.cpp
    InvertedListDictionary.cpp
    SmallIntMixedOperations.cpp
)

set(HASHTABLE_HDRS
    HashTableDictionary.hpp
    InvertedListDictionary.hpp
    SmallIntMixedOperations.hpp
    Operations.hpp
    RunResults.hpp
    RunMetaData.hpp
    utils/TraceConfig.hpp
)


add_executable(HashTablesOpenAddressing
    main.cpp
    ${HASHTABLE_SRCS}
    ${HASHTABLE_HDRS}
)


add_executable(lru_harness
    lru_harness.cpp
    ${HASHTABLE_SRCS}
    ${HASHTABLE_HDRS}
)


add_executable(lru_tracegen
    lru_tracegen.cpp
    ${HASHTABLE_SRCS}
    ${HASHTABLE_HDRS}
)


