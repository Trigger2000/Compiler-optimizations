#ifndef PASS_MANAGER_H
#define PASS_MANAGER_H

#include <tuple>
#include <type_traits>

#include "ir/graph.h"

class RPO;
class DomTreeSlow;
class DomTreeFast;
class LoopAnalyzer;
class ConstFolding;
class DCE;

using PassList = std::tuple<RPO, DomTreeSlow, DomTreeFast, LoopAnalyzer, ConstFolding, DCE>;

class PassManager {
protected:
    template <typename Pass>
    void RunPass(Graph *g)
    {
        if (g->template IsPassValid<Pass>()) {
            return;
        }
        Pass pass;
        pass.RunPassImpl(g);
        g->template SetPassValidity<Pass>(true);
    }

    template <typename Pass>
    constexpr size_t GetPassIndex() {
        GetPassIndexHelper<Pass, 0>();
    }

private:
    template <typename Pass, size_t Index>
    constexpr size_t GetPassIndexHelper() {
        static_assert(Index < std::tuple_size_v<PassList>);
        if constexpr (std::is_same_v<Pass, std::tuple_element_t<Index, PassList>>) {
            return Index;
        } else {
            return GetPassIndexHelper<Pass, Index + 1>();
        }
    }
};

#endif // PASS_MANAGER_H
