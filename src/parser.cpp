#include "parser.h"
#include "utils.h"

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
    } else if (input == "LDX") {
        return Opcode::LDX;
    } else if (input == "LDY") {
        return Opcode::LDY;
    } else if (input == "INX") {
        return Opcode::INX;
    } else if (input == "INY") {
        return Opcode::INY;
    }
    return Opcode::HLT;
}

bool IsInstructionValue(TokenType type) {
    switch (type) {
        case TokenType::Immediate:
        case TokenType::ZeroPage:
        case TokenType::Absolute:
        case TokenType::Register:
        case TokenType::XIndexed:
        case TokenType::YIndexed:
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

ParserOutput* Parser::ParseTokens(Token* token) {
    Token* currentToken = token;
    std::string currentLabel = "";

    std::vector<Instruction*> instructionList;
    std::vector<LabelPointer*> labelList;

    while (currentToken != nullptr) {
        if (currentToken->type == TokenType::Instruction) {
            Instruction* newInstruction = new Instruction;
            newInstruction->label = currentLabel;
            newInstruction->opcode = OpcodeFromName(currentToken->value);
            newInstruction->token = currentToken;
            if (currentToken->nextToken != nullptr) {
                //std::cout << currentToken->value << "/" << (int)currentToken->nextToken->type << "/" << std::hex << (int)newInstruction->opcode << std::endl;
                if (IsInstructionValue(currentToken->nextToken->type)) {
                    AddressingMode addressMode = GetAddressingMode(currentToken->nextToken->type);

                    //std::cout << currentToken->value << "/" << currentToken->nextToken->value << "/" << currentToken->nextToken->nextToken->value << std::endl;

                    newInstruction->value.mode = addressMode;

                    std::stringstream hex2num;
                    std::smatch regResults;
                    std::regex_search(currentToken->nextToken->value, regResults, std::regex("[0-9a-fA-F]{2,}"));
                    int v;
                    hex2num << std::hex << regResults.str();
                    hex2num >> v;

                    AddressingModeSize size = GetSizeFromAddressingMode(addressMode);
                    if (size == AddressingModeSize::DoubleByte) {
                        newInstruction->value.value.push_back((uint8_t)(v >> 8) & 0xFF);
                        newInstruction->value.value.push_back((uint8_t)(v) & 0xFF);
                    } else if (size == AddressingModeSize::SingleByte) {
                        newInstruction->value.value.push_back((uint8_t)(v) & 0xFF);
                    }
                    //currentInstruction->value.value.push_back((uint8_t)(labelPointers[a]->memoryLocation >> 8) & 0xFF);
                    //currentInstruction->value.value.push_back((uint8_t)(labelPointers[a]->memoryLocation) & 0xFF);
                } else if (currentToken->nextToken->type == TokenType::Label) {
                    LabelPointer* pointer = new LabelPointer;
                    newInstruction->value.mode = AddressingMode::Absolute;
                    if (currentToken->nextToken->nextToken != nullptr) {
                        if (currentToken->nextToken->nextToken->type == TokenType::XIndexed) {
                            newInstruction->value.mode = AddressingMode::XIndexedAbsolute;
                        } else if (currentToken->nextToken->nextToken->type == TokenType::YIndexed) {
                            {
                                newInstruction->value.mode = AddressingMode::YIndexedAbsolute;
                            }
                        }
                        newInstruction->labelReference = currentToken->nextToken->value;
                        pointer->instruction = newInstruction;
                        pointer->label = currentToken->nextToken->value;
                        labelList.push_back(pointer);
                    }

                    newInstruction->value.tokenType = currentToken->nextToken->type;
                } else {
                    newInstruction->value.tokenType = TokenType::None;
                    newInstruction->value.mode = AddressingMode::Implied;
                }

                currentLabel = "";
                instructionList.push_back(newInstruction);
            }
        } else if (currentToken->type == TokenType::Macro) {
            Instruction* newInstruction = new Instruction;
            newInstruction->token = currentToken;
            newInstruction->label = currentLabel;

            newInstruction->opcode = (Opcode)0x00;
            newInstruction->value.mode = (AddressingMode)0x00;

            instructionList.push_back(newInstruction);
        } else if (currentToken->type == TokenType::LabelDeclaration) {
            std::string label = currentToken->value;
            currentLabel = label.substr(0, label.length() - 1);
            //std::cout << label << " LABEL MADE" << std::endl;
        }
        currentToken = currentToken->nextToken;
    }

    return new ParserOutput{
        instructionList,
        labelList};
}