#ifndef DOM_TREE_FAST_H
#define DOM_TREE_FAST_H

#include <unordered_map>

#include "ir/graph.h"

// algorithm from https://www.cs.princeton.edu/courses/archive/fall03/cs528/handouts/a%20fast%20algorithm%20for%20finding.pdf
class DomTreeFast {
public:
    void RunPassImpl(Graph *g);

private:
    struct HelperNode {
        HelperNode(BasicBlock* current, HelperNode* parent, int& curr_dfs_num_) :
        bb_(current), dfs_num_(curr_dfs_num_), sdom_dfs_num_(curr_dfs_num_), parent_(parent), label_(this)
        {
            curr_dfs_num_++;
        }

        uint32_t sdom_dfs_num_ = 0;
        uint32_t dfs_num_ = 0;
        BasicBlock* bb_ = nullptr;
        HelperNode* parent_ = nullptr;
        HelperNode* ancestor_ = nullptr;
        HelperNode* dom_ = nullptr;
        HelperNode* label_ = nullptr;
    };

    HelperNode* eval(HelperNode* node);
    void compress(HelperNode* node);
    void link(HelperNode* parent, HelperNode* child);
    void PreOrderVisitor(std::vector<HelperNode*>& result_vector, BasicBlock* current, HelperNode* parent,
                        marker visited_marker);
    std::vector<HelperNode*> GetPreOrder(Graph *g);

    int curr_dfs_num_ = 1;

    // for fast access dfs_num via bb
    std::unordered_map<BasicBlock*, HelperNode*> bb_to_dfs_num;
    // bucket from algorithm
    std::unordered_map<uint32_t /* sdom_dfs_num_ */, std::list<HelperNode*>> bucket;
};

#endif // DOM_TREE_FAST_H
