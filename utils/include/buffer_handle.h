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

#ifndef UTILS_INCLUDE_BUFFER_HANDLE_H
#define UTILS_INCLUDE_BUFFER_HANDLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#define BUFFER_HANDLE_MAX_RESERVE_INTS 64
#define BUFFER_HANDLE_MAX_RESERVE_FDS 64

typedef struct {
    int32_t fd;           /**< buffer fd, -1 if not supported */
    int32_t width;        /**< the width of memory */
    int32_t stride;       /**< the stride of memory */
    int32_t height;       /**< the heigh of memory */
    int32_t size;         /* < size of memory */
    int32_t format;       /**< the format of memory */
    int64_t usage;        /**< the usage of memory */
    void *virAddr;        /**< Virtual address of memory  */
    uint64_t phyAddr;     /**< Physical address */
    int32_t key;          /**< Shared memory key */
    uint32_t reserveFds;  /**< the number of reserved fd value */
    uint32_t reserveInts; /**< the number of reserved integer value */
    int32_t reserve[0];   /**< the data */
} BufferHandle;

/**
* @Description: Init buffer handle, and must be freeed by FreeBufferHandle to avoid memory leak
* @param reserveInts The number of reserved integers
* @param reserveFds The number of reserved fds
* @return Returns pointer to buffer handle if the operation is successful; returns <b>nullptr</b> otherwise.
*/
BufferHandle* AllocateBufferHandle(uint32_t reserveInts, uint32_t reserveFds);

/**
* @Description: Free buffer handle allocated by AllocateBufferHandle, and close the fd at the same time.
* @param handle Buffer handle which is to be freed.
* @return  Returns <b>0</b> if the operation is successful; returns <b>-1</b> if failed
*/
int32_t FreeBufferHandle(BufferHandle* handle);

/**
* @Description: clone a new buffer handle based on given buffer handle
* @param handle Buffer handle which is to be cloned.
* @return  Returns pointer to buffer handle if the operation is successful; returns <b>nullptr</b> otherwise.
*/
BufferHandle* CloneBufferHandle(const BufferHandle* handle);

#ifdef __cplusplus
}
#endif

#endif // UTILS_INCLUDE_BUFFER_HANDLE_H
