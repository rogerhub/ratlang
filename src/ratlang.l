%option noyywrap

%{
#include <stdlib.h>
#include "ratlang.tab.h"

#define ATOF_FULL 1024
#define ATOF_L 1025
#define ATOF_R 1026
#define ATOF_NAKED 1027
double rat_atof (char const * c, int clen, int type);
%}

%%
[ \t\n]+          /* Throw away whitespace */
[0-9]+\.[0-9]+    yylval.dval = rat_atof (yytext, yyleng, ATOF_FULL); return NUM;
[0-9]+\.          yylval.dval = rat_atof (yytext, yyleng, ATOF_L); return NUM;
\.[0-9]+          yylval.dval = rat_atof (yytext, yyleng, ATOF_R); return NUM;
[0-9]+            yylval.dval = rat_atof (yytext, yyleng, ATOF_NAKED); return NUM;
[A-Z_]+           yylval.cpval = strdup (yytext); return ID;
[a-z]{2,}         yylval.cpval = strdup (yytext); return FUNCTION;
l                 return LET;
=                 return EQUAL;
s                 return SUM;
\+                return PLUS;
d                 return DIFFERENCE;
-                 return MINUS;
p                 return PRODUCT;
\*                return TIMES;
q                 return QUOTIENT;
\/                return SLASH;
r                 return REMAINDER;
%                 return MOD;
t                 return TOTHE;
\^                return POWER;
e                 return EXPONENTIATE;
\(                return LPAREN;
\)                return RPAREN;
\\                return PRINT;
;                 return SEMICOLON;

%%

double rat_atof (char const * c, int clen, int type) {
	if (type == ATOF_NAKED) {
		double result = 0.0;
		for (clen -= 1; clen >= 0; clen -= 1) {
			const char n = *(c + clen);
			result = (result + (n - '0')) / 10;
		}
		return result;
	} else {
		return atof (c);
	}
}
