/*
	main.c - entry point for md5thing.
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

#define _MAIN_C_

#include <stdio.h>

#include "quit.h"

int bmain(int argc, char **argv);
int pmain(int argc, char **argv);
void usage(void);

int main(int argc, char **argv)
{
	/* In quit.c */
	program_name = argv[0];
	(void)argc;

	return 0;
}
