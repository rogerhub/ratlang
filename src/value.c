#include "env.h"
#include "runtime.h"
#include "value.h"
#include <gmp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mp_bitcnt_t value_precision () {
	EnvPair* p = env_get (runtime_global_env, "SCALE");
	if (p == NULL || p->value == NULL || p->type != PAIRTYPE_VALUE || ((Value*) p->value)->type != VALUETYPE_INTEGER) {
		return PRECISION_DEFAULT;
	} else {
		/** TODO implement this */
		return PRECISION_DEFAULT;
	}
}

void value_destroy (Value* v) {
	if (v->type == VALUETYPE_PRIMITIVE) {
		mpf_clear (v->fval);
	} else if (v->type == VALUETYPE_INTEGER) {
		mpz_clear (v->zval);
	} else if (v->type == VALUETYPE_STRING) {
		free (v->cpval);
	}
	free (v);
}

Value* value_from_double (double d) {
	Value* v = (Value*) malloc (sizeof (Value));
	v->type = VALUETYPE_PRIMITIVE;
	mpf_init2 (v->fval, value_precision ());
	mpf_set_d (v->fval, d);
	return v;
}

Value* value_from_mpf (mpf_t* f) {
	Value* v = (Value*) malloc (sizeof (Value));
	v->type = VALUETYPE_PRIMITIVE;
	mpf_init2 (v->fval, value_precision ());
	mpf_set (v->fval, *f);
	return v;
}

Value* value_from_integer (int i) {
	Value* v = (Value*) malloc (sizeof (Value));
	v->type = VALUETYPE_INTEGER;
	mpz_init2 (v->zval, value_precision ());
	mpz_set_si (v->zval, i);
	return v;
}

Value* value_from_mpz (mpz_t* z) {
	Value* v = (Value*) malloc (sizeof (Value));
	v->type = VALUETYPE_INTEGER;
	mpz_init2 (v->zval, value_precision ());
	mpz_set (v->zval, *z);
	return v;
}

Value* value_from_string (char* s) {
	Value* v = (Value*) malloc (sizeof (Value));
	v->type = VALUETYPE_STRING;
	v->cpval = strdup (s);
	return v;
}

Value* value_none = NULL;
Value* value_from_none () {
	if (value_none == NULL) {
		value_none = (Value*) malloc (sizeof (Value));
		value_none->type = VALUETYPE_NONE;
	}
	return value_none;
}

Value* value_from_function (void* formals, void* expression) {
	Value* v = (Value*) malloc (sizeof (Value));
	v->type = VALUETYPE_FUNCTION;
	v->fformals = formals;
	v->fexpression = expression;
	return v;
}

char* value_string (Value* v) {
	int size = 50;
	int len;
	char* buf;
	if (v == NULL || v->type == VALUETYPE_NONE) {
		return strdup ("");
	} else if (v->type == VALUETYPE_STRING) {
		return strdup (v->cpval);
	} else if (v->type == VALUETYPE_FUNCTION) {
		return strdup ("(function)");
	} else if (v->type == VALUETYPE_INTEGER) {
		/** TODO test this thoroughly.. */
		do {
			size *= 2;
			buf = (char*) malloc (size * sizeof (char));
			len = gmp_snprintf (buf, size, "%Zd", v->zval);
		} while (len > size);
		return buf;
	} else if (v->type == VALUETYPE_PRIMITIVE) {
		do {
			size *= 2;
			buf = (char*) malloc (size * sizeof (char));
			len = gmp_snprintf (buf, size, "%.Ff", v->fval);
		} while (len > size);
		return buf;
	} else {
		return strdup ("(unknown value)");
	}
}

int value_type (Value* v) {
	return v->type;
}

void* value_formals (Value* v) {
	return v->fformals;
}

void* value_expression (Value* v) {
	return v->fexpression;
}

