//
// Created by Ali Kooshesh on 10/31/25.
//

#include<utility>
#include "comparator.hpp"

bool compare_pair(const std::pair<int,int>& a,
                  const std::pair<int,int>& b) {
    if (a.first != b.first) return a.first < b.first;  // compare keys
    return a.second < b.second;                        // tie-break by id
}
