#include "node.h"
#include "ratlang_bison.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Node* node_from_double (double d) {
	Node* n = malloc (sizeof (Node));
	n->type = NODETYPE_DOUBLE;
	n->dval = d;
	n->v = NULL;
	n->children = malloc (sizeof (NodeList));
	nodelist_init (n->children);
	return n;
}

Node* node_from_value (Value* v) {
	Node* n = malloc (sizeof (Node));
	n->type = NODETYPE_VALUE;
	n->v = v;
	n->children = malloc (sizeof (NodeList));
	nodelist_init (n->children);
	return n;
}

Node* node_from_identifier (char* c) {
	Node* n = malloc (sizeof (Node));
	n->type = NODETYPE_IDENTIFIER;
	n->cpval = c;
	n->v = NULL;
	n->children = malloc (sizeof (NodeList));
	nodelist_init (n->children);
	return n;
}

Node* node_from_string (char* c) {
	Node* n = malloc (sizeof (Node));
	n->type = NODETYPE_STRING;
	n->cpval = c;
	n->v = NULL;
	n->children = malloc (sizeof (NodeList));
	nodelist_init (n->children);
	return n;
}

Node* node_from_token (int t) {
	Node* n = malloc (sizeof (Node));
	n->type = NODETYPE_TOKEN;
	n->ival = t;
	n->v = NULL;
	n->children = malloc (sizeof (NodeList));
	nodelist_init (n->children);
	return n;
}

Node* node_from_token_c1 (int t, Node* c1) {
	Node* n = node_from_token (t);
	node_append_child (n, c1);
	return n;
}

Node* node_from_token_c2 (int t, Node* c1, Node* c2) {
	Node* n = node_from_token (t);
	node_append_child (n, c1);
	node_append_child (n, c2);
	return n;
}

Node* node_from_function (Node* formals, Node* expression) {
	Node* n = malloc (sizeof (Node));
	n->type = NODETYPE_FUNCTION;
	n->children = malloc (sizeof (NodeList));
	nodelist_init (n->children);
	node_append_child (n, formals);
	node_append_child (n, expression);
	return n;
}

void node_calculate_value (Node* n, Env* e) {
	if (n->v != NULL) return;

	/** Environment lookups */
	EnvPair* p;

	if (n->type == NODETYPE_DOUBLE) {
		n->v = value_from_double (n->dval);
	} else if (n->type == NODETYPE_STRING) {
		n->v = value_from_string (n->cpval);
	} else if (n->type == NODETYPE_IDENTIFIER) {
		p = env_get (e, n->cpval);
		if (p == NULL) {
			n->v = value_from_double (0);
			// TODO abort?
		} else {
			n->v = (Value*) p->value;
		}
	} else if (n->type == NODETYPE_FUNCTION) {
		n->v = value_from_function (node_child (n, 0), node_child (n, 1));
	} else if (n->type == NODETYPE_TOKEN) {
		int i_max = node_arity (n);
		int i = 0;
		int i_step = 1;

		for (; i != i_max; i += i_step) {
			
			/** Exception: Do not evalulate LHS of assignment. */
			if (n->ival == LET && i == 0) continue;

			node_calculate_value (node_child (n, i), e);
		}

		/** For printing, if necessary */
		char* rep;
		switch (n->ival) {
			case SUM:
				n->v = value_add (node_child (n, 0)->v, node_child (n, 1)->v); break;
			case DIFFERENCE:
				n->v = value_subtract (node_child (n, 0)->v, node_child (n, 1)->v); break;
			case PRODUCT:
				n->v = value_multiply (node_child (n, 0)->v, node_child (n, 1)->v); break;
			case QUOTIENT:
				n->v = value_divide (node_child (n, 0)->v, node_child (n, 1)->v); break;
			case REMAINDER:
				n->v = value_remainder (node_child (n, 0)->v, node_child (n, 1)->v); break;
			case RAISE:
				n->v = value_raise (node_child (n, 0)->v, node_child (n, 1)->v); break;
			case EXPONENTIATE:
				n->v = value_exponentiate (node_child (n, 0)->v, node_child (n, 1)->v); break;
			case MINUS:
				n->v = value_negate (node_child (n, 0)->v); break;
			case SEMICOLON:
				i_max = node_arity (n);
				if (i_max == 0) {
					n->v = value_from_double (0);
				} else {
					n->v = node_child (n, i - i_step)->v;
				}
				break;
			case LET:
				env_put (e, node_child (n, 0)->cpval, PAIRTYPE_VALUE, node_child (n, 1)->v);
				n->v = node_child (n, 1)->v;
				break;
			case PRINT:
				rep = value_string( node_child (n, 0)->v );
				printf ("%s\n", rep);
				free (rep);
				n->v = node_child (n, 0)->v;
				break;
			case LPAREN:
				node_run_function (n, e);
				break;
		}
	}
}

void node_reset_value (Node* n) {
	n->v = NULL;
	int i;
	for (i = 0; i < node_arity (n); i++) {
		node_reset_value (node_child (n, i));
	}
}

void node_run_function (Node* n, Env* e) {
	Value* function_value = node_child (n, 0)->v;
	if (function_value == NULL || value_type (function_value) != VALUETYPE_FUNCTION) {
		// TODO
		printf ("Not callable\n");
		return;
	}
	Env* call_frame = malloc (sizeof (Env));
	env_init (call_frame);
	env_set_parent (call_frame, e);
	Node* function_parameters = node_child (n, 1);
	Node* formal_parameters = value_formals (function_value);
	Node* expression = value_expression (function_value);
	int parameters_arity = node_arity (function_parameters);
	if (parameters_arity != node_arity (formal_parameters)) {
		// TODO
		printf ("Incorrect number of parameters\n");
		return;
	}
	int i;
	for (i = 0; i < parameters_arity; i++) {
		env_put (call_frame, node_child (formal_parameters, i)->cpval,
			PAIRTYPE_VALUE, node_child (function_parameters, i)->v);
	}
	node_reset_value (expression);
	node_calculate_value (expression, call_frame);
	n->v = expression->v;
	if (value_type (n->v) != VALUETYPE_FUNCTION) {
		free (call_frame);
	}
}

Value* node_value (Node* n) {
	return n->v;
}

int node_arity (Node* n) {
	return n->children->size;
}

Node* node_child (Node* n, int number) {
	return nodelist_get (n->children, number);
}

void node_append_child (Node* n, Node* c) {
	if (c != NULL) {
		nodelist_insert (n->children, c);
	}
}

void nodelist_init (NodeList* l) {
	l->size = 0;
	l->capacity = 2;
	l->children = malloc (l->capacity * sizeof (Node*));
}

Node* nodelist_get (NodeList* l, int i) {
	if (i >= l->size) {
		return NULL;
	}
	return l->children[i];
}

void nodelist_insert (NodeList* l, Node* n) {
	if (l->size >= l->capacity) {
		nodelist_expand (l);
	}
	l->children[l->size] = n;
	l->size += 1;
}

void nodelist_expand (NodeList* l) {
	int new_capacity = 2 * l->capacity;
	Node** new_children = malloc (new_capacity * sizeof (Node*));
	memcpy (new_children, l->children, l->size * sizeof (Node*));
	free (l->children);
	l->capacity = new_capacity;
	l->children = new_children;
}

