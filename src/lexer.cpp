#include "lexer.h"

#include <cctype>
#include <cstring>
#include "utils.h"

bool debugTokens = false;

std::vector<Lexer::RegexPattern> patterns;

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
    TokenType type;
    type = TokenType::Unknown;
    for (int i = 0; i < patterns.size(); i++) {
        Lexer::RegexPattern pattern = patterns[i];
        if (std::regex_match(string, pattern.pattern)) {
            type = pattern.type;
            break;
        }
    }
    t->type = type;
    t->value = string;
}

Token *Lexer::ReadLine(std::string currentLine, Token *firstToken) {
    Token *lastToken = firstToken;

    //currentLine.replace(currentLine.begin(), currentLine.end(), ",x", " ,x");
    findAndReplaceAll(currentLine, ",x", " ,x");

    int size = currentLine.length() + 1;
    char *cstr = new char[size];
    std::strcpy(cstr, currentLine.c_str());

    std::string temp;
    bool isMacro = false;
    for (int i = 0; i < size; i++) {
        // If current char is ; the rest of the line is a comment. Create Token of comment and exit loop.
        if (cstr[i] == ';') {
            Token *newToken = new Token;
            ParseToken(currentLine.substr(i), newToken);

            lastToken->nextToken = newToken;
            lastToken = newToken;
            break;
        }
        if (isMacro) {
            if ((i == size - 1) || cstr[i + 1] == ';') {
                Token *newToken = Lexer::Macro::ParseMacro(temp);
                lastToken->nextToken = newToken;
                lastToken = newToken->nextToken;

                isMacro = false;
                temp = "";
            } else {
                temp += cstr[i];
            }
        } else {
            if (isWhitespace(cstr[i]) == true) {
                if (temp.length() != 0) {
                    Token *newToken = new Token;

                    ParseToken(temp, newToken);

                    lastToken->nextToken = newToken;
                    lastToken = newToken;
                    temp.clear();
                }
                continue;
            } else if (cstr[i] == '.') {
                isMacro = true;
            } else {
                temp += cstr[i];
            }
        }
    }
    return lastToken;
}

void Lexer::LoadPatterns() {
    patterns.push_back(RegexPattern{std::regex("^[A-Z]{3}$"), TokenType::Instruction});
    patterns.push_back(RegexPattern{std::regex("^\\#\\$[0-9a-fA-F]{2}$"), TokenType::Immediate});
    patterns.push_back(RegexPattern{std::regex("^\\$[0-9a-fA-F]{2}$"), TokenType::ZeroPage});
    patterns.push_back(RegexPattern{std::regex("^\\$[0-9a-fA-F]{4}$"), TokenType::Absolute});
    patterns.push_back(RegexPattern{std::regex("^\\w{2,}$"), TokenType::Label});
    patterns.push_back(RegexPattern{std::regex("^\\w{2,}:$"), TokenType::LabelDeclaration});
    patterns.push_back(RegexPattern{std::regex("^;.*$"), TokenType::Comment});
    patterns.push_back(RegexPattern{std::regex("^\\w$"), TokenType::Register});

    patterns.push_back(RegexPattern{std::regex("^,x$"), TokenType::XIndexed});
    patterns.push_back(RegexPattern{std::regex("^,y$"), TokenType::YIndexed});

    patterns.push_back(RegexPattern{std::regex("^.\\w+$"), TokenType::Macro});
}

Token *Lexer::Lexer(std::string file) {
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

// Macro

Token *Lexer::Macro::ParseMacro(std::string line) {
    Token *firstToken = new Token;
    firstToken->type = TokenType::Macro;

    std::string macroType = "";

    int mode = 0;  // 0 = none, 1 = string, 2 = number
    std::string macroName;
    for (char c : line) {
        if (isWhitespace(c)) {
            macroType = macroName;
            break;
        } else {
            macroName += c;
        }
    }

    line.erase(0, macroName.length() + 1);
    firstToken->value = macroName;

    Token *macroValue = new Token;
    macroValue->value = line;
    macroValue->type = TokenType::MacroValue;

    firstToken->nextToken = macroValue;

    return firstToken;
}