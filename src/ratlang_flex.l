%option noyywrap

%{
#include <errno.h>
#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "config.h"
#include "node.h"
#include "ratlang_bison.h"
#include "runtime.h"
#include "value.h"

#define ATOF_FULL 1024
#define ATOF_L 1025
#define ATOF_R 1026
#define ATOF_NAKED 1027
mpf_t * rat_atof (char const * c, int clen, int type);
mpz_t * rat_atoz (char const * c, int clen);
void rat_strclear ();
void rat_strappend (char c);
void rat_strappends (char const * c, int leng);
char * rat_strget ();
int rat_octalescape (char const * c, int leng);
%}

%x STR
%x INT
%%
<INITIAL>\"                 BEGIN(STR); rat_strclear ();
<STR>\"                     BEGIN(INITIAL); yylval.cpval = strdup (rat_strget ()); return STRING;
<STR>\n                     RUNTIME_ERROR ("Lexer: unterminated string"); BEGIN(INITIAL);
<STR>\\['"?\\]              rat_strappend (yytext[1]);
<STR>\\a                    rat_strappend ('\a');
<STR>\\b                    rat_strappend ('\b');
<STR>\\f                    rat_strappend ('\f');
<STR>\\n                    rat_strappend ('\n');
<STR>\\r                    rat_strappend ('\r');
<STR>\\t                    rat_strappend ('\t');
<STR>\\v                    rat_strappend ('\v');
<STR>\\[0-7]{1,3}           rat_strappend (rat_octalescape (yytext, yyleng));
<STR>[^\"\n\\]+             rat_strappends (yytext, yyleng);
[ \t]+                      /* Throw away whitespace */
[0-9]+\.[0-9]+              yylval.fval = rat_atof (yytext, yyleng, ATOF_FULL); return NUM;
[0-9]+\.                    yylval.fval = rat_atof (yytext, yyleng, ATOF_L); return NUM;
\.[0-9]+                    yylval.fval = rat_atof (yytext, yyleng, ATOF_R); return NUM;
[0-9]+                      yylval.fval = rat_atof (yytext, yyleng, ATOF_NAKED); return NUM;
<INT>[0-9]+                 yylval.zval = rat_atoz (yytext, yyleng); BEGIN(INITIAL); return INTEGER;
[A-Z_]+                     yylval.cpval = strdup (yytext); return ID;
[a-z]{2,}                   yylval.cpval = strdup (yytext); return FUNCTION_NAME;
l                           return LET;
=                           return EQUAL;
s                           return SUM;
\+                          return PLUS;
d                           return DIFFERENCE;
-                           return MINUS;
p                           return PRODUCT;
\*                          return TIMES;
q                           return QUOTIENT;
\/                          return SLASH;
r                           return REMAINDER;
%                           return MOD;
t                           return RAISE;
\^                          return POWER;
e                           return EXPONENTIATE;
\(                          return LPAREN;
\)                          return RPAREN;
\{                          return LCURLY;
\}                          return RCURLY;
\\                          return PRINT;
;                           return SEMICOLON;
\n                          return SEMICOLON;
,                           return COMMA;
f                           return FUNCTION;
@                           BEGIN(INT);
.                           RUNTIME_ERROR ("Lexer: unrecognized token");

%%

mpf_t* rat_atof (char const * c, int clen, int type) {
	mpf_t* result = (mpf_t*) malloc (sizeof (mpf_t));
	mpf_init2 (*result, value_precision ());
	mpf_set_si (*result, 0);
	int i;
	char n;
	if (type == ATOF_NAKED) {
		for (clen -= 1; clen >= 0; clen -= 1) {
			n = c[clen];
			mpf_add_ui (*result, *result, n - '0');
			mpf_div_ui (*result, *result, 10);
		}
		return result;
	} else if (type == ATOF_L) {
		for (i = 0; i < clen - 1; i += 1) {
			n = c[i];
			mpf_mul_ui (*result, *result, 10);
			mpf_add_ui (*result, *result, n - '0');
		}
		return result;
	} else if (type == ATOF_R) {
		for (clen -= 1; clen > 0; clen -= 1) {
			n = c[clen];
			mpf_add_ui (*result, *result, n - '0');
			mpf_div_ui (*result, *result, 10);
		}
		return result;
	} else if (type == ATOF_FULL) {
		i = 0;
		while (i < clen && c[i] != '.') i++;
		mpf_t* left = rat_atof (c, i + 1, ATOF_L);
		mpf_t* right = rat_atof (c + i, clen - i, ATOF_R);
		mpf_add (*result, *left, *right);
		free (left);
		free (right);
		return result;
	}
}

mpz_t* rat_atoz (char const * c, int clen) {
	mpz_t* result = (mpz_t*) malloc (sizeof (mpz_t));
	mpz_init2 (*result, value_precision ());
	mpz_set_si (*result, 0);
	int i;
	char n;
	for (i = 0; i < clen; i += 1) {
		n = c[i];
		mpz_mul_ui (*result, *result, 10);
		mpz_add_ui (*result, *result, n - '0');
	}
	return result;
}

char* rat_strbuffer = NULL;
int rat_strbuffersize;
int rat_strbufferlength;
void rat_strclear () {
	if (rat_strbuffer != NULL) {
		free (rat_strbuffer);
	}
	rat_strbuffer = (char*) malloc (sizeof (char) * rat_strbuffersize);
	rat_strbuffersize = 1;
	rat_strbufferlength = 0;
}

void rat_strappend (char c) {
	if (rat_strbufferlength > rat_strbuffersize) {
		char* new_rat_strbuffer = (char*) malloc (sizeof (char) * 2 * rat_strbuffersize);
		memcpy (new_rat_strbuffer, rat_strbuffer, rat_strbufferlength * sizeof (char));
		free (rat_strbuffer);
		rat_strbuffer = new_rat_strbuffer;
		rat_strbuffersize *= 2;
	}
	rat_strbuffer[rat_strbufferlength] = c;
	rat_strbufferlength += 1;
}

void rat_strappends (char const * c, int leng) {
	int i;
	for (i = 0; i < leng; i++) {
		rat_strappend (c[i]);
	}
}

char * rat_strget () {
	return rat_strbuffer;
}

int rat_octalescape (char const * c, int leng) {
	int code = c[1] - '0';
	if (leng > 2) {
		code *= 8;
		code += c[2] - '0';
	}
	if (leng > 3) {
		code *= 8;
		code += c[3] - '0';
	}
	return code;
}

char* newline_dup (char* c) {
	char* c_new;
	int c_len = strlen (c);
	if (c[c_len - 1] != '\n') {
		c_new = (char*) malloc (c_len + 2);
		memcpy (c_new, c, sizeof (char) * (c_len + 2));
		*(c_new + c_len) = '\n';
		*(c_new + c_len + 1) = '\0';
		return c_new;
	} else {
		return strdup (c);
	}
}

void usage (FILE* s, char* binary_name) {
	fprintf (s, "usage: %s [-h|--help] [-i|--interactive] [FILE]\n\n", binary_name);
	fprintf (s, "command-line arguments:\n");
	fprintf (s, "   -h  --help           Displays this usage message\n");
	fprintf (s, "   -i  --interactive    Starts an interactive prompt\n");
	fprintf (s, "\n");
	fprintf (s, "This is %s: %s%s%s\n", PACKAGE_STRING, "report bugs to <", PACKAGE_BUGREPORT, ">");
}

void process_line (char* l) {
	l = newline_dup (l);
	YY_BUFFER_STATE b = yy_scan_string (l);
	yy_switch_to_buffer (b);
	yyparse ();
	free (l);
}

int read_line_from_file (FILE* file, char** buffer_target) {
	int current_buffer_size = 4095;
	char* buffer = (char*) malloc (sizeof (char) * current_buffer_size);
	int length = 0;
	char ch = 0;

	while (1) {
		if (length == current_buffer_size) {
			current_buffer_size *= 2;
			buffer = realloc (buffer, current_buffer_size);
		}
		ch = getc (file);
		if (ch == '\n' || ch == EOF) {
			buffer[length] = '\0';
			break;
		} else {
			buffer[length] = ch;
			length += 1;
		}
	}
	*buffer_target = buffer;
	if (ch == EOF) {
		return 0;
	} else {
		return 1;
	}
}

#define PROGACTION_HELP 512
#define PROGACTION_INTERACTIVE 513
#define PROGACTION_FILE 514
#define PROGACTION_FILE_INTERACTIVE 515
#define PROGACTION_STDIN 516

int main (int argc, char** argv) {

	int i;
	int action = isatty (0) ? PROGACTION_INTERACTIVE : PROGACTION_STDIN;
	char* input_file_name = NULL;
	for (i = 1; i < argc; i++) {
		if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0) {
			action = PROGACTION_HELP;
		} else if (strcmp (argv[i], "-i") == 0 || strcmp (argv[i], "--interactive") == 0) {
			if (action == PROGACTION_FILE || action == PROGACTION_FILE_INTERACTIVE) {
				action = PROGACTION_FILE_INTERACTIVE;
			} else {
				action = PROGACTION_INTERACTIVE;
			}
		} else if (*(argv[i]) == '-') {
			fprintf (stderr, "unrecognized option: %s\ntry %s --help\n", argv[i], argv[0]);
			return EINVAL;
		} else {
			if (input_file_name == NULL) {
				input_file_name = argv[i];
				if (action == PROGACTION_INTERACTIVE || action == PROGACTION_FILE_INTERACTIVE) {
					action = PROGACTION_FILE_INTERACTIVE;
				} else {
					action = PROGACTION_FILE;
				}
			} else {
				fprintf (stderr, "usage error: unexpected argument %s\ntry %s --help\n", argv[i], argv[0]);
				return EINVAL;
			}
		}
	}

	if (action == PROGACTION_HELP) {
		usage (stderr, argv[0]);
		return 0;
	} else {
		/** Initialize persistent environment */
		runtime_global_env = (Env*) malloc (sizeof (Env));
		env_init (runtime_global_env);
		env_put (runtime_global_env, "SCALE", PAIRTYPE_VALUE, value_from_integer (PRECISION_DEFAULT));
		env_put (runtime_global_env, "IDP", PAIRTYPE_VALUE, value_from_integer (1));
		print_result = 0;
		runtime_errors = 0;
		runtime_errors_overflow = 0;

		if (action == PROGACTION_FILE || action == PROGACTION_FILE_INTERACTIVE) {
			FILE* in = fopen (input_file_name, "r");
			if (in == NULL) {
				fprintf (stderr, "unable to open file for reading: %s\n", input_file_name);
				return ENOENT;
			}
			char* buffer;
			int has_more;
			while (1) {
				has_more = read_line_from_file (in, &buffer);
				process_line (buffer);
				free (buffer);
				if (!has_more) {
					break;
				}
			}
		}

		if (action == PROGACTION_FILE_INTERACTIVE ||
			action == PROGACTION_INTERACTIVE ||
			action == PROGACTION_STDIN) {
			char* line;
			char* prompt;
			if (action == PROGACTION_INTERACTIVE || action == PROGACTION_FILE_INTERACTIVE) {
				prompt = "rat> ";
				print_result = 1;
			} else {
				prompt = "";
			}
			while (line = readline (prompt)) {
				add_history (line);
				process_line (line);
				free (line);
			}
		}
	}

    return 0;
}
