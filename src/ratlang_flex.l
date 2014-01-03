%option noyywrap

%{
#include <errno.h>
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

#define ATOF_FULL 1024
#define ATOF_L 1025
#define ATOF_R 1026
#define ATOF_NAKED 1027
double rat_atof (char const * c, int clen, int type);
%}

%%
\"([^\"]|\\\")+\" yylval.cpval = strdup (yytext); return STRING;
[ \t]+            /* Throw away whitespace */
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
\n                return SEMICOLON;
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
		print_result = 0;

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
