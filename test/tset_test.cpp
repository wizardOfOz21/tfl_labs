#include <fstream>

#include "parser/tset.hpp"
#include "StateMachine.h"

#include "gtest/gtest.h"

TEST(TSet_Test, Union_Test) {
    cchar A{'a', 1};
    cchar B{'b', 2};
    TSet a = A;
    a.plus(B);
    EXPECT_EQ(a.first, cset({A, B}));
    EXPECT_EQ(a.follow, dset({}));
    EXPECT_EQ(a.last, cset({A, B}));
}

TEST(TSet_Test, Concat_Test) {
    cchar A{'a', 1};
    cchar B{'b', 2};
    cchar B2{'b', 2};
    TSet a = A;
    a.concat(B);
    EXPECT_EQ(a.first, cset({A}));
    EXPECT_EQ(a.follow, dset({cpair(A, B)}));
    EXPECT_EQ(a.last, cset({B}));
    a.e_flag = true;
    a.concat(B2);
    EXPECT_EQ(a.first, cset({A, B2}));
    EXPECT_EQ(a.follow, dset({cpair(A, B), cpair(B, B2)}));
    EXPECT_EQ(a.last, cset({B2}));
}

TEST(TSet_Test, Iter_Test) {
    cchar A{'a', 1};
    cchar B{'b', 2};
    TSet a = A;
    a.concat(B);
    a.iter();
    EXPECT_EQ(a.first, cset({A}));
    EXPECT_EQ(a.follow, dset({cpair(A, B), cpair(B, A)}));
    EXPECT_EQ(a.last, cset({B}));
    EXPECT_EQ(a.e_flag, true);
    a.concat(B);
    EXPECT_EQ(a.first, cset({A, B}));
    EXPECT_EQ(a.follow,
              dset({cpair(A, B), cpair(B, A), cpair(B, A)}));
    EXPECT_EQ(a.last, cset({B}));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
