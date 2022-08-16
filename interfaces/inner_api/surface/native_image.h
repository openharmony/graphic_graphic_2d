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

#ifndef NDK_INCLUDE_NATIVE_IMAGE_H_
#define NDK_INCLUDE_NATIVE_IMAGE_H_

/**
 * @addtogroup OH_NativeImage
 * @{
 *
 * @brief Provides the native image capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeImage
 * @since 9
 * @version 1.0
 */

/**
 * @file native_image.h
 *
 * @brief Defines the functions for obtaining and using a native image.
 *
 * @since 9
 * @version 1.0
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct OH_NativeImage;
typedef struct OH_NativeImage OH_NativeImage;
typedef struct NativeWindow OHNativeWindow;

/**
 * @brief Create a <b>OH_NativeImage</b> related to an OPENGL ES texture and target. \n
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeImage
 * @param textureId Indicates the id of the OPENGL ES texture which the native image attached to.
 * @param textureTarget Indicates the OPENGL ES target.
 * @return Returns the pointer to the <b>OH_NativeImage</b> instance created if the operation is successful, \n
 * returns <b>NULL</b> otherwise.
 * @since 9
 * @version 1.0
 */
OH_NativeImage* OH_NativeImage_Create(uint32_t textureId, uint32_t textureTarget);

/**
 * @brief Acquire the OHNativeWindow for the OH_NativeImage. This OHNativeWindow should be released by \n
 * OH_NativeWindow_DestroyNativeWindow when no longer needed.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @return Returns the pointer to the OHNativeWindow if the operation is successful, returns <b>NULL</b> otherwise.
 * @since 9
 * @version 1.0
 */
OHNativeWindow* OH_NativeImage_AcquireNativeWindow(OH_NativeImage* image);

/**
 * @brief Attach the OH_NativeImage to OPENGL ES context, and the OPENGL ES texture is bound to the \n
 * GL_TEXTURE_EXTERNAL_OES, which will update by the OH_NativeImage.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param textureId Indicates the id of the OPENGL ES texture which the native image attached to.
 * @return Returns an error code defined in <b>SurfaceError</b>.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeImage_AttachContext(OH_NativeImage* image, uint32_t textureId);

/**
 * @brief Detach the OH_NativeImage from the OPENGL ES context.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @return Returns an error code defined in <b>SurfaceError</b>.
 * @since 9
 * @version 1.0
 */

int32_t OH_NativeImage_DetachContext(OH_NativeImage* image);

/**
 * @brief Update the related OPENGL ES texture with the OH_NativeImage acquired buffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @return Returns an error code defined in <b>SurfaceError</b>.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeImage_UpdateSurfaceImage(OH_NativeImage* image);

/**
 * @brief Get the timestamp of the texture image set by the most recent call to OH_NativeImage_UpdateSurfaceImage.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @return Returns the timestamp associated to the texture image.
 * @since 9
 * @version 1.0
 */
int64_t OH_NativeImage_GetTimestamp(OH_NativeImage* image);

/**
 * @brief Return the transform matrix of the texture image set by the most recent call to \n
 * OH_NativeImage_UpdateSurfaceImage.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param matrix Indicates the retrieved 4*4 transform matrix .
 * @return Returns an error code defined in <b>SurfaceError</b>.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeImage_GetTransformMatrix(OH_NativeImage* image, float matrix[16]);

/**
 * @brief Destroy the <b>OH_NativeImage</b> created by OH_NativeImage_Create, and the pointer to \n
 * <b>OH_NativeImage</b> will be null after this operation.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> pointer.
 * @since 9
 * @version 1.0
 */
void OH_NativeImage_Destroy(OH_NativeImage** image);

#ifdef __cplusplus
}
#endif

/** @} */
#endif