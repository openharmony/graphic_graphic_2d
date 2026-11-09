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

#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_SCB_NODE_MEMORY_POOL_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_SCB_NODE_MEMORY_POOL_H

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
namespace Rosen {

// Contiguous memory pool for SCB (SceneBoard) CANVAS_NODE allocations.
// Memory is allocated in chunks, each chunk holding a number of slots sized
// to fit one RSCanvasRenderNode. The initial chunk pre-allocates INITIAL_SLOTS
// slots; each subsequent expansion adds EXPAND_SLOTS slots. Slots are handed
// out and returned via a free-list, giving cache-friendly contiguous layout
// for long-lived SCB nodes.
class RSB_EXPORT RSScbNodeMemoryPool {
public:
    static RSScbNodeMemoryPool& Instance();

    // Allocate one slot of memory (sizeof(RSCanvasRenderNode), properly aligned).
    // Returns nullptr when the pool is full and cannot expand further.
    void* Allocate();

    // Return a previously allocated slot to the pool.
    // The caller must have already invoked the destructor on the object.
    // Returns false when ptr does not belong to this pool.
    bool Deallocate(void* ptr);

    // Check whether ptr falls within any chunk managed by this pool.
    bool Owns(const void* ptr) const;

    // Release chunks whose slots are all free, keeping at least INITIAL_SLOTS
    // slots in the pool. Called during idle GC to return memory to the system.
    void Shrink();

    // Dump memory pool status for Hildumper integration.
    // Outputs: slot size, total slots, used slots, free slots, usage rate, chunks count,
    //          free queue size, live nodes, fragmentation, per-chunk detail.
    void Dump(std::string& result) const;

    // Slot location info for a given pointer.
    struct SlotInfo {
        bool found = false;       // whether ptr belongs to this pool
        uint32_t chunkIndex = 0;  // index of the chunk containing ptr
        uint32_t slotInChunk = 0; // slot offset within that chunk
        uint32_t globalSlot = 0;  // global slot index across all chunks
    };
    // Locate which chunk/slot a pointer falls into. Takes spinlock; safe to call from any thread.
    SlotInfo GetSlotInfo(const void* ptr) const;

    // Best-effort snapshot getters for diagnostics/dump only.
    // No locking; values may be transiently inconsistent under concurrent Allocate/Deallocate/Shrink.
    // Use GetSlotInfo()/Owns() for thread-safe queries.
    size_t GetFreeSlotCount() const { return freeSlots_.size(); }
    uint32_t GetTotalUsed() const { return totalSlots_ - freeSlots_.size(); }
    size_t GetChunkCount() const { return chunks_.size(); }
    uint32_t GetTotalSlots() const { return totalSlots_; }

private:
    RSScbNodeMemoryPool();
    ~RSScbNodeMemoryPool();
    RSScbNodeMemoryPool(const RSScbNodeMemoryPool&) = delete;
    RSScbNodeMemoryPool& operator=(const RSScbNodeMemoryPool&) = delete;

    class Spinlock {
    public:
        Spinlock() : flag_(false) {}
        void lock()
        {
            while (flag_.exchange(true, std::memory_order_acquire)) {}
        }
        void unlock()
        {
            flag_.store(false, std::memory_order_release);
        }

    private:
        std::atomic<bool> flag_;
    };

    struct Chunk {
        // Memory allocated via posix_memalign (OHOS) or _aligned_malloc (Windows SDK),
        // freed with free() or _aligned_free() respectively. The deleter is always
        // supplied explicitly in Expand(); this default is only a placeholder.
        std::unique_ptr<char[], void (*)(char*)> memory{nullptr, [](char* p) { std::free(p); }};
        uint32_t slotCount = 0;
        // Flat offset of this chunk's first slot in the global slot space.
        uint32_t baseSlot = 0;
    };

    // Grow the pool by appending one chunk. Called under spinlock.
    // slotCount controls how many slots the new chunk holds.
    // Returns false when max slot count is reached.
    bool Expand(uint32_t slotCount);

    static constexpr uint32_t INITIAL_SLOTS = 1024;
    static constexpr uint32_t EXPAND_SLOTS = 32;
    static constexpr uint32_t MAX_SLOTS = 4096;

    std::vector<Chunk> chunks_;
    // Free slot indices, ordered front-to-back so that earlier chunks are
    // filled first. This keeps later expand chunks fully free so Shrink()
    // can release them. Allocate pops from the front; Deallocate pushes
    // to the front (reuse holes before touching newer chunks).
    std::deque<uint32_t> freeSlots_;
    // Total number of slots across all chunks.
    uint32_t totalSlots_ = 0;
    mutable Spinlock spinlock_;

    const size_t slotSize_;
    const size_t slotAlign_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIPELINE_RS_SCB_NODE_MEMORY_POOL_H
