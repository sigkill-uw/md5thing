/*
	palloc.c - pooled string allocator.
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

#define _PALLOC_C_

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "palloc.h"
#include "quit.h"

/*
	The motivation here is to trim the fat introduced by the surfeit of malloc calls in the old design.
	I don't have precise numbers and they will vary from implimentation to implementation,
	but there's certainly both a memory and processing overhead to calls to malloc.
	For a memory-bound program allocating many small buffers, I believe it's quite significant.
	Given that, in the current design, strings don't need to be individually freed, we can avoid the overhead
	of many malloc calls by allocating strings from large buffers.

	It would be quite simple to create single large pool, dynamically resizeable via realloc;
	however, a call to realloc can invalidate the old pointer, and hence invalidate every string in the pool.
	No good.

	This solution does create some wastage, but not more than (HASH_SIZE - INDEX_SIZE + MAX_PLAIN_LENGTH)
	[300ish] bytes per pool, against about 64MB of data per pool.
*/

/*
	STRIDE is the length any given pool
	MAX_POOLS is the maximum number of individual pools we might allocate
	These particular values imply a maximum heap size of 64GB - enough for the forseeable future
*/
#define STRIDE		(64 * 1024 * 1024)
#define MAX_POOLS	1024

/*
	pin represents the index of the pool currently processed.
	vin is an index into that pool, representing the next value return by palloc_alloc.
	pools is just an array of pointers, each being a pool.
*/
static int pin;
static unsigned char *pools[MAX_POOLS];
static int vin;

/* Initialize the state. For these values, the first call to palloc_alloc should allocate a new pool and reset the indices */
void palloc_init(void)
{
	pin = -1;
	vin = STRIDE;
}

/* Free all the pools. For our specific use case, this is super unnecessary, but it's grating to leave it out */
void palloc_quit(void)
{
	/* Free everything */
	while(pin >= 0)
		free(pools[pin --]);
}

/* Return a pointer to at least `size` contiguous unsigned chars  */
unsigned char *palloc_alloc(int size)
{
	/* Wild assumption: we aren't serving huge requests */
	assert(size <= STRIDE);

	/* If we can't fit the request in the current pool, */
	if(vin + size >= STRIDE)
	{
		++ pin;

		/* Another wild assumption: we're not allocating insane amounts of memory in general */
		assert(pin < MAX_POOLS);

		/* Move on and allocate the next one */
		pools[pin] = (unsigned char *)malloc(size);

		/*
			Design choice: should we be consistent with malloc and return null on failure?
			Or should we be cognizant of our very specific use case and just kill the program?
			For the moment, it's the latter.
		*/
		if(!pools[pin])
			quit("error allocating store", strerror(errno));

		/* Reset index */
		vin = 0;
	}

	/* Increment index and return the appropriate pointer */
	vin += size;
	return pools[pin] + vin - size;
}
