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

#include "pipeline/rs_scb_node_memory_pool.h"

#include <algorithm>
#include <cstdlib>
#include <malloc.h>
#include <new>
#include "securec.h"

#include "common/rs_optional_trace.h"
#include "pipeline/rs_render_node_allocator.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

RSScbNodeMemoryPool& RSScbNodeMemoryPool::Instance()
{
    static RSScbNodeMemoryPool instance;
    return instance;
}

RSScbNodeMemoryPool::RSScbNodeMemoryPool()
    : slotSize_(sizeof(RSCanvasRenderNode)), slotAlign_(alignof(RSCanvasRenderNode))
{
    // Round slotSize up to a multiple of slotAlign so every slot is properly aligned.
    size_t alignedSlotSize = ((slotSize_ + slotAlign_ - 1) / slotAlign_) * slotAlign_;
    const_cast<size_t&>(slotSize_) = alignedSlotSize;

    // Initial chunk allocation is deferred to the first Allocate() call (lazy init).
    // This avoids pre-allocating memory when the SCB pool is not used by the process.
    RS_LOGI("RSScbNodeMemoryPool created (lazy init), slotSize:%{public}zu, slotAlign:%{public}zu",
        slotSize_, slotAlign_);
}

RSScbNodeMemoryPool::~RSScbNodeMemoryPool()
{
    // Chunks own their memory via unique_ptr; vector destruction frees everything.
}

bool RSScbNodeMemoryPool::Expand(uint32_t slotCount)
{
    if (totalSlots_ >= MAX_SLOTS) {
        RS_LOGW("RSScbNodeMemoryPool::Expand max slots reached, total:%{public}u", totalSlots_);
        return false;
    }

    // Clamp to not exceed MAX_SLOTS.
    uint32_t remaining = MAX_SLOTS - totalSlots_;
    if (slotCount > remaining) {
        slotCount = remaining;
    }

    size_t chunkBytes = slotSize_ * slotCount;
    size_t alignment = slotAlign_;
    if (alignment < sizeof(void*)) {
        alignment = sizeof(void*);
    }
    void* raw = nullptr;
#ifdef _WIN32
    raw = _aligned_malloc(chunkBytes, alignment);
    if (raw == nullptr) {
        RS_LOGW("RSScbNodeMemoryPool::Expand _aligned_malloc failed, bytes:%{public}zu, align:%{public}zu",
            chunkBytes, alignment);
        return false;
    }
    Chunk chunk;
    chunk.memory = std::unique_ptr<char[], void (*)(char*)>(
        static_cast<char*>(raw), [](char* p) { _aligned_free(p); });
#else
    if (posix_memalign(&raw, alignment, chunkBytes) != 0 || raw == nullptr) {
        RS_LOGW("RSScbNodeMemoryPool::Expand posix_memalign failed, bytes:%{public}zu, align:%{public}zu",
            chunkBytes, alignment);
        return false;
    }
    Chunk chunk;
    chunk.memory = std::unique_ptr<char[], void (*)(char*)>(
        static_cast<char*>(raw), [](char* p) { std::free(p); });
#endif
    chunk.slotCount = slotCount;
    chunk.baseSlot = totalSlots_;
    chunks_.push_back(std::move(chunk));

    for (uint32_t i = 0; i < slotCount; ++i) {
        freeSlots_.push_back(totalSlots_ + i);
    }
    totalSlots_ += slotCount;
    RS_LOGD_IF(DEBUG_NODE, "RSScbNodeMemoryPool::Expand added %{public}u slots, total slots:%{public}u, "
        "chunks:%{public}zu", slotCount, totalSlots_, chunks_.size());
    return true;
}

void* RSScbNodeMemoryPool::Allocate()
{
    spinlock_.lock();
    if (freeSlots_.empty()) {
        uint32_t expandCount = (totalSlots_ == 0) ? INITIAL_SLOTS : EXPAND_SLOTS;
        if (!Expand(expandCount)) {
            spinlock_.unlock();
            RS_LOGW("RSScbNodeMemoryPool::Allocate pool exhausted, total slots:%{public}u", totalSlots_);
            return nullptr;
        }
    }
    uint32_t globalSlot = freeSlots_.front();
    freeSlots_.pop_front();
    // Locate the chunk that owns this global slot and compute the address.
    void* addr = nullptr;
    for (const auto& chunk : chunks_) {
        if (globalSlot >= chunk.baseSlot && globalSlot < chunk.baseSlot + chunk.slotCount) {
            uint32_t offset = globalSlot - chunk.baseSlot;
            addr = chunk.memory.get() + offset * slotSize_;
            break;
        }
    }
    spinlock_.unlock();
    RS_LOGD_IF(DEBUG_NODE, "RSScbNodeMemoryPool::Allocate slot:%{public}u, freeSlots:%{public}zu, "
        "totalSlots:%{public}u", globalSlot, freeSlots_.size(), totalSlots_);
    return addr;
}

bool RSScbNodeMemoryPool::Deallocate(void* ptr)
{
    if (ptr == nullptr) {
        return false;
    }
    spinlock_.lock();
    bool found = false;
    for (const auto& chunk : chunks_) {
        const char* base = chunk.memory.get();
        const char* end = base + chunk.slotCount * slotSize_;
        const char* p = static_cast<const char*>(ptr);
        if (p >= base && p < end) {
            uint32_t offset = static_cast<uint32_t>((p - base) / slotSize_);
            freeSlots_.push_front(chunk.baseSlot + offset);
            found = true;
            break;
        }
    }
    spinlock_.unlock();
    if (!found) {
        RS_LOGW("RSScbNodeMemoryPool::Deallocate ptr not in pool, ptr:%{public}p", ptr);
    }
    return found;
}

bool RSScbNodeMemoryPool::Owns(const void* ptr) const
{
    if (ptr == nullptr) {
        return false;
    }
    spinlock_.lock();
    bool found = false;
    for (const auto& chunk : chunks_) {
        const char* base = chunk.memory.get();
        const char* end = base + chunk.slotCount * slotSize_;
        const char* p = static_cast<const char*>(ptr);
        if (p >= base && p < end) {
            found = true;
            break;
        }
    }
    spinlock_.unlock();
    return found;
}

void RSScbNodeMemoryPool::Shrink()
{
    RS_TRACE_NAME_FMT("ScbPool::Shrink start, totalSlots=%u, chunks=%zu", totalSlots_, chunks_.size());
    spinlock_.lock();
    if (totalSlots_ <= INITIAL_SLOTS) {
        spinlock_.unlock();
        return;
    }

    // Build a fast lookup of which global slots are free.
    std::vector<bool> isFree(totalSlots_, false);
    for (uint32_t slot : freeSlots_) {
        isFree[slot] = true;
    }

    // Count free slots per chunk.
    std::vector<uint32_t> freeCountPerChunk(chunks_.size(), 0);
    for (uint32_t i = 0; i < chunks_.size(); ++i) {
        for (uint32_t s = 0; s < chunks_[i].slotCount; ++s) {
            if (isFree[chunks_[i].baseSlot + s]) {
                ++freeCountPerChunk[i];
            }
        }
    }

    // From the last chunk backwards, mark fully-free chunks for release
    // as long as the remaining pool stays >= INITIAL_SLOTS.
    uint32_t remainingSlots = totalSlots_;
    std::vector<bool> shouldRelease(chunks_.size(), false);
    uint32_t releasedChunks = 0;
    for (uint32_t idx = static_cast<uint32_t>(chunks_.size()); idx > 0; --idx) {
        uint32_t i = idx - 1;
        if (freeCountPerChunk[i] == chunks_[i].slotCount &&
            remainingSlots - chunks_[i].slotCount >= INITIAL_SLOTS) {
            shouldRelease[i] = true;
            remainingSlots -= chunks_[i].slotCount;
            ++releasedChunks;
        }
    }

    if (remainingSlots == totalSlots_) {
        spinlock_.unlock();
        return;
    }

    // Rebuild chunks_ and freeSlots_ without released chunks.
    std::vector<Chunk> newChunks;
    newChunks.reserve(chunks_.size());
    std::deque<uint32_t> newFreeSlots;
    uint32_t newBaseSlot = 0;
    for (uint32_t i = 0; i < chunks_.size(); ++i) {
        if (shouldRelease[i]) {
            continue; // Chunk destroyed, memory freed via unique_ptr deleter.
        }
        uint32_t oldBase = chunks_[i].baseSlot;
        chunks_[i].baseSlot = newBaseSlot;
        for (uint32_t s = 0; s < chunks_[i].slotCount; ++s) {
            if (isFree[oldBase + s]) {
                newFreeSlots.push_back(newBaseSlot + s);
            }
        }
        newBaseSlot += chunks_[i].slotCount;
        newChunks.push_back(std::move(chunks_[i]));
    }

    chunks_ = std::move(newChunks);
    freeSlots_ = std::move(newFreeSlots);
    totalSlots_ = remainingSlots;
    RS_LOGI("RSScbNodeMemoryPool::Shrink released %{public}u chunks, remaining slots:%{public}u, "
        "chunks:%{public}zu, freeSlots:%{public}zu",
        releasedChunks, totalSlots_, chunks_.size(), freeSlots_.size());
    RS_TRACE_NAME_FMT("ScbPool::Shrink done, releasedChunks=%u, totalSlots=%u, chunks=%zu, freeSlots=%zu",
        releasedChunks, totalSlots_, chunks_.size(), freeSlots_.size());
    spinlock_.unlock();
}

