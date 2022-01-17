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

#ifndef NDK_INCLUDE_EXTERNAL_NATIVE_WINDOW_H_
#define NDK_INCLUDE_EXTERNAL_NATIVE_WINDOW_H_

#include "window.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NativeWindow* OH_NATIVE_WINDOW_CreateNativeWindowFromSurface(void* pSurface);
void OH_NATIVE_WINDOW_DestoryNativeWindow(struct NativeWindow* window);

struct NativeWindowBuffer* OH_NATIVE_WINDOW_CreateNativeWindowBufferFromSurfaceBuffer(void* pSurfaceBuffer);
void OH_NATIVE_WINDOW_DestoryNativeWindowBuffer(struct NativeWindowBuffer* buffer);

int32_t OH_NATIVE_WINDOW_NativeWindowRequestBuffer(struct NativeWindow *window,
    /* [out] */ struct NativeWindowBuffer **buffer,
    /* [out] get release fence */ int *fenceFd);
int32_t OH_NATIVE_WINDOW_NativeWindowFlushBuffer(struct NativeWindow *window, struct NativeWindowBuffer *buffer,
    int fenceFd, Region region);
int32_t OH_NATIVE_WINDOW_NativeWindowCancelBuffer(struct NativeWindow *window, struct NativeWindowBuffer *buffer);

int32_t OH_NATIVE_WINDOW_NativeWindowHandleOpt(struct NativeWindow *window, int code, ...);
BufferHandle *OH_NATIVE_WINDOW_GetBufferHandleFromNative(struct NativeWindowBuffer *buffer);

int32_t OH_NATIVE_WINDOW_NativeObjectReference(void *obj);
int32_t OH_NATIVE_WINDOW_NativeObjectUnreference(void *obj);
int32_t OH_NATIVE_WINDOW_GetNativeObjectMagic(void *obj);

#ifdef __cplusplus
}
#endif

#endif