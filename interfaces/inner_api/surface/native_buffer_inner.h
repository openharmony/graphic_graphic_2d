/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NDK_INCLUDE_NATIVE_BUFFER_INNER_H_
#define NDK_INCLUDE_NATIVE_BUFFER_INNER_H_

#include <stdint.h>
#include <buffer_handle.h>
#include "native_buffer.h"
#include "native_window.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct NativeWindowBuffer OHNativeWindowBuffer;

const BufferHandle* OH_NativeBuffer_GetBufferHandle(const OH_NativeBuffer *buffer);

void OH_NativeBuffer_GetNativeBufferConfig(const OH_NativeBuffer *buffer, OH_NativeBuffer_Config* config);

OH_NativeBuffer* OH_NativeBufferFromNativeWindowBuffer(OHNativeWindowBuffer* nativeWindowBuffer);

#ifdef __cplusplus
}
#endif

/** @} */
#endif