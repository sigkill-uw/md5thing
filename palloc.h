/*
	palloc.h - header file for pooled string allocator
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

#ifndef _PALLOC_H_
#define _PALLOC_H_

/* Initialize/destroy the pooled allocator subsystem */
void palloc_init(void);
void palloc_quit(void);

/*
	Allocate an array of unsigned char of at least `size` byte.
	This doesn't return NULL on failure; in the spirit of our very specific use case,
	it just kills the program.
*/
unsigned char *palloc_alloc(int size);

#endif
