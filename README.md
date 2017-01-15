# pdpoolalloc
Public Domain Object Pool in ANSI C

Written by Jordan Halase. Credit is not required, although would be appreciated.

Has absolutely no dependencies aside from `stddef.h` which defines `NULL`.

Hard real-time safe by default. If you choose for your objects to allocate memory, then ensure to deallocate it when each object is freed back into the pool.

Also has the ability to steal objects if the pool is full, which is useful for synthesizer voice allocation, although will return `NULL` by default when the pool is full.

## How to use
You modify the source code and incorporate it into your project. This project is nothing more than a starting template, as the correctness of building an object pool from scratch is difficult to determine.

Iterating the pool is easy. It is a doubly linked list.
```c
struct Object *elem;
for (elem = pool->first; elem; elem = elem->next) {
	/* Do things with elem except for calling obj_free() on it */
}
```

Be sure you do not lose `elem->next` if you are freeing objects from the pool.
```c
struct Object *elem, *next;
for (elem = pool->first; elem; elem = next) {
	next = elem->next;
	/* elem->destroy(elem); */
	obj_free(pool, elem);
	/* elem->next is lost after call to obj_free() */
}
```

You can risk memory leaks or corrupted state if you use an object after you freed it back into the pool. It is good practice to set every object to `NULL` after freeing it. (We did not set elem to `NULL` in the above for() loop because it was an iterator and no other code was written after it was freed.)

```c
/* obj->destroy(obj); */
obj_free(pool, obj);
obj = NULL;
```

