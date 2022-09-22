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

#ifndef NDK_INCLUDE_EXTERNAL_NATIVE_WINDOW_H_
#define NDK_INCLUDE_EXTERNAL_NATIVE_WINDOW_H_

/**
 * @addtogroup NativeWindow
 * @{
 *
 * @brief Provides the native window capability for connection to the EGL.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @since 8
 * @version 1.0
 */

/**
 * @file external_window.h
 *
 * @brief Defines the functions for obtaining and using a native window.
 *
 * @since 8
 * @version 1.0
 */

#include <stdint.h>
#include "buffer_handle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief native window.
 */
struct NativeWindow;

/**
 * @brief native window buffer.
 */
struct NativeWindowBuffer;

/**
 * @brief define the new type name OHNativeWindow for struct NativeWindow.
 */
typedef struct NativeWindow OHNativeWindow;

/**
 * @brief define the new type name OHNativeWindowBuffer for struct NativeWindowBuffer.
 */
typedef struct NativeWindowBuffer OHNativeWindowBuffer;

/**
 * @brief indicates a dirty region where content is updated.
 */
typedef struct Region {
    /** if rects is nullptr, fill the Buffer dirty size by default */
    struct Rect {
        int32_t x;
        int32_t y;
        uint32_t w;
        uint32_t h;
    } *rects;
    /** if rectNumber is 0, fill the Buffer dirty size by default */
    int32_t rectNumber;
}Region;


/**
 * @brief Indicates the operation code in the function OH_NativeWindow_NativeWindowHandleOpt.
 */
enum NativeWindowOperation {
    /**
     * set native window buffer geometry,
     * variable parameter in function is
     * [in] int32_t height, [in] int32_t width
     */
    SET_BUFFER_GEOMETRY,
    /**
     * get native window buffer geometry,
     * variable parameter in function is
     * [out] int32_t *height, [out] int32_t *width
     */
    GET_BUFFER_GEOMETRY,
    /**
     * get native window buffer format,
     * variable parameter in function is
     * [out] int32_t *format
     */
    GET_FORMAT,
    /**
     * set native window buffer format,
     * variable parameter in function is
     * [in] int32_t format
     */
    SET_FORMAT,
    /**
     * get native window buffer usage,
     * variable parameter in function is
     * [out] int32_t *usage.
     */
    GET_USAGE,
    /**
     * set native window buffer usage,
     * variable parameter in function is
     * [in] int32_t usage.
     */
    SET_USAGE,
    /**
     * set native window buffer stride,
     * variable parameter in function is
     * [in] int32_t stride.
     */
    SET_STRIDE,
    /**
     * get native window buffer stride,
     * variable parameter in function is
     * [out] int32_t *stride.
     */
    GET_STRIDE,
    /**
     * set native window buffer swap interval,
     * variable parameter in function is
     * [in] int32_t interval.
     */
    SET_SWAP_INTERVAL,
    /**
     * get native window buffer swap interval,
     * variable parameter in function is
     * [out] int32_t *interval.
     */
    GET_SWAP_INTERVAL,
    /**
     * set native window buffer timeout,
     * variable parameter in function is
     * [in] int32_t timeout.
     */
    SET_TIMEOUT,
    /**
     * get native window buffer timeout,
     * variable parameter in function is
     * [out] int32_t *timeout.
     */
    GET_TIMEOUT,
    /**
     * set native window buffer colorGamut,
     * variable parameter in function is
     * [in] int32_t colorGamut.
     */
    SET_COLOR_GAMUT,
    /**
     * get native window buffer colorGamut,
     * variable parameter in function is
     * [out int32_t *colorGamut].
     */
    GET_COLOR_GAMUT,
    /**
     * set native window buffer transform,
     * variable parameter in function is
     * [in] int32_t transform.
     */
    SET_TRANSFORM,
    /**
     * get native window buffer transform,
     * variable parameter in function is
     * [out] int32_t *transform.
     */
    GET_TRANSFORM,
    /**
     * set native window buffer uiTimestamp,
     * variable parameter in function is
     * [in] uint64_t uiTimestamp.
     */
    SET_UI_TIMESTAMP,
};

