# pdpoolalloc
 Public Domain Object Pool in ANSI C

Hard real-time safe by default. Ensure to deallocate any memory allocated by your objects if you do so.

Also has the ability to steal objects if the pool is full, which is useful for synthesizer voice allocation.
