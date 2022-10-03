#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "inst.h"

class InstNode;

class BasicBlock {
private:
    InstNode *first_inst = 0;
};


#endif // BASIC_BLOCK_H
