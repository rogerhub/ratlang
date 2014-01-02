#include "env.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void env_print (Env* e) {
    int i;
    for (i = 0; i < e->buckets; i++) {
        printf ("The bucket %d has %d elements\n", i, e->bucket[i].size);
    }
}

void env_init (Env* e) {
	e->size = 0;
	e->buckets = 1;
	e->bucket = (EnvBucket*) malloc (e->buckets * sizeof (EnvBucket));
	envbucket_init (e->bucket + 0);
	e->parent = NULL;
}

void env_set_parent (Env* e, Env* p) {
	e->parent = p;
}

void env_put (Env* e, char* key, int type, void* value) {
	EnvPair* p = malloc (sizeof (EnvPair));
	p->key = key;
	p->type = type;
	p->value = value;
	env_define (e, p);
	free (p);
}

void env_define (Env* e, EnvPair* p) {
	int bucket = env_hash (p->key, e->buckets);
	if (e->size > e->buckets) {
		env_rehash (e);
	}
	e->size += envbucket_define (e->bucket + bucket, p);
}

EnvPair* env_get (Env* e, char* key) {
	int bucket = env_hash (key, e->buckets);
	EnvPair* result = envbucket_get (e->bucket + bucket, key);
	if (result == NULL && e->parent != NULL) {
		return env_get (e->parent, key);
	}
	return result;
}

void env_rehash (Env* e) {

	/** Prepare new environment */
	int new_buckets = 2 * e->size;
	int i;
	int j;
	EnvBucket* new_bucket = (EnvBucket*) malloc (new_buckets * sizeof (EnvBucket));
	for (i = 0; i < new_buckets; i++) {
		envbucket_init (new_bucket + i);
	}

	/** Migrate data */
	EnvBucket* b;
	EnvPair* p;
	int p_bucket;
	for (i = 0, b = e->bucket; i < e->buckets; i++, b++) {
		for (j = 0; j < b->size; j++) {
			p = b->element + j;
			p_bucket = env_hash (p->key, new_buckets);
			envbucket_define (new_bucket + p_bucket, p);
		}
		envbucket_destroy (b);
	}

	/** Cleanup */
	e->buckets = new_buckets;
	e->bucket = new_bucket;
}

long env_djb2_hash (char* s) {
	long hash = 5381;
	int c;

	while (c = *s++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

int env_hash (char* k, int limit) {
	long hash = env_djb2_hash (k);
	return hash % limit;
}

int env_strequal (char* a, char* b) {
	while ((*a) != '\0') {
		if ((*a) != (*b)) {
			return 0;
		}
		a += 1;
		b += 1;
	}
	return (*b) == '\0';
}

void envbucket_init (EnvBucket* b) {
	b->size = 0;
	b->capacity = 2;
	b->element = (EnvPair*) malloc (b->capacity * sizeof (EnvPair));
}

void envbucket_destroy (EnvBucket* b) {
	free (b->element);
}

void envbucket_expand (EnvBucket* b) {
	int new_capacity = b->capacity * 2;
	EnvPair* new_element = (EnvPair*) malloc (new_capacity * sizeof (EnvPair));
	int i;
	for (i = 0; i < b->size; i++) {
		new_element[i].key = b->element[i].key;
		new_element[i].type = b->element[i].type;
		new_element[i].value = b->element[i].value;
	}
	free (b->element);
	b->element = new_element;
	b->capacity = new_capacity;
}

int envbucket_define (EnvBucket* b, EnvPair* p) {
	int i;
	EnvPair* target = envbucket_get (b, p->key);
	if (target != NULL) {
		target->type = p->type;
		target->value = p->value;
		return 0;
	}
	if (b->size >= b->capacity) {
		envbucket_expand (b);
	}
	b->element[b->size].key = p->key;
	b->element[b->size].type = p->type;
	b->element[b->size].value = p->value;
	b->size += 1;
	return 1;
}

EnvPair* envbucket_get (EnvBucket* b, char* key) {
	int i;
	for (i = 0; i < b->size; i++) {
		if (env_strequal (b->element[i].key, key)) {
			return b->element + i;
		}
	}
	return NULL;
}
