#ifndef FLEX_BISON_DRIVER_H
#define FLEX_BISON_DRIVER_H

#include <iostream>
#include <map>
#include <string>

#include "scanner.h"
#include "parser.hpp"

namespace OHOS {
class Driver {
public:
    Driver();
    virtual ~Driver() = default;

    int parse(std::istream &infile);

    struct CSSBlock {
        std::map<std::string, std::string> declares;
        std::map<std::string, struct CSSBlock> blocks;
        struct CSSBlock *parent;
    } global;
    struct CSSBlock *current = &global;

private:
    Parser parser;
    Scanner scanner;
};
} // namespace OHOS
#endif //FLEX_BISON_DRIVER_H
