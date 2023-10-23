#include "parser/tset.hpp"

#include <fstream>

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
    cchar B2{'b', 2};
    cchar B3{'b', 3};
    TSet a = A;
    a.concat(B2);
    EXPECT_EQ(a.first, cset({A}));
    EXPECT_EQ(a.follow, dset({cpair(A, B2)}));
    EXPECT_EQ(a.last, cset({B2}));
    a.e_flag = true;
    a.concat(B3);
    EXPECT_EQ(a.first, cset({A, B3}));
    EXPECT_EQ(a.follow, dset({cpair(A, B2), cpair(B2, B3)}));
    EXPECT_EQ(a.last, cset({B3}));
}

TEST(TSet_Test, Iter_Test) {
    cchar A{'a', 1};
    cchar B2{'b', 2};
    cchar B3{'b', 3};
    TSet a = A;
    a.concat(B2);
    a.iter();
    EXPECT_EQ(a.first, cset({A}));
    EXPECT_EQ(a.follow, dset({cpair(A, B2), cpair(B2, A)}));
    EXPECT_EQ(a.last, cset({B2}));
    EXPECT_EQ(a.e_flag, true);
    a.concat(B3);
    EXPECT_EQ(a.first, cset({A, B3}));
    EXPECT_EQ(a.follow,
              dset({cpair(A, B2), cpair(B2, A), cpair(B2, B3)}));
    EXPECT_EQ(a.last, cset({B3}));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
