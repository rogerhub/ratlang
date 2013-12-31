#include "value.h"
#include <math.h>
#include <stdlib.h>

Value* value_from_double (double d) {
	Value* v = malloc (sizeof (Value));
	*v = d;
	return v;
}

double value_get (Value* v) {
	return (double) *v;
}

Value* value_add (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	*r = ((double) (*v)) + ((double) (*w));
	return r;
}

Value* value_subtract (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	*r = ((double) (*v)) - ((double) (*w));
	return r;
}

Value* value_multiply (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	*r = ((double) (*v)) * ((double) (*w));
	return r;
}

Value* value_divide (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	*r = ((double) (*v)) / ((double) (*w));
	return r;
}

Value* value_remainder (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	*r = fmod (((double) (*v)) , ((double) (*w)));
	return r;
}

Value* value_raise (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	*r = pow (((double) (*v)) , ((double) (*w)));
	return r;
}

Value* value_exponentiate (Value* v, Value* w) {
	Value* r = malloc (sizeof (Value));
	*r = pow (10, ((double) (*w))) * ((double) (*v));
	return r;
}

Value* value_negate (Value* v) {
	Value* r = malloc (sizeof (Value));
	*r = -1 * ((double) (*v));
	return r;
}

