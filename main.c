#define _MAIN_C_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <openssl/md5.h>

#include "table.h"

inline void usage(char *name);

int main(int argc, char **argv)
{
	//FILE *input;
	//FILE *output;

	/*struct
	{
		unsigned char *pool;
		int index;
		int cap;
	} spool;

	if(argc == 1)
	{
		usage(argv[0]);
	}
	if(strcmp(argv[1], "--build") == 0 && argc <= 3)
	{
		spool.pool = (unsigned char *)malloc(sizeof(unsigned char *) * 256);
		if(spool.pool == NULL)
		{}
	}
	else if(strcmp(argv[1], "--query") == 0 && argc <= 3)
	{
	}
	else
	{
		usage(argv[0]);
	}*/

	if(argc == (int)**argv){}

	int i;
	table_t table[TABLE_SIZE];
	FILE *in = fopen("words2.txt", "r");
	unsigned char *this;
	unsigned char *that;
	int c;

	table_init(table);

	i = 0;
	while(!feof(in))
	{
		i ++;
		if(i % 10000 == 0)
			printf("Read %d\n", i);

		this = (unsigned char *)malloc(256);
		*this = 0;
		for(c = fgetc(in); c != '\n' && c != EOF; c = fgetc(in))
			this[(int)((*this) ++) + 1] = (unsigned char)c;

		that = (unsigned char *)malloc(16);
		MD5(this + 1, (int)*this, that);

		/*for(c = 0; c < (int)*this; c ++)
			putchar(this[1 + c]);
		putchar('\t');
		for(c = 0; c < 16; c ++)
			printf("%02x", (int)that[c]);
		putchar('\n');*/

		table_insert(table, this, that);
	}

			printf("Read %d\n", i);

	table_finalize(table);

	for(c = 0; c < 80; c ++)
		printf("%d\n", table[c].n);

	unsigned char foo[16];
	while(!feof(stdin))
	{
		scanf(" ");

		int p, q;
		for(c = 0; c < 16; c ++)
		{
			p = getchar();
			q = getchar();

			if('0' <= p && p <= '9') p -= '0';
			else if ('a' <= p && p <= 'f') p = p -'a' + 10;
			else puts("no");

			if('0' <= q && q <= '9') q -= '0';
			else if ('a' <= q && q <= 'f') q = q - 'a' + 10;
			else puts("no");

			foo[c] = p * 16 + q;
		}

		unsigned char *bar = table_search(table, foo);

		if(bar == NULL)
			puts("No go");
		else
		{
			for(c = 0; c < *bar; c ++)
				putchar(bar[1 + c]);
			putchar('\n');
		}
	}

	return 0;
}

inline void usage(char *name)
{
	fprintf(stderr, "usage: %s <options>\n", name);
	fprintf(stderr, "  --build [wordlist=stdin] [table=stdout]   Reads a newline-delimited wordlist and builds the corresponding MD5 table\n");
	fprintf(stderr, "  --query <table> [queries=stdin]           Loads an existing MD5 table file and answers lookup queries\n");
	fprintf(stderr, "  --help                                    Displays this dialogue\n");

	exit(1);
}
