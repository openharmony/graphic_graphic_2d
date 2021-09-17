/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FRAMEWORKS_WMSERVER_SRC_WMLAYOUT_SCSS_PARSER_SCANNER_H
#define FRAMEWORKS_WMSERVER_SRC_WMLAYOUT_SCSS_PARSER_SCANNER_H

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
    explicit Scanner(Driver &d);
    virtual ~Scanner() = default;

    // implement by YY_DECL
    virtual Parser::symbol_type nextToken();

private:
    Driver &driver;
};
} // namespace OHOS

#endif // FRAMEWORKS_WMSERVER_SRC_WMLAYOUT_SCSS_PARSER_SCANNER_H
