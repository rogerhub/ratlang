#include "ratlang_unit_test.h"
#include "env.h"
#include <stdio.h>
#include <string.h>

test_all {

	Env* e;
	Env* parent;
	EnvPair* p;
	int* value = (int*) 10;

	/** Basic assertions */
	char* key = (char*) malloc (sizeof (char) * 6);
	memcpy (key, "key 1", sizeof (char) * 6);
	e = (Env*) malloc (sizeof (Env));
	env_init (e);
	env_put (e, key, PAIRTYPE_VALUE, value);
	p = env_get (e, key);
	mu_assert ("lookup found", p != NULL);
	mu_assert ("value correct 1", strcmp (p->key, "key 1") == 0);
	mu_assert ("value correct 2", p->type == PAIRTYPE_VALUE);
	mu_assert ("value correct 3", p->value == value);
	key[0] = 'h';
	mu_assert ("key should be duplicated", strcmp (p->key, "key 1") == 0);

	/** Parent */
	value = (int*) 6;
	parent = (Env*) malloc (sizeof (Env));
	env_init (parent);
	env_set_parent (e, parent);
	env_put (parent, "key 2", PAIRTYPE_VALUE, value);
	p = env_get (e, "key 2");
	mu_assert ("parent lookup", p != NULL);
	mu_assert ("parent lookup correct", p->value == value);


} end_test
