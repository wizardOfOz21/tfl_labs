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

    TSet() {
        first = last = {};
        follow = {};
        e_flag = false;
        num = 0;
    }

    TSet(cchar a) {
        first = last = {a};
        follow = {};
        e_flag = false;
        num = 1;
    }

    static TSet* plus(TSet* arg1, TSet* arg2);
    static TSet* concat(TSet* arg1, TSet* arg2);
    static TSet* iter(TSet* arg);
    StateMachine to_machine();
};
