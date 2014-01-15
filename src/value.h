#ifndef VALUE_H
#define VALUE_H

#include <gmp.h>

#define VALUETYPE_NONE 8192
#define VALUETYPE_PRIMITIVE 8193
#define VALUETYPE_FUNCTION 8194
#define VALUETYPE_STRING 8195
#define VALUETYPE_INTEGER 8196

typedef struct {
	int type;
	int ival;
	mpf_t fval;
	mpz_t zval;
	char* cpval;
	void* fformals;
	void* fexpression;
} Value;

mp_bitcnt_t value_precision ();

void value_destroy (Value* v);

Value* value_from_double (double d);

Value* value_from_mpf (mpf_t* f);

Value* value_from_integer (int i);

Value* value_from_mpz (mpz_t* z);

Value* value_from_string (char* s);

Value* value_from_none ();

Value* value_from_function (void* formals, void* expression);

char* value_string (Value* v);

int value_type (Value* v);

void* value_formals (Value* v);

void* value_expression (Value* v);

int value_common_type (Value* v, Value* w);

Value* value_add (Value* v, Value* w);

Value* value_subtract (Value* v, Value* w);

Value* value_multiply (Value* v, Value* w);

Value* value_divide (Value* v, Value* w);

Value* value_remainder (Value* v, Value* w);

Value* value_raise (Value* v, Value* w);

Value* value_exponentiate (Value* v, Value* w);

Value* value_negate (Value* v);


#endif
