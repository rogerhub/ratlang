#include "ratlang_unit_test.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

test_begin

/** Basic assertions */
test
	Env* e = (Env*) malloc (sizeof (Env));
	EnvPair* p;
	env_init (e);
	env_put (e, "key 1", 4, (void*) 9);
	p = env_get (e, "key 1");
	mu_assert ("lookup found", p != NULL);
	mu_assert ("value correct 1",
		p != NULL && strcmp (p->key, "key 1") == 0);
	mu_assert ("value correct 2",
		p != NULL && p->type == 4);
	mu_assert ("value correct 3",
		p != NULL && p->value == (void*) 9);
	env_destroy (e);
end_test

/** Nonexistant key */
test
	Env* e = (Env*) malloc (sizeof (Env));
	env_init (e);
	env_put (e, "key 1", 4, (void*) 9);
	mu_assert ("nonexistant key", env_get (e, "key 2") == NULL);
	env_destroy (e);
end_test

/** Parent lookup */
test
	Env* e = (Env*) malloc (sizeof (Env));
	Env* parent = (Env*) malloc (sizeof (Env));
	env_init (e); env_init (parent);
	env_set_parent (e, parent);
	env_put (parent, "key 1", 4, (void*) 9);
	EnvPair* p = env_get (e, "key 1");
	mu_assert ("parent lookup",
		p != NULL && p->value == (void*) 9);
	env_destroy (e);
end_test

/** Edge case: mutating key */
test
	Env* e = (Env*) malloc (sizeof (Env));
	env_init (e);
	char* key = (char*) malloc (sizeof (char) * 2);
	key[0] = 'a';
	key[1] = '\0';
	env_put (e, key, 4, (void*) 9);
	EnvPair* p;
	p = env_get (e, "a");
	key[0] = 'b';
	mu_assert ("key integrity after key mutation",
		p != NULL && strcmp (p->key, "a") == 0);
	mu_assert ("lookup after key mutation", env_get (e, "a") != NULL);
	env_destroy (e);
	free (key);
end_test

/** Edge case: load balancing */
test
	Env* e = (Env*) malloc (sizeof (Env));
	env_init (e);
	char* key = (char*) malloc (sizeof (char) * 2);
	key[1] = '\0';
	int i;
	for (i = 0; i < 26; i++) {
		key[0] = 'a' + i;
		env_put (e, key, 4, (void*) 9);
	}
	EnvPair* p;
	p = env_get (e, "a");
	mu_assert ("load balancing - first",
		p != NULL && strcmp (p->key, "a") == 0 && p->value == (void*) 9);
	p = env_get (e, "z");
	mu_assert ("load balancing - last",
		p != NULL && strcmp (p->key, "z") == 0 && p->value == (void*) 9);
	env_destroy (e);
end_test


test_finish
