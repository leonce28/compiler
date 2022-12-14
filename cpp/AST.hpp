/*
    AST.hpp
    =======
        Struct __AST implementation.
*/

#pragma once

#include <string>
#include <vector>
#include "AST.h"
#include "TokenType.hpp"
#include "Token.h"

namespace CMM
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Using
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using std::string;
using std::vector;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__AST::__AST(__TokenType tokenType, const string &tokenStr, const vector<__AST *> &subList, int lineNo):
    __tokenType(tokenType),
    __tokenStr (tokenStr),
    __subList  (subList),
    __lineNo   (lineNo) {}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor (With tokenPtr)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__AST::__AST(const __Token *tokenPtr):
    __tokenType(tokenPtr->__tokenType),
    __tokenStr (tokenPtr->__tokenStr),
    __lineNo   (tokenPtr->__lineNo) {}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__AST::~__AST()
{
    for (auto subPtr: __subList)
    {
        delete subPtr;
    }
}


}  // End namespace CMM
