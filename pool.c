#include <stddef.h>

struct Object{
	struct Object *next, *prev;
	int value;
};

#define POOL_SIZE 8

struct Pool {
	struct Object *first, *last, *firstfree, *lastfree;
	struct Object objects[POOL_SIZE];
};

/* This can be used to empty the pool,
 * but not if any object allocates its own memory
 */
void pool_init(struct Pool *pool)
{
	pool->first = NULL;
	pool->last = NULL;
	pool->firstfree = &pool->objects[0];
	pool->lastfree = &pool->objects[POOL_SIZE-1];
	if (POOL_SIZE < 2) {
		pool->objects[0].next = NULL;
	} else {
		int i;
		for (i = 0; i < POOL_SIZE-1; ++i) {
			pool->objects[i].next = &pool->objects[i+1];
		}
		pool->objects[i].next = NULL;
	}
}

/* O(n) computation */
int pool_population(struct Pool *pool)
{
	struct Object *elem;
	int population = 0;
	for (elem = pool->first; elem; elem = elem->next) {
		++population;
	}
	return population;
}

/* Can return NULL if full and stealing is not enabled */
struct Object *obj_new(struct Pool *pool)
{
	struct Object *obj;
	if (pool->firstfree) {
		obj = pool->firstfree;
		pool->firstfree = pool->firstfree->next;
	} else {
		return NULL;

		/* Steal the first object,
		 * remember to deallocate any memory allocated by the object
		 */
		/*
		 *obj = pool->first;
		 *if (obj->prev) {
		 *        obj->prev = pool->last;
		 *}
		 *if (obj->next) {
		 *        pool->first = obj->next;
		 *        obj->next->prev = NULL;
		 *}
		 */
	}
	if (obj == pool->lastfree) {
		pool->lastfree = NULL;
	}
	obj->prev = pool->last;
	if (!pool->first) {
		pool->first = obj;
	}
	if (pool->last) {
		pool->last->next = obj;
	}
	pool->last = obj;
	obj->next = NULL;
	return obj;
}

/* Remember to deallocate any memory allocated by the object before freeing */
void obj_free(struct Pool *pool, struct Object *obj)
{
	if (obj->prev) {
		obj->prev->next = obj->next;
	}
	if (obj->next) {
		obj->next->prev = obj->prev;
	}
	if (obj == pool->first) {
		pool->first = obj->next;
	}
	if (obj == pool->last) {
		pool->last = obj->prev;
	}
	obj->prev = pool->lastfree;
	if (pool->lastfree) {
		pool->lastfree->next = obj;
	}
	if (!pool->firstfree) {
		pool->firstfree = obj;
	}
	pool->lastfree = obj;
	obj->next = NULL;
}

#include <stdio.h>
#include <stdlib.h>

void printpopulation(struct Pool *pool)
{
	int population = pool_population(pool);
	printf("Pool population: %d\n\n", population);
}

void printcontents(struct Pool *pool)
{
	struct Object *elem;
	printf("Contents: ");

	/* Iterate through the pool */
	for (elem = pool->first; elem; elem = elem->next) {
		printf("'%d' ", elem->value);
	}
	printf("\n\n");
}

int main()
{
	struct Pool *pool = malloc(sizeof(*pool));
	struct Object *elem, *next;
	int i;

	pool_init(pool);
	printf("Pool initialized\n");
	printpopulation(pool);

	/* Populate the pool */
	for (i = 0; i < POOL_SIZE; ++i) {
		elem = obj_new(pool);
		if (!elem)
			break;
		elem->value = i;
	}

	printf("Pool populated\n");
	printpopulation(pool);
	printcontents(pool);

	/* Attempt to overpopulate */
	elem = obj_new(pool);
	if (elem) {
		elem->value = i;
		printf("Pool overpopulated\n");
	} else {
		printf("Stealing was not enabled\nPool not overpopulated\n");
	}

	printpopulation(pool);
	printcontents(pool);

	/* Remove an object from the pool */
	for (elem = pool->first; elem; elem = next) {
		next = elem->next;
		if (elem->value == POOL_SIZE/2) {
			obj_free(pool, elem);
		}
	}

	printf("Element removed: '%d'\n", POOL_SIZE/2);
	printpopulation(pool);
	printcontents(pool);

	elem = obj_new(pool);
	elem->value = -1;
	printf("Element added: '%d'\n", elem->value);
	printcontents(pool);

	/* Empty the pool */
	for (elem = pool->first; elem; elem = next) {
		next = elem->next;
		obj_free(pool, elem);
	}

	printf("Emptied pool\n");
	printpopulation(pool);

	/* Re-overpopulate the pool */
	for (i = 0; i < POOL_SIZE+1; ++i) {
		elem = obj_new(pool);
		if (!elem)
			break;
		elem->value = i;
	}

	printf("Pool re-overpopulated\n");
	printpopulation(pool);
	printcontents(pool);

	/* Empty the pool */
	for (elem = pool->first; elem; elem = next) {
		next = elem->next;
		obj_free(pool, elem);
	}

	printf("Emptied pool\n");
	printpopulation(pool);

	free(pool);
	return 0;
}

