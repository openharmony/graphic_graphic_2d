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

%skeleton "lalr1.cc"
%require "3.0.4"
%define api.namespace {OHOS}
%define parser_class_name {Parser}
%define api.token.constructor
%define api.value.type variant
%define api.token.prefix {TOKEN_}
%define parse.assert
%defines
%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>
    #include <cmath>
    using namespace std;

    namespace OHOS {
        class Scanner;
        class Driver;
    }
}

%code top
{
    #include <iostream>
    #include "scanner.h"
    #include "parser.hpp"
    #include "driver.h"
    #include "location.hh"

    static OHOS::Parser::symbol_type yylex(OHOS::Scanner& scanner,OHOS::Driver &driver)
    {
        return scanner.nextToken();
    }
    using namespace OHOS;
}

%lex-param {OHOS::Scanner& scanner}
%lex-param {OHOS::Driver& driver}
%parse-param {OHOS::Scanner& scanner}
%parse-param {OHOS::Driver& driver}

%locations

%token END 0

/* import from lexer.l */
%token<string> SELECTOR
%token<string> ATTRIBUTE
%token<string> VALUE
%token CHAR_L_BRACE              /* ( */
%token CHAR_R_BRACE              /* ) */

/* non-terminate-symbol */
%start statements
%type<string> css_block css_block_begin css_block_end
%type<string> declare

%%

statements: css_block statements
{
}

statements: declare statements
{
}

statements: %empty
{
}

css_block: css_block_begin css_block_end
{
}

css_block_begin: SELECTOR CHAR_L_BRACE
{
    std::string selector;
    if ($1.substr(0, 1) == "#") {
        selector = $1.substr(1);
    }
    if ($1.substr(0, 2) == "&:") {
        selector = $1.substr(2);
    }
    driver.current->blocks[selector].parent = driver.current;
    driver.current = &driver.current->blocks[selector];
}

css_block_end: statements CHAR_R_BRACE
{
    driver.current = driver.current->parent;
}

declare: ATTRIBUTE VALUE
{
    $$ = $1 + $2;
    std::string attribute = $1;
    std::string value = $2;

    // remove end :
    attribute = attribute.substr(0, attribute.length() - 1);

    // remove end ;
    value = value.substr(0, value.length() - 1);
    driver.current->declares[attribute] = value;
}

%%

void OHOS::Parser::error(const OHOS::location& location,const std::string& message)
{
    for (int i = 0; i < 5; i++) {
        fprintf(stderr, "wmlayout scss parse error: (%d,%d) - (%d,%d): %s\n",
                location.begin.line, location.begin.column,
                location.end.line, location.end.column,
                message.c_str());
    }
}
