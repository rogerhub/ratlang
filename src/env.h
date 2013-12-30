#ifndef ENV_H
#define ENV_H

#define PAIRTYPE_DOUBLE 2048

/** A dictionary from strings to doubles. (Environment) */
typedef struct {
	char* key;
	int type;
	void* value;
} EnvPair;
typedef struct {
	int size;
	int capacity;
	EnvPair* element;
} EnvBucket;
typedef struct {
	int size;
	int buckets;
	EnvBucket* bucket;
} Env;

void env_print (Env* e);

void env_init (Env* e);

void env_put (Env* e, char* key, int type, void* value);

void env_define (Env* e, EnvPair* p);

EnvPair* env_get (Env* e, char* key);

void env_rehash (Env* e);

long env_djb2_hash (char* s);

int env_hash (char* k, int limit);

int env_strequal (char* a, char* b);

void envbucket_init (EnvBucket* b);

void envbucket_destroy (EnvBucket* b);

void envbucket_expand (EnvBucket* b);

int envbucket_define (EnvBucket* b, EnvPair* p);

EnvPair* envbucket_get (EnvBucket* b, char* key);


#endif
