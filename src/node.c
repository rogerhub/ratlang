#include "node.h"
#include "parser.h"
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

void node_calculate_value (Node* n, Env* e) {
	if (n->v != NULL) return;

	if (n->type == NODETYPE_DOUBLE) {
		n->v = value_from_double (n->dval);
	} else if (n->type == NODETYPE_IDENTIFIER) {
		EnvPair* p = env_get (e, n->cpval);
		if (p == NULL) {
			// TODO abort?
		} else {
			n->v = (Value*) p->value;
		}
	} else if (n->type == NODETYPE_TOKEN) {
		int i_max = node_arity (n);
		int i = 0;
		int i_step = 1;

		/** Expression lists need to be parsed backward. */
		if (n->ival == SEMICOLON) {
			i = i_max - 1;
			i_max = -1;
			i_step = -1;
		}

		for (; i != i_max; i += i_step) {
			
			/** Exception: Do not evalulate LHS of assignment. */
			if (n->ival == LET && i == 0) continue;

			node_calculate_value (node_child (n, i), e);
		}
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
					n->v = node_child (n, i_max - 1)->v;
				}
				break;
			case LET:
				env_put (e, node_child (n, 0)->cpval, PAIRTYPE_VALUE, node_child (n, 1)->v);
				n->v = node_child (n, 1)->v;
				break;
			case PRINT:
				printf ("%f\n", value_get( node_child (n, 0)->v ));
				n->v = node_child (n, 0)->v;
				break;
		}

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
	nodelist_insert (n->children, c);
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

