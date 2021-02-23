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
    OTC = 0x09
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

enum class TokenType {
    Instruction,
    Immediate,
    ZeroPage,
    Absolute,
    Label,
    LabelDeclaration,
    Comment,
    Register,
    Start
};

struct Value {
    AddressingMode mode;
    uint16_t value;
    TokenType tokenType;
};

struct Instruction {
    std::string label;
    std::string labelReference;
    Opcode opcode;
    Value value;
    uint16_t memoryAddress;
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