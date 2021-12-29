/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
MemAllocater& MemAllocater::GetInstance()
{
    static MemAllocater allocater;
    return allocater;
}

MemAllocater::~MemAllocater()
{
    for (void* ptr : blocks_) {
        if (ptr != nullptr) {
            free(ptr);
        }
    }
    blocks_.clear();
    memCaches_.clear();
}

void* MemAllocater::Alloc(size_t size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    Cache* cachePtr = nullptr;
    auto itr = memCaches_.find(size);
    if (itr == memCaches_.end()) {
        Cache tempCache;
        memCaches_.insert(std::pair<size_t, Cache>(size, tempCache));
        itr = memCaches_.find(size);
        cachePtr = &(itr->second);
        cachePtr->reserve(sizeStep_);
    } else {
        cachePtr = &(itr->second);
    }

    if (cachePtr == nullptr) {
        return nullptr;
    }
    size_t memSize = (size + sizeof(BlockHead));
    if (cachePtr->empty()) {
        char* block = static_cast<char*>(malloc(memSize * sizeStep_));
        if (block == nullptr) {
            return nullptr;
        }
        blocks_.push_back(block);
        for (unsigned i = 0; i < sizeStep_; ++i) {
            cachePtr->push_back(block + (i * memSize));
        }
    }

    char* mem = cachePtr->back();
    cachePtr->pop_back();
    BlockHead* head = reinterpret_cast<BlockHead*>(mem);
    head->size = static_cast<int>(size);
    return head->ptr;
}

void MemAllocater::Free(void* ptr)
{
    if (ptr == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    char* p = static_cast<char*>(ptr) - sizeof(BlockHead);
    BlockHead* head = reinterpret_cast<BlockHead*>(p);
    auto itr = memCaches_.find(head->size);
    if (itr == memCaches_.end()) {
        free(p);
    } else {
        itr->second.push_back(p);
    }
}

void* MemObject::operator new(size_t size)
{
    return MemAllocater::GetInstance().Alloc(size);
}

void MemObject::operator delete(void* ptr)
{
    return MemAllocater::GetInstance().Free(ptr);
}

void* MemObject::operator new(size_t size, const std::nothrow_t&) noexcept
{
    return MemAllocater::GetInstance().Alloc(size);
}

void MemObject::operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    return MemAllocater::GetInstance().Free(ptr);
}
} // namespace Rosen
} // namespace OHOS