int value_common_type (Value* v, Value* w) {
	if (v->type == VALUETYPE_FUNCTION ||
		w->type == VALUETYPE_FUNCTION) {
		/** TODO */
		return VALUETYPE_FUNCTION;
	}
	if (v->type == VALUETYPE_STRING ||
		w->type == VALUETYPE_STRING ||
		v->type == VALUETYPE_NONE ||
		w->type == VALUETYPE_NONE) {
		return VALUETYPE_STRING;
	} else if (v->type == VALUETYPE_PRIMITIVE ||
		w->type == VALUETYPE_PRIMITIVE) {
		return VALUETYPE_PRIMITIVE;
	} else if (v->type == VALUETYPE_INTEGER &&
		w->type == VALUETYPE_INTEGER) {
		return VALUETYPE_INTEGER;
	} else {
		/** TODO */
		return VALUETYPE_NONE;
	}
}

Value* value_promote_string (Value* v) {
	return value_from_string (value_string (v));
}

Value* value_promote_primitive (Value* v) {
	Value* promoted = (Value*) malloc (sizeof (Value));
	promoted->type = VALUETYPE_PRIMITIVE;
	mpf_init2 (promoted->fval, value_precision ());
	if (v->type == VALUETYPE_INTEGER) {
		mpf_set_z (promoted->fval, v->zval);
	} else if (v->type == VALUETYPE_PRIMITIVE) {
		mpf_set (promoted->fval, v->fval);
	} else {
		RUNTIME_ERROR ("PromotePrimitive: unrecognized operand type");
		value_destroy (promoted);
		return value_from_none ();
	}
	return promoted;
}

Value* value_add (Value* v, Value* w) {
	int common_type = value_common_type (v, w);
	Value* r = malloc (sizeof (Value));
	r->type = common_type;
	if (common_type == VALUETYPE_STRING) {
		Value* op1 = value_promote_string (v);
		Value* op2 = value_promote_string (w);
		int length = strlen (op1->cpval) + strlen (op2->cpval);
		r->cpval = (char*) malloc (sizeof (char) * (length + 1));
		strcpy (r->cpval, op1->cpval);
		strcat (r->cpval, op2->cpval);
		value_destroy (op1);
		value_destroy (op2);
	} else if (common_type == VALUETYPE_PRIMITIVE) {
		mpf_init2 (r->fval, value_precision ());
		Value* op1 = value_promote_primitive (v);
		Value* op2 = value_promote_primitive (w);
		mpf_add (r->fval, op1->fval, op2->fval);
		value_destroy (op1);
		value_destroy (op2);
	} else if (common_type == VALUETYPE_INTEGER) {
		mpz_init2 (r->zval, value_precision ());
		mpz_add (r->zval, v->zval, w->zval);
	} else {
		RUNTIME_ERROR ("ValueAdd: incompatible types");
		free (r);
		return value_from_none ();
	}
	return r;
}

Value* value_subtract (Value* v, Value* w) {
	int common_type = value_common_type (v, w);
	Value* r = malloc (sizeof (Value));
	r->type = common_type;
	if (common_type == VALUETYPE_PRIMITIVE) {
		mpf_init2 (r->fval, value_precision ());
		Value* op1 = value_promote_primitive (v);
		Value* op2 = value_promote_primitive (w);
		mpf_sub (r->fval, op1->fval, op2->fval);
		value_destroy (op1);
		value_destroy (op2);
	} else if (common_type == VALUETYPE_INTEGER) {
		mpz_init2 (r->zval, value_precision ());
		mpz_sub (r->zval, v->zval, w->zval);
	} else {
		RUNTIME_ERROR ("ValueSubtract: incompatible types");
		free (r);
		return value_from_none ();
	}
	return r;
}

