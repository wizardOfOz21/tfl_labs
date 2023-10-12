#include <fstream>

#include "RegexGenerator.h"
#include "gtest/gtest.h"
#include "parser/parser2.hpp"

const vector<string> tricky_regex{
    /*0*/ "^ab((?=.*(aa|b)$)(a|ab)*)ab((?=.*(ba|aa)$)(b|ba)*)a$",
    /*1*/ "^a|(?=.*(aa|b)$)(a|ab)*|(ba)*|(?=.*(ba|aa)$)(b|ba)*|a$",
    /*2*/ "^ab((?=.*(aa|b)$)(a|ab)*|(?=.*(ba|aa)$)(b|ba)*)$",
    /*3*/ "^a*((?=bb$)|(?=a|a$))|a((b|aa)a)((((aa((((b))))))))$",
    /*4*/ "^a|b|(ab)*|ba(ab*(ab))$",
    /*5*/ "^(a(ab)*)*|(ba)*$",
    /*6*/ "^ba*(?=((ca*|(ba*|b|((a))|(b)b)))$)$",
    /*7*/ "^a|b|((?=a)|(?=b$))|c$",
    /*8*/ "^a(b)(c(?=a*c*)(b*))c$",
    /*9*/ "^(((((?=c)))))b$",
    /*10*/ "^(a(ab)*)*|(ba)*$",
};

TEST(Parser_Test, Special_Regex_Parse_Test) {
    unordered_set<char> alphabet{'a', 'b', 'c'};
    for (string regex : tricky_regex) {
        Parser parser(regex.data(), regex.length(), alphabet);
        node_ptr R = parser.Parse();
        EXPECT_TRUE(R);
    }
}

TEST(Parser_Test, Empty_Regex_Parse_Test) {
    unordered_set<char> alphabet{'a', 'b', 'c'};
    string e_regex = "^$";
    Parser parser(e_regex.data(), e_regex.length(), alphabet);
    node_ptr R = parser.Parse();
    EXPECT_TRUE(R);
    EXPECT_EQ(R->type, NodeType::REGEX);
    EXPECT_FALSE(R->syntax_tree);
}

TEST(Parser_Test, Random_Regex_Parse_Test) {
    RegexGenerator generator;
    for (int i = 0; i < 1000; ++i) {
        string regex = generator.GenerateRegex();
        Parser r(regex.data(), regex.length(), {'a', 'b', 'c'});
        node_ptr R = r.Parse();
        EXPECT_TRUE(R);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
