/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NDK_INCLUDE_NATIVE_WINDOW_H_
#define NDK_INCLUDE_NATIVE_WINDOW_H_

#include "external_window.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MKMAGIC(a, b, c, d) (((a) << 24) + ((b) << 16) + ((c) << 8) + ((d) << 0))

enum NativeObjectMagic {
    NATIVE_OBJECT_MAGIC_WINDOW = MKMAGIC('W', 'I', 'N', 'D'),
    NATIVE_OBJECT_MAGIC_WINDOW_BUFFER = MKMAGIC('W', 'B', 'U', 'F'),
};

// pSurface type is OHOS::sptr<OHOS::Surface>*
OHNativeWindow* CreateNativeWindowFromSurface(void* pSurface);
void DestoryNativeWindow(OHNativeWindow* window);

// pSurfaceBuffer type is OHOS::sptr<OHOS::SurfaceBuffer>*
OHNativeWindowBuffer* CreateNativeWindowBufferFromSurfaceBuffer(void* pSurfaceBuffer);
OHNativeWindowBuffer* CreateNativeWindowBufferFromNativeBuffer(OH_NativeBuffer* nativeBuffer);
void DestroyNativeWindowBuffer(OHNativeWindowBuffer* buffer);

int32_t NativeWindowRequestBuffer(OHNativeWindow *window, /* [out] */ OHNativeWindowBuffer **buffer,
    /* [out] get release fence */ int *fenceFd);
int32_t NativeWindowFlushBuffer(OHNativeWindow *window, OHNativeWindowBuffer *buffer,
    int fenceFd, Region region);
int32_t GetLastFlushedBuffer(OHNativeWindow *window, OHNativeWindowBuffer **buffer,
    int *fenceFd, float matrix[16]);
int32_t NativeWindowCancelBuffer(OHNativeWindow *window, OHNativeWindowBuffer *buffer);

// The meaning and quantity of parameters vary according to the code type.
// For details, see the NativeWindowOperation comment.
int32_t NativeWindowHandleOpt(OHNativeWindow *window, int code, ...);
BufferHandle *GetBufferHandleFromNative(OHNativeWindowBuffer *buffer);

// NativeObject: NativeWindow, NativeWindowBuffer
int32_t NativeObjectReference(void *obj);
int32_t NativeObjectUnreference(void *obj);
int32_t GetNativeObjectMagic(void *obj);

int32_t NativeWindowSetScalingMode(OHNativeWindow *window, uint32_t sequence, OHScalingMode scalingMode);
int32_t NativeWindowSetMetaData(OHNativeWindow *window, uint32_t sequence, int32_t size,
                                const OHHDRMetaData *metaData);
int32_t NativeWindowSetMetaDataSet(OHNativeWindow *window, uint32_t sequence, OHHDRMetadataKey key,
                                   int32_t size, const uint8_t *metaData);
int32_t NativeWindowSetTunnelHandle(OHNativeWindow *window, const OHExtDataHandle *handle);
int32_t GetSurfaceId(OHNativeWindow *window, uint64_t *surfaceId);
int32_t CreateNativeWindowFromSurfaceId(uint64_t surfaceId, OHNativeWindow **window);

#ifdef __cplusplus
}
#endif

#endif