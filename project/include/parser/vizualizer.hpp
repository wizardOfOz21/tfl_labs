#include <string>
#include <vector>
#include <iostream>
#include <memory>

#include "parser/parser.hpp"

static std::string ch_hor = "-", ch_ver = "|", ch_ddia = "/", ch_rddia = "\\",
                   ch_udia = "\\", ch_ver_hor = "|-", ch_udia_hor = "\\-",
                   ch_ddia_hor = "/-", ch_ver_spa = "| ";

void dump4(
    Node const* node, bool high = true,
    std::vector<std::string> const& lpref = std::vector<std::string>(),
    std::vector<std::string> const& cpref = std::vector<std::string>(),
    std::vector<std::string> const& rpref = std::vector<std::string>(),
    bool root = true, bool left = true,
    std::shared_ptr<std::vector<std::vector<std::string>>> lines = nullptr) {
    if (!node) return;
    typedef std::vector<std::string> VS;
    auto VSCat = [](VS const& a, VS const& b) {
        auto r = a;
        r.insert(r.end(), b.begin(), b.end());
        return r;
    };
    if (root) lines = std::make_shared<std::vector<VS>>();
    if (node->left)
        dump4(node->left, high, VSCat(lpref, high ? VS({" ", " "}) : VS({" "})),
              VSCat(lpref, high ? VS({ch_ddia, ch_ver}) : VS({ch_ddia})),
              VSCat(lpref, high ? VS({ch_hor, " "}) : VS({ch_hor})), false,
              true, lines);
    auto sval = node->value;
    size_t sm =
        left || sval.empty() ? sval.size() / 2 : ((sval.size() + 1) / 2 - 1);
    for (size_t i = 0; i < sval.size(); ++i)
        lines->push_back(VSCat(i < sm    ? lpref
                               : i == sm ? cpref
                                         : rpref,
                               {std::string(1, sval[i])}));
    if (node->right)
        dump4(node->right, high,
              VSCat(rpref, high ? VS({ch_hor, " "}) : VS({ch_hor})),
              VSCat(rpref, high ? VS({ch_rddia, ch_ver}) : VS({ch_rddia})),
              VSCat(rpref, high ? VS({" ", " "}) : VS({" "})), false, false,
              lines);
    if (root) {
        VS out;
        for (size_t l = 0;; ++l) {
            bool last = true;
            std::string line;
            for (size_t i = 0; i < lines->size(); ++i)
                if (l < (*lines).at(i).size()) {
                    line += lines->at(i)[l];
                    last = false;
                } else
                    line += " ";
            if (last) break;
            out.push_back(line);
        }
        for (size_t i = 0; i < out.size(); ++i)
            std::cout << out[i] << std::endl;
    }
}
