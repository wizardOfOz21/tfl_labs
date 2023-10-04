#include "parser/tset.hpp"

#include "gtest/gtest.h"

TEST(TSet_Test, Union_Test) {
    TSet a = 'a';
    TSet b = 'b';
    a.plus(b);
    EXPECT_EQ(a.first, cset({'a', 'b'}));
    EXPECT_EQ(a.follow, dset({}));
    EXPECT_EQ(a.last, cset({'a', 'b'}));
}

TEST(TSet_Test, Concat_Test) {
    TSet a = 'a';
    TSet b = 'b';
    a.concat(b);
    EXPECT_EQ(a.first, cset({'a'}));
    EXPECT_EQ(a.follow, dset({cpair('a','b')}));
    EXPECT_EQ(a.last, cset({'b'}));
    a.e_flag = true;
    a.concat(b);
    EXPECT_EQ(a.first, cset({'a', 'b'}));
    EXPECT_EQ(a.follow, dset({cpair('a','b'), cpair('b','b')}));
    EXPECT_EQ(a.last, cset({'b'}));
}

TEST(TSet_Test, Iter_Test) {
    TSet a = 'a';
    a.concat('b');
    a.iter();
    EXPECT_EQ(a.first, cset({'a'}));
    EXPECT_EQ(a.follow, dset({cpair('a','b'), cpair('b','a')}));
    EXPECT_EQ(a.last, cset({'b'}));
    EXPECT_EQ(a.e_flag, true);
    a.concat('b');
    EXPECT_EQ(a.first, cset({'a', 'b'}));
    EXPECT_EQ(a.follow, dset({cpair('a','b'), cpair('b','a'), cpair('b','b')}));
    EXPECT_EQ(a.last, cset({'b'}));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
