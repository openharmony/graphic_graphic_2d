/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides functions such as 2D graphics rendering, text drawing, and image display.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 8
 * @version 1.0
 */

/**
 * @file drawing_surface.h
 *
 * @brief Declares functions related to the <b>OH_Drawing_Surface</b> object in the drawing module.
 *
 * @kit ArkGraphics2D
 * @library libnative_drawing.so
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @since 12
 * @version 1.0
 */

#ifndef C_INCLUDE_DRAWING_SURFACE_H
#define C_INCLUDE_DRAWING_SURFACE_H

#include "drawing_error_code.h"
#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Drawing_Surface</b> object on GPU indicated by context.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param gpuContext Indicates the pointer to an <b>OH_Drawing_GpuContext</b> object.
 * @param flag Indicates whether an allocation should count against a cache budget.
 * @param imageInfo Indicates the image info.
 * @return Returns the pointer to the <b>OH_Drawing_Surface</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Surface* OH_Drawing_SurfaceCreateFromGpuContext(
    OH_Drawing_GpuContext* gpuContext, bool flag, OH_Drawing_Image_Info imageInfo);

/**
 * @brief Creates an <b>OH_Drawing_Surface</b> object on GPU indicated by context which is on-screen.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param gpuContext Indicates the pointer to an <b>OH_Drawing_GpuContext</b> object.
 * @param imageInfo Indicates the image info.
 * @param window Indicates the pointer of the screen window.
 * @return Returns the pointer to the <b>OH_Drawing_Surface</b> object created.
 * @since 16
 * @version 1.0
 */
OH_Drawing_Surface* OH_Drawing_SurfaceCreateOnScreen(
    OH_Drawing_GpuContext* gpuContext, OH_Drawing_Image_Info imageInfo, void* window);

/**
 * @brief Gets the canvas that draws into surface.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param surface Indicates the pointer to an <b>OH_Drawing_Surface</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_Canvas</b> object. The returned pointer does not need to be managed
 *         by the caller.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Canvas* OH_Drawing_SurfaceGetCanvas(OH_Drawing_Surface* surface);

/**
 * @brief Resolves all pending GPU operations on the surface.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param surface Indicates the pointer to an <b>OH_Drawing_Surface</b> object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INVALID_PARAMETER} if surface is nullptr.
 * @since 16
 * @version 1.0
 */
OH_Drawing_ErrorCode OH_Drawing_SurfaceFlush(OH_Drawing_Surface* surface);

/**
 * @brief Destroys an <b>OH_Drawing_Surface</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param surface Indicates the pointer to an <b>OH_Drawing_Surface</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_SurfaceDestroy(OH_Drawing_Surface* surface);

#ifdef __cplusplus
}
#endif
/** @} */
#endif