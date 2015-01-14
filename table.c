#define _TABLE_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "table.h"

int cmp_records(const void *x, const void *y);
void branch_insert(branch_t *branch, unsigned char *plain, unsigned char *hash);

void table_init(table_t *table)
{
	int i;
	for(i = TABLE_SIZE; i --;)
		table[i].n = table[i].c = 0;
}

void table_insert(table_t *table, unsigned char *plain, unsigned char *hash)
{
	branch_insert(&table[(int)hash[0] + 256 * (int)hash[1]], plain, hash);
	//printf("(%d, %d)\n", table[(int)hash[0] + 256 * (int)hash[1]].n, table[(int)hash[0] + 256 * (int)hash[1]].c);
}

void table_finalize(table_t *table)
{
	int i;
	for(i = TABLE_SIZE; i --;)
		qsort(table[i].r, table[i].n, sizeof(*table[i].r), cmp_records);
}

void table_read(table_t *table, FILE *input)
{
	table = (table_t *)NULL;
	input = (FILE *)NULL;
	if(table && input) {}
}

void table_write(table_t *table, FILE *output)
{
	table = (table_t *)NULL;
	output = (FILE *)NULL;
	if(table && output) {}
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
	int i;
	for(i = TABLE_SIZE; i --;)
		if(table[i].c != 0)
			free(table[i].r);
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

	branch->r[branch->n].plain = plain;
	branch->r[branch->n].hash = hash;
	branch->n ++;
}
