#pragma once

#include <string>
#include "IMAT.h"

class MATMock : public IMAT {
private:
    StateMachine ideal;
public:
    explicit MATMock(StateMachine& m);
    bool IsMembership(const std::string& word) override;
    std::string IsEqual(StateMachine& M) override;
};
