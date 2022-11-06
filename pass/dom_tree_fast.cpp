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
    g->SetMarker(PassType::DomTree, true);
}
