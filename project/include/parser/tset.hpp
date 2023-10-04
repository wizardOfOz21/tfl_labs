#pragma once
#include <set>
#include <utility>
#include "StateMachine.h"

using std::pair;
using cpair = pair<char,char>;
using cset = std::set<char>;
using dset = std::set<cpair>;

struct TSet {
    cset first;
    dset follow;
    cset last;
    bool e_flag;

    TSet(char a) {
        first = last = {a};
        follow = {};
        e_flag = false;
    }

    void plus(const TSet& add);
    void concat(const TSet& add);
    void iter();
    StateMachine create_machine();
};
