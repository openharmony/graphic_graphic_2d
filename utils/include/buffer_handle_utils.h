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

#ifndef INCLUDE_BUFFER_HANDLE_UTILS_H
#define INCLUDE_BUFFER_HANDLE_UTILS_H

#include <stdint.h>
#include <buffer_handle.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#endif // INCLUDE_BUFFER_HANDLE_UTILS_H
