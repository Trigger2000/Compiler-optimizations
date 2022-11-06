### Inst.h
Implements basic сlass `Inst` and all its successors (different types of instructions). Access to all methods in derived from `Inst` classes can be made using virtual functions in `Inst` itself.  
Contains `Users` class and class `Input` and several derived classes: `JmpInput` - special "input" for jump/branch instructions and `PhiInput` - extended `Input`.  
To build DFG currently each input consists of `input_id` and reference to dedicated instruction `Inst*` with this id. While constructing Graph all ids are resolved with references. If possible, in the future ids will be removed.

Creation of all instructions is implemented via static method `Inst::InstBuilder(Opcode, inputs...)`, destruction via `InstDestroyer(Inst* inst_node)`

### basic_block.h
Several instructions, passed to `BasicBlock::BasicBlockBuilder(insts...)`, bind together and form basic block. This file contains BasicBlock class that holds pointer to head and tail of linked list of instructions. Also each basic block contains all it's predecessors and successors.

### graph.h
Contains `Graph` class, which holds several basic blocks. Passing arguments to `Graph`'s constructor, binds basic block with each other and assigns predecessors and successors for each basic block. Also `Graph` constructs DFG using `BuildDFG` method, resolving ids inputs to references and assigning users.

### Usage
```a
GRAPH{
    BASIC_BLOCK<id, successors>({
        INST(id, OPCODE, inputs...),
        INST(id, OPCODE, inputs...),
        ...
        INST(id, OPCODE, inputs...)
    }),
    BASIC_BLOCK<id, successors>({
        INST(id, OPCODE, inputs...),
        INST(id, OPCODE, inputs...),
        ...
        INST(id, OPCODE, inputs...)
    }),
    ...
    BASIC_BLOCK<id, successors>({
        INST(id, OPCODE, inputs...),
        INST(id, OPCODE, inputs...),
        ...
        INST(id, OPCODE, inputs...)
    })
}
```

Id in BASIC_BLOCK must be unique accross all basic blocks in GRAPH. Id in INST must be unique accross all instructions in GRAPH. 

To dump CFG and DFG call `Graph.Dump()`
