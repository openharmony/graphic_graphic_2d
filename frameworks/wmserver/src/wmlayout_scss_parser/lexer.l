%{
#include <iostream>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>

#include "parser.hpp"
#include "scanner.h"
#include "location.hh"

static OHOS::location loc;
#define YY_USER_ACTION       \
    do {                     \
        loc.columns(yyleng); \
    } while (0);

#undef yywrap
#define yywrap() 1
%}
%option c++
%option noyywrap debug
%option yyclass="Scanner"
%option prefix="OHOS"

%x COMMENT

/* basic char */
digit            ([0-9])
alpha            ([A-Za-z_])
id_all           (({alpha}|{digit}|-)*)
blanks           ([ \t]*)

/* regular word */
number           ({digit}+([.]{digit}+)?)
identifier       ({alpha}{id_all})

/* selector */
type_selector    (#{identifier})
mode_selector    (&:(free|full))

/* attribute */
layout_attribute (left|top|width|height)
align_attribute  (horizon-align|vertical-align)
other_attribute  (z-index|position)

/* value */
number_value     ({number}(%|px)?)
h_align_value    (left|middle|right)
v_align_value    (top|middle|bottom)
position_value   (relative|fixed|static)

/* export for parser.y */
selector         ({type_selector}|{mode_selector})
attribute        (({layout_attribute}|{align_attribute}|{other_attribute}):)
value            ((initial|{number_value}|{h_align_value}|{v_align_value}|{position_value});)

%%

%{
    loc.step(); // for C++
%}

"//" {
    BEGIN COMMENT;
}

"{" {
    return OHOS::Parser::make_CHAR_L_BRACE(loc);
}

"}" {
    return OHOS::Parser::make_CHAR_R_BRACE(loc);
}

{selector} {
    printf("selector: %s\n", yytext);
    return OHOS::Parser::make_SELECTOR(yytext, loc);
}

{attribute} {
    printf("attribute: %s\n", yytext);
    return OHOS::Parser::make_ATTRIBUTE(yytext, loc);
}

{value} {
    printf("value: %s\n", yytext);
    return OHOS::Parser::make_VALUE(yytext, loc);
}

"\n" {
}

. {
    if (yytext[0] != ' ') {
        printf("ignore '%s'\n", yytext);
    }
}

<COMMENT>\n {
    BEGIN INITIAL;
}

<COMMENT>. {
}

<<EOF>> {
    return OHOS::Parser::make_END(loc);
}

%%
