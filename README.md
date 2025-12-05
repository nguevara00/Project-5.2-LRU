
# Project 5, Part 2: Empirical Analysis of LRU Profile

## Student Information
**Name:** Nicholas Guevara  
**Student ID:** 002-85-1971
**Repository:** https://github.com/nguevara00/Project-5.2-LRU

## Collaboration & Sources
The code was initially provided by Dr. Kooshesh, and I minimally adapted it to support the LRU profile and the hash table implementation.
I consulted with another student Parker Tennier to compare trace output files for deterministic results, since we don't have a testing script for this project. 

## Implementation Details

### Design 
The code base and experimental design was provided by Dr. Kooshesh and are similar to the Huffman analysis. Two main executables are produced, in addition to the provided testing implementation already provided:
	- **Trace Generator** — Constructs deterministic LRU traces for values of N = 2^10^ ... 2^20^.
	- **Harness** — Replays each trace against the provided hash table implementation using Single Probing and Double Probing with compaction on, and outputs the timing information into a CSV file. 

### Data Structures
The experiment evaluates Dr. Kooshesh’s open-addressed hash table with AVAILABLE / ACTIVE / DELETED slot states, tombstones, and compaction. Both single and double probing are supported.

### Algorithms

The LRU profile loads 4*N keys from the provided dataset, 20980712_uniq_words.txt, then runs an access pattern designed to churn the table near capacity, producing repeated insert/erase cycles typical of the LRU pattern.

## Testing & Status

- **Status** Both executables successfully compile, run, and produce the expected outputs, the csv file and the traces.
- **Testing** Tested on blue successfully.
- **Known Issues** Minor compiler warnings involving integer comparison that do not affect correctness.

### How to compile and run my solution
This project has two parts: the trace generator and the harness. A makefile is included. 

- **Compile the programs:**  
  ```
  make clean
  make
  ```


	Two executables are created in the project root folder 

- **Run the trace generator:**  
  ```
  ./lru_tracegen
  ```
  
   
  This creates the trace files in:  
  `traceFiles/lru_profile/`
  Each file created corresponds to one N.

- **Run the harness:**  
  ```
  ./lru_harness > results.csv
  ```
  
  This outputs only the CSV file with timing results, created in:  
  `results.csv`









