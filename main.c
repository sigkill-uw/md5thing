#define _MAIN_C_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "table.h"

inline void usage(char *name);
inline int bmain(int argc, char **argv);
inline int qmain(int argc, char **argv);

int main(int argc, char **argv)
{
	if(argc == 1)
	{
		usage(argv[0]);
		return 1;
	}
	else if(argc <= 4)
	{
		if(strcmp(argv[1], "--build") == 0)
		{
			return bmain(argc, argv);
		}
		else if(argc >= 2 && strcmp(argv[1], "--query") == 0)
		{
			return qmain(argc, argv);
		}
		else
		{
			usage(argv[0]);
			return 1;
		}
	}
	else
	{
		usage(argv[0]);
		return 1;
	}
}

inline int bmain(int argc, char **argv)
{
	table_t table[TABLE_SIZE];
	FILE *words;
	FILE *output;

	words = (argc > 2) ? fopen(argv[2], "r") : stdin;
	assert(words); /* FIXME */

	table_populate(table, words);

	output = (argc > 3) ? fopen(argv[3], "wb") : stdout;
	assert(output);	/* FIXME */

	table_write(table, output);

	if(output != stdout)
		fclose(output);

	table_destroy(table);

	return 0;
}

inline int qmain(int argc, char **argv)
{
	FILE *db;
	FILE *input;
	int i, j, k;
	table_t table[TABLE_SIZE];
	unsigned char buf[16];
	unsigned char *res;

	db = fopen(argv[2], "rb");
	assert(db); /* FIXME */

	table_read(table, db);

	fclose(db);

	input = (argc > 3) ? fopen(argv[3], "r") : stdin;

	do
	{
		fscanf(input, " ");

		for(i = 0; i < 16; i ++)
		{
			j = fgetc(input);
			k = fgetc(input);

			if('0' <= j && j <= '9') j -= '0';
			else if('a' <= j && j <= 'f') j -= 'a' - 10;
			else if('A' <= j && j <= 'F') j -= 'A' - 10;
			else assert(0);	/* FIXME */

			if('0' <= k && k <= '9') k -= '0';
			else if('a' <= k && k <= 'f') k -= 'a' - 10;
			else if('A' <= k && k <= 'F') k -= 'A' - 10;
			else assert(0);	/* FIXME */

			buf[i] = 16 * j + k;
		}

		res = table_search(table, buf);
		if(res == NULL)
			puts("Nope.");
		else
		{
			for(i = 0; i < res[0]; i ++)
				putchar(res[1 + i]);
			putchar('\n');
		}
	} while(!feof(input));

	fclose(input);

	return 0;
}

inline void usage(char *name)
{
	fprintf(stderr, "usage: %s <options>\n", name);
	fprintf(stderr, "  --build [wordlist=stdin] [table=stdout]   Reads a newline-delimited wordlist and builds the corresponding MD5 table\n");
	fprintf(stderr, "  --query <table> [queries=stdin]           Loads an existing MD5 table file and answers lookup queries\n");
	fprintf(stderr, "  --help                                    Displays this dialogue\n");
}
