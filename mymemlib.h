#ifndef MYMEMLIB_H
#define MYMEMLIB_H

void* mymalloc(size_t);
int myfree(void*);
void* myrealloc(void*, size_t);
void* mycalloc(size_t, size_t);

#endif