Value* value_multiply (Value* v, Value* w) {
	int common_type = value_common_type (v, w);
	Value* r = malloc (sizeof (Value));
	r->type = common_type;
	if (common_type == VALUETYPE_PRIMITIVE) {
		mpf_init2 (r->fval, value_precision ());
		Value* op1 = value_promote_primitive (v);
		Value* op2 = value_promote_primitive (w);
		mpf_mul (r->fval, op1->fval, op2->fval);
		value_destroy (op1);
		value_destroy (op2);
	} else if (common_type == VALUETYPE_INTEGER) {
		mpz_init2 (r->zval, value_precision ());
		mpz_mul (r->zval, v->zval, w->zval);
	} else {
		RUNTIME_ERROR ("ValueMultiply: incompatible types");
		free (r);
		return value_from_none ();
	}
	return r;
}

Value* value_divide (Value* v, Value* w) {
	int common_type = value_common_type (v, w);
	Value* r = malloc (sizeof (Value));
	r->type = common_type;
	if (common_type == VALUETYPE_PRIMITIVE) {
		mpf_init2 (r->fval, value_precision ());
		Value* op1 = value_promote_primitive (v);
		Value* op2 = value_promote_primitive (w);
		if (mpf_sgn (op2->fval) == 0) {
			RUNTIME_ERROR ("ValueDivide: division by zero");
			free (r);
			value_destroy (op1);
			value_destroy (op2);
			return value_from_none ();
		} else {
			mpf_div (r->fval, op1->fval, op2->fval);
			value_destroy (op1);
			value_destroy (op2);
		}
	} else if (common_type == VALUETYPE_INTEGER) {
		mpz_init2 (r->zval, value_precision ());
		if (mpz_sgn (w->zval) == 0) {
			RUNTIME_ERROR ("ValueDivide: division by zero");
			free (r);
			return value_from_none ();
		} else {
			mpz_tdiv_q (r->zval, v->zval, w->zval);
		}
	} else {
		RUNTIME_ERROR ("ValueDivide: incompatible types");
		free (r);
		return value_from_none ();
	}
	return r;
}

Value* value_remainder (Value* v, Value* w) {
	int common_type = value_common_type (v, w);
	Value* r = malloc (sizeof (Value));
	r->type = common_type;
	if (common_type == VALUETYPE_PRIMITIVE) {
		mpf_init2 (r->fval, value_precision ());
		Value* op1 = value_promote_primitive (v);
		Value* op2 = value_promote_primitive (w);
		if (mpf_sgn (op2->fval) == 0) {
			RUNTIME_ERROR ("ValueRemainder: remainder by zero");
			free (r);
			value_destroy (op1);
			value_destroy (op2);
			return value_from_none ();
		} else {
			mpf_t quotient;
			mpf_t quotient_truncated;
			mpf_t remainder_multiplier;
			mpf_init2 (quotient, value_precision ());
			mpf_init2 (quotient_truncated, value_precision ());
			mpf_init2 (remainder_multiplier, value_precision ());
			mpf_div (quotient, op1->fval, op2->fval);
			mpf_trunc (quotient_truncated, quotient);
			mpf_sub (remainder_multiplier, quotient, quotient_truncated);
			mpf_mul (r->fval, remainder_multiplier, op2->fval);
			mpf_clear (quotient);
			mpf_clear (quotient_truncated);
			mpf_clear (remainder_multiplier);
			value_destroy (op1);
			value_destroy (op2);
		}
	} else if (common_type == VALUETYPE_INTEGER) {
		mpz_init2 (r->zval, value_precision ());
		if (mpz_sgn (w->zval) == 0) {
			RUNTIME_ERROR ("ValueRemainder: remainder by zero");
			free (r);
			return value_from_none ();
		} else {
			mpz_mod (r->zval, v->zval, w->zval);
		}
	} else {
		RUNTIME_ERROR ("ValueDivide: incompatible types");
		free (r);
		return value_from_none ();
	}
	return r;
}

Value* value_raise (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	return r;
}

Value* value_exponentiate (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	return r;
}

Value* value_negate (Value* v) {
	Value* r = malloc (sizeof (Value));
	return r;
}

