#include "dom_tree_fast.h"

void DomTreeFast::RunPassImpl(Graph *g)
{
    // step 1
    std::vector<HelperNode*> result_vector = GetPreOrder(g);

    for (auto node = result_vector.rbegin(); node != std::prev(result_vector.rend(), 1); std::advance(node, 1)) {
        // step 2
        for (auto pred: (*node)->bb_->GetPreds()) {
            HelperNode *u = eval(bb_to_dfs_num[pred]);
            if (u->sdom_dfs_num_ < (*node)->sdom_dfs_num_) {
                (*node)->sdom_dfs_num_ = u->sdom_dfs_num_;
            }
        }
        bucket[(*node)->sdom_dfs_num_].push_back((*node));
        link((*node)->parent_, (*node));
        // step 3
        auto& bucket_entity = bucket[(*node)->parent_->dfs_num_];
        for (auto v = bucket_entity.begin(); v != bucket_entity.end();) {
            auto next = std::next(v, 1);
            bucket_entity.remove(*v);

            HelperNode *u = eval(*v);
            if (u->sdom_dfs_num_ < (*v)->sdom_dfs_num_) {
                (*v)->dom_ = u;
            } else {
                (*v)->dom_ = (*node)->parent_;
            }

            v = next;
        }

    }

    // step 4
    for (auto node = std::next(result_vector.begin(), 1); node != result_vector.end(); std::advance(node, 1)) {
        if ((*node)->dom_->dfs_num_ != (*node)->sdom_dfs_num_) {
            (*node)->dom_ = (*node)->dom_->dom_;
        }
    }

    // assign immediate dominators to basic blocks
    for (auto node = std::next(result_vector.begin(), 1); node != result_vector.end(); std::advance(node, 1)) {
        (*node)->bb_->SetIDom((*node)->dom_->bb_);
    }

    for (auto node: result_vector) {
        delete node;
    }
}

void DomTreeFast::compress(HelperNode* node)
{
    if (node->ancestor_->ancestor_ != nullptr) {
        compress(node->ancestor_);
        if (node->ancestor_->label_->sdom_dfs_num_ < node->label_->sdom_dfs_num_) {
            node->label_ = node->ancestor_->label_;
        }
        node->ancestor_ = node->ancestor_->ancestor_;
    }
}

DomTreeFast::HelperNode* DomTreeFast::eval(HelperNode* node)
{
    if (node->ancestor_ == nullptr) {
        return node;
    } else {
        compress(node);
        return node->label_;
    }
}

void DomTreeFast::link(HelperNode* parent, HelperNode* child)
{
    child->ancestor_ = parent;
}

void DomTreeFast::PreOrderVisitor(std::vector<HelperNode*>& result_vector, BasicBlock* current, HelperNode* parent,
                    marker visited_marker)
{
    if (current->IsMarked(visited_marker)) {
        return;
    }
    current->SetMarker(visited_marker);
    HelperNode* helper_node = new HelperNode(current, parent, curr_dfs_num_);
    bb_to_dfs_num[current] = helper_node;
    result_vector.push_back(helper_node);
    for (auto succ: current->GetSuccs()) {
        PreOrderVisitor(result_vector, succ, helper_node, visited_marker);
    }
}

std::vector<DomTreeFast::HelperNode*> DomTreeFast::GetPreOrder(Graph *g)
{
    std::vector<HelperNode*> result_vector;
    std::vector<BasicBlock*> basic_blocks = g->GetBasicBlocks();
    marker visited_marker = g->NewMarker();
    
    assert(basic_blocks.size() > 0);
    basic_blocks[0]->SetMarker(visited_marker);
    // TODO change to emplace_back and remove new
    HelperNode* helper_node = new HelperNode(basic_blocks[0], nullptr, curr_dfs_num_);
    bb_to_dfs_num[basic_blocks[0]] = helper_node;
    result_vector.push_back(helper_node);
    for (auto succ: basic_blocks[0]->GetSuccs()) {
        PreOrderVisitor(result_vector, succ, helper_node, visited_marker);
    }
    g->EraseMarker(visited_marker);
    return result_vector;
}
