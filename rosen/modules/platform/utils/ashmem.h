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

 /**
 * @file ashmem.h
 *
 * @brief Provide class Ashmem implemented in c_utils to operate the
 * Anonymous Shared Memory(Ashmem).
 */

#ifndef UTILS_BASE_ASHMEM_H
#define UTILS_BASE_ASHMEM_H

#include <cstddef>
#include <linux/ashmem.h>
#include "refbase.h"
#include "parcel.h"

namespace OHOS {
/**
 * @brief Create ashmem in kernel with specified name and size.
 *
 * @param name Name for identifying the ashmem in kernel.
 * @param size Size of the ashmem region.
 * @return File descriptor of the ashmem.
 */
int AshmemCreate(const char *name, size_t size);

/**
 * @brief Set protection flag of specified ashmem region in kernel.

 * @param fd Corresponding file descriptor.
 * @param prot Value of protection flag.
 * @return Return 0 on success, -1 on failure.
 */
int AshmemSetProt(int fd, int prot);

/**
 * @brief Get size of specified ashmem region in kernel.
 *
 * @param fd Corresponding file descriptor.
 * @return Size of ashmem.
 */
int AshmemGetSize(int fd);

/**
 * @brief Ashmem implemented in c_utils, to operate the
 * Anonymous Shared Memory(Ashmem).
 *
 * Including create, map an ashmem region and thus write/read to/from it, etc.
 *
 * @note Ashmem object should be unmapped and closed manually, though managed
 * by smart pointer.
 */
class Ashmem : public virtual RefBase {
public:
    /**
     * @brief Create an Ashmem object with specified name and region size.
     *
     * File '/dev/ashmem' will be opened whose file descriptor will be held by
     * this Ashmem object.
     *
     * @param name Name for identifying the ashmem in kernel.
     * @param size Size of the ashmem region.
     * @return An Ashmem object referenced by a smart pointer.
     * @note Memory has not been mapped, use `MapAshmem()` before
     * writing/reading.
     */
    static sptr<Ashmem> CreateAshmem(const char *name, int32_t size);

    /**
     * @brief Close the ashmem(i.e. close it via file descriptor).
     *
     * All inner parameters will be cleared.
     *
     * @note An ashmem  will be unmapped first by `UnmapAshmem()` before close.
     */
    void CloseAshmem();

    /**
     * @brief Map the ashmem region in the kernel to user space.
     *
     * @param mapType Protection flag of the mapped region in user space.
     * @return True if mapping successful.
     */
    bool MapAshmem(int mapType);

    /**
     * @brief Map the ashmem region in Read/Write mode.
     *
     * It calls `MapAshmem(PROT_READ | PROT_WRITE)`.
     *
     * @return True if mapping successful.
     */
    bool MapReadAndWriteAshmem();

    /**
     * @brief Map the ashmem region in Read-Only mode.
     *
     * It calls `MapAshmem(PROT_READ)`.
     *
     * @return True if mapping successful.
     */
    bool MapReadOnlyAshmem();

    /**
     * @brief Unmap the ashmem.
     *
     * Unmap when mapped ashmem exists, protection flag will be cleared
     * meanwhile.
     */
    void UnmapAshmem();

    /**
     * @brief Set the protection flag of ashmem region in kernel.
     *
     * @param protectionType Value of protection flag.
     * @return True if set successful.
     */
    bool SetProtection(int protectionType);

    /**
     * @brief Get the protection flag of ashmem region in kernel.
     *
     * @return Value of protection flag. Refer to linux manual.
     */
    int GetProtection();

    /**
     * @brief Get the size of ashmem region in kernel.
     *
     * @return Value of size.
     */
    int32_t GetAshmemSize();

    /**
     * @brief Write data to the `offset` position of ashmem region.
     *
     * Bounds and protection flag will be checked.
     *
     * @param data Pointer to input data.
     * @param size Size of the input data(bytes).
     * @param offset Offset from beginning of the region.
     * @return True if writting successful. False if data to be write overflows
     * or protection flag is illegal.
     * @note Require write authority of both ashmem in kernel and the mapped
     * one in user space.
     */
    bool WriteToAshmem(const void *data, int32_t size, int32_t offset);

    /**
     * @brief Read data from the `offset` position of ashmem region.
     *
     * Bounds and protection flag will be checked.
     *
     * @param size Size of data to be read(bytes).
     * @param offset Offset from beginning of the region.
     * @return Void-type pointer to the data. `nullptr` returns if data to be
     * read overflows or protection flag is illegal.
     * @note Require read authority of both ashmem in kernel and the mapped one
     * in user space.
     */
    const void *ReadFromAshmem(int32_t size, int32_t offset);

    /**
     * @brief Construct a new Ashmem object.
     *
     * @param fd File descriptor of an ashmem in kenrel.
     * @param size Size of the corresponding ashmem region in kernel.
     */
    Ashmem(int fd, int32_t size);
    ~Ashmem() override;

    /**
     * @brief Get file descriptor of the corresponding ashmem in kernel.
     *
     * @return Corresponding file descriptor of this Ashmem object. It will be
     * 0 when ashmem is closed.
     */
    int GetAshmemFd() const
    {
        return memoryFd_;
    };
    
private:
    int memoryFd_; // corresponding file descriptor of ashmem.
    int32_t memorySize_; // size of ashmem.
    int flag_; // protection flag of ashmem in user space.
    void *startAddr_; // start address of ashmem region.
    bool CheckValid(int32_t size, int32_t offset, int cmd);
};
} // namespace OHOS
#endif
