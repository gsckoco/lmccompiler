#include <iostream>
#include "compiler.h"
#include "parser.h"
#include "lexer.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "Not enough arguments." << std::endl;
        return 0;
    }

    std::string inputFile(argv[1]);
    std::cout << "Reading from file: " << inputFile << std::endl;
    Lexer::Token* lexTokens = Lexer::Lexer(inputFile);
    ParserOutput* tokens = Parser::ParseTokens(lexTokens);

    std::string outputFile = "asm.out";

    if (argv[2] != nullptr) {
        outputFile = argv[2];
    }

    Compiler::Compile(outputFile, tokens);
}