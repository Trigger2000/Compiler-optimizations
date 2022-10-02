#include "inst.h"

int main()
{
    InstNode *ins1 = InstNode::InstBuilder(Opcode::ADD, 1, 2, 3);
    InstNode *ins2 = InstNode::InstBuilder(Opcode::ADD, 1, 2, 3);
    InstNode *ins3 = InstNode::InstBuilder(Opcode::ADD, 1, 2, 3);
    InstNode *ins4 = InstNode::InstBuilder(Opcode::ADD, 1, 2, 3);
    std::cout << ins3->GetId() << "\n";
    return 0;
}