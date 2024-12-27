#pragma once

#include <vector>
#include "core.h"

class ComboAggregator {
public:
    ComboAggregator() = default;
    ~ComboAggregator() {
        combos.clear();
    }
    void loop(State& state) {
        for (auto combo : combos) {
            combo->loop(state);
        }
    }
    void add(ComboBase* combo) {
        if (std::find(combos.begin(), combos.end(), combo) == combos.end()) {
            combos.push_back(combo);
        }
    }
    void remove(ComboBase* combo) {
        auto newEnd = std::remove(combos.begin(), combos.end(), combo);
        combos.erase(newEnd, combos.end());
    }
    void clear() {
        combos.clear();
    }

private:
    std::vector<ComboBase*> combos;
};
