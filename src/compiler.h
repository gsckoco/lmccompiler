#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <string>
#include <vector>
#include "structs.h"

namespace Compiler {
    void Compile(std::string output, ParserOutput* tokens);
}  // namespace Compiler

#endif  // !COMPILER_H