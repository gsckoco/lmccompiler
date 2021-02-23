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
            std::cout << "TL " << std::hex << (int)instruction << std::endl;
            tokensReferencingLabels.push_back(currentInstruction);
        }

        if (!currentInstruction->label.empty()) {
            for (int a = 0; a < labelPointers.size(); a++) {
                if (labelPointers[a]->label == currentInstruction->label) {
                    labelPointers[a]->memoryLocation = currentInstructionMemoryAddress;
                    std::cout << "t" << currentInstruction->label << " Code:" << std::hex << (int)currentInstruction->opcode << " Mem:" << currentInstructionMemoryAddress << std::endl;
                    a = labelPointers.size();  // BReak out of only this loop
                }
            }
        }

        currentInstructionMemoryAddress += 1 + (int)GetSizeFromAddressingMode(currentInstruction->value.mode);
    }
    for (int i = 0; i < tokensReferencingLabels.size(); i++) {
        Instruction* currentInstruction = tokensReferencingLabels[i];
        for (int a = 0; a < labelPointers.size(); a++) {
            std::cout << labelPointers[a]->label << "/ " << currentInstruction->labelReference << " / " << labelPointers.size() << std::endl;
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
        if (GetSizeFromAddressingMode(currentInstruction->value.mode) == AddressingModeSize::DoubleByte) {
            //outputFile << currentInstruction->value.value;
            char out[2] = { currentInstruction->value.value >> 8, currentInstruction->value.value & 0xff };
            outputFile.write(out, 2);
        } else if (GetSizeFromAddressingMode(currentInstruction->value.mode) == AddressingModeSize::SingleByte) {
            //outputFile << (uint8_t)currentInstruction->value.value;
            char out[1] = {(uint8_t)currentInstruction->value.value};
            outputFile.write(out, 1);
        }
    }
    outputFile.close();
}