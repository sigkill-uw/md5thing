/*
	quit.h - header file for graceful quit function.
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

#ifndef _QUIT_H_
#define _QUIT_H_

/* In quit.c */
extern char *program_name;

/* Prints some messages (NULL if nothing is to be printed) and dies, yielding a value of 1 to the operating system */
void quit(const char *mx, const char *my);

#endif
