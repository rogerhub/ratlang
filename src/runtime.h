#include "env.h"

#ifndef RUNTIME_H
#define RUNTIME_H

#define PRECISION_DEFAULT 40

Env* runtime_global_env;
int print_result;

char* runtime_error[10];
int runtime_errors;
int runtime_errors_overflow;

#define RUNTIME_ERROR(msg) do { \
	if (runtime_errors < 10) { \
		runtime_error[runtime_errors] = msg; runtime_errors++; } else { runtime_errors_overflow++; } } while (0)

#endif
