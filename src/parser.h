/*

{Value: '', Type: 7}
{Value: 'main:', Type: 5}
{Value: 'LDA', Type: 0}
{Value: '#$0A', Type: 1}
{Value: '; Loads 10 in to accumulator ', Type: 6}
{Value: 'loop:', Type: 5}
{Value: 'OUT', Type: 0}
{Value: 'A', Type: 4}
{Value: '; Outputs the accumulator's value ', Type: 6}
{Value: 'OTC', Type: 0}
{Value: '#$0A', Type: 1}
{Value: '; Outputs newline ASCII character ', Type: 6}
{Value: 'SUB', Type: 0}
{Value: '#$01', Type: 1}
{Value: '; Subtracts one ', Type: 6}
{Value: 'BRP', Type: 0}
{Value: 'loop', Type: 4}
{Value: '; Go back to label loop ', Type: 6}
{Value: 'OUT', Type: 0}
{Value: '#$7F', Type: 1}
{Value: '; Output 127 to signify finished ', Type: 6}

*/

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "structs.h"

#include "lexer.h"

namespace Parser {
    Opcode OpcodeFromName(std::string input);
    AddressingMode GetAddressingMode(TokenType type);
    ParserOutput* ParseTokens(Token* token);
}  // namespace Parser

#endif  // !PARSER_H