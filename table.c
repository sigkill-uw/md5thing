/*
	table.c - implementation of MD5 associative table structure.
	Copyright (C) 2015 Adam Richardson

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _TABLE_C_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include <openssl/md5.h>

#include "table.h"
#include "palloc.h"
#include "quit.h"

/* Initial capacity for a sub-list of the table */
#define BRANCH_INITIAL_CAP	256

/* Helper function. Just a vector insertion. It doesn't fail, in the sense that it'll just kill the program if it screws up */
inline void insert_blob(struct branch *br, unsigned char *blob);

/* Comparison function for use with qsort. A blob is a plaintext prepended with a hash - we just compare the hash. */
int cmp_blob(const void *x, const void *y);

/* Initialize a table */
void table_init(table_t table)
{
	int i;

	/* Every sub-array is empty and can't hold anything (since the pointer is garbage) */
	for(i = TABLE_SIZE; i --;)
		table[i].count = table[i].cap = 0;
}

/* Destroy a table */
void table_destroy(table_t table)
{
	/* This doesn't actually need to do anything. Allocation stuff is handled in palloc.c */
	(void)table;
}

int table_build(table_t table, FILE *list)
{
	/* Pascal-style strings for plaintext */
	unsigned char plain[MAX_PLAIN_LENGTH + 1];

	/* MD5 buffer is statically allocated by OpenSSL */
	unsigned char *digest;

	/* Data blob to be inserted into the table */
	unsigned char *blob;

	/* Counters/characters */
	int n, i, c;

	/* Read until EOF */
	for(n = 0, c = fgetc(list); c != EOF; n ++)
	{
		/* The code to read a plaintext line is probably going to pop up elsewhere, and so will probably be re-refactored */

		/* Read newline-delimited words, but only up to the appropriate length (255 on pretty much every platform) */
		for(i = 0; i < MAX_PLAIN_LENGTH && (c = fgetc(list)) != EOF && c != '\n'; i ++)
			plain[1 + i] = c;

		/* We had to store the length in i, because otherwise the Pascal-style counter would have overflowed */
		plain[0] = (unsigned char)i;

		/*
			FIXME: We should warn about the truncated input here, probably.
			FIXME: Also, should we check for an error indicator on the file? Or shall we just take EOF at face value?
		*/

		/* If the word is too long, just read until EOF or newline */
		if(i == MAX_PLAIN_LENGTH && c != EOF && c != '\n')
			while((c = fgetc(list)) != EOF && c != '\n');

		/* Compute the hash, ignoring the leading size byte in plain. The assertion definitely shouldn't fail. */
		digest = MD5(plain + 1, i, NULL);
		assert(digest);

		/* Take the tabular prefix */
		i = TABLE_PREFIX(digest);

		/* We need to store within the table the tail of the hash along with the entirety of the plaintext string */
		blob = palloc_alloc((HASH_SIZE - INDEX_SIZE) + (1 + (int)plain[0])); /* Shouldn't fail, at least not here */

		/* Copy everything over */
		memcpy(blob, digest + INDEX_SIZE, HASH_SIZE - INDEX_SIZE);
		memcpy(blob + HASH_SIZE - INDEX_SIZE, plain, 1 + (int)plain[0]);

		/* Inser the blob of data into the appropriate sub-list of the dictioanry */
		insert_blob(&table[i], blob); /* Can't fail here either */
	}

	/* Sort everythign so we can search it later. I'm not even sure how much this saves, but it's fun. */
	for(i = TABLE_SIZE; i --;)
		qsort(table[i].data, table[i].count, sizeof(*table[i].data), cmp_blob);

	/* Might as well return something useful - the count of all the items in the table */
	return n;
}

inline void insert_blob(struct branch *br, unsigned char *blob)
{
	/* A capacity of 0 indicates that the pointer is garbage */
	if(br->cap == 0)
	{
		/*
			Allocate room for some amount of data. As of this writing:
			256 slots across 256^2 different table sub-lists is enough for 16 million-ish hash:plain entries.
			Might tune this upwards after testing.
		*/

		br->data = (unsigned char **)malloc(sizeof(unsigned char **) * BRANCH_INITIAL_CAP);

		/*
			The same design choice alluded to elsewhere: do we die on failure,
			or do we act like malloc and return something sensible?
			I like the latter for now.
		*/
		if(!br->data)
			quit("error allocating store", strerror(errno));

		/* Capacity has changed */
		br->cap = BRANCH_INITIAL_CAP;

		/* NB: our initialization function for the table also set count = 0 */
	}
	else if(br->count == br->cap) /* If we're out of room, */
	{
		/* Double the capacity */
		br->cap *= 2;
		br->data = (unsigned char **)realloc(br->data, sizeof(unsigned char **) * br->cap);

		if(!br->data)
			quit("error allocating store", strerror(errno));
	}

	/* Copy the blob and increment the count */
	br->data[br->count ++] = blob;
}

int cmp_blob(const void *x, const void *y)
{
	/* Compare the hash tails to one another */
	return memcmp(x, y, HASH_SIZE - INDEX_SIZE);
}
