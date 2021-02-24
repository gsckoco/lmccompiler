#include "compiler.h"

#include <fstream>
#include <regex>

#include "utils.h"

void Compiler::Compile(std::string output, ParserOutput* parserOutput) {
    // Instruction  0xX0 Opcode
    //              0x0X Addressing Mode
    std::vector<Instruction*> tokens = parserOutput->instructionList;
    std::vector<LabelPointer*> labelPointers = parserOutput->labelList;

    std::vector<Instruction*> tokensReferencingLabels;

    uint16_t currentInstructionMemoryAddress = 0;
    for (int i = 0; i < tokens.size(); i++) {
        Instruction* currentInstruction = tokens[i];
        uint8_t instruction = (uint8_t)currentInstruction->opcode << 4;
        instruction += (uint8_t)currentInstruction->value.mode;

        currentInstruction->memoryAddress = currentInstructionMemoryAddress;

        if (currentInstruction->value.tokenType == TokenType::Label) {
            tokensReferencingLabels.push_back(currentInstruction);
        } else if (currentInstruction->token->type == TokenType::Macro) {
            std::vector<std::string> splitStr = split(currentInstruction->token->nextToken->value, ",");
            std::smatch match;
            for (auto out : splitStr) {
                if (std::regex_search(out, match, std::regex("\"(?:[^\\\\\"]|\\\\.)*\""))) {
                    auto string = match.str();
                    string.erase(0, 1);
                    string.erase(string.size() - 1);
                    std::smatch escapeMatch;
                    while (std::regex_search(string, escapeMatch, std::regex("\\\\."))) {
                        string.erase(escapeMatch.position(0), 1);
                    }

                    for (auto c : string) {
                        currentInstruction->value.value.push_back((uint8_t)c);
                    }
                } else if (std::regex_search(out, match, std::regex("[0-9a-fA-F]{2,}"))) {
                    std::stringstream hex2num;

                    hex2num << std::hex << match.str();
                    int t;
                    hex2num >> t;
                    currentInstruction->value.value.push_back(t);
                }
            }
        }

        if (!currentInstruction->label.empty()) {
            for (int a = 0; a < labelPointers.size(); a++) {
                if (labelPointers[a]->label == currentInstruction->label) {
                    labelPointers[a]->memoryLocation = currentInstructionMemoryAddress;
                    a = labelPointers.size();  // Break out of only this loop
                }
            }
        }

        currentInstructionMemoryAddress += 1 + (int)GetSizeFromAddressingMode(currentInstruction->value.mode);
    }
    for (int i = 0; i < tokensReferencingLabels.size(); i++) {
        Instruction* currentInstruction = tokensReferencingLabels[i];

        for (int a = 0; a < labelPointers.size(); a++) {
            if (labelPointers[a]->label == currentInstruction->labelReference) {
                //labelPointers[a]->memoryLocation
                //TODO
                currentInstruction->value.value.push_back((uint8_t)(labelPointers[a]->memoryLocation >> 8) & 0xFF);
                currentInstruction->value.value.push_back((uint8_t)(labelPointers[a]->memoryLocation) & 0xFF);
                a = labelPointers.size();  // Break out of only this loop
            }
        }
    };
    std::ofstream outputFile(output, std::ios::binary);
    for (int i = 0; i < tokens.size(); i++) {
        Instruction* currentInstruction = tokens[i];

        if (currentInstruction->token->type == TokenType::Macro) {

        } else {
            uint8_t instruction = (uint8_t)currentInstruction->opcode << 4;
            instruction += (uint8_t)currentInstruction->value.mode;
            outputFile << instruction;
        }
        for (uint8_t byte : currentInstruction->value.value) {
            outputFile << (char)byte;

        }
    }
    outputFile.close();
}