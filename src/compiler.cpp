#include "compiler.h"

#include <fstream>

AddressingModeSize Compiler::GetSizeFromAddressingMode(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::Implied:
            return AddressingModeSize::NoBytes;
        default:
        case AddressingMode::Immediate:
        case AddressingMode::ZeroPage:
        case AddressingMode::XIndexed:
        case AddressingMode::YIndexed:
        case AddressingMode::Registry:
            return AddressingModeSize::SingleByte;

        case AddressingMode::Absolute:
        case AddressingMode::XIndexedAbsolute:
        case AddressingMode::YIndexedAbsolute:
            return AddressingModeSize::DoubleByte;
    }
}

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
                currentInstruction->value.value = labelPointers[a]->memoryLocation;
                a = labelPointers.size();  // Break out of only this loop
            }
        }
    };
    std::ofstream outputFile(output, std::ios::binary);
    for (int i = 0; i < tokens.size(); i++) {
        Instruction* currentInstruction = tokens[i];
        uint8_t instruction = (uint8_t)currentInstruction->opcode << 4;
        instruction += (uint8_t)currentInstruction->value.mode;
        outputFile << instruction;

        if (currentInstruction->value.tokenType != TokenType::None) {
            if (GetSizeFromAddressingMode(currentInstruction->value.mode) == AddressingModeSize::DoubleByte) {
                char out[2] = {(char)((currentInstruction->value.value >> 8) & 0xFF), (char)(currentInstruction->value.value & 0xFF)};
                outputFile.write(out, 2);
            } else if (GetSizeFromAddressingMode(currentInstruction->value.mode) == AddressingModeSize::SingleByte) {
                char out[1] = {(char)(currentInstruction->value.value)};
                outputFile.write(out, 1);
            }
        }
    }
    outputFile.close();
}