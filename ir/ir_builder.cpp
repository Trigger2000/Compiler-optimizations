#include "ir_builder.h"

Graph* IrBuilder::GraphBuilder(std::initializer_list<BasicBlock*> bbs)
{
    Graph* result = new Graph{bbs};

    for (auto item: bb_id_to_succs_ids_) {
        auto bb = result->GetBBbyId(item.first);
        for (auto succ_id: item.second) {
            bb->AddSucc(result->GetBBbyId(succ_id));
        }
    }

    for (auto curr_bb : bbs) {
        for (auto bb : bbs) {
            if (bb->HasSucc(curr_bb)) {
                curr_bb->AddPred(bb);
            }
        }
        curr_bb->SetGraph(result);
    }

    BuildDFG(result);
    return result;
}

void IrBuilder::BuildDFG(Graph* g)
{
    for (auto bb: g->GetBasicBlocks()) {
        for (auto inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext()) {
            switch (inst->GetType())
            {
            case Type::InstWithOneInput: {
                InstWithOneInput* inst_casted = inst->CastToInstWithOneInput();
                inst_casted->SetInput1(g->GetInstById(inst_id_to_inputs_ids_[inst->GetId()][0]));
                inst_casted->GetInput1()->AddUser(inst);
                break;
            }
            case Type::InstWithTwoInputs: {
                InstWithTwoInputs* inst_casted = inst->CastToInstWithTwoInputs();
                inst_casted->SetInput1(g->GetInstById(inst_id_to_inputs_ids_[inst->GetId()][0]));
                inst_casted->GetInput1()->AddUser(inst);
                inst_casted->SetInput2(g->GetInstById(inst_id_to_inputs_ids_[inst->GetId()][1]));
                inst_casted->GetInput2()->AddUser(inst);
                break;
            }
            case Type::InstWithNoInputs: {
                break;
            }
            case Type::InstPhi: {
                InstPhi* inst_casted = inst->CastToInstPhi();
                for (int i = 0; i < inst_id_to_inputs_ids_[inst->GetId()].size();) {
                    inst_casted->AddInput(g->GetInstById(inst_id_to_inputs_ids_[inst->GetId()][i]),
                                          g->GetBBbyId(inst_id_to_inputs_ids_[inst->GetId()][i + 1]));
                    g->GetInstById(inst_id_to_inputs_ids_[inst->GetId()][i])->AddUser(inst);
                    i += 2;
                }
                break;
            }
            case Type::InstConstant: {
                inst->CastToInstConstant()->SetConstant(inst_id_to_inputs_ids_[inst->GetId()][0]);
                break;
            }
            case Type::InstCall: {
                std::vector<Inst*> call_arguments;
                for (int i = 0; i < inst_id_to_inputs_ids_[inst->GetId()].size(); ++i) {
                    call_arguments.push_back(g->GetInstById(inst_id_to_inputs_ids_[inst->GetId()][i]));
                    call_arguments[i]->AddUser(inst);
                }
                inst->CastToInstCall()->SetArguments(call_arguments);
                break;
            }
            case Type::InstJmp: {
                inst->CastToInstJmp()->SetTargetBB(g->GetBBbyId(inst_id_to_inputs_ids_[inst->GetId()][0]));
                break;
            }
            default:
                break;
            }
        }
    }
}