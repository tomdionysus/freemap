#include <gtest/gtest.h>

extern "C" {
  #include "freemap.h"
}

// Instantiate, check 
TEST(FreemapTest, NewDestroy) {
  freemap_t *map = freemap_new(127);

  EXPECT_NE(map->bitmap, nullptr);
  EXPECT_EQ(map->total, 127);
  EXPECT_EQ(map->free, 127);

  freemap_destroy(map);
}

// Allocate / Deallocate
TEST(FreemapTest, AllocateDeallocate16Bits) {
  // Result general
  freemap_result_t res;

  // Make a new freemap
  freemap_t *map = freemap_new(16);

  // Initial state checks
  EXPECT_NE(map->bitmap, nullptr);
  EXPECT_EQ(map->total, 16);
  EXPECT_EQ(map->free, 16);

  // Allocate 16 blocks
  for(uint8_t i=0; i<16; i++) {
    res = freemap_allocate(map);
    EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
    EXPECT_EQ(res.block, i);
    EXPECT_EQ(res.map, map);
    EXPECT_EQ(map->free, 15-i);
  }

  // Expect free space to be zero
  EXPECT_EQ(map->free, 0);

  // Expect next allocation to fail
  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_FAILURE);
  EXPECT_EQ(res.block, 0);
  EXPECT_EQ(res.map, map);

  // Deallocate a block in the middle
  res = freemap_deallocate(map, 5);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_DEALLOCATED);
  EXPECT_EQ(res.block, 5);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 1);

  // Ensure the bitmap is accurate
  EXPECT_EQ(map->bitmap[0],0b1111111111011111);

  // Reallocate the block in the middle
  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
  EXPECT_EQ(res.block, 5);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 0);

  // Cleanup
  freemap_destroy(map);
}


// Check / Deallocate Out of Range
TEST(FreemapTest, CheckDeallocateOutOfRange) {
  // Result general
  freemap_result_t res;

  // Make a new freemap
  freemap_t *map = freemap_new(16);

  // Initial state checks
  EXPECT_NE(map->bitmap, nullptr);
  EXPECT_EQ(map->total, 16);
  EXPECT_EQ(map->free, 16);

  // Allocate 8 blocks
  for(uint8_t i=0; i<16; i+=2) {
    res = freemap_allocate(map);
    EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
    EXPECT_EQ(res.block, i/2);
    EXPECT_EQ(res.map, map);
    EXPECT_EQ(map->free, 15-(i/2));
  }

  // Check block 7,8
  res = freemap_check(map, 7);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
  EXPECT_EQ(res.block, 7);
  EXPECT_EQ(res.map, map);

  res = freemap_check(map, 8);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_DEALLOCATED);
  EXPECT_EQ(res.block, 8);
  EXPECT_EQ(res.map, map);

  // Expect free space to be eight
  EXPECT_EQ(map->free, 8);

  // Expect next allocation to be 8
  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
  EXPECT_EQ(res.block, 8);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 7);

  // Deallocate a block past the end
  res = freemap_deallocate(map, 16);
  EXPECT_EQ(res.status, FREEMAP_STATUS_FAILURE);
  EXPECT_EQ(res.block, 16);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 7);

  // Check a block past the end 
  res = freemap_check(map, 16);
  EXPECT_EQ(res.status, FREEMAP_STATUS_FAILURE);
  EXPECT_EQ(res.block, 16);
  EXPECT_EQ(res.map, map);

  // Cleanup
  freemap_destroy(map);
}

// Allocate / Deallocate Larger than 48 bits
TEST(FreemapTest, AllocateDeallocate48Bits) {
  // Result general
  freemap_result_t res;

  // Make a new freemap
  freemap_t *map = freemap_new(48);

  // Initial state checks
  EXPECT_NE(map->bitmap, nullptr);
  EXPECT_EQ(map->total, 48);
  EXPECT_EQ(map->free, 48);

  // Allocate 48 blocks
  for(uint8_t i=0; i<48; i++) {
    res = freemap_allocate(map);
    EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
    EXPECT_EQ(res.block, i);
    EXPECT_EQ(res.map, map);
    EXPECT_EQ(map->free, 47-i);
  }

  // Expect free space to be zero
  EXPECT_EQ(map->free, 0);

  // Expect next allocation to fail
  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_FAILURE);
  EXPECT_EQ(res.block, 0);
  EXPECT_EQ(res.map, map);

  // Deallocate some blocks in the middle
  res = freemap_deallocate(map, 21);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_DEALLOCATED);
  EXPECT_EQ(res.block, 21);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 1);

  res = freemap_deallocate(map, 30);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_DEALLOCATED);
  EXPECT_EQ(res.block, 30);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 2);

  res = freemap_deallocate(map, 47);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_DEALLOCATED);
  EXPECT_EQ(res.block, 47);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 3);

  // Reallocate blocks in the middle
  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
  EXPECT_EQ(res.block, 21);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 2);

  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
  EXPECT_EQ(res.block, 30);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 1);

  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
  EXPECT_EQ(res.block, 47);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 0);

  // Cleanup
  freemap_destroy(map);
}

// Allocate / Deallocate Larger than 113 bits
TEST(FreemapTest, AllocateDeallocate113Bits) {
  // Result general
  freemap_result_t res;

  // Make a new freemap
  freemap_t *map = freemap_new(113);

  // Initial state checks
  EXPECT_NE(map->bitmap, nullptr);
  EXPECT_EQ(map->total, 113);
  EXPECT_EQ(map->free, 113);

  // Allocate 113 blocks
  for(uint8_t i=0; i<113; i++) {
    res = freemap_allocate(map);
    EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
    EXPECT_EQ(res.block, i);
    EXPECT_EQ(res.map, map);
    EXPECT_EQ(map->free, 112-i);
  }

  // Expect free space to be zero
  EXPECT_EQ(map->free, 0);

  // Expect next allocation to fail
  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_FAILURE);
  EXPECT_EQ(res.block, 0);
  EXPECT_EQ(res.map, map);

  // Deallocate blocks in the middle
  res = freemap_deallocate(map, 37);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_DEALLOCATED);
  EXPECT_EQ(res.block, 37);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 1);

  res = freemap_deallocate(map, 51);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_DEALLOCATED);
  EXPECT_EQ(res.block, 51);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 2);

  res = freemap_deallocate(map, 111);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_DEALLOCATED);
  EXPECT_EQ(res.block, 111);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 3);

  // Reallocate the block in the middle
  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
  EXPECT_EQ(res.block, 37);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 2);

  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
  EXPECT_EQ(res.block, 51);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 1);

  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
  EXPECT_EQ(res.block, 111);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 0);

  // Cleanup
  freemap_destroy(map);
}

// Allocate / Deallocate
TEST(FreemapTest, Sync) {
  // Make a new freemap
  freemap_t *map = freemap_new(110);

  // Set the first word (32 or 64 bits) to 11111....
  map->bitmap[0] = FREEMAP_FULL;
  map->bitmap[1] = 0UL;

  EXPECT_EQ(freemap_sync(map, 110), 110-FREEMAP_BITS);
  EXPECT_EQ(map->free, 110-FREEMAP_BITS);

  // Cleanup
  freemap_destroy(map);
}

// Freemap _firstfree
TEST(FreemapTest, FirstFree) {
  EXPECT_EQ(_freemap_firstfree(0), 0);
  EXPECT_EQ(_freemap_firstfree(1), 1);
  EXPECT_EQ(_freemap_firstfree(0b1111), 4);
  EXPECT_EQ(_freemap_firstfree(0b1011), 2);

  EXPECT_EQ(_freemap_firstfree(0b11111111), 8);
}

// Hamming Test
TEST(FreemapTest, Hamming) {
    EXPECT_EQ(FREEMAP_HAMMING(0), 0);
    EXPECT_EQ(FREEMAP_HAMMING(0xFF), 8);
    EXPECT_EQ(FREEMAP_HAMMING(0xFFFF), 16);
    EXPECT_EQ(FREEMAP_HAMMING(0xAAAA), 8);

    EXPECT_EQ(FREEMAP_HAMMING(0xFFFFFFFF), 32);
    EXPECT_EQ(FREEMAP_HAMMING(0xAAAAAAAA), 16);
    EXPECT_EQ(FREEMAP_HAMMING(0xAAAA5555), 16);
}