%{
    #include <stdio.h>
    int yylex (void);
    void yyerror (char const *);
%}

%token NUM
%token ID
%token FUNCTION
%token LET
%token EQUAL
%token SUM
%token PLUS
%token DIFFERENCE
%token MINUS
%token PRODUCT
%token TIMES
%token QUOTIENT
%token SLASH
%token REMAINDER
%token MOD
%token TOTHE
%token POWER
%token EXPONENTIATE
%token LPAREN
%token RPAREN
%token PRINT
%token SEMICOLON

%union {
    int ival;
    double dval;
    char* cpval;
}

%type <dval> number
%type <cpval> identifier
%type <cpval> function

%left PLUS MINUS
%left TIMES SLASH
%left MOD
%right POWER
%left SUM DIFFERENCE
%left PRODUCT QUOTIENT
%left REMAINDER
%right TOTHE
%right EXPONENTIATE

%%

program : stmts
        ;
stmts : stmt stmts
      | /* empty */
      ;
stmt : assign_stmt
     | print_stmt
     | expression_stmt
     ;
assign_stmt : LET target EQUAL expression SEMICOLON;
            ;
print_stmt : PRINT expression SEMICOLON
           ;
expression_stmt : expression SEMICOLON { printf ("%10.10f\n", $<dval>1); }
                ;
expression : expr1
           ;
expr1 : expr2

      /** Normal operators */
      | expr1 PLUS expr1
      | expr1 MINUS expr1
      | expr1 TIMES expr1
      | expr1 SLASH expr1
      | expr1 MOD expr1
      | expr1 POWER expr1

      /** Alternate operators */
      | expr1 SUM expr1
      | expr1 DIFFERENCE expr1
      | expr1 PRODUCT expr1
      | expr1 QUOTIENT expr1
      | expr1 REMAINDER expr1
      | expr1 TOTHE expr1

      /** Custom exponentiation operator */
      | expr1 EXPONENTIATE expr1
      ;
expr2 : subexpression
      | MINUS expr2
      ;
subexpression : call
              | LPAREN expression RPAREN
              ;
call : atom
     | function LPAREN expression RPAREN
     ;
atom : number
     | identifier
     ;
target : identifier
       ;
number : NUM { $$ = yylval.dval; }
       ;
identifier : ID { $$ = yylval.cpval; }
           ;
function : FUNCTION { $$ = yylval.cpval; }
         ;

%%
void yyerror (char const * c) {
    printf ("%s\n", c);
}

// FOR TESTING ONLY
#include "env.h"

int main(int argc, char** argv) {
    yyparse ();
    return 0;
}
