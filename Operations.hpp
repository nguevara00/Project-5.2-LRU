//
// Created by Ali Kooshesh on 11/16/25.
//

#pragma once
#include <cassert>
#include <string>
#include <iostream>

enum class OpCode {
    Insert,     // I key
    Erase  // E key (delete a key)
  };

struct Operation {
    OpCode tag;
    std::string key;

    // Either of the two op_codes take a string argument.
    Operation(OpCode op_code, const std::string &k) : tag(op_code), key(k) {
        assert(op_code == OpCode::Insert || op_code == OpCode::Erase);
    }

    void print() const {
        switch (tag) {
            case OpCode::Insert:
                std::cout << "I" << " " << key << " " << std::endl;
                break;
            case OpCode::Erase:
                std::cout << "E " << key << std::endl;
                break;
            default:
                std::cout << "Unknown operation: " << static_cast<int>(tag) << std::endl;
        }

    }
    // Identify the instance
    [[nodiscard]] bool isInsert()     const { return tag == OpCode::Insert; }
    [[nodiscard]] bool isFindMin()    const { return tag == OpCode::Erase; }
};
