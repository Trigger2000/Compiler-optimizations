#ifndef ID_BUILDER_H
#define ID_BUILDER_H

#include <map>
#include <vector>

#include "graph.h"

class IrBuilder
{
public:
    Graph* GraphBuilder(std::initializer_list<BasicBlock*> bbs);

    // first successor is true branch
    // second successor is false branch
    template <uint32_t bb_id, uint32_t... successors>
    BasicBlock* BasicBlockBuilder(std::vector<Inst*> insts);

    template <Opcode opcode, typename... Args>
    Inst* InstBuilder(uint32_t inst_id, Args... args);

private:
    // assigns inputs and users to instructions
    void BuildDFG(Graph* g);

    template <Opcode opcode, typename... Args>
    Inst* BuildCallInst(uint32_t inst_id, Graph* callee, Args... args);

    template <Opcode opcode, typename... Args>
    Inst* BuildInst(uint32_t inst_id, Args... args);

    std::map<uint32_t, std::vector<uint32_t>> inst_id_to_inputs_ids_;
    std::map<uint32_t, std::vector<uint32_t>> bb_id_to_succs_ids_;
};

template <Opcode opcode, typename... Args>
Inst* IrBuilder::InstBuilder(uint32_t inst_id, Args... args)
{
    if constexpr (opcode == Opcode::CALL_STATIC) {
        return BuildCallInst<opcode>(inst_id, args...);
    } else {
        return BuildInst<opcode>(inst_id, args...);
    }
}

template <Opcode opcode, typename... Args>
Inst* IrBuilder::BuildCallInst(uint32_t inst_id, Graph* callee, Args... args)
{
    Inst* res = Inst::InstBuilder<opcode>(inst_id);
    res->CastToInstCall()->SetCallee(callee);
    std::vector<uint32_t> inputs = {args...};
    for (int i = 0; i < inputs.size(); ++i) {
        inst_id_to_inputs_ids_[inst_id].push_back(inputs[i]);
    }
    return res;
}

template <Opcode opcode, typename... Args>
Inst* IrBuilder::BuildInst(uint32_t inst_id, Args... args)
{
    std::vector<uint32_t> inputs = {args...};
    for (int i = 0; i < inputs.size(); ++i) {
        inst_id_to_inputs_ids_[inst_id].push_back(inputs[i]);
    }
    return Inst::InstBuilder<opcode>(inst_id);
}

template <uint32_t bb_id, uint32_t... successors>
BasicBlock* IrBuilder::BasicBlockBuilder(std::vector<Inst*> insts)
{
    BasicBlock* result = new BasicBlock(bb_id);

    result->SetSize(insts.size());

    // Bind instructions within basic block
    if (insts.size() > 0) {
        result->SetFirstInst(insts[0]);
        result->SetLastInst(insts[insts.size() - 1]);
        result->GetFirstInst()->SetBB(result);
        result->GetLastInst()->SetBB(result);
    }
    if (insts.size() > 1) {
        result->GetFirstInst()->SetNext(insts[1]);
        result->GetLastInst()->SetPrev(insts[insts.size() - 2]);

        for (size_t i = 1; i < insts.size() - 1; ++i) {
            insts[i]->SetBB(result);
            insts[i]->SetPrev(insts[i - 1]);
            insts[i]->SetNext(insts[i + 1]);
        }
    }
    
    if constexpr (sizeof...(successors) != 0) {
        for (auto succ_num: std::vector{ successors... }) {
            bb_id_to_succs_ids_[bb_id].push_back(succ_num);
        }
    }

    return result;
}

#endif // ID_BUILDER_H
