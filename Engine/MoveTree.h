#include "Move.h"
#include <vector>
#pragma once

class MoveNode{
    public:
        std::string value;
        std::vector<MoveNode> children;
        MoveNode(std::string val);
        MoveNode();


    bool operator==(const MoveNode& other) const {
		return this->value == other.value;
	}
	bool operator!=(const MoveNode& other) const {
        return !(*this == other);
    }
};

class MoveTree{
    public:
    MoveNode  root;
};
