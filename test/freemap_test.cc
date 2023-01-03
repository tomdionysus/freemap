#include <gtest/gtest.h>

extern "C" {
  #include "freemap.h"
}

// Instantiate, check 
TEST(FreemapTest, Instantiation) {
  freemap_t *map = freemap_new(127);

  EXPECT_NE(map->bitmap, nullptr);
  EXPECT_EQ(map->total, 127);
  EXPECT_EQ(map->free, 127);

  freemap_destroy(map);
}

// Freemap _firstfree
TEST(FreemapTest, FirstFree) {
  EXPECT_EQ(_freemap_firstfree(0), 0);
  EXPECT_EQ(_freemap_firstfree(1), 1);
  EXPECT_EQ(_freemap_firstfree(0b1111), 4);
  EXPECT_EQ(_freemap_firstfree(0b1011), 2);

  EXPECT_EQ(_freemap_firstfree(0xFFFFFFFF), 255);

}

// Allocate / Deallocate
TEST(FreemapTest, AllocateDeallocate) {
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

  // Reallocate the block in the middle
  res = freemap_allocate(map);
  EXPECT_EQ(res.status, FREEMAP_STATUS_SUCCESS | FREEMAP_STATUS_ALLOCATED);
  EXPECT_EQ(res.block, 5);
  EXPECT_EQ(res.map, map);
  EXPECT_EQ(map->free, 0);

  // Cleanup
  freemap_destroy(map);
}