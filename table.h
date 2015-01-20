/*
	table.h - header file for MD5 associative dictionary structure
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

#ifndef _TABLE_H_
#define _TABLE_H_

/* An MD5 hash is 16 bytes */
#define HASH_SIZE	16

/* We use the first 2 bytes as indexes into a dictionary */
#define INDEX_SIZE	2
#define TABLE_SIZE	(256 * 256)

/* Given a hash, computers the "table prefix" - that is, the integer index into the table - from the first n bytes. */
/* Maybe should be function? FIXME? */
#define TABLE_PREFIX(c)	((int)((c)[0]) + (int)((c)[1]) * 256)

/* 255 allows us to use Pascal-style strings */
#define MAX_PLAIN_LENGTH	255

/* Table structure */
typedef struct branch
{
	int count;				/* The number of data nodes stored herein */
	int cap;				/* The maximum number of data nodes we can store, pending a realloc */
	unsigned char **data;	/* An array of actual data. Each item is a blob containing the tail of the hash and the plaintext */
} table_t[TABLE_SIZE];

/*
	A table is a dictionary. It associates an MD5 hash value with a corresponding plaintext.
	The first n bytes of the hash are used as an index into an array. This is a "hash table", but it is not related to "MD5 hashing".
	The remaining (h - n) bytes are associated with the appropriate plaintext and inserted into a sorted array (for ez binary search).
*/

/* Initialize/destroy a table structure */
void table_init(table_t table);
void table_destroy(table_t table);

const unsigned char *table_search(table_t table, char hash[HASH_SIZE]);

int table_build(table_t table, FILE *list);
int table_read(table_t table, FILE *in);
void table_write(table_t table, FILE *out);

#endif
