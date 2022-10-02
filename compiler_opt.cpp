#include "inst.h"

int main()
{
    IntrusiveListNode *tmp = Inst::InstBuilder(Opcode::ADD, 1, 2, 3);
    return 0;
}