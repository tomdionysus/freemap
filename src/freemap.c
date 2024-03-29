#include <stdint.h>
#include <stdlib.h>

#include "freemap.h"

freemap_t *freemap_new(uint32_t blocks) {
	freemap_t *map = (freemap_t*)malloc(sizeof(freemap_t));
	map->bitmap = (freemap_store_type_t*)calloc(blocks/FREEMAP_BITS,1);
	map->total = blocks;
	map->free = blocks;
	return map;
}

freemap_result_t freemap_check(freemap_t *map, uint32_t block) {
	freemap_result_t result;
	result.map = map;

	// Detect overflow
	if(block > map->total-1) {
		result.status = FREEMAP_STATUS_FAILURE;
		result.block = block;
		return result;
	}

	uint32_t offset = (block / FREEMAP_BITS);
	uint8_t bit = (block % FREEMAP_BITS);
	freemap_store_type_t checkword = map->bitmap[offset];
	result.block = block;
	result.status = FREEMAP_STATUS_SUCCESS;
	if(checkword & (freemap_store_type_t)(1UL << bit)) {
		result.status |= FREEMAP_STATUS_ALLOCATED;
	} else {
		result.status |= FREEMAP_STATUS_DEALLOCATED;
	}

	return result;
}

freemap_result_t freemap_allocate(freemap_t *map) {
	freemap_result_t result;
	result.map = map;

	for(uint32_t offset = 0; offset < (map->total / FREEMAP_BITS + 1); offset++) {
		freemap_store_type_t cw = map->bitmap[offset];
		if(cw != FREEMAP_FULL) {
			uint8_t bit = _freemap_firstfree(cw);

			if(bit==0xFF) {
				// In theory this can't happen, something has gone badly wrong.
				result.status = FREEMAP_STATUS_FAILURE;
				return result;
			}
			
			if(offset * FREEMAP_BITS + bit >= map->total) break;

			map->bitmap[offset] |= (1UL << bit);

			result.block = offset * FREEMAP_BITS + bit;
			result.status = FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED;

			map->free--;

			return result;
		}
	} 

	result.block = 0;
	result.status = FREEMAP_STATUS_FAILURE;

	return result;
}

freemap_result_t freemap_deallocate(freemap_t *map, uint32_t block) {
	freemap_result_t result;
	result.map = map;

	// Detect overflow
	if(block > map->total-1) {
		result.status = FREEMAP_STATUS_FAILURE;
		result.block = block;
		return result;
	}

	uint32_t offset = (block / FREEMAP_BITS);
	uint8_t bit = (block % FREEMAP_BITS);
	freemap_store_type_t checkword = map->bitmap[offset];
	result.block = block;
	result.status = FREEMAP_STATUS_SUCCESS;
	if(checkword & (freemap_store_type_t)(1UL << bit)) {
		result.status |= FREEMAP_STATUS_DEALLOCATED;
	}
	map->free++;
	map->bitmap[offset] &= (freemap_store_type_t)(~(1UL << bit));

	return result;
}

uint32_t freemap_sync(freemap_t *map, uint32_t total) {
	map->total = total;
	map->free = total;
	for(uint32_t offset = 0; offset < (map->total / FREEMAP_BITS); offset++) {
		freemap_store_type_t cw = map->bitmap[offset];
		map->free -= FREEMAP_HAMMING(cw & FREEMAP_FULL);
	}
	return map->free;
}

void freemap_destroy(freemap_t *map) {
	free(map->bitmap);
	free(map);
}

uint32_t freemap_sizeof(freemap_t *map) {
	return map->total * sizeof(freemap_store_type_t);
}

uint8_t _freemap_firstfree(freemap_store_type_t num) {
	for(uint8_t i=0; i<FREEMAP_BITS; i++) {
		if(!(num & (1UL << i))) return i;
	}

	return 0xFF;
}
