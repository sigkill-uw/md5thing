#ifndef _TABLE_H_
#define _TABLE_H_

#include <stdio.h>

/* Length of the hash, in bytes. 16 for MD5 obviously */
#define HASH_BYTE_SIZE	16

/* We use the first two bytes of the hash to index into our trie */
#define TABLE_SIZE		(256 * 256)

/* A single (hash, plaintext) pair, easily sortable */
typedef struct
{
	unsigned char *plain;
	unsigned char *hash;
} record_t;

typedef struct
{
	int n;
	int c;
	record_t *r;
} branch_t;

typedef branch_t table_t;

void table_init(table_t *table);
void table_insert(table_t *table, unsigned char *plain, unsigned char *hash);
void table_finalize(table_t *table);
void table_read(table_t *table, FILE *input);
void table_write(table_t *table, FILE *output);
unsigned char *table_search(table_t *table, unsigned char *hash);
void table_destroy(table_t *table);

#endif
