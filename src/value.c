#include "value.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Value* value_from_double (double d) {
	Value* v = malloc (sizeof (Value));
	v->type = VALUETYPE_PRIMITIVE;
	v->dval = d;
	return v;
}

Value* value_from_string (char* s) {
	Value* v = malloc (sizeof (Value));
	v->type = VALUETYPE_STRING;
	v->cpval = s; /** TODO String parsing */
	return v;
}

Value* value_none = NULL;
Value* value_from_none () {
	if (value_none == NULL) {
		value_none = malloc (sizeof (Value));
		value_none->type = VALUETYPE_NONE;
	}
	return value_none;
}

Value* value_from_function (void* formals, void* expression) {
	Value* v = malloc (sizeof (Value));
	v->type = VALUETYPE_FUNCTION;
	v->fformals = formals;
	v->fexpression = expression;
	return v;
}

char* value_string (Value* v) {
	if (v == NULL) {
		return strdup ("(Unidentified value)");
	} else if (v->type == VALUETYPE_NONE) {
		return strdup ("");
	} else if (v->type == VALUETYPE_STRING) {
		return strdup (v->cpval);
	} else if (v->type == VALUETYPE_FUNCTION) {
		return strdup ("(function)");
	} else {
		/** TODO detect size of float */
		char* buf = (char*) malloc (100 * sizeof (char));
		sprintf (buf, "%10.10f", v->dval);
		return buf;
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

Value* value_add (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	r->dval = ((double) (v->dval)) + ((double) (w->dval));
	return r;
}

Value* value_subtract (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	r->dval = ((double) (v->dval)) - ((double) (w->dval));
	return r;
}

Value* value_multiply (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	r->dval = ((double) (v->dval)) * ((double) (w->dval));
	return r;
}

Value* value_divide (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	r->dval = ((double) (v->dval)) / ((double) (w->dval));
	return r;
}

Value* value_remainder (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	r->dval = fmod (((double) (v->dval)) , ((double) (w->dval)));
	return r;
}

Value* value_raise (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	r->dval = pow (((double) (v->dval)) , ((double) (w->dval)));
	return r;
}

Value* value_exponentiate (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	r->dval = pow (10, ((double) (w->dval))) * ((double) (v->dval));
	return r;
}

Value* value_negate (Value* v) {
	Value* r = malloc (sizeof (Value));
	r->dval = -1 * ((double) (v->dval));
	return r;
}

