#include <fstream>
#include <regex>

#include "RegexGenerator.h"
#include "StateMachine.h"
#include "StringGenerator.h"
#include "gtest/gtest.h"
#include "parser/parser2.hpp"

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2/pcre2.h"

int re_match(pcre2_code *re, const string &str, pcre2_match_data *match_data) {
    return pcre2_match(re, (PCRE2_SPTR)str.data(), str.length(), 0, 0,
                       match_data, NULL);
}

pcre2_code *re_compile(const std::string &regex) {
    int error_number = 0;
    int error_offset = 0;
    return pcre2_compile((PCRE2_SPTR)regex.data(), regex.length(), 0,
                         &error_number, (size_t *)&error_offset, NULL);
}

TEST(Convert_Test, Convert_Test) {
    RegexGenerator generator(13, 2, 2, 0, 2);
    for (int i = 0; i < 1000; ++i) {
        string regex = generator.GenerateRegex();
        Parser r(regex.data(), regex.length());
        node_ptr R = r.Parse();
        StateMachine M = R->to_machine_dfs();
    }
}

// TEST(Fuzz_Test, Match_Regular_Test) {
//     RegexGenerator generator(4, 2, 0, 0, 2);
//     for (int i = 0; i < 100; ++i) {
//         string regex = generator.GenerateRegex();
//         Parser r(regex.data(), regex.length());
//         node_ptr R = r.Parse();
//         StateMachine M = R->to_machine_dfs();
//         std::string converted = M.ConvertToRegularExpr();

//         pcre2_code *original = re_compile(regex);
//         pcre2_code *result = re_compile(converted);
//         assert(original != NULL);
//         assert(result != NULL);

//         pcre2_match_data *match_data1 =
//             pcre2_match_data_create_from_pattern(original, NULL);
//         pcre2_match_data *match_data2 =
//             pcre2_match_data_create_from_pattern(result, NULL);

//         StringGenerator sg;
//         for (int i = 0; i < 1000; i++) {
//             std::string curStr = sg.GenerateString(M);
//             int match_code_orig = re_match(original, curStr, match_data1);
//             int match_code_res = re_match(result, curStr, match_data2);

//             if (match_code_orig == PCRE2_ERROR_MATCHLIMIT)
//                 continue;  // если всё-таки не вывезла
//             if (match_code_res == PCRE2_ERROR_MATCHLIMIT) continue;

//             EXPECT_EQ(match_code_orig >= 0, match_code_res >= 0);
//         }

//         pcre2_match_data_free(match_data1);
//         pcre2_match_data_free(match_data2);
//         pcre2_code_free(original);
//         pcre2_code_free(result);
//     }
// }

// TEST(Fuzz_Test, Match_Lookahead_Test) {
//     RegexGenerator generator(10, 2, 5, 0, 2);
//     for (int i = 0; i < 100; ++i) {
//         string regex = generator.GenerateRegex();
//         Parser r(regex.data(), regex.length());
//         node_ptr R = r.Parse();
//         StateMachine M = R->to_machine_dfs();
//         std::string converted = M.ConvertToRegularExpr();

//         pcre2_code *original = re_compile(regex);
//         pcre2_code *result = re_compile(converted);
//         assert(original != NULL);
//         assert(result != NULL);

//         pcre2_match_data *match_data1 =
//             pcre2_match_data_create_from_pattern(original, NULL);
//         pcre2_match_data *match_data2 =
//             pcre2_match_data_create_from_pattern(result, NULL);

//         StringGenerator sg;
//         for (int i = 0; i < 1000; i++) {
//             std::string curStr = sg.GenerateString(M);
//             int match_code_orig = re_match(original, curStr, match_data1);
//             int match_code_res = re_match(result, curStr, match_data2);

//             if (match_code_orig == PCRE2_ERROR_MATCHLIMIT)
//                 continue;  // если всё-таки не вывезла
//             if (match_code_res == PCRE2_ERROR_MATCHLIMIT) continue;

//             EXPECT_EQ(match_code_orig >= 0, match_code_res >= 0);
//         }

//         pcre2_match_data_free(match_data1);
//         pcre2_match_data_free(match_data2);
//         pcre2_code_free(original);
//         pcre2_code_free(result);
//     }
// }

