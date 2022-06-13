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

#include "window.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @param code Indicates the operation code.
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
 * @param sequence Indicates the senquence to a produce buffer.
 * @param scalingMode Indicates the enum value to <b>OHScalingMode</b>
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowSetScalingMode(OHNativeWindow *window, uint32_t sequence,
                                                   OHScalingMode scalingMode);

/**
 * @brief Sets metaData of a native window.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @param sequence Indicates the senquence to a produce buffer.
 * @param size Indicates the size of a <b>OHHDRMetaData</b> vector.
 * @param metaDate Indicates the pointer to a <b>OHHDRMetaData</b> vector.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowSetMetaData(OHNativeWindow *window, uint32_t sequence, int32_t size,
                                                const OHHDRMetaData *metaData);

/**
 * @brief Sets metaDataSet of a native window.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates the pointer to a <b>NativeWindow</b> instance.
 * @param sequence Indicates the senquence to a produce buffer.
 * @param key Indicates the enum value to <b>OHHDRMetadataKey</b>
 * @param size Indicates the size of a uint8_t vector.
 * @param metaDate Indicates the pointer to a uint8_t vector.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowSetMetaDataSet(OHNativeWindow *window, uint32_t sequence, OHHDRMetadataKey key,
                                                   int32_t size, const uint8_t *metaData);

#ifdef __cplusplus
}
#endif

/** @} */
#endif