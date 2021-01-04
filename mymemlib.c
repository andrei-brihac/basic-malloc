#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef struct block_info block_info;

const size_t ALIGNMENT = 8;

size_t _byteAlign(size_t size, const size_t alignment) {
	return (size + (alignment - 1)) & ~(alignment - 1);
}

struct block_info {
	size_t size;
	block_info* next;
	bool free;
};

const size_t INFO_SIZE = sizeof(block_info);
void* first = NULL;

void _blockStatus() {  // Prints the block list, for debug
	block_info* current = first;
	while (current != NULL) {
		printf("%ld | %p | %p | %d\n", current->size, current, current->next, current->free);
		current = current->next;
	}
	printf("\n");
}

block_info* _requestMemory(block_info* last, size_t size) {
	block_info* block;
	block = sbrk(0);
	void* request = sbrk(size);
	if (block != request) {  // If the two program breakpoint adresses don't match, error!
		return NULL;
	}
	if (request == (void*) -1) {  // Function sbrk has an error.
		return NULL;
	}
	if (last != NULL) {
		last->next = block;
	}
	block->size = size;
	block->next = NULL;
	block->free = false;
	return block;
}

block_info* _findFreeMemory(block_info** last, size_t size) {
	block_info* current = first;
	while (current != NULL && (current->size < size || current->free == false)) {
		*last = current;
		current = current->next;
	}
	return current;
}

void _splitBlock(block_info* ptr, size_t size) {
	block_info* free_block = (void*) ptr + size;
	free_block->size = ptr->size - size;
	free_block->next = ptr->next;
	free_block->free = true;
	ptr->next = free_block;
	ptr->size = size;
}

void* mymalloc(size_t size) {
	block_info* block;
	size = _byteAlign(size + INFO_SIZE, ALIGNMENT);
	if (size <= 0) {
		return NULL;
	}
	if (first == NULL) {  // Branch for the first time this function is called.
		block = _requestMemory(NULL, size);
		if (block == NULL) {  // Error check.
			return NULL;
		}
		first = block;
	} else {
		block_info* last = first;
		block = _findFreeMemory(&last, size);
		if (block == NULL) {  // Free memory is not found, so it is allocated.
			block = _requestMemory(last, size);
			if (block == NULL) {  // Error check.
				return NULL;
			}
		}
		else {  // Free memory is found, split if necessary and used.
			if (block->size > size) {
				_splitBlock(block, size);
			}
			block->free = false;
		}
	}
	// _blockStatus();
	return block + 1;
}

block_info* _getInfoPtr(void* ptr) {
	return (block_info*) ptr - 1;
}

void _mergeBlocks() {  // Iterating through block list and merging free adjacents = O(n)
			// Advantageous when there aren't many free() calls
			// Could turn list into doubly-linked and merge prev, current and next = O(1)
	block_info* current = first;
	while (current->next != NULL) {
		if (current->free == true && current->next->free == true) {
			current->size += current->next->size;
			current->next = current->next->next;
		}
		else {
			current = current->next;
		}
	}
}

int myfree(void* ptr) {
	if (ptr == NULL) {
		return 0;
	}
	block_info* info_ptr = _getInfoPtr(ptr);
	if (info_ptr->free == true) {  // If trying to free an already freed block.
		return -1;
	}
	info_ptr->free = true;	
	_mergeBlocks();
	// _blockStatus();
	return 0;
}

void* myrealloc(void* ptr, size_t size) {
	if (ptr == NULL) {
		return mymalloc(size);
	}
	block_info* info_ptr = _getInfoPtr(ptr);
	if (size < info_ptr->size) {
		_splitBlock(info_ptr, info_ptr->size);
		return ptr;
	}
	void* new_ptr = mymalloc(size);
	if (new_ptr == NULL) {  // Error allocating space.
		return NULL;
	}
	memcpy(new_ptr, ptr, info_ptr->size);
	myfree(ptr);
	// _blockStatus();
	return new_ptr;
}

void* mycalloc(size_t nelem, size_t elsize) {
	size_t size = nelem * elsize;
	if (nelem != size / elsize) {
		return NULL;
	}
	void* new_ptr = mymalloc(size);
	memset(new_ptr, 0, size);
	// _blockStatus();
	return new_ptr;
}
