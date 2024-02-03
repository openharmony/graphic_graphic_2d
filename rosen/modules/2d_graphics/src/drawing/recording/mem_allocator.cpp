/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "recording/mem_allocator.h"

#include "securec.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
constexpr size_t LARGE_MALLOC = 200000000;
}
static constexpr size_t MEM_SIZE_MAX = SIZE_MAX;

MemAllocator::MemAllocator() : isReadOnly_(false), capacity_(0), size_(0), startPtr_(nullptr) {}

MemAllocator::~MemAllocator()
{
    Clear();
}

bool MemAllocator::BuildFromData(const void* data, size_t size)
{
    if (!data || size == 0 || size > MEM_SIZE_MAX) {
        return false;
    }

    Clear();
    isReadOnly_ = true;
    startPtr_ = const_cast<char*>(static_cast<const char*>(data));
    capacity_ = size;
    size_ = size;

    return true;
}

bool MemAllocator::BuildFromDataWithCopy(const void* data, size_t size)
{
    if (!data || size == 0 || size > MEM_SIZE_MAX) {
        return false;
    }

    Clear();
    isReadOnly_ = false;
    Add(data, size);

    return true;
}

void MemAllocator::Clear()
{
    if (!isReadOnly_ && startPtr_) {
        delete[] startPtr_;
    }
    isReadOnly_ = true;
    startPtr_ = nullptr;
    capacity_ = 0;
    size_ = 0;
}

void MemAllocator::ClearData()
{
    if (!isReadOnly_ && startPtr_) {
        delete[] startPtr_;
    }
    startPtr_ = nullptr;
    capacity_ = 0;
    size_ = 0;
}

bool MemAllocator::Resize(size_t size)
{
    if (isReadOnly_ || size == 0 || size > MEM_SIZE_MAX || size < size_) {
        return false;
    }
    if (size > LARGE_MALLOC) {
        LOGW("MemAllocator::Resize this time malloc large memory, size:%{public}zu", size);
    }
    char* newData = new char[size];
    if (!newData) {
        return false;
    }

    if (startPtr_) {
        if (!memcpy_s(newData, size, startPtr_, size_)) {
            delete[] startPtr_;
        } else {
            delete[] newData;
            return false;
        }
    }
    startPtr_ = newData;
    capacity_ = size;
    return true;
}

void* MemAllocator::Add(const void* data, size_t size)
{
    if (isReadOnly_ || !data || size == 0 || size > MEM_SIZE_MAX) {
        return nullptr;
    }
    auto current = startPtr_ + size_;
    if (auto mod = reinterpret_cast<uintptr_t>(current) % ALIGN_SIZE; mod != 0) {
        size_ += ALIGN_SIZE - mod;
    }

    if (capacity_ == 0 || capacity_ < size_ + size) {
        // The capacity is not enough, expand the capacity
        if (Resize((capacity_ + size) * MEMORY_EXPANSION_FACTOR) == false) {
            return nullptr;
        }
    }
    if (!memcpy_s(startPtr_ + size_, capacity_ - size_, data, size)) {
        size_ += size;
        return startPtr_ + size_ - size;
    } else {
        return nullptr;
    }
}

size_t MemAllocator::GetSize() const
{
    return size_;
}

const void* MemAllocator::GetData() const
{
    return startPtr_;
}

uint32_t MemAllocator::AddrToOffset(const void* addr) const
{
    if (!addr) {
        return 0;
    }

    auto offset = static_cast<uint32_t>(static_cast<const char*>(addr) - startPtr_);
    if (offset > size_) {
        return 0;
    }
    return offset;
}

void* MemAllocator::OffsetToAddr(size_t offset) const
{
    if (offset >= size_) {
        return nullptr;
    }

    return static_cast<void*>(startPtr_ + offset);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
