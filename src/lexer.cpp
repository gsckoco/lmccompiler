#include "lexer.h"

#include <cctype>
#include <cstring>

bool debugTokens = false;

Lexer::RegexPattern patterns[8];

bool isWhitespace(char c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\n':
        case '\v':
        case '\f':
        case '\r':
            return true;
        default:
            return false;
    }
}

void Lexer::ParseToken(std::string string, Token *t) {
    //std::cout << string << "/" << std::endl;
    TokenType type;

    for (int i = 0; i < 8; i++) {
        Lexer::RegexPattern pattern = patterns[i];
        if (std::regex_match(string, pattern.pattern)) {
            //std::cout << "Value:" << string << " Type:" << (int)pattern.type + 1 << std::endl;
            type = pattern.type;
            break;
        }
    }
    t->type = type;
    t->value = string;
}

Lexer::Token* Lexer::ReadLine(std::string currentLine, Token *firstToken) {
    Token *lastToken = firstToken;

    int size = currentLine.length() + 1;
    char *cstr = new char[size];
    std::strcpy(cstr, currentLine.c_str());

    std::string temp;
    for (int i = 0; i < size; i++) {
        // If current char is ; the rest of the line is a comment. Create Token of comment and exit loop.
        if (cstr[i] == ';') {
            Token *newToken = new Token;
            ParseToken(currentLine.substr(i), newToken);

            lastToken->nextToken = newToken;
            lastToken = newToken;
            break;
        }
        if (isWhitespace(cstr[i]) == true ) {
            if (temp.length() != 0) {
                Token *newToken = new Token;

                ParseToken(temp, newToken);

                lastToken->nextToken = newToken;
                lastToken = newToken;
                temp.clear();
            }
            continue;
        } else {
            temp += cstr[i];
        }
    }
    return lastToken;
}

void Lexer::LoadPatterns() {
    patterns[0] = RegexPattern{std::regex("^[A-Z]{3}$"), TokenType::Instruction};
    patterns[1] = RegexPattern{std::regex("^\\#\\$[0-9a-fA-F]{2}$"), TokenType::Immediate};
    patterns[2] = RegexPattern{std::regex("^\\$[0-9a-fA-F]{2}$"), TokenType::ZeroPage};
    patterns[3] = RegexPattern{std::regex("^\\$[0-9a-fA-F]{4}$"), TokenType::Absolute};
    patterns[4] = RegexPattern{std::regex("^\\w{2,}$"), TokenType::Label};
    patterns[5] = RegexPattern{std::regex("^\\w{2,}:$"), TokenType::LabelDeclaration};
    patterns[6] = RegexPattern{std::regex("^;.*$"), TokenType::Comment};
    patterns[7] = RegexPattern{std::regex("^\\w$"), TokenType::Register};
}

Lexer::Token* Lexer::Lexer(std::string file) {
    LoadPatterns();

    std::ifstream input(file);
    std::string currentLine;
    Token *firstToken = new Token;
    firstToken->type = TokenType::Start;
    Token *lastToken = firstToken;

    while (getline(input, currentLine)) {
        lastToken = ReadLine(currentLine + " ", lastToken);
    }

    Token *currentToken = firstToken;
    while (currentToken != nullptr) {
        if (debugTokens) {
            std::cout << "{Value: '" << currentToken->value << "', Type: " << (int)currentToken->type << "}" << std::endl;
        }
        currentToken = currentToken->nextToken;
    }

    input.close();
    return firstToken;
}