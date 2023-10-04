#include "parser/tset.hpp"

template <class T>
std::set<T>& set_union(std::set<T>& A, const std::set<T>& B) {
    for (T b : B) {
        A.insert(b);
    }
    return A;
}

void TSet::plus(const TSet& add) {
    set_union(first, add.first);
    set_union(follow, add.follow);
    set_union(last, add.last);
    e_flag = e_flag || add.e_flag;
}

void TSet::concat(const TSet& add) {
    if (e_flag) {
        for (char b : add.first) {
            first.insert(b);
        }
    }

    for (auto ab : add.follow) {
        follow.insert(ab);
    }

    for (char a : last) {
        for (char b : add.first) {
            follow.insert(pair(a, b));
        }
    }

    if (add.e_flag) {
        for (char b : add.last) {
            last.insert(b);
        }
    } else {
        last = add.last;
    }

    e_flag = e_flag && add.e_flag;
}

void TSet::iter() {
    for (char a : last) {
        for (char b : first) {
            follow.insert(pair(a, b));
        }
    }
    e_flag = true;
}
