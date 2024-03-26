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

#ifndef MEM_ALLOCATOR_H
#define MEM_ALLOCATOR_H

#include <memory>
#include <mutex>
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API MemAllocator {
public:
    static constexpr uint32_t MEMORY_EXPANSION_FACTOR = 2;
    static constexpr size_t ALIGN_SIZE = 4;

    MemAllocator();
    ~MemAllocator();

    /**
     * @brief       Creates a read-only memory allocator form a read-only buffer that will not be freed when destroyed.
     * @param data  A read-only buffer.
     * @param size  The size of a read-only buffer.
     * @return      true if build succeeded, otherwise false.
     */
    bool BuildFromData(const void* data, size_t size);

    /**
     * @brief       Creates a memory allocator by copying the specified data.
     * @param data  A read-only buffer.
     * @param size  The size of a read-only buffer.
     * @return      true if build succeeded, otherwise false.
     */
    bool BuildFromDataWithCopy(const void* data, size_t size);

    /**
     * @brief   Creates an object of T from a contiguous buffer in the memory allocator.
     * @param T     The name of object class.
     * @param Args  Constructs arguments.
     * @return  The pointer to an object of T.
     */
    template<typename T, typename... Args>
    T* Allocate(Args&&... args)
    {
        if (isReadOnly_) {
            return nullptr;
        }

        if (capacity_ - size_ < sizeof(T)) {
            // The capacity is not enough, expand the capacity
            if (Resize((capacity_ + sizeof(T)) * MEMORY_EXPANSION_FACTOR) == false) {
                return nullptr;
            }
        }
        T* obj = nullptr;
        void* addr = static_cast<void*>(startPtr_ + size_);
        obj = new (addr) T{std::forward<Args>(args)...};
        if (obj) {
            size_ += sizeof(T);
        }
        return obj;
    }

    /**
     * @brief Copies a read-only buffer into contiguous memory held by the memory allocator.
     * @param data A ready-only buffer.
     * @param size The size of ready-only buffer.
     */
    void* Add(const void* data, size_t size);

    /**
     * @brief Gets the size of the contiguous memory buffer held by MemAllocator.
     */
    size_t GetSize() const;

    /**
     * @brief Gets the address of the contiguous memory buffer held by MemAllocator.
     */
    const void* GetData() const;

    /**
     * @brief Gets the offset from the contiguous memory buffer header pointer
     * held by the MemAllocator based on the addr.
     * @param addr To get the offset from the header pointer.
     */
    uint32_t AddrToOffset(const void* addr) const;

    /**
     * @brief Gets the address of the contiguous memory buffer held by the memory allocator from the offset.
     * @param offset To get the address of the offset.
     */
    void* OffsetToAddr(size_t offset) const;

    void ClearData();

    MemAllocator(MemAllocator&&) = delete;
    MemAllocator(const MemAllocator&) = default;
    MemAllocator& operator=(MemAllocator&&) = delete;
    MemAllocator& operator=(const MemAllocator&) = default;
private:
    bool Resize(size_t size);
    void Clear();

    bool isReadOnly_;
    size_t capacity_;   // The size of the memory block
    size_t size_;       // The size already used
    char* startPtr_;    // Points to the beginning of the memory block
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // MEM_ALLOCATOR_H
