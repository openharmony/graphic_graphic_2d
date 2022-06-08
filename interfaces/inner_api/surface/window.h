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

#include <stdint.h>
#include <buffer_handle.h>

#ifdef __cplusplus
extern "C" {
#endif

struct NativeWindow;
struct NativeWindowBuffer;

typedef struct NativeWindow OHNativeWindow;
typedef struct NativeWindowBuffer OHNativeWindowBuffer;

#define MKMAGIC(a, b, c, d) (((a) << 24) + ((b) << 16) + ((c) << 8) + ((d) << 0))

enum NativeObjectMagic {
    NATIVE_OBJECT_MAGIC_WINDOW = MKMAGIC('W', 'I', 'N', 'D'),
    NATIVE_OBJECT_MAGIC_WINDOW_BUFFER = MKMAGIC('W', 'B', 'U', 'F'),
};

enum NativeObjectType {
    NATIVE_OBJECT_WINDOW,
    NATIVE_OBJECT_WINDOW_BUFFER,
};

struct Region {
    struct Rect {
        int32_t x;
        int32_t y;
        uint32_t w;
        uint32_t h;
    } *rects;           // if nullptr,  fill the Buffer dirty size by defualt
    int32_t rectNumber; // if rectNumber is 0, fill the Buffer dirty size by defualt
};

enum NativeWindowOperation {
    SET_BUFFER_GEOMETRY,    // ([in] int32_t height, [in] int32_t width)
    GET_BUFFER_GEOMETRY,    // ([out] int32_t *height, [out] int32_t *width)
    GET_FORMAT,             // ([out] int32_t *format)
    SET_FORMAT,             // ([in] int32_t format)
    GET_USAGE,              // ([out] int32_t *usage)
    SET_USAGE,              // ([in] int32_t usage)
    SET_STRIDE,             // ([in] int32_t stride)
    GET_STRIDE,             // ([out] int32_t *stride)
    SET_SWAP_INTERVAL,      // ([in] int32_t interval)
    GET_SWAP_INTERVAL,      // ([out] int32_t *interval)
    SET_TIMEOUT,            // ([in] int32_t timeout)
    GET_TIMEOUT,            // ([out] int32_t *timeout)
    SET_COLOR_GAMUT,        // ([in] int32_t colorGamut)
    GET_COLOR_GAMUT,        // ([out int32_t *colorGamut])
    SET_TRANSFORM,          // ([in] int32_t transform)
    GET_TRANSFORM,          // ([out] int32_t *transform)
    SET_SCALING_MODE,       // ([in] int32_t scalingMode)
    GET_SCALING_MODE,       // ([out] int32_t *scalingMode)
};

typedef enum {
    OH_MATAKEY_RED_PRIMARY_X = 0,
    OH_MATAKEY_RED_PRIMARY_Y = 1,
    OH_MATAKEY_GREEN_PRIMARY_X = 2,
    OH_MATAKEY_GREEN_PRIMARY_Y = 3,
    OH_MATAKEY_BLUE_PRIMARY_X = 4,
    OH_MATAKEY_BLUE_PRIMARY_Y = 5,
    OH_MATAKEY_WHITE_PRIMARY_X = 6,
    OH_MATAKEY_WHITE_PRIMARY_Y = 7,
    OH_MATAKEY_MAX_LUMINANCE = 8,
    OH_MATAKEY_MIN_LUMINANCE = 9,
    OH_MATAKEY_MAX_CONTENT_LIGHT_LEVEL = 10,
    OH_MATAKEY_MAX_FRAME_AVERAGE_LIGHT_LEVEL = 11,
    OH_MATAKEY_HDR10_PLUS = 12,
    OH_MATAKEY_HDR_VIVID = 13,
} OHHDRMetadataKey;

typedef struct {
    OHHDRMetadataKey key;
    float value;
} OHHDRMetaData;

// pSurface type is OHOS::sptr<OHOS::Surface>*
OHNativeWindow* CreateNativeWindowFromSurface(void* pSurface);
void DestoryNativeWindow(OHNativeWindow* window);

// pSurfaceBuffer type is OHOS::sptr<OHOS::SurfaceBuffer>*
OHNativeWindowBuffer* CreateNativeWindowBufferFromSurfaceBuffer(void* pSurfaceBuffer);
void DestoryNativeWindowBuffer(OHNativeWindowBuffer* buffer);

int32_t NativeWindowRequestBuffer(OHNativeWindow *window, /* [out] */ OHNativeWindowBuffer **buffer,
    /* [out] get release fence */ int *fenceFd);
int32_t NativeWindowFlushBuffer(OHNativeWindow *window, OHNativeWindowBuffer *buffer,
    int fenceFd, Region region);
int32_t NativeWindowCancelBuffer(OHNativeWindow *window, OHNativeWindowBuffer *buffer);

// The meaning and quantity of parameters vary according to the code type.
// For details, see the NativeWindowOperation comment.
int32_t NativeWindowHandleOpt(OHNativeWindow *window, int code, ...);
BufferHandle *GetBufferHandleFromNative(OHNativeWindowBuffer *buffer);

// NativeObject: NativeWindow, NativeWindowBuffer
int32_t NativeObjectReference(void *obj);
int32_t NativeObjectUnreference(void *obj);
int32_t GetNativeObjectMagic(void *obj);

int32_t NativeWindowSetMetaData(OHNativeWindow *window, int32_t sequence, int32_t size,
                                const OHHDRMetaData *metaData);
int32_t NativeWindowSetMetaDataSet(OHNativeWindow *window, int32_t sequence, OHHDRMetadataKey key,
                                   int32_t size, const uint8_t *metaData);

#ifdef __cplusplus
}
#endif

#endif