#pragma once
#include <set>
#include <utility>

#include "StateMachine.h"

std::string alf = "ab";

struct cchar {
    char s;
    int num;
};

bool operator<(const cchar& a, const cchar& b) {
    if (a.s < b.s) return true;
    if (a.s == b.s) return a.num < b.num;
    return false;
}

bool operator==(const cchar& a, const cchar& b) {
    return a.s == b.s && a.num == b.num;
}
template <class T>
std::set<T>& set_union(std::set<T>& A, const std::set<T>& B) {
    for (T b : B) {
        A.insert(b);
    }
    return A;
}

using std::pair;
using cpair = pair<cchar, cchar>;
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
        if (a.s == '.') {
            int count = a.num;
            for (char s : alf) {
                first.insert(cchar{s, count});
                last.insert(cchar{s, count++});
            }
            num = alf.length();
        } else {
            first = last = {a};
            e_flag = false;
            num = 1;
        }
    }

    void plus(const TSet& arg) {
        set_union(first, arg.first);
        set_union(follow, arg.follow);
        set_union(last, arg.last);
        e_flag = e_flag || arg.e_flag;
        num += arg.num;
    };

    void concat(const TSet& arg) {
        if (e_flag) {
            for (cchar b : arg.first) {
                first.insert(b);
            }
        }

        for (auto ab : arg.follow) {
            follow.insert(ab);
        }

        for (cchar a : last) {
            for (cchar b : arg.first) {
                follow.insert(cpair(a, b));
            }
        }

        if (arg.e_flag) {
            for (cchar b : arg.last) {
                last.insert(b);
            }
        } else {
            last = arg.last;
        }

        e_flag = e_flag && arg.e_flag;
        num += arg.num;
    };

    void iter() {
        for (const cchar& a : last) {
            for (const cchar& b : first) {
                follow.insert(cpair(a, b));
            }
        }
        e_flag = true;
    };

    StateMachine to_machine() {
        StateMachine machine(num);
        for (cchar a : first) {
            machine.AddTransition(0, a.s, a.num);
        }
        for (auto ab : follow) {
            machine.AddTransition(ab.first.num, ab.second.s, ab.second.num);
        }
        for (cchar b : last) {
            machine.AddFinalState(b.num);
        }
        if (e_flag) {
            machine.AddFinalState(0);
        }
        return machine;
    };
};
