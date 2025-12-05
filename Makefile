CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -I.

UTILS = utils/TraceConfig.cpp utils/comparator.cpp
COMMON = HashTableDictionary.cpp $(UTILS)

all: lru_tracegen lru_harness standalone

lru_tracegen: lru_tracegen.cpp $(COMMON)
	$(CXX) $(CXXFLAGS) $^ -o $@

lru_harness: lru_harness.cpp $(COMMON)
	$(CXX) $(CXXFLAGS) $^ -o $@

standalone: main.cpp $(COMMON)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f lru_tracegen lru_harness standalone