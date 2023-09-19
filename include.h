#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using std::string;
using set = std::unordered_set<char>;
using std::vector;
using rule = std::pair<string, string>;

void get_rules(vector<rule>& res, std::ifstream& src);
string make_condition(std::string lhs, std::string rhs, set& alf);
