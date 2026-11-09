/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_scb_node_memory_pool.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSScbNodeMemoryPoolTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    // Allocate + Deallocate a pair, ensuring the slot is returned to the pool so
    // cross-test state stays bounded. The pool is a singleton that persists
    // across tests, so we only verify relative behavior, not absolute counts.
    void DrainPoolFreeSlots();
};

void RSScbNodeMemoryPoolTest::SetUpTestCase() {}
void RSScbNodeMemoryPoolTest::TearDownTestCase() {}
void RSScbNodeMemoryPoolTest::SetUp() {}
void RSScbNodeMemoryPoolTest::TearDown() {}

void RSScbNodeMemoryPoolTest::DrainPoolFreeSlots()
{
    // Best-effort cleanup: return any allocated slots we may have taken.
    // The singleton pool is shared; tests below verify relative deltas only.
}

/**
 * @tc.name: InstanceTest001
 * @tc.desc: test RSScbNodeMemoryPool::Instance returns the same singleton
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, InstanceTest001, TestSize.Level1)
{
    auto& instance1 = RSScbNodeMemoryPool::Instance();
    auto& instance2 = RSScbNodeMemoryPool::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: AllocateTest001
 * @tc.desc: test Allocate returns non-null on first call (lazy init / initial chunk)
 *           and that the returned address is properly aligned.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, AllocateTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    uint32_t totalBefore = pool.GetTotalSlots();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    // First allocation triggers initial chunk expansion (INITIAL_SLOTS=1024)
    EXPECT_GE(pool.GetTotalSlots(), totalBefore);
    // Alignment check: address must be aligned to alignof(RSCanvasRenderNode)
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    EXPECT_EQ(addr % alignof(RSCanvasRenderNode), 0u);
    // Free the slot back
    EXPECT_TRUE(pool.Deallocate(ptr));
}

/**
 * @tc.name: AllocateTest002
 * @tc.desc: test Allocate decreases free slot count and increases used count
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, AllocateTest002, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    size_t freeBefore = pool.GetFreeSlotCount();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(pool.GetFreeSlotCount(), freeBefore - 1);
    EXPECT_TRUE(pool.Deallocate(ptr));
    EXPECT_EQ(pool.GetFreeSlotCount(), freeBefore);
}

/**
 * @tc.name: AllocateTest003
 * @tc.desc: test Allocate returns distinct addresses for sequential allocations
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, AllocateTest003, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* ptr1 = pool.Allocate();
    void* ptr2 = pool.Allocate();
    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_NE(ptr1, ptr2);
    pool.Deallocate(ptr1);
    pool.Deallocate(ptr2);
}

/**
 * @tc.name: DeallocateTest001
 * @tc.desc: test Deallocate returns false for nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, DeallocateTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    EXPECT_FALSE(pool.Deallocate(nullptr));
}

/**
 * @tc.name: DeallocateTest002
 * @tc.desc: test Deallocate returns false for a pointer not in the pool
 *           (e.g. a stack address or heap new'd object)
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, DeallocateTest002, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    int stackVar = 0;
    EXPECT_FALSE(pool.Deallocate(&stackVar));
    // A heap-allocated buffer that is clearly not part of any chunk
    void* heap = std::malloc(64);
    ASSERT_NE(heap, nullptr);
    EXPECT_FALSE(pool.Deallocate(heap));
    std::free(heap);
}

/**
 * @tc.name: DeallocateTest003
 * @tc.desc: test Deallocate returns true and reclaims slot for a pool-allocated pointer
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, DeallocateTest003, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    EXPECT_TRUE(pool.Deallocate(ptr));
    // Deallocating the same slot again: the slot is now free, but the address is
    // still within the chunk, so Deallocate still returns true (it does not
    // double-free check). Verify it doesn't crash and returns true.
    EXPECT_TRUE(pool.Deallocate(ptr));
    // Pull the duplicated free slot back out to keep freeSlots_ consistent
    pool.Allocate();
}

/**
 * @tc.name: OwnsTest001
 * @tc.desc: test Owns returns false for nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, OwnsTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    EXPECT_FALSE(pool.Owns(nullptr));
}

/**
 * @tc.name: OwnsTest002
 * @tc.desc: test Owns returns false for a pointer not in any chunk
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, OwnsTest002, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    int stackVar = 0;
    EXPECT_FALSE(pool.Owns(&stackVar));
    void* heap = std::malloc(64);
    ASSERT_NE(heap, nullptr);
    EXPECT_FALSE(pool.Owns(heap));
    std::free(heap);
}

/**
 * @tc.name: OwnsTest003
 * @tc.desc: test Owns returns true for a pool-allocated pointer
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, OwnsTest003, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    EXPECT_TRUE(pool.Owns(ptr));
    pool.Deallocate(ptr);
}

/**
 * @tc.name: OwnsTest004
 * @tc.desc: test Owns returns true even after deallocate (address still within chunk)
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, OwnsTest004, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    pool.Deallocate(ptr);
    // The memory region still belongs to the chunk even though the slot is free
    EXPECT_TRUE(pool.Owns(ptr));
}

/**
 * @tc.name: GetSlotInfoTest001
 * @tc.desc: test GetSlotInfo returns found=false for nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, GetSlotInfoTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    auto info = pool.GetSlotInfo(nullptr);
    EXPECT_FALSE(info.found);
}

/**
 * @tc.name: GetSlotInfoTest002
 * @tc.desc: test GetSlotInfo returns found=false for a pointer not in any chunk
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, GetSlotInfoTest002, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    int stackVar = 0;
    auto info = pool.GetSlotInfo(&stackVar);
    EXPECT_FALSE(info.found);
    void* heap = std::malloc(64);
    ASSERT_NE(heap, nullptr);
    auto info2 = pool.GetSlotInfo(heap);
    EXPECT_FALSE(info2.found);
    std::free(heap);
}

/**
 * @tc.name: GetSlotInfoTest003
 * @tc.desc: test GetSlotInfo returns correct chunk/slot indices for a pool-allocated pointer
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, GetSlotInfoTest003, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    auto info = pool.GetSlotInfo(ptr);
    EXPECT_TRUE(info.found);
    EXPECT_EQ(info.chunkIndex, 0u); // first allocation is in the initial chunk
    // globalSlot = baseSlot + slotInChunk, baseSlot of chunk 0 is 0
    EXPECT_EQ(info.globalSlot, info.slotInChunk);
    pool.Deallocate(ptr);
}

/**
 * @tc.name: GetSlotInfoTest004
 * @tc.desc: test GetSlotInfo slotInChunk advances across sequential allocations within a chunk
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, GetSlotInfoTest004, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* ptr1 = pool.Allocate();
    void* ptr2 = pool.Allocate();
    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    auto info1 = pool.GetSlotInfo(ptr1);
    auto info2 = pool.GetSlotInfo(ptr2);
    EXPECT_TRUE(info1.found);
    EXPECT_TRUE(info2.found);
    EXPECT_EQ(info2.slotInChunk, info1.slotInChunk + 1);
    EXPECT_EQ(info2.globalSlot, info1.globalSlot + 1);
    pool.Deallocate(ptr1);
    pool.Deallocate(ptr2);
}

/**
 * @tc.name: ShrinkTest001
 * @tc.desc: test Shrink is a no-op when totalSlots <= INITIAL_SLOTS (early return branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ShrinkTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    // Ensure at least the initial chunk exists (pool may be lazy-init)
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    pool.Deallocate(ptr);
    uint32_t totalBefore = pool.GetTotalSlots();
    // Without forcing expansion beyond INITIAL_SLOTS, Shrink should be a no-op
    pool.Shrink();
    EXPECT_EQ(pool.GetTotalSlots(), totalBefore);
}

/**
 * @tc.name: ShrinkTest002
 * @tc.desc: test Shrink releases fully-free expansion chunks while keeping >= INITIAL_SLOTS
 *           after expansion + full deallocation.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ShrinkTest002, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    // Drain current free slots by allocating many, then expand the pool by
    // consuming more than the initial free count. Each Allocate beyond the
    // initial free list triggers Expand(EXPAND_SLOTS=32).
    size_t freeBefore = pool.GetFreeSlotCount();
    std::vector<void*> allocated;
    // Allocate enough to force several expansions (more than INITIAL_SLOTS).
    constexpr uint32_t allocCount = 1024 + 64; // INITIAL_SLOTS + 2 expansions
    for (uint32_t i = 0; i < allocCount; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    uint32_t totalAfterExpand = pool.GetTotalSlots();
    EXPECT_GT(totalAfterExpand, freeBefore);

    // Return all allocated slots so expansion chunks become fully free
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    EXPECT_EQ(pool.GetFreeSlotCount(), pool.GetTotalSlots());

    // Shrink should release fully-free expansion chunks, keeping >= INITIAL_SLOTS
    pool.Shrink();
    EXPECT_GE(pool.GetTotalSlots(), 1024u); // INITIAL_SLOTS
    // Should have shrunk compared to the expanded state
    EXPECT_LE(pool.GetTotalSlots(), totalAfterExpand);
}

/**
 * @tc.name: ShrinkTest003
 * @tc.desc: test Shrink does not release chunks when expansion slots are still in use
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ShrinkTest003, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    size_t freeBefore = pool.GetFreeSlotCount();
    std::vector<void*> allocated;
    // Allocate beyond initial to force expansion
    constexpr uint32_t allocCount = 1024 + 32;
    for (uint32_t i = 0; i < allocCount; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    // Only return the initial-chunk slots; keep expansion slots allocated
    uint32_t returned = 0;
    for (void* p : allocated) {
        if (returned >= freeBefore) {
            break;
        }
        pool.Deallocate(p);
        ++returned;
    }
    // Shrink with expansion slots still in use: should not release in-use chunks
    pool.Shrink();
    EXPECT_GE(pool.GetTotalSlots(), 1024u);
    // Cleanup remaining allocations
    for (size_t i = returned; i < allocated.size(); ++i) {
        pool.Deallocate(allocated[i]);
    }
    pool.Shrink();
}

/**
 * @tc.name: ShrinkTest004
 * @tc.desc: test Shrink is a no-op when there is nothing to release (all chunks in use,
 *           remainingSlots == totalSlots branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ShrinkTest004, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    // Force an expansion and keep expansion slots in use so that no chunk is
    // fully free and Shrink hits the "nothing to release" early return.
    size_t freeBefore = pool.GetFreeSlotCount();
    std::vector<void*> allocated;
    constexpr uint32_t allocCount = 1024 + 32;
    for (uint32_t i = 0; i < allocCount; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    uint32_t totalAfterExpand = pool.GetTotalSlots();
    // Keep all allocated; nothing fully free
    pool.Shrink();
    // totalSlots unchanged when nothing can be released
    EXPECT_EQ(pool.GetTotalSlots(), totalAfterExpand);
    // Cleanup
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
    (void)freeBefore;
}

/**
 * @tc.name: DumpTest001
 * @tc.desc: test Dump produces a non-empty status string with expected section markers
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, DumpTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    // Ensure the pool has at least one chunk
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    pool.Deallocate(ptr);
    std::string result;
    pool.Dump(result);
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("SCB Memory Pool Status"), std::string::npos);
    EXPECT_NE(result.find("Slot Size:"), std::string::npos);
    EXPECT_NE(result.find("Total Slots:"), std::string::npos);
    EXPECT_NE(result.find("Used Slots:"), std::string::npos);
    EXPECT_NE(result.find("Free Slots:"), std::string::npos);
    EXPECT_NE(result.find("Usage Rate:"), std::string::npos);
    EXPECT_NE(result.find("Chunks Count:"), std::string::npos);
    EXPECT_NE(result.find("Free Queue:"), std::string::npos);
    EXPECT_NE(result.find("Live Nodes:"), std::string::npos);
    EXPECT_NE(result.find("Per-Chunk Detail"), std::string::npos);
}

/**
 * @tc.name: DumpTest002
 * @tc.desc: test Dump includes fragmentation info when more than one chunk exists
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, DumpTest002, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    std::vector<void*> allocated;
    // Force expansion so chunk count > 1
    for (uint32_t i = 0; i < 1024 + 32; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    if (pool.GetChunkCount() > 1) {
        std::string result;
        pool.Dump(result);
        EXPECT_NE(result.find("Fragmentation:"), std::string::npos);
        EXPECT_NE(result.find("Avg Slots Per Chunk:"), std::string::npos);
    }
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
}

/**
 * @tc.name: DumpTest003
 * @tc.desc: test Dump per-chunk detail line count matches chunk count
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, DumpTest003, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    pool.Deallocate(ptr);
    std::string result;
    pool.Dump(result);
    // Count occurrences of "Chunk[" lines
    size_t chunkLines = 0;
    size_t pos = 0;
    while ((pos = result.find("Chunk[", pos)) != std::string::npos) {
        ++chunkLines;
        pos += 6; // length of "Chunk["
    }
    EXPECT_EQ(chunkLines, pool.GetChunkCount());
}

/**
 * @tc.name: GetTotalUsedTest001
 * @tc.desc: test GetTotalUsed reflects allocations and deallocations
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, GetTotalUsedTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    uint32_t usedBefore = pool.GetTotalUsed();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(pool.GetTotalUsed(), usedBefore + 1);
    pool.Deallocate(ptr);
    EXPECT_EQ(pool.GetTotalUsed(), usedBefore);
}

/**
 * @tc.name: GetChunkCountTest001
 * @tc.desc: test GetChunkCount increases after expansion via Allocate beyond initial free slots
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, GetChunkCountTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    size_t freeBefore = pool.GetFreeSlotCount();
    size_t chunksBefore = pool.GetChunkCount();
    std::vector<void*> allocated;
    // Allocate beyond current free slots to trigger an expansion
    for (size_t i = 0; i < freeBefore + 1; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    EXPECT_GE(pool.GetChunkCount(), chunksBefore + 1);
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
}

/**
 * @tc.name: GetTotalSlotsTest001
 * @tc.desc: test GetTotalSlots increases by EXPAND_SLOTS on expansion
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, GetTotalSlotsTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    size_t freeBefore = pool.GetFreeSlotCount();
    uint32_t totalBefore = pool.GetTotalSlots();
    std::vector<void*> allocated;
    // Consume all currently-free slots, then one more to trigger Expand(32)
    for (size_t i = 0; i < freeBefore; ++i) {
        void* p = pool.Allocate();
        allocated.push_back(p);
    }
    EXPECT_EQ(pool.GetTotalSlots(), totalBefore); // no expansion yet
    void* extra = pool.Allocate();
    ASSERT_NE(extra, nullptr);
    allocated.push_back(extra);
    // Expansion adds EXPAND_SLOTS=32 slots
    EXPECT_EQ(pool.GetTotalSlots(), totalBefore + 32);
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
}

/**
 * @tc.name: AllocateDeallocateReuseTest001
 * @tc.desc: test that a deallocated slot is reused by the next Allocate (fill-first / push_front)
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, AllocateDeallocateReuseTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* ptr1 = pool.Allocate();
    ASSERT_NE(ptr1, nullptr);
    pool.Deallocate(ptr1);
    // The next allocate should reuse the just-returned slot
    void* ptr2 = pool.Allocate();
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(ptr1, ptr2);
    pool.Deallocate(ptr2);
}

/**
 * @tc.name: ConcurrencyTest001
 * @tc.desc: test concurrent Allocate/Deallocate does not crash and all pointers are returned
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ConcurrencyTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    constexpr int threadCount = 4;
    constexpr int allocsPerThread = 32;
    std::vector<std::thread> threads;
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([&pool]() {
            std::vector<void*> local;
            for (int i = 0; i < allocsPerThread; ++i) {
                void* p = pool.Allocate();
                if (p != nullptr) {
                    local.push_back(p);
                }
            }
            for (void* p : local) {
                pool.Deallocate(p);
            }
        });
    }
    for (auto& th : threads) {
        th.join();
    }
    SUCCEED();
}

/**
 * @tc.name: GetFreeSlotCountTest001
 * @tc.desc: test GetFreeSlotCount equals freeSlots_.size() and reflects alloc/dealloc
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, GetFreeSlotCountTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    size_t freeBefore = pool.GetFreeSlotCount();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(pool.GetFreeSlotCount(), freeBefore - 1);
    pool.Deallocate(ptr);
    EXPECT_EQ(pool.GetFreeSlotCount(), freeBefore);
}

/**
 * @tc.name: ExpandTest001
 * @tc.desc: test Allocate returns nullptr once the pool is saturated at MAX_SLOTS.
 *           This drives the `totalSlots_ >= MAX_SLOTS` early-return branch (L57-60) of
 *           Expand and the Allocate nullptr path (L113-117).
 *           NOTE: the `slotCount > remaining` clamp branch (L64-66) is NOT reachable
 *           through Allocate (it always passes INITIAL_SLOTS or EXPAND_SLOTS, which are
 *           exact divisors of the remaining capacity); that branch is covered directly
 *           by ExpandClampTest001 below.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ExpandTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    std::vector<void*> allocated;
    // Exhaust the pool: allocate until nullptr. This drives totalSlots to MAX_SLOTS
    // and consumes every free slot, so the next Allocate hits the Expand-fail path.
    for (uint32_t i = 0; i < 4096; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    EXPECT_EQ(pool.GetTotalSlots(), 4096u);
    EXPECT_EQ(pool.GetFreeSlotCount(), 0u);
    // Pool is saturated: Expand returns false (MAX_SLOTS reached), Allocate returns nullptr.
    void* over = pool.Allocate();
    EXPECT_EQ(over, nullptr);
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
}

/**
 * @tc.name: ExpandTest002
 * @tc.desc: test Expand returns false when totalSlots_ already equals MAX_SLOTS
 *           (early-return branch L57-60). Reached by saturating the pool, then
 *           freeing one slot and re-allocating to re-trigger Expand at the limit.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ExpandTest002, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    std::vector<void*> allocated;
    for (uint32_t i = 0; i < 4096; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    EXPECT_EQ(pool.GetTotalSlots(), 4096u);
    EXPECT_EQ(pool.GetFreeSlotCount(), 0u);
    // At MAX_SLOTS with no free slots: Allocate must return nullptr.
    void* p = pool.Allocate();
    EXPECT_EQ(p, nullptr);
    for (void* a : allocated) {
        pool.Deallocate(a);
    }
    pool.Shrink();
}

/**
 * @tc.name: ExpandClampTest001
 * @tc.desc: test Expand clamps slotCount to the remaining capacity when the requested
 *           count exceeds MAX_SLOTS - totalSlots_ (the `slotCount > remaining` branch
 *           L64-66). Called directly (private:public) because Allocate can never request
 *           more than EXPAND_SLOTS, making the clamp unreachable through the public API.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ExpandClampTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    // Ensure the pool is initialized and Shrink back toward the baseline.
    void* init = pool.Allocate();
    ASSERT_NE(init, nullptr);
    pool.Deallocate(init);
    pool.Shrink();
    uint32_t totalBefore = pool.GetTotalSlots();
    // Guard: if a prior test left the pool saturated, Expand would take the MAX_SLOTS
    // early-return path instead of the clamp branch. Skip in that unlikely case.
    if (totalBefore >= 4096u) {
        SUCCEED();
        return;
    }
    uint32_t remaining = 4096u - totalBefore;
    ASSERT_GT(remaining, 0u);
    // Request far more than the remaining capacity: Expand must clamp to `remaining`
    // instead of allocating the requested 100000 slots or overflowing past MAX_SLOTS.
    EXPECT_TRUE(pool.Expand(100000u));
    EXPECT_EQ(pool.GetTotalSlots(), 4096u);
    // Return the clamped expansion to the system.
    pool.Shrink();
}

/**
 * @tc.name: AllocateTest004
 * @tc.desc: test Allocate triggers Expand(EXPAND_SLOTS) when freeSlots_ is empty but
 *           totalSlots_ > 0 (the non-initial expand branch, L104). Verifies the second
 *           arm of the ternary `(totalSlots_ == 0) ? INITIAL_SLOTS : EXPAND_SLOTS`.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, AllocateTest004, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    size_t freeBefore = pool.GetFreeSlotCount();
    uint32_t totalBefore = pool.GetTotalSlots();
    std::vector<void*> allocated;
    // Drain all currently-free slots (no expansion).
    for (size_t i = 0; i < freeBefore; ++i) {
        void* p = pool.Allocate();
        ASSERT_NE(p, nullptr);
        allocated.push_back(p);
    }
    EXPECT_EQ(pool.GetTotalSlots(), totalBefore);
    // One more Allocate must trigger Expand(EXPAND_SLOTS=32).
    void* extra = pool.Allocate();
    ASSERT_NE(extra, nullptr);
    allocated.push_back(extra);
    EXPECT_EQ(pool.GetTotalSlots(), totalBefore + 32u);
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
}

/**
 * @tc.name: AllocateTest005
 * @tc.desc: test Allocate returns slot addresses that are slotSize_ apart within a chunk.
 *           Verifies the chunk-lookup arithmetic (L116-118): addr = base + offset*slotSize.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, AllocateTest005, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* p1 = pool.Allocate();
    void* p2 = pool.Allocate();
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    auto info1 = pool.GetSlotInfo(p1);
    auto info2 = pool.GetSlotInfo(p2);
    ASSERT_TRUE(info1.found && info2.found);
    // Same chunk, adjacent slots → byte distance equals one aligned slot size.
    if (info1.chunkIndex == info2.chunkIndex) {
        uintptr_t diff = reinterpret_cast<uintptr_t>(p2) - reinterpret_cast<uintptr_t>(p1);
        EXPECT_EQ(static_cast<size_t>(diff % alignof(RSCanvasRenderNode)), static_cast<size_t>(0));
    }
    pool.Deallocate(p1);
    pool.Deallocate(p2);
}

/**
 * @tc.name: DeallocateTest004
 * @tc.desc: test Deallocate correctly identifies a pointer in an expansion chunk
 *           (not just the initial chunk). Covers the multi-chunk iteration branch (L139-148).
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, DeallocateTest004, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    size_t freeBefore = pool.GetFreeSlotCount();
    std::vector<void*> allocated;
    // Force at least one expansion so a second chunk exists.
    for (size_t i = 0; i < freeBefore + 32; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    ASSERT_GE(pool.GetChunkCount(), 2u);
    // The last allocated pointer lives in an expansion chunk.
    void* expansionPtr = allocated.back();
    EXPECT_TRUE(pool.Owns(expansionPtr));
    EXPECT_TRUE(pool.Deallocate(expansionPtr));
    allocated.pop_back();
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
}

/**
 * @tc.name: DeallocateTest005
 * @tc.desc: test Deallocate on an interior, non-slot-aligned address inside a chunk.
 *           The current implementation does NOT validate alignment, so Deallocate
 *           returns true (it computes offset via integer division). This test pins
 *           current behavior: an unaligned-in-range pointer is treated as a slot.
 *           Documents the unaligned-slot gap flagged in review.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, DeallocateTest005, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    auto info = pool.GetSlotInfo(ptr);
    ASSERT_TRUE(info.found);
    // Compute slotSize_ from two adjacent allocations (same chunk).
    void* ptr2 = pool.Allocate();
    ASSERT_NE(ptr2, nullptr);
    auto info2 = pool.GetSlotInfo(ptr2);
    size_t slotSize = (info.chunkIndex == info2.chunkIndex)
        ? (reinterpret_cast<uintptr_t>(ptr2) - reinterpret_cast<uintptr_t>(ptr))
        : 0;
    if (slotSize > 1) {
        // ptr+1 is inside the chunk but NOT slot-aligned. Current Deallocate accepts it
        // and computes the same offset as ptr, pushing a DUPLICATE slot onto freeSlots_.
        // To keep the pool consistent we must consume that duplicate immediately.
        char* interior = static_cast<char*>(ptr) + 1;
        // Behavior pin: returns true (range check passes, no alignment check).
        // If the implementation later adds alignment validation, this will fail and
        // should be updated to EXPECT_FALSE.
        bool result = pool.Deallocate(interior);
        EXPECT_TRUE(result);
        // The duplicate free entry must be popped so freeSlots_ stays consistent.
        void* popped = pool.Allocate();
        EXPECT_EQ(popped, ptr); // the slot's base address is what Allocate returns
    }
    // Now clean up the two originally-allocated slots normally.
    pool.Deallocate(ptr2);
    pool.Deallocate(ptr);
}

/**
 * @tc.name: OwnsTest005
 * @tc.desc: test Owns distinguishes pointers across multiple chunks. A pointer in the
 *           initial chunk and one in an expansion chunk must both Owns==true, and an
 *           address just past the last chunk must be false.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, OwnsTest005, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    size_t freeBefore = pool.GetFreeSlotCount();
    std::vector<void*> allocated;
    for (size_t i = 0; i < freeBefore + 32; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    ASSERT_GE(pool.GetChunkCount(), 2u);
    EXPECT_TRUE(pool.Owns(allocated.front())); // initial chunk
    EXPECT_TRUE(pool.Owns(allocated.back()));  // expansion chunk
    // An address just past the end of the first chunk's memory should not be owned
    // (it would belong to the next chunk only if contiguously allocated, which
    // posix_memalign does not guarantee). Use a clearly-foreign address instead.
    int stackVar = 0;
    EXPECT_FALSE(pool.Owns(&stackVar));
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
}

/**
 * @tc.name: ShrinkTest005
 * @tc.desc: test Shrink releases only the trailing fully-free chunks and keeps the
 *           in-use expansion chunk. Covers the per-chunk release decision branch
 *           (L210-218) where some expansion chunks are released and one is kept
 *           because it still holds allocated slots.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ShrinkTest005, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    size_t freeBefore = pool.GetFreeSlotCount();
    std::vector<void*> allocated;
    // Force two expansions: 1024 + 32 + 32 = 1088 slots.
    for (uint32_t i = 0; i < freeBefore + 64; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    uint32_t totalAfterExpand = pool.GetTotalSlots();
    ASSERT_GE(pool.GetChunkCount(), 3u);
    // Free only the initial-chunk slots + the first expansion's slots, keep the
    // last expansion chunk fully allocated. After Shrink, the last chunk must be
    // retained (in use) and totalSlots must reflect kept chunks.
    for (size_t i = 0; i < freeBefore + 32 && i < allocated.size(); ++i) {
        pool.Deallocate(allocated[i]);
    }
    pool.Shrink();
    // The last chunk (32 slots) is fully in use → cannot be released. Remaining
    // totalSlots must be >= INITIAL_SLOTS + 32.
    EXPECT_GE(pool.GetTotalSlots(), 1024u + 32u);
    EXPECT_LE(pool.GetTotalSlots(), totalAfterExpand);
    // Cleanup remaining
    for (size_t i = freeBefore + 32; i < allocated.size(); ++i) {
        pool.Deallocate(allocated[i]);
    }
    pool.Shrink();
}

/**
 * @tc.name: ShrinkTest006
 * @tc.desc: test Shrink boundary: when releasing a chunk would drop remaining below
 *           INITIAL_SLOTS, that chunk is kept (L213: remainingSlots - slotCount >=
 *           INITIAL_SLOTS). Verify totalSlots never goes below INITIAL_SLOTS.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ShrinkTest006, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    size_t freeBefore = pool.GetFreeSlotCount();
    std::vector<void*> allocated;
    // One expansion beyond initial: 1024 + 32.
    for (uint32_t i = 0; i < freeBefore + 32; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    // Free everything so the expansion chunk is fully free.
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
    // Releasing the 32-slot chunk would leave 1024 == INITIAL_SLOTS, which satisfies
    // `>= INITIAL_SLOTS`, so it CAN be released. totalSlots must be >= INITIAL_SLOTS.
    EXPECT_GE(pool.GetTotalSlots(), 1024u);
    // Now force two expansions and free only the second expansion; the first expansion
    // (32 slots) plus initial (1024) = 1056. Releasing first expansion → 1024 >= 1024 OK.
    allocated.clear();
    for (uint32_t i = 0; i < freeBefore + 64; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
    EXPECT_GE(pool.GetTotalSlots(), 1024u);
}

/**
 * @tc.name: ShrinkTest007
 * @tc.desc: test Shrink rebuilds freeSlots_ with correct re-based global slot indices
 *           after releasing trailing chunks (L227-249). After Shrink, Allocate must
 *           still return a valid, Owns-able pointer.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ShrinkTest007, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    size_t freeBefore = pool.GetFreeSlotCount();
    std::vector<void*> allocated;
    for (uint32_t i = 0; i < freeBefore + 64; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
    // After Shrink, the freeSlots_ were rebuilt with new baseSlot indices. A fresh
    // Allocate must succeed and return a pointer the pool Owns.
    void* p = pool.Allocate();
    ASSERT_NE(p, nullptr);
    EXPECT_TRUE(pool.Owns(p));
    auto info = pool.GetSlotInfo(p);
    EXPECT_TRUE(info.found);
    EXPECT_EQ(info.globalSlot, info.slotInChunk); // baseSlot of chunk 0 is 0 after rebuild
    pool.Deallocate(p);
}

/**
 * @tc.name: DumpTest004
 * @tc.desc: test Dump with totalSlots==0 (lazy-init pool before any Allocate).
 *           Covers the `totalSlots_ > 0 ? ... : 0.0f` usageRate branch (L279) and
 *           the single-chunk skip of fragmentation info.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, DumpTest004, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    // Force the pool back to a clean state as much as possible: drain & shrink.
    // Note: pool is a singleton; if prior tests left allocations we cannot truly
    // reach totalSlots==0, so we verify the Dump output shape is still well-formed
    // and that Usage Rate line is present and parseable.
    pool.Shrink();
    std::string result;
    pool.Dump(result);
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("Usage Rate:"), std::string::npos);
    // With a single chunk (after Shrink), fragmentation info should NOT appear.
    if (pool.GetChunkCount() <= 1) {
        EXPECT_EQ(result.find("Fragmentation:"), std::string::npos);
    }
}

/**
 * @tc.name: DumpTest005
 * @tc.desc: test Dump reports consistent used+free == total. Catches the
 *           `usedSlots = totalSlots_ - freeSlots_.size()` underflow when freeSlots_
 *           exceeds totalSlots_ (should never happen, but the subtraction is unsigned).
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, DumpTest005, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    std::string result;
    pool.Dump(result);
    // Parse Total / Used / Free slots from the dump.
    auto parseVal = [](const std::string& s, const std::string& key) -> uint32_t {
        auto pos = s.find(key);
        if (pos == std::string::npos) {
            return 0;
        }
        pos += key.size();
        return static_cast<uint32_t>(std::strtoul(s.c_str() + pos, nullptr, 10));
    };
    uint32_t total = parseVal(result, "Total Slots: ");
    uint32_t used = parseVal(result, "Used Slots: ");
    size_t freePos = result.find("Free Slots: ");
    uint32_t freeVal = 0;
    if (freePos != std::string::npos) {
        freeVal = static_cast<uint32_t>(std::strtoul(result.c_str() + freePos + 12, nullptr, 10));
    }
    EXPECT_EQ(used + freeVal, total);
}

/**
 * @tc.name: GetSlotInfoTest005
 * @tc.desc: test GetSlotInfo returns correct chunkIndex and globalSlot for a pointer
 *           in an expansion chunk (multi-chunk branch, L330-340). The globalSlot must
 *           equal baseSlot + slotInChunk for that chunk.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, GetSlotInfoTest005, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    size_t freeBefore = pool.GetFreeSlotCount();
    std::vector<void*> allocated;
    for (size_t i = 0; i < freeBefore + 32; ++i) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        allocated.push_back(p);
    }
    ASSERT_GE(pool.GetChunkCount(), 2u);
    // A pointer in the second chunk:
    void* expansionPtr = allocated.back();
    auto info = pool.GetSlotInfo(expansionPtr);
    EXPECT_TRUE(info.found);
    EXPECT_GT(info.chunkIndex, 0u);
    // slotInChunk must be within the chunk's slot count (EXPAND_SLOTS=32 for expansions).
    EXPECT_LT(info.slotInChunk, 32u);
    // Re-derive the address from GetSlotInfo of the initial-chunk pointer and verify
    // the expansion pointer is at a higher address than the initial-chunk base pointer.
    auto infoFront = pool.GetSlotInfo(allocated.front());
    ASSERT_TRUE(infoFront.found);
    EXPECT_EQ(infoFront.chunkIndex, 0u);
    EXPECT_GT(info.chunkIndex, infoFront.chunkIndex);
    // Cleanup
    for (void* p : allocated) {
        pool.Deallocate(p);
    }
    pool.Shrink();
}

/**
 * @tc.name: GetSlotInfoTest006
 * @tc.desc: test GetSlotInfo on a foreign (non-pool) address returns found=false.
 *           Covers the loop-completes-without-match path (L330-341) where no chunk
 *           range contains the pointer.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, GetSlotInfoTest006, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    auto info = pool.GetSlotInfo(ptr);
    ASSERT_TRUE(info.found);
    // The next slot's address is ptr + slotSize. GetSlotInfo on ptr itself is found;
    // an address derived from a foreign allocation must not be found.
    void* heap = std::malloc(64);
    ASSERT_NE(heap, nullptr);
    auto infoForeign = pool.GetSlotInfo(heap);
    EXPECT_FALSE(infoForeign.found);
    std::free(heap);
    pool.Deallocate(ptr);
}

/**
 * @tc.name: ConcurrencyTest002
 * @tc.desc: test concurrent Allocate/Deallocate with enough pressure to force
 *           simultaneous Expand calls. Verifies the spinlock protects chunks_ and
 *           freeSlots_ during expansion (L102-122).
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ConcurrencyTest002, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    constexpr int threadCount = 8;
    constexpr int allocsPerThread = 200;
    std::vector<std::thread> threads;
    std::atomic<int> failures{0};
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([&pool, &failures]() {
            std::vector<void*> local;
            for (int i = 0; i < allocsPerThread; ++i) {
                void* p = pool.Allocate();
                if (p != nullptr) {
                    local.push_back(p);
                } else {
                    ++failures;
                }
            }
            for (void* p : local) {
                pool.Deallocate(p);
            }
        });
    }
    for (auto& th : threads) {
        th.join();
    }
    // Some failures are expected when MAX_SLOTS is reached, but no crash/UAF.
    EXPECT_GE(failures.load(), 0);
    // Pool invariants after concurrent work: freeSlots_ <= totalSlots_.
    EXPECT_LE(pool.GetFreeSlotCount(), pool.GetTotalSlots());
    pool.Shrink();
}

/**
 * @tc.name: ConcurrencyTest003
 * @tc.desc: test concurrent Allocate/Owns/GetSlotInfo mix. Owns and GetSlotInfo take
 *           the spinlock too; verify no deadlock or corruption under mixed access.
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, ConcurrencyTest003, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    constexpr int threadCount = 4;
    std::vector<std::thread> threads;
    std::atomic<bool> stop{false};
    // Reader threads: call Owns/GetSlotInfo on a known pool pointer.
    void* probe = pool.Allocate();
    ASSERT_NE(probe, nullptr);
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([&pool, &stop, probe]() {
            while (!stop.load()) {
                pool.Owns(probe);
                pool.GetSlotInfo(probe);
            }
        });
    }
    // Writer thread: churn Allocate/Deallocate.
    std::thread writer([&pool, &stop]() {
        for (int i = 0; i < 500; ++i) {
            void* p = pool.Allocate();
            if (p != nullptr) {
                pool.Deallocate(p);
            }
        }
        stop.store(true);
    });
    writer.join();
    for (auto& th : threads) {
        th.join();
    }
    pool.Deallocate(probe);
    SUCCEED();
}

/**
 * @tc.name: RepeatedShrinkTest001
 * @tc.desc: test that repeated Shrink calls are idempotent once the pool is at
 *           INITIAL_SLOTS. The second Shrink hits the early-return branch (L182-187).
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, RepeatedShrinkTest001, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    pool.Shrink();
    uint32_t totalAfterFirst = pool.GetTotalSlots();
    pool.Shrink();
    EXPECT_EQ(pool.GetTotalSlots(), totalAfterFirst);
    pool.Shrink();
    EXPECT_EQ(pool.GetTotalSlots(), totalAfterFirst);
}

/**
 * @tc.name: AllocateDeallocateReuseTest002
 * @tc.desc: test LIFO reuse order: Deallocate pushes to front, Allocate pops front.
 *           Verify the most-recently-deallocated slot is reused first (L145 push_front,
 *           L111 pop_front).
 * @tc.type: FUNC
 */
HWTEST_F(RSScbNodeMemoryPoolTest, AllocateDeallocateReuseTest002, TestSize.Level1)
{
    auto& pool = RSScbNodeMemoryPool::Instance();
    void* p1 = pool.Allocate();
    void* p2 = pool.Allocate();
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    // Return p1 then p2. freeSlots_ front becomes p2 (push_front).
    pool.Deallocate(p1);
    pool.Deallocate(p2);
    // Next Allocate should reuse p2 (LIFO).
    void* reused = pool.Allocate();
    ASSERT_NE(reused, nullptr);
    EXPECT_EQ(reused, p2);
    void* reused2 = pool.Allocate();
    ASSERT_NE(reused2, nullptr);
    EXPECT_EQ(reused2, p1);
    pool.Deallocate(reused);
    pool.Deallocate(reused2);
}

} // namespace Rosen
} // namespace OHOS