void RSScbNodeMemoryPool::Dump(std::string& result) const
{
    char buf[256];
    int len;

    result.append("=== SCB Memory Pool Status ===\n");

    len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Slot Size: %zu bytes\n", slotSize_);
    result.append(buf, len);

    len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Total Slots: %u\n", totalSlots_);
    result.append(buf, len);

    uint32_t usedSlots = totalSlots_ - static_cast<uint32_t>(freeSlots_.size());
    len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Used Slots: %u\n", usedSlots);
    result.append(buf, len);

    len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Free Slots: %zu\n", freeSlots_.size());
    result.append(buf, len);

    float usageRate = totalSlots_ > 0 ? (float)usedSlots / totalSlots_ * 100 : 0.0f;
    len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Usage Rate: %.2f%%\n", usageRate);
    result.append(buf, len);

    len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Chunks Count: %zu\n", chunks_.size());
    result.append(buf, len);

    size_t freeQueueSize = RSRenderNodeAllocator::Instance().GetScbFreeQueueSize();
    uint32_t liveNodes = (usedSlots >= static_cast<uint32_t>(freeQueueSize))
        ? usedSlots - static_cast<uint32_t>(freeQueueSize) : 0;
    len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Free Queue: %zu (destructed nodes waiting for reuse)\n",
        freeQueueSize);
    result.append(buf, len);

    len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Live Nodes: %u (actual alive nodes in pool)\n", liveNodes);
    result.append(buf, len);

    // Fragmentation analysis
    if (chunks_.size() > 1) {
        len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Fragmentation: %zu chunks for %u slots\n",
            chunks_.size(), totalSlots_);
        result.append(buf, len);
        len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Avg Slots Per Chunk: %.1f\n",
            (float)totalSlots_ / chunks_.size());
        result.append(buf, len);
    }

    // Per-chunk detail
    result.append("\n--- Per-Chunk Detail ---\n");
    uint32_t chunkIndex = 0;
    for (const auto& chunk : chunks_) {
        uint32_t usedInChunk = 0;
        for (uint32_t s = 0; s < chunk.slotCount; ++s) {
            uint32_t slotId = chunk.baseSlot + s;
            bool isFree = std::find(freeSlots_.begin(), freeSlots_.end(), slotId) != freeSlots_.end();
            if (!isFree) {
                ++usedInChunk;
            }
        }
        len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "Chunk[%u]: base=%u, count=%u, used=%u, free=%u, addr=%p\n",
            chunkIndex++, chunk.baseSlot, chunk.slotCount, usedInChunk,
            chunk.slotCount - usedInChunk, chunk.memory.get());
        result.append(buf, len);
    }
    result.append("==============================\n");
}

RSScbNodeMemoryPool::SlotInfo RSScbNodeMemoryPool::GetSlotInfo(const void* ptr) const
{
    SlotInfo info;
    if (ptr == nullptr) {
        return info;
    }
    spinlock_.lock();
    for (uint32_t i = 0; i < chunks_.size(); ++i) {
        const char* base = chunks_[i].memory.get();
        const char* end = base + chunks_[i].slotCount * slotSize_;
        const char* p = static_cast<const char*>(ptr);
        if (p >= base && p < end) {
            info.found = true;
            info.chunkIndex = i;
            info.slotInChunk = static_cast<uint32_t>((p - base) / slotSize_);
            info.globalSlot = chunks_[i].baseSlot + info.slotInChunk;
            break;
        }
    }
    spinlock_.unlock();
    return info;
}

} // namespace Rosen
} // namespace OHOS
