//
// Created by Ali Kooshesh on 11/4/25.
//

#ifndef PRIORITY_QUEUE_STUDY_RUNPARAMS_HPP
#define PRIORITY_QUEUE_STUDY_RUNPARAMS_HPP



#include <string>
struct RunMetaData {

    // dataset metadata
    std::string profile;      // e.g., "huffman"
    int N   = 0;    // problem size for the trace (e.g., initial inserts)
    int seed = 0;   // RNG seed used to generate the trace

};

#endif //PRIORITY_QUEUE_STUDY_RUNPARAMS_HPP