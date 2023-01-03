# freemap

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A minimal C library to store a bitmap of free space (blocks, sectors etc). Freemap detects a 32 or 64 bit architecture and will compile to use the system width efficiently.

## Building

freemap uses [CMake](https://cmake.org/) so building is trivial:

```bash
mkdir build
cd build
cmake ..
make
```

You'll find the `libfreemap.a` in the `build/lib` folder. 

## Testing

freemap uses [GoogleTest](https://github.com/google/googletest) which is installed by **CMake**:

```bash
mkdir build
cd build
cmake ..
make
./freemap_test
```

## Constants

```C
FREEMAP_STATUS_SUCCESS 		0b00000001
FREEMAP_STATUS_FAILURE 		0b00000010
FREEMAP_STATUS_ALLOCATED   	0b00000100
FREEMAP_STATUS_DEALLOCATED	0b00001000
````

These constants are used as flags of of the `status` field in `freemap_result_t`, see below.

## Structures

### `freemap_t`

```C
struct {
	freemap_store_type_t *bitmap;
	uint32_t total;
	uint32_t free;
} freemap_t;
```

`freemap_t` represents a freemap. `bitmap` is a pointer  `total` is the number of blocks handled by the freemap, and `free` is the number of blocks currently free.


### `freemap_result_t`

```C
struct {
	uint8_t status;
	uint32_t block;
	freemap_t *map;
} freemap_result_t;
```

`freemap_result_t` holds the result of an allocation, deallocation or check. The `map` field will always point to the freemap the result pertains to.

## Functions

Freemap stores whether blocks on an arbitary resource are free or not, packed into a bitmap. To use, create a new freemap using `freemap_new`. Remember to free the memory used when you're done using `freemap_destroy`.

### `freemap_t *freemap_new(uint32_t blocks)`

Create a new freemap to store the status of the specified number of blocks and return a pointer to it.

### `freemap_result_t freemap_check(freemap_t *map, uint32_t block);`

Return a result specifying whether the specified block is allocated. If successful, the `freemap_result_t` will have `FREEMAP_STATUS_SUCCESS` and either the `FREEMAP_STATUS_ALLOCATED` or `FREEMAP_STATUS_DEALLOCATED` flags on the `status` field will be set. The `block` field will contain the block number. 

If the specified block is larger than the total number of blocks in the freemap, the `freemap_result_t` status will have `FREEMAP_STATUS_FAILURE` set, the `block` field will contain the block number.

### `freemap_result_t freemap_allocate(freemap_t *map)`

Allocate the next available block. If successful, the `freemap_result_t` will have `FREEMAP_STATUS_SUCCESS` and `FREEMAP_STATUS_ALLOCATED` flags on the `status` field set. The `block` field will contain the allocated block number. 

If there are no blocks to allocate, the `freemap_result_t` will have `FREEMAP_STATUS_FAILURE` flag on the `status` field set. 

Blocks are numbered from `0` so in a 64 block freemap, the number of the blocks will range from `0-63`. 

### `freemap_result_t freemap_deallocate(freemap_t *map, uint32_t block)`

Deallocate a specified block. On success, the `freemap_result_t` status will have `FREEMAP_STATUS_SUCCESS` set - whether the specified block was allocated or not can be seen from the `FREEMAP_STATUS_ALLOCATED` and `FREEMAP_STATUS_DEALLOCATED` flags on the `status` field of the result. The `block` field will contain the block number.

If the specified block is larger than the total number of blocks in the freemap, the `freemap_result_t` status will have `FREEMAP_STATUS_FAILURE` set, the `block` field will contain the block number.

### `uint32_t freemap_sizeof(freemap_t *map)`

Return the size in bytes of the current bitmap. This can be used with `freemap_t->bitmap` to load and save the bitmap - be careful to sync the new bitmap using the `freemap_sync` function once loaded.

### `uint32_t freemap_sync(freemap_t *map, uint32_t total)`

Recompute the free blocks available after directly modifying the bitmap. Returns the number of free blocks.

### `void freemap_destroy(freemap_t *map)`

Free the memory used by the freemap. 

**CAVEAT:** Do not deference the map pointer once you have called this function.