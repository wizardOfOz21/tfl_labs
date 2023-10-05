#pragma once
#include <set>
#include <utility>
#include "StateMachine.h"

struct cchar {
    char s;
    int num;
};

bool operator<(const cchar& a, const cchar& b);
bool operator==(const cchar& a, const cchar& b);

using std::pair;
using cpair = pair<cchar,cchar>;
using cset = std::set<cchar>;
using dset = std::set<cpair>;

struct TSet {
    cset first;
    dset follow;
    cset last;
    bool e_flag;
    int num;

    TSet(cchar a) {
        first = last = {a};
        follow = {};
        e_flag = false;
        num = 1;
    }

    void plus(const TSet& add);
    void concat(const TSet& add);
    void iter();
    StateMachine to_machine();
};