/**
 * @brief Indicates Scaling Mode.
 */
typedef enum {
    /**
     * the window content is not updated until a buffer of
     * the window size is received
     */
    OH_SCALING_MODE_FREEZE = 0,
    /**
     * the buffer is scaled in two dimensions to match the window size
     */
    OH_SCALING_MODE_SCALE_TO_WINDOW,
    /**
     * the buffer is uniformly scaled so that the smaller size of
     * the buffer matches the window size
     */
    OH_SCALING_MODE_SCALE_CROP,
    /**
     * the window is clipped to the size of the buffer's clipping rectangle
     * pixels outside the clipping rectangle are considered fully transparent.
     */
    OH_SCALING_MODE_NO_SCALE_CROP,
} OHScalingMode;

/**
 * @brief Enumerates the HDR metadata keys.
 */
typedef enum {
    OH_METAKEY_RED_PRIMARY_X = 0,
    OH_METAKEY_RED_PRIMARY_Y = 1,
    OH_METAKEY_GREEN_PRIMARY_X = 2,
    OH_METAKEY_GREEN_PRIMARY_Y = 3,
    OH_METAKEY_BLUE_PRIMARY_X = 4,
    OH_METAKEY_BLUE_PRIMARY_Y = 5,
    OH_METAKEY_WHITE_PRIMARY_X = 6,
    OH_METAKEY_WHITE_PRIMARY_Y = 7,
    OH_METAKEY_MAX_LUMINANCE = 8,
    OH_METAKEY_MIN_LUMINANCE = 9,
    OH_METAKEY_MAX_CONTENT_LIGHT_LEVEL = 10,
    OH_METAKEY_MAX_FRAME_AVERAGE_LIGHT_LEVEL = 11,
    OH_METAKEY_HDR10_PLUS = 12,
    OH_METAKEY_HDR_VIVID = 13,
} OHHDRMetadataKey;

/**
 * @brief Defines the HDR metadata.
 */
typedef struct {
    OHHDRMetadataKey key;
    float value;
} OHHDRMetaData;

/**
 * @brief Defines the ExtData Handle
 */
typedef struct {
    /**< Handle fd, -1 if not supported */
    int32_t fd;
    /**< the number of reserved integer value */
    uint32_t reserveInts;
    /**< the reserved data */
    int32_t reserve[0];
} OHExtDataHandle;

/**
 * @brief Creates a <b>NativeWindow</b> instance. A new <b>NativeWindow</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param pSurface Indicates the pointer to a <b>ProduceSurface</b>. The type is a pointer to <b>sptr<OHOS::Surface></b>.
 * @return Returns the pointer to the <b>NativeWindow</b> instance created.
 * @since 8
 * @version 1.0
 */
OHNativeWindow* OH_NativeWindow_CreateNativeWindow(void* pSurface);

/**
 * @brief Decreases the reference count of a <b>NativeWindow</b> instance by 1, and when the reference count reaches 0, destroys the instance.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @since 8
 * @version 1.0
 */
void OH_NativeWindow_DestroyNativeWindow(OHNativeWindow* window);

/**
 * @brief Creates a <b>NativeWindowBuffer</b> instance. A new <b>NativeWindowBuffer</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param pSurfaceBuffer Indicates the pointer to a produce buffer. The type is <b>sptr<OHOS::SurfaceBuffer></b>.
 * @return Returns the pointer to the <b>NativeWindowBuffer</b> instance created.
 * @since 8
 * @version 1.0
 */
OHNativeWindowBuffer* OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(void* pSurfaceBuffer);

/**
 * @brief Decreases the reference count of a <b>NativeWindowBuffer</b> instance by 1 and, when the reference count reaches 0, destroys the instance.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param buffer Indicates the pointer to a <b>NativeWindowBuffer</b> instance.
 * @since 8
 * @version 1.0
 */
void OH_NativeWindow_DestroyNativeWindowBuffer(OHNativeWindowBuffer* buffer);

