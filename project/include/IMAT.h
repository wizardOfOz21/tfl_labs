#pragma once

#include <string>
#include "StateMachine.h"

class IMAT {
public:
    virtual bool IsMembership(const std::string& word)=0;
    virtual std::string IsEqual(StateMachine& M) = 0;
    virtual ~IMAT() = default;
};
