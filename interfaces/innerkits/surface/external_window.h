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
 * @brief Provides NativeWindow functions
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @since 8
 * @version 1.0
 */

/**
 * @file external_window.h
 *
 * @brief API for accessing a native window
 *
 * @since 8
 * @version 1.0
 */

#include "window.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a NativeWindow from producer surface, Each call will generates a new NativeWindow
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param pSurface Indicates producer surface
 * @return NativeWindow
 * @since 8
 * @version 1.0
 */
struct NativeWindow* OH_NativeWindow_CreateNativeWindowFromSurface(void* pSurface);

/**
 * @brief Unreference NativeWindow object, When the reference count == 0, destroy the NativeWindow
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates NativeWindow
 * @since 8
 * @version 1.0
 */
void OH_NativeWindow_DestroyNativeWindow(struct NativeWindow* window);

/**
 * @brief Create a NativeWindowBuffer from producer surface buffer
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param pSurfaceBuffer Indicates producer surface buffer
 * @return NativeWindowBuffer
 * @since 8
 * @version 1.0
 */
struct NativeWindowBuffer* OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(void* pSurfaceBuffer);

/**
 * @brief Unreference NativeWindowBuffer object, when the reference count == 0, destroy a NativeWindow buffer
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param buffer Indicates NativeWindow buffer
 * @since 8
 * @version 1.0
 */
void OH_NativeWindow_DestroyNativeWindowBuffer(struct NativeWindowBuffer* buffer);

/**
 * @brief Dequeue a buffer from NativeWindow
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates NativeWindow
 * @param buffer Indicates [out] get NativeWindowBuffer
 * @param fenceFd Indicates [out] get release fence
 * @return GSError
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowRequestBuffer(struct NativeWindow *window,
    struct NativeWindowBuffer **buffer, int *fenceFd);

/**
 * @brief Queue a buffer to NativeWindow
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates NativeWindow
 * @param buffer Indicates NativeWindowBuffer
 * @param fenceFd Indicates acquire fence
 * @param region Indicates the Buffer dirty size
 * @return GSError
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowFlushBuffer(struct NativeWindow *window, struct NativeWindowBuffer *buffer,
    int fenceFd, Region region);

/**
 * @brief Cancel the NativeWindowBuffer to be queued
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates NativeWindow
 * @param buffer Indicates NativeWindowBuffer
 * @return GSError
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowCancelBuffer(struct NativeWindow *window, struct NativeWindowBuffer *buffer);

/**
 * @brief Handle the NativeWindowOperation of the NativeWindow
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window Indicates NativeWindow
 * @param code Indicates NativeWindowOperation
 * @return GSError
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowHandleOpt(struct NativeWindow *window, int code, ...);

/**
 * @brief Get the BufferHandle from the NativeWindowBuffer
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param buffer Indicates NativeWindowBuffer
 * @return BufferHandle
 * @since 8
 * @version 1.0
 */
BufferHandle *OH_NativeWindow_GetBufferHandleFromNative(struct NativeWindowBuffer *buffer);

/**
 * @brief Increase the reference count of the NativeObject
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param obj Indicates NativeWindow or NativeWindowBuffer
 * @return GSError
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeObjectReference(void *obj);

/**
 * @brief decrease the reference count of the NativeObject
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param obj Indicates NativeWindow or NativeWindowBuffer
 * @return GSError
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeObjectUnreference(void *obj);

/**
 * @brief Get the MagicId of the NativeObject
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param obj Indicates NativeWindow or NativeWindowBuffer
 * @return MagicId
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_GetNativeObjectMagic(void *obj);

#ifdef __cplusplus
}
#endif

/** @} */
#endif