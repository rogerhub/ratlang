%option noyywrap

%{
#include <stdlib.h>
#include "node.h"
#include "parser.h"
#include "runtime.h"

#define ATOF_FULL 1024
#define ATOF_L 1025
#define ATOF_R 1026
#define ATOF_NAKED 1027
double rat_atof (char const * c, int clen, int type);
%}

%%
\"([^\"]|\\\")+\" yylval.cpval = strdup (yytext); return STRING;
[ \t\n]+          /* Throw away whitespace */
[0-9]+\.[0-9]+    yylval.dval = rat_atof (yytext, yyleng, ATOF_FULL); return NUM;
[0-9]+\.          yylval.dval = rat_atof (yytext, yyleng, ATOF_L); return NUM;
\.[0-9]+          yylval.dval = rat_atof (yytext, yyleng, ATOF_R); return NUM;
[0-9]+            yylval.dval = rat_atof (yytext, yyleng, ATOF_NAKED); return NUM;
[A-Z_]+           yylval.cpval = strdup (yytext); return ID;
[a-z]{2,}         yylval.cpval = strdup (yytext); return FUNCTION_NAME;
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
t                 return RAISE;
\^                return POWER;
e                 return EXPONENTIATE;
\(                return LPAREN;
\)                return RPAREN;
\{                return LCURLY;
\}                return RCURLY;
\\                return PRINT;
;                 return SEMICOLON;
,                 return COMMA;
f                 return FUNCTION;

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

int main(int argc, char** argv) {

	/** Initialize persistent environment */
	runtime_global_env = (Env*) malloc (sizeof (Env));
	env_init (runtime_global_env);

	char* k;

	while (k = readline ("rat> ")) {

	add_history (k);
	YY_BUFFER_STATE b = yy_scan_string (k);
	yy_switch_to_buffer (b);
    yyparse ();

	}

    return 0;
}