// TEST(Fuzz_Test, Match_Lookbehind_Test) {
//     RegexGenerator generator(3, 2, 0, 3, 2);
//     for (int i = 0; i < 1000; ++i) {
//         string regex = generator.GenerateRegex();
//         Parser r(regex.data(), regex.length());
//         node_ptr R = r.Parse();
//         StateMachine M = R->to_machine_dfs();
//         std::string converted = M.ConvertToRegularExpr();

//         pcre2_code *original = re_compile(regex);
//         pcre2_code *result = re_compile(converted);
//         // assert(original != NULL);
//         // assert(result != NULL);
//         if (original == NULL) {
//             std::cout << "null-orig: " << regex << std::endl; 
//         }
//         if (result == NULL) {
//             std::cout << "null-result" << regex << std::endl; 
//         }

//         pcre2_match_data *match_data1 =
//             pcre2_match_data_create_from_pattern(original, NULL);
//         pcre2_match_data *match_data2 =
//             pcre2_match_data_create_from_pattern(result, NULL);

//         StringGenerator sg;
//         for (int i = 0; i < 1000; i++) {
//             std::string curStr = sg.GenerateString(M);
//             int match_code_orig = re_match(original, curStr, match_data1);
//             int match_code_res = re_match(result, curStr, match_data2);

//             if (match_code_orig == PCRE2_ERROR_MATCHLIMIT)
//                 continue;  // если всё-таки не вывезла
//             if (match_code_res == PCRE2_ERROR_MATCHLIMIT) continue;

//             if ((match_code_orig >= 0) != (match_code_res >= 0)) {
//                 std::cout << "__________" << std::endl; 
//                 std::cout << regex << ":" << match_code_orig << std::endl; 
//                 std::cout << converted << ":" << match_code_res << std::endl;
//                 std::cout << curStr << std::endl; 
//                 std::cout << "__________" << std::endl; 
//             }

//             // EXPECT_EQ(match_code_orig >= 0, match_code_res >= 0);
//         }

//         pcre2_match_data_free(match_data1);
//         pcre2_match_data_free(match_data2);
//         pcre2_code_free(original);
//         pcre2_code_free(result);
//     }
// }


TEST(Fuzz_Test, Match_Lookbehind_and_Lookahead_Test) {
    RegexGenerator generator(3, 2, 3, 3, 2);
    for (int i = 0; i < 1000; ++i) {
        string regex = generator.GenerateRegex();
        std::cout<<regex<<std::endl;
        Parser r(regex.data(), regex.length());
        node_ptr R = r.Parse();
        StateMachine M = R->to_machine_dfs();
        std::string converted = M.ConvertToRegularExpr();

        pcre2_code *original = re_compile(regex);
        pcre2_code *result = re_compile(converted);
        // assert(original != NULL);
        // assert(result != NULL);
        if (original == NULL) {
            std::cout << "null-orig: " << regex << std::endl; 
        }
        if (result == NULL) {
            std::cout << "null-result" << regex << std::endl; 
        }

        pcre2_match_data *match_data1 =
            pcre2_match_data_create_from_pattern(original, NULL);
        pcre2_match_data *match_data2 =
            pcre2_match_data_create_from_pattern(result, NULL);

        StringGenerator sg;
        for (int i = 0; i < 1000; i++) {
            std::string curStr = sg.GenerateString(M);
            int match_code_orig = re_match(original, curStr, match_data1);
            int match_code_res = re_match(result, curStr, match_data2);

            if (match_code_orig == PCRE2_ERROR_MATCHLIMIT)
                continue;  // если всё-таки не вывезла
            if (match_code_res == PCRE2_ERROR_MATCHLIMIT) continue;

            if ((match_code_orig >= 0) != (match_code_res >= 0)) {
                std::cout << "__________" << std::endl; 
                std::cout << regex << ":" << match_code_orig << std::endl; 
                std::cout << converted << ":" << match_code_res << std::endl;
                std::cout << curStr << std::endl; 
                std::cout << "__________" << std::endl; 
            }

            // EXPECT_EQ(match_code_orig >= 0, match_code_res >= 0);
        }

        pcre2_match_data_free(match_data1);
        pcre2_match_data_free(match_data2);
        pcre2_code_free(original);
        pcre2_code_free(result);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
