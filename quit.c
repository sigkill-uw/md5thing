/*
	quit.c - graceful quit function.
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

#define _QUIT_C_

#include <stdio.h>
#include <stdlib.h>

#include "quit.h"

/* This is kind of kludgy, and is already implemented to an extent in err.h. Whatever. */

/* Program name (ie. argv[0]). Set this from main before using quit */
char *program_name;

void quit(const char *mx, const char *my)
{
	/* If mx is NULL, we just die. Else, */
	if(mx)
	{
		/* Print the program name and mx */
		fputs(program_name, stderr);
		fputc(':', stderr);
		fputs(mx, stderr);

		/* Maybe my as well */
		if(my)
		{
			fputc(':', stderr);
			fputs(my, stderr);
		}

		fputc('\n', stderr);
	}

	/* Die. No cleanup, but it shouldn't be necessary. */
	exit(1);
}
