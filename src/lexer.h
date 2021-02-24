
/*

main:
    LDA #0x0A   ; Loads 10 in to accumulator
loop:
    OUT A       ; Outputs the accumulator's value
    OTC #0x0A   ; Outputs newline ASCII character
    SUB #0x01   ; Subtracts one
    BRP `loop`  ; Go back to label loop
    OUT #0x7F   ; Output 127 to signify finished

ADD     = Instruction
#0x0A   = Number
0x0A    = ZeroPage Address
0xFFFF  = Absolute Address
`main`  = Label

*/
#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <iostream>
#include <regex>

#include "structs.h"

/*

Instruction         ^[A-Z]{3}$
Immediate           ^\#\$[0-9a-fA-F]{2}$
ZeroPage            ^\$[0-9a-fA-F]{2}$
Absolute            ^\$[0-9a-fA-F]{4}$
Label               ^\w*$
LabelDeclaration    ^\w*:$  -- Check that size of label declaration is greater than 1. Size of 1 means only ':' is present
Comment             ^;.*$
Register            ^\w$

*/

extern bool debugTokens;

namespace Lexer {

    struct RegexPattern {
        std::regex pattern;
        TokenType type;
    };

    void ParseToken(std::string string, Token *t);
    Token* ReadLine(std::string string, Token *firstToken);
    void LoadPatterns();
    Token* Lexer(std::string file);

    namespace Macro {
        Token* ParseMacro(std::string line);
    }
}  // namespace lexer


#endif // !LEXER_H