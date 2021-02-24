#pragma once
#include <iostream>
#include <vector>

enum class Opcode : uint8_t {
    HLT = 0x00,
    ADD = 0x01,
    SUB = 0x02,
    STA = 0x03,
    LDA = 0x04,
    BRA = 0x05,
    BRZ = 0x06,
    BRP = 0x07,
    OUT = 0x08,
    OTC = 0x09,
    LDX = 0x0A,
    LDY = 0x0B,
    INX = 0x0C,
    INY = 0x0D,
};

enum class AddressingMode : uint8_t {
    Immediate = 0x00,
    Implied = 0x01,
    Absolute = 0x02,
    ZeroPage = 0x03,
    XIndexed = 0x04,
    YIndexed = 0x05,
    XIndexedAbsolute = 0x06,
    YIndexedAbsolute = 0x07,
    Registry = 0x08
};

enum class AddressingModeSize : uint8_t {
    NoBytes,
    SingleByte,
    DoubleByte
};

enum class TokenType : int {
    Instruction,
    Immediate,
    ZeroPage,
    Absolute,
    XIndexed,
    YIndexed,
    Label,
    LabelDeclaration,
    Comment,
    Register,
    Macro,
    MacroValue,
    Start,
    None,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    Token *nextToken = nullptr;
};

struct Value {
    AddressingMode mode;
    std::vector<uint8_t> value;
    TokenType tokenType;
};

struct Instruction {
    std::string label;
    std::string labelReference;
    Opcode opcode;
    Value value;
    uint16_t memoryAddress;
    Token *token;
};

struct LabelPointer {
    std::string label;
    uint16_t memoryLocation;
    Instruction *instruction;
};

struct ParserOutput {
    std::vector<Instruction *> instructionList;
    std::vector<LabelPointer *> labelList;
};