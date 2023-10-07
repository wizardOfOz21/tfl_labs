#include "parser/tset.hpp"

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

TSet* TSet::plus(TSet* arg1, TSet* arg2) {
    set_union(arg1->first, arg2->first);
    set_union(arg1->follow, arg2->follow);
    set_union(arg1->last, arg2->last);
    arg1->e_flag = arg2->e_flag || arg2->e_flag;
    arg1->num += arg2->num;
    delete arg2;
    return arg1;
}

TSet* TSet::concat(TSet* arg1, TSet* arg2) {
    if (arg1->e_flag) {
        for (cchar b : arg2->first) {
            arg1->first.insert(b);
        }
    }

    for (auto ab : arg2->follow) {
        arg1->follow.insert(ab);
    }

    for (cchar a : arg1->last) {
        for (cchar b : arg2->first) {
            arg1->follow.insert(pair(a, b));
        }
    }

    if (arg2->e_flag) {
        for (cchar b : arg2->last) {
            arg1->last.insert(b);
        }
    } else {
        arg1->last = arg2->last;
    }

    arg1->e_flag = arg1->e_flag && arg2->e_flag;
    arg1->num += arg2->num;
    delete arg2;
    return arg1;
}

TSet* TSet::iter(TSet* arg) {
    for (cchar a : arg->last) {
        for (cchar b : arg->first) {
            bool c = arg->follow.insert(pair(a, b)).second;
        }
    }
    arg->e_flag = true;
    return arg;
}

StateMachine TSet::to_machine() {
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
    return std::move(machine);
};
