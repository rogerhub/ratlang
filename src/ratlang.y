%{
    #include <stdio.h>
    #include "node.h"
    int yylex (void);
    void yyerror (char const *);
%}

%union {
    Node* npval;
    double dval;
    char* cpval;
}

%output "parser.c"
%defines "parser.h"

%token <dval> NUM
%token <cpval> ID
%token <cpval> FUNCTION
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
%token RAISE
%token POWER
%token EXPONENTIATE
%token LPAREN
%token RPAREN
%token PRINT
%token SEMICOLON

%left PLUS MINUS
%left TIMES SLASH
%left MOD
%right POWER
%left SUM DIFFERENCE
%left PRODUCT QUOTIENT
%left REMAINDER
%right RAISE
%right EXPONENTIATE

// %type <npval> program;
%type <npval> stmts;
%type <npval> stmt;
%type <npval> assign_stmt;
%type <npval> print_stmt;
%type <npval> expression_stmt;
%type <npval> expression;
%type <npval> expr1;
%type <npval> expr2;
%type <npval> subexpression;
%type <npval> call;
%type <npval> atom;
%type <npval> target;
%type <npval> number;
%type <npval> identifier;
%type <npval> function;

%%

program : stmts {
            Env e;
            env_init (&e);
            node_calculate_value ($1, &e);
            printf ("program value: %10.10f\n", value_get ( node_value ($1)) ); }
        ;
stmts : stmt stmts { node_append_child ($2, $1); $$ = $2; }
      | /* empty */ { $$ = node_from_token (SEMICOLON); }
      ;
stmt : assign_stmt
     | print_stmt
     | expression_stmt
     ;
assign_stmt : LET target EQUAL expression SEMICOLON { $$ = node_from_token_c2 (LET, $2, $4); }
            ;
print_stmt : PRINT expression SEMICOLON { $$ = node_from_token_c1 (PRINT, $2); }
           ;
expression_stmt : expression SEMICOLON { $$ = $1; }
                ;
expression : expr1
           ;
expr1 : expr2

      /** Normal operators */
      | expr1 PLUS expr1 { $$ = node_from_token_c2 (SUM , $1, $3); }
      | expr1 MINUS expr1 { $$ = node_from_token_c2 (DIFFERENCE , $1, $3); }
      | expr1 TIMES expr1 { $$ = node_from_token_c2 (PRODUCT , $1, $3); }
      | expr1 SLASH expr1 { $$ = node_from_token_c2 (QUOTIENT , $1, $3); }
      | expr1 MOD expr1 { $$ = node_from_token_c2 (REMAINDER , $1, $3); }
      | expr1 POWER expr1 { $$ = node_from_token_c2 (RAISE , $1, $3); }

      /** Alternate operators */
      | expr1 SUM expr1 { $$ = node_from_token_c2 (SUM , $1, $3); }
      | expr1 DIFFERENCE expr1 { $$ = node_from_token_c2 (DIFFERENCE , $1, $3); }
      | expr1 PRODUCT expr1 { $$ = node_from_token_c2 (PRODUCT , $1, $3); }
      | expr1 QUOTIENT expr1 { $$ = node_from_token_c2 (QUOTIENT , $1, $3); }
      | expr1 REMAINDER expr1 { $$ = node_from_token_c2 (REMAINDER , $1, $3); }
      | expr1 RAISE expr1 { $$ = node_from_token_c2 (RAISE , $1, $3); }

      /** Custom exponentiation operator */
      | expr1 EXPONENTIATE expr1 { $$ = node_from_token_c2 (EXPONENTIATE , $1, $3); }
      ;
expr2 : subexpression
      | MINUS expr2 { $$ = node_from_token_c1 (MINUS, $2); }
      ;
subexpression : call
              | LPAREN expression RPAREN { $$ = $2; }
              ;
call : atom
     | function LPAREN expression RPAREN { $$ = node_from_token_c2 (LPAREN, $1, $3); }
     ;
atom : number
     | identifier
     ;
target : identifier
       ;
number : NUM { $$ = node_from_double ($1); }
       ;
identifier : ID { $$ = node_from_identifier ($1); }
           ;
function : FUNCTION { $$ = node_from_identifier ($1); }
         ;

%%
void yyerror (char const * c) {
    printf ("%s\n", c);
}
