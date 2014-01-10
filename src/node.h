#include "env.h"
#include "value.h"

#ifndef NODE_H
#define NODE_H

#define NODETYPE_VALUE 4095
#define NODETYPE_DOUBLE 4096
#define NODETYPE_IDENTIFIER 4097
#define NODETYPE_TOKEN 4098
#define NODETYPE_STRING 4099
#define NODETYPE_FUNCTION 4100
#define NODETYPE_INTEGER 4101

struct NodeList;

typedef struct Node {
	/** Node type */
	int type;

	/** Node raw values */
	double dval;
	char* cpval;
	int ival; /** For tokens too */

	/** Node calculated symbolic tree value */
	Value* v;

	/** Children */
	struct NodeList* children;
} Node;

typedef struct NodeList {
	int size;
	int capacity;
	struct Node** children;
} NodeList;

Node* node_from_double (double d);

Node* node_from_integer (int i);

Node* node_from_value (Value* v);

Node* node_from_identifier (char* c);

Node* node_from_string (char* c);

Node* node_from_token (int t);

Node* node_from_token_c1 (int t, Node* c1);

Node* node_from_token_c2 (int t, Node* c1, Node* c2);

Node* node_from_function (Node* formals, Node* expression);

void node_calculate_value (Node* n, Env* e);

void node_reset_value (Node* n);

void node_run_function (Node* n, Env* e);

Value* node_value (Node* n);

int node_arity (Node* n);

Node* node_child (Node* n, int number);

void node_append_child (Node* n, Node* c);

void node_childlist_init (Node* n);

void nodelist_init (NodeList* l);

Node* nodelist_get (NodeList* l, int i);

void nodelist_insert (NodeList* l, Node* n);

void nodelist_expand (NodeList* l);

#endif
