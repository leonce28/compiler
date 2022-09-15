/*
    Token.h
    =======
        Struct Token header.
*/

#pragma once

#include <string>
#include "TokenType.hpp"

namespace CMM
{

////////////////////////////////////////////////////////////////////////////////
// Using
////////////////////////////////////////////////////////////////////////////////

using std::string;


////////////////////////////////////////////////////////////////////////////////
// Class Token
////////////////////////////////////////////////////////////////////////////////

class Token
{
public:

    // Constructor
    explicit Token(TokenType tokenType, const string &tokenStr, int lineNo);


    // Getter: __tokenType
    TokenType tokenType() const;


    // Getter: __tokenStr
    string &tokenStr();
    const string &tokenStr() const;


    // Getter: __lineNo
    int lineNo() const;


private:

    // Attribute
    TokenType __tokenType;
    string __tokenStr;
    int __lineNo;
};


}  // End namespace CMM