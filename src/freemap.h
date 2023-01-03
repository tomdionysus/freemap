#ifndef FREEMAP
#define FREEMAP

#include <stdint.h>
#include <stdlib.h>

#define FREEMAP_BITS 				32

#define FREEMAP_STATUS_SUCCESS 		0b00000001
#define FREEMAP_STATUS_FAILURE 		0b00000010
#define FREEMAP_STATUS_ALLOCATED   	0b00000100
#define FREEMAP_STATUS_DEALLOCATED	0b00001000


typedef struct freemap {
	uint32_t *bitmap;
	uint32_t total;
	uint32_t free;
} freemap_t;

typedef struct freemap_result {
	uint8_t status;
	uint32_t block;
	freemap_t *map;
} freemap_result_t;

freemap_t *freemap_new(uint32_t blocks);
freemap_result_t freemap_check(freemap_t *map, uint32_t block);
freemap_result_t freemap_allocate(freemap_t *map);
freemap_result_t freemap_deallocate(freemap_t *map, uint32_t block);
void freemap_destroy(freemap_t *map);

uint8_t _freemap_firstfree(uint32_t num);

#endif // FREEMAP