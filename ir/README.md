### Inst.h
Implements basic сlass `Inst` and all its successors (different types of instructions). Operations with all instructions are impemented via intrusive linked list, implemented in class `IListNode`.

Creation of all instructions is implemented via static method `IListNode::InstBuilder(Opcode, inputs...)`, destruction via `InstDestroyer(IListNode* inst_node)`

### basic_block.h
Several instructions, passed to `BasicBlock::BasicBlockBuilder(insts...)`, bind together and form basic block. This file contains BasicBlock class that holds pointer to head and tail of intrusive linked list of instructions. Also each basic block contains all it's predecessors and successors.

### graph.h
Contains `Graph` class, which holds several basic blocks. Passing arguments to `Graph`'s constructor, binds basic block with each other and assigns predecessors and successors for each basic block

### Usage
```
GRAPH{
    BASIC_BLOCK<id, successors>({
        INST<id>(OPCODE, inputs...),
        INST<id>(OPCODE, inputs...),
        ...
        INST<id>(OPCODE, inputs...)
    }),
    BASIC_BLOCK<id, successors>({
        INST<id>(OPCODE, inputs...),
        INST<id>(OPCODE, inputs...),
        ...
        INST<id>(OPCODE, inputs...)
    }),
    ...
    BASIC_BLOCK<id, successors>({
        INST<id>(OPCODE, inputs...),
        INST<id>(OPCODE, inputs...),
        ...
        INST<id>(OPCODE, inputs...)
    })
}
```

Id in BASIC_BLOCK must be unique accross all basic blocks in GRAPH. Id in INST must be unique accross all instructions in BASIC_BLOCK. 

To dump CFG call `Graph.Dump()`. Only CFG construction is supported currently. DFG will be added after changing std::vector to std::unordered_map in BasicBlock and creating corresponding getters.
