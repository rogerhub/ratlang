#ifndef VALUE_H
#define VALUE_H

typedef double Value;

Value* value_from_double(double d);

double value_get (Value* v);

Value* value_add (Value* v, Value* w);

Value* value_subtract (Value* v, Value* w);

Value* value_multiply (Value* v, Value* w);

Value* value_divide (Value* v, Value* w);

Value* value_remainder (Value* v, Value* w);

Value* value_raise (Value* v, Value* w);

Value* value_exponentiate (Value* v, Value* w);

Value* value_negate (Value* v);


#endif
