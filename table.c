#define _TABLE_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <openssl/md5.h>

#include "table.h"

int cmp_records(const void *x, const void *y);
void branch_insert(branch_t *branch, unsigned char *plain, unsigned char *hash);

void table_init(table_t *table)
{
	int i;
	for(i = TABLE_SIZE; i --;)
		table[i].n = table[i].c = 0;
}

void table_insert(table_t *table, unsigned char *hash, unsigned char *plain)
{
	branch_insert(&table[(int)hash[0] + 256 * (int)hash[1]], plain, hash);
}

void table_finalize(table_t *table)
{
	int i;
	for(i = TABLE_SIZE; i --;)
		qsort(table[i].r, table[i].n, sizeof(record_t), cmp_records);
}

void table_populate(table_t *table, FILE *words)
{
	int i, c;
	unsigned char *word;

	table_init(table);

	do
	{
		word = (unsigned char *)malloc(256 + HASH_BYTE_SIZE);
		assert(word);	/* FIXME */

		for(c = fgetc(words), i = 0; c != '\n' && c != EOF && i < 255; c = fgetc(words), i ++)
			word[HASH_BYTE_SIZE + 1 + i] = (unsigned char)c;

		if(i == 255 && c != '\n' && c != EOF)
		{
			/* FIXME */
			puts("Word too long");
			exit(1);
		}

		/* FIXME */
		word = realloc(word, HASH_BYTE_SIZE + 1 + i);
		assert(word);

		word[HASH_BYTE_SIZE] = (unsigned char)i;
		MD5(&word[HASH_BYTE_SIZE + 1], i, word);

		table_insert(table, word, &word[HASH_BYTE_SIZE]);
	} while(c != EOF);

	table_finalize(table);
}

void table_read(table_t *table, FILE *input)
{
	int i;
	unsigned char *word;

	table_init(table);

	do
	{
		word = (unsigned char *)malloc(256 + HASH_BYTE_SIZE);
		assert(word);	/* FIXME */

		/* FIXME */
		i = (int)fread(word, sizeof(unsigned char), HASH_BYTE_SIZE, input);
		assert(i == HASH_BYTE_SIZE || i == 0);

		if(i == 0)
			break;

		i = fgetc(input);	/* Unsure about signedness here */
		assert(i != EOF);
		word[HASH_BYTE_SIZE] = (unsigned char)i;
		assert(fread(&word[HASH_BYTE_SIZE + 1], sizeof(unsigned char), i, input) == (size_t)i);

		word = (unsigned char *)realloc(word, HASH_BYTE_SIZE + 1 + i);

		table_insert(table, word, &word[HASH_BYTE_SIZE]);
	} while(!feof(input));
}	

void table_write(table_t *table, FILE *output)
{
	int i, j;

	for(i = 0; i < TABLE_SIZE; i ++)
		for(j = 0; j < table[i].n; j ++)
		{
			/* FIXME */
			assert(fwrite(table[i].r[j].hash, sizeof(unsigned char), HASH_BYTE_SIZE, output) == HASH_BYTE_SIZE);
			assert(fwrite(table[i].r[j].plain, sizeof(unsigned char), (size_t)*table[i].r[j].plain + 1, output)
				== (size_t)*table[i].r[j].plain + 1);
		}
}

unsigned char *table_search(table_t *table, unsigned char *hash)
{
	int i, j, k, c;
	record_t *rl;

	rl = table[(int)hash[0] + 256 * (int)hash[1]].r;

	i = 0;
	j = table[(int)hash[0] + 256 * (int)hash[1]].n - 1;

	while(i <= j)
	{
		k = i + (j - i) / 2;
		c = memcmp(rl[k].hash, hash, HASH_BYTE_SIZE);

		if(c == 0)
			return rl[k].plain;
		else if(c < 0)
			i = k + 1;
		else
			j = k - 1;
	}

	return NULL;
}

void table_destroy(table_t *table)
{
	int i, j;
	for(i = TABLE_SIZE; i --;)
	{
		if(table[i].c != 0)
		{
			for(j = table[i].n; j --;)
			{
				/* These are malloc'ed as one. Only need to free the hash */
				free(table[i].r[j].hash);
				/* free(table[i].r[j].plain); */
			}

			free(table[i].r);
		}
	}
}

int cmp_records(const void *x, const void *y)
{
	return memcmp(((const record_t *)x)->hash, ((const record_t *)y)->hash, HASH_BYTE_SIZE);
}

void branch_insert(branch_t *branch, unsigned char *plain, unsigned char *hash)
{
	assert(branch->n <= branch->c);

	if(branch->c == 0)
	{
		branch->r = (record_t *)malloc(sizeof(record_t) * 256);
		branch->c = 256;
	}
	else if(branch->n == branch->c)
	{
		branch->c *= 2;
		branch->r = (record_t *)realloc((void *)branch->r, sizeof(record_t) * branch->c);
	}

	assert(branch->r);

	branch->r[branch->n].hash = hash;
	branch->r[branch->n].plain = plain;
	branch->n ++;
}
