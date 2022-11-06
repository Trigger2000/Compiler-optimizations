#ifndef DOM_TREE_FAST_H
#define DOM_TREE_FAST_H

#include "pass.h"

// algorithm from https://www.cs.princeton.edu/courses/archive/fall03/cs528/handouts/a%20fast%20algorithm%20for%20finding.pdf
class DomTreeFast {
public:
    void RunPassImpl(Graph *g);

private:
    struct HelperNode {
        HelperNode(BasicBlock* current, HelperNode* parent, int& counter) :
        bb_(current), dfs_num_(counter), sdom_dfs_num_(counter), parent_(parent), label_(this)
        {
            counter++;
        }

        uint32_t sdom_dfs_num_ = 0;
        uint32_t dfs_num_ = 0;
        BasicBlock* bb_ = nullptr;
        HelperNode* parent_ = nullptr;
        HelperNode* ancestor_ = nullptr;
        HelperNode* dom_ = nullptr;
        HelperNode* label_ = nullptr;
    };

    void compress(HelperNode* node)
    {
        if (node->ancestor_->ancestor_ != nullptr) {
            compress(node->ancestor_);
            if (node->ancestor_->label_->sdom_dfs_num_ < node->label_->sdom_dfs_num_) {
                node->label_ = node->ancestor_->label_;
            }
            node->ancestor_ = node->ancestor_->ancestor_;
        }
    }

    HelperNode* eval(HelperNode* node)
    {
        if (node->ancestor_ == nullptr) {
            return node;
        } else {
            compress(node);
            return node->label_;
        }
    }

    void link(HelperNode* parent, HelperNode* child)
    {
        child->ancestor_ = parent;
    }

    void PreOrderVisitor(std::vector<HelperNode*>& result_vector, BasicBlock* current, HelperNode* parent)
    {
        if (current->GetMarker(BB_VISITED_MARKER)) {
            return;
        }
        current->SetMarker(BB_VISITED_MARKER, true);


        HelperNode* helper_node = new HelperNode(current, parent, counter);
        bb_to_dfs_num[current] = helper_node;
        result_vector.push_back(helper_node);


        for (auto succ: current->GetSuccs()) {
            PreOrderVisitor(result_vector, std::get<BasicBlock*>(succ), helper_node);
        }
    }

    std::vector<HelperNode*> GetPreOrder(Graph *g)
    {
        std::vector<HelperNode*> result_vector;
        std::vector<BasicBlock*> basic_blocks = g->GetBasicBlocks();
        assert(basic_blocks.size() > 0);
        basic_blocks[0]->SetMarker(BB_VISITED_MARKER, true);

        // TODO change to emplace_back and remove new
        HelperNode* helper_node = new HelperNode(basic_blocks[0], nullptr, counter);
        bb_to_dfs_num[basic_blocks[0]] = helper_node;
        result_vector.push_back(helper_node);

        for (auto succ: basic_blocks[0]->GetSuccs()) {
            PreOrderVisitor(result_vector, std::get<BasicBlock*>(succ), helper_node);
        }

        for (auto bb: basic_blocks) {
            bb->SetMarker(BB_VISITED_MARKER, false);
        }

        return result_vector;
    }

    const int BB_VISITED_MARKER = 0;

    int counter = 1;

    // for fast access dfs_num via bb
    std::unordered_map<BasicBlock*, HelperNode*> bb_to_dfs_num;
    // bucket from algorithm
    std::unordered_map<uint32_t /* sdom_dfs_num_ */, std::list<HelperNode*>> bucket;
};

#endif // DOM_TREE_FAST_H