/**
 * @brief Requests a <b>NativeWindowBuffer</b> through a <b>NativeWindow</b> instance for content production.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @param buffer Indicates the double pointer to a <b>NativeWindowBuffer</b> instance.
 * @param fenceFd Indicates the pointer to a file descriptor handle.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowRequestBuffer(OHNativeWindow *window,
    OHNativeWindowBuffer **buffer, int *fenceFd);

/**
 * @brief Flushes the <b>NativeWindowBuffer</b> filled with the content to the buffer queue through a <b>NativeWindow</b> instance for content consumption.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @param buffer Indicates the pointer to a <b>NativeWindowBuffer</b> instance.
 * @param fenceFd Indicates a file descriptor handle, which is used for timing synchronization.
 * @param region Indicates a dirty region where content is updated.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowFlushBuffer(OHNativeWindow *window, OHNativeWindowBuffer *buffer,
    int fenceFd, Region region);

 /**
 * @brief Returns the <b>NativeWindowBuffer</b> to the buffer queue through a <b>NativeWindow</b> instance, without filling in any content. The <b>NativeWindowBuffer</b> can be used for another request.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @param buffer Indicates the pointer to a <b>NativeWindowBuffer</b> instance.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowAbortBuffer(OHNativeWindow *window, OHNativeWindowBuffer *buffer);

/**
 * @brief Sets or obtains the attributes of a native window, including the width, height, and content format.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @param code Indicates the operation code, pointer to <b>NativeWindowOperation</b>.
 * @param ... variable parameter, must correspond to code one-to-one.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowHandleOpt(OHNativeWindow *window, int code, ...);

/**
 * @brief Obtains the pointer to a <b>BufferHandle</b> of a <b>NativeWindowBuffer</b> instance.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param buffer Indicates the pointer to a <b>NativeWindowBuffer</b> instance.
 * @return Returns the pointer to the <b>BufferHandle</b> instance obtained.
 * @since 8
 * @version 1.0
 */
BufferHandle *OH_NativeWindow_GetBufferHandleFromNative(OHNativeWindowBuffer *buffer);

/**
 * @brief Adds the reference count of a native object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param obj Indicates the pointer to a <b>NativeWindow</b> or <b>NativeWindowBuffer</b> instance.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeObjectReference(void *obj);

/**
 * @brief Decreases the reference count of a native object and, when the reference count reaches 0, destroys this object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param obj Indicates the pointer to a <b>NativeWindow</b> or <b>NativeWindowBuffer</b> instance.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeObjectUnreference(void *obj);

/**
 * @brief Obtains the magic ID of a native object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param obj Indicates the pointer to a <b>NativeWindow</b> or <b>NativeWindowBuffer</b> instance.
 * @return Returns the magic ID, which is unique for each native object.
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_GetNativeObjectMagic(void *obj);

/**
 * @brief Sets scalingMode of a native window.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @param sequence Indicates the sequence to a produce buffer.
 * @param scalingMode Indicates the enum value to <b>OHScalingMode</b>
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowSetScalingMode(OHNativeWindow *window, uint32_t sequence,
                                                   OHScalingMode scalingMode);

/**
 * @brief Sets metaData of a native window.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @param sequence Indicates the sequence to a produce buffer.
 * @param size Indicates the size of a <b>OHHDRMetaData</b> vector.
 * @param metaDate Indicates the pointer to a <b>OHHDRMetaData</b> vector.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowSetMetaData(OHNativeWindow *window, uint32_t sequence, int32_t size,
                                                const OHHDRMetaData *metaData);

/**
 * @brief Sets metaDataSet of a native window.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @param sequence Indicates the sequence to a produce buffer.
 * @param key Indicates the enum value to <b>OHHDRMetadataKey</b>
 * @param size Indicates the size of a uint8_t vector.
 * @param metaDate Indicates the pointer to a uint8_t vector.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowSetMetaDataSet(OHNativeWindow *window, uint32_t sequence, OHHDRMetadataKey key,
                                                   int32_t size, const uint8_t *metaData);

/**
 * @brief Sets tunnel handle of a native window.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @param handle Indicates the pointer to a <b>OHExtDataHandle</b>.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowSetTunnelHandle(OHNativeWindow *window, const OHExtDataHandle *handle);

#ifdef __cplusplus
}
#endif

/** @} */
#endif