#ifndef FLEX_BISON_SCANNER_H
#define FLEX_BISON_SCANNER_H

#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer OHOSFlexLexer

#include <FlexLexer.h>

#endif // yyFlexLexerOnce

#undef YY_DECL
#define YY_DECL OHOS::Parser::symbol_type OHOS::Scanner::nextToken()

#include "parser.hpp"

namespace OHOS {
class Driver;
class Scanner : public yyFlexLexer {
public:
    Scanner(Driver &d);
    virtual ~Scanner() = default;

    // implement by YY_DECL
    virtual Parser::symbol_type nextToken();

private:
    Driver &driver;
};
} // namespace OHOS


#endif //FLEX_BISON_SCANNER_H
