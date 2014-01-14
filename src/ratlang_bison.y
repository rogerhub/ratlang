%{
    #include <stdio.h>
    #include "node.h"
    #include "runtime.h"
    #define BISON_ERROR_FORMAT " ~ %s\n"
    #define BISON_ERROR_OVERFLOW_FORMAT " ~ ErrorReporting: %d more errors suppressed\n"
    int yylex (void);
    void runtime_error_print ();
    void yyerror (char const *);
%}

%union {
    Node* npval;
    double dval;
    char* cpval;
}

%token <dval> NUM
%token <cpval> ID
%token <cpval> FUNCTION_NAME
%token <cpval> STRING
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
%token LCURLY
%token RCURLY
%token PRINT
%token SEMICOLON
%token COMMA
%token FUNCTION
%token INTEGER

%left PLUS MINUS
%left TIMES SLASH
%left MOD
%right POWER
%left SUM DIFFERENCE
%left PRODUCT QUOTIENT
%left REMAINDER
%right RAISE
%right EXPONENTIATE
%left COMMA

%type <npval> program
%type <npval> stmts
%type <npval> stmt
%type <npval> assign_stmt
%type <npval> print_stmt
%type <npval> expression_stmt
%type <npval> semicolon_stmt
%type <npval> params
%type <npval> formals
%type <npval> function_value
%type <npval> expression
%type <npval> expr1
%type <npval> expr2
%type <npval> subexpression
%type <npval> call
%type <npval> atom
%type <npval> target
%type <npval> integer
%type <npval> number
%type <npval> identifier
%type <npval> function_name
%type <npval> string

%%

program : stmts {
            node_calculate_value ($1, runtime_global_env);
            char* rep = value_string (node_value ($1));
            if (runtime_errors) {
                runtime_error_print ();
            } else if (print_result && rep[0] != '\0') {
                printf ("%s\n", rep);
            }
            free (rep); }
        ;
stmts : stmts stmt { node_append_child ($1, $2); $$ = $1; }
      | /* empty */ { $$ = node_from_token (SEMICOLON); }
      ;
stmt : assign_stmt
     | print_stmt
     | expression_stmt
     | semicolon_stmt
     ;
assign_stmt : LET target EQUAL expression semicolon { $$ = node_from_token_c2 (LET, $2, $4); }
            ;
print_stmt : PRINT expression semicolon { $$ = node_from_token_c1 (PRINT, $2); }
           ;
expression_stmt : expression semicolon { $$ = $1; }
                ;
semicolon_stmt : semicolon { $$ = NULL; }
               ;
params : params COMMA expression { node_append_child ($1, $3); $$ = $1; }
       | expression { $$ = node_from_token_c1 (COMMA, $1); }
       ;
formals : formals COMMA identifier { node_append_child ($1, $3); $$ = $1; }
        | identifier { $$ = node_from_token_c1 (COMMA, $1); }
        ;
function_value : FUNCTION LPAREN formals RPAREN LCURLY expression RCURLY { $$ = node_from_function ($3, $6); }
         ;
expression : expr1
           | function_value
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
     | function_name LPAREN params RPAREN { $$ = node_from_token_c2 (LPAREN, $1, $3); }
     ;
atom : number
     | identifier
     | string
     | integer
     ;
target : identifier
       ;
integer : INTEGER NUM { $$ = node_from_integer ($2); }
        ;
number : NUM { $$ = node_from_double ($1); }
       ;
identifier : ID { $$ = node_from_identifier ($1); }
           ;
function_name : FUNCTION_NAME { $$ = node_from_identifier ($1); }
              | identifier
              ;
string : STRING { $$ = node_from_string ($1); }
       ;
semicolon : SEMICOLON
          ;

%%
void runtime_error_print () {
    int i;
    for (i = 0; i < runtime_errors; i++) {
        fprintf (stderr, BISON_ERROR_FORMAT, runtime_error[i]);
    }
    if (runtime_errors_overflow) {
        fprintf (stderr, BISON_ERROR_OVERFLOW_FORMAT, runtime_errors_overflow);
    }
    runtime_errors = 0;
    runtime_errors_overflow = 0;
}

void yyerror (char const * c) {
    runtime_error_print ();
    fprintf (stderr, BISON_ERROR_FORMAT, "Parser: invalid syntax");
}
