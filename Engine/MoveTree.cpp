#include "MoveTree.h"

MoveNode::MoveNode(std::string val){
    this->value = val;
    this->children = {};
}

MoveNode::MoveNode(){
    this->value = "";
    this->children = {};
}