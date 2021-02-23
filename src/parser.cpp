#include "parser.h"

/*

enum class TokenType {
    Instruction,
    Immediate,
    ZeroPage,
    Absolute,
    Label,
    LabelDeclaration,
    Comment,
    Start
};

*/

Opcode Parser::OpcodeFromName(std::string input) {
    if (input == "HLT") {
        return Opcode::HLT;
    } else if (input == "ADD") {
        return Opcode::ADD;
    } else if (input == "SUB") {
        return Opcode::SUB;
    } else if (input == "STA") {
        return Opcode::STA;
    } else if (input == "LDA") {
        return Opcode::LDA;
    } else if (input == "BRA") {
        return Opcode::BRA;
    } else if (input == "BRZ") {
        return Opcode::BRZ;
    } else if (input == "BRP") {
        return Opcode::BRP;
    } else if (input == "OUT") {
        return Opcode::OUT;
    } else if (input == "OTC") {
        return Opcode::OTC;
    }
    return Opcode::HLT;
}

/*
enum class TokenType {
    Instruction,
    Immediate,
    ZeroPage,
    Absolute,
    Label,
    LabelDeclaration,
    Comment,
    Start
};

if (currentToken->type == TokenType::LabelDeclaration) {
*/

/*

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

*/

bool IsInstructionValue(TokenType type) {
    switch (type) {
        case TokenType::Immediate:
        case TokenType::ZeroPage:
        case TokenType::Absolute:
        case TokenType::Register:
            return true;
        default:
            return false;
    }
}

AddressingMode Parser::GetAddressingMode(TokenType type) {
    switch (type) {
        case TokenType::Immediate:
            return AddressingMode::Immediate;
        case TokenType::ZeroPage:
            return AddressingMode::ZeroPage;
        case TokenType::Absolute:
            return AddressingMode::Absolute;
        case TokenType::Register:
            return AddressingMode::Registry;

        default:
            return AddressingMode::Immediate;
    }
}

ParserOutput* Parser::ParseTokens(Lexer::Token* token) {
    Lexer::Token* currentToken = token;
    std::string currentLabel = "";

    std::vector<Instruction*> instructionList;
    std::vector<LabelPointer*> labelList;

    while (currentToken != nullptr) {
        if (currentToken->type == TokenType::Instruction) {
            Instruction* newInstruction = new Instruction;
            newInstruction->label = currentLabel;
            newInstruction->opcode = OpcodeFromName(currentToken->value);
            if (currentToken->nextToken != nullptr) {
                if (IsInstructionValue(currentToken->nextToken->type)) {
                    newInstruction->value.mode = GetAddressingMode(currentToken->nextToken->type);
                    std::stringstream hex2num;
                    std::smatch regResults;
                    std::regex_search(currentToken->nextToken->value, regResults, std::regex("[0-9a-fA-F]{2,}"));
                    hex2num << std::hex << regResults.str();
                    hex2num >> newInstruction->value.value;
                } else if (currentToken->nextToken->type == TokenType::Label) {
                    LabelPointer* pointer = new LabelPointer;
                    newInstruction->value.mode = AddressingMode::Absolute;
                    newInstruction->labelReference = currentToken->nextToken->value;
                    pointer->instruction = newInstruction;
                    pointer->label = currentToken->nextToken->value;
                    labelList.push_back(pointer);
                }

                newInstruction->value.tokenType = currentToken->nextToken->type;
            } else {
                newInstruction->value.tokenType = TokenType::None;
                newInstruction->value.value = 0;
                newInstruction->value.mode = AddressingMode::Implied;
            }

            currentLabel = "";
            instructionList.push_back(newInstruction);
        } else if (currentToken->type == TokenType::LabelDeclaration) {
            std::string label = currentToken->value;
            currentLabel = label.substr(0, label.length() - 1);
        }
        currentToken = currentToken->nextToken;
    }

    return new ParserOutput{
        instructionList,
        labelList};
}