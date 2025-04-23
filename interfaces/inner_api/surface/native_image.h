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

/**
 * @addtogroup OH_NativeImage
 * @{
 *
 * @brief Provides the native image capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @since 9
 * @version 1.0
 */

/**
 * @file native_image.h
 *
 * @brief Defines the functions for obtaining and using a native image.
 *
 * @library libnative_image.so
 * @since 9
 * @version 1.0
 */
 
#ifndef NDK_INCLUDE_NATIVE_IMAGE_H_
#define NDK_INCLUDE_NATIVE_IMAGE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct OH_NativeImage;
typedef struct OH_NativeImage OH_NativeImage;
typedef struct NativeWindow OHNativeWindow;
/**
 * @brief define the new type name OHNativeWindowBuffer for struct NativeWindowBuffer.
 * @since 12
 */
typedef struct NativeWindowBuffer OHNativeWindowBuffer;

/**
 * @brief The callback function of frame available.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param context User defined context, returned to the user in the callback function
 * @since 11
 * @version 1.0
 */
typedef void (*OH_OnFrameAvailable)(void *context);

/**
 * @brief A listener for native image, use <b>OH_NativeImage_SetOnFrameAvailableListener</b> to register \n
 * the listener object to <b>OH_NativeImage</b>, the callback will be triggered when there is available frame
 *
 * @since 11
 * @version 1.0
 */
typedef struct OH_OnFrameAvailableListener {
    void *context;
    OH_OnFrameAvailable onFrameAvailable;
} OH_OnFrameAvailableListener;


/**
 * @brief Create a <b>OH_NativeImage</b> related to an Opengl ES texture and target. \n
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param textureId Indicates the id of the Opengl ES texture which the native image attached to.
 * @param textureTarget Indicates the Opengl ES target.
 * @return Returns the pointer to the <b>OH_NativeImage</b> instance created if the operation is successful, \n
 * returns <b>NULL</b> otherwise.
 * @since 9
 * @version 1.0
 */
OH_NativeImage* OH_NativeImage_Create(uint32_t textureId, uint32_t textureTarget);

/**
 * @brief Acquire the OHNativeWindow for the OH_NativeImage.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @return Returns the pointer to the OHNativeWindow if the operation is successful, returns <b>NULL</b> otherwise.
 * @since 9
 * @version 1.0
 */
OHNativeWindow* OH_NativeImage_AcquireNativeWindow(OH_NativeImage* image);

/**
 * @brief Attach the OH_NativeImage to Opengl ES context, and the Opengl ES texture is bound to the \n
 * GL_TEXTURE_EXTERNAL_OES, which will update by the OH_NativeImage.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param textureId Indicates the id of the Opengl ES texture which the native image attached to.
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeImage_AttachContext(OH_NativeImage* image, uint32_t textureId);

/**
 * @brief Detach the OH_NativeImage from the Opengl ES context.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 9
 * @version 1.0
 */

int32_t OH_NativeImage_DetachContext(OH_NativeImage* image);

/**
 * @brief Update the related Opengl ES texture with the OH_NativeImage acquired buffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeImage_UpdateSurfaceImage(OH_NativeImage* image);

/**
 * @brief Get the timestamp of the texture image set by the most recent call to OH_NativeImage_UpdateSurfaceImage.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @return Returns the timestamp associated to the texture image.
 * @since 9
 * @version 1.0
 */
int64_t OH_NativeImage_GetTimestamp(OH_NativeImage* image);

/**
 * @brief Obtains the transform matrix of the texture image set by the most recent call to \n
 * OH_NativeImage_UpdateSurfaceImage.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param matrix Indicates the retrieved 4*4 transform matrix .
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 9
 * @version 1.0
 * @deprecated since 12
 * @useinstead OH_NativeImage_GetTransformMatrixV2
 */
int32_t OH_NativeImage_GetTransformMatrix(OH_NativeImage* image, float matrix[16]);

/**
 * @brief Obtains the native image's surface id.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param surfaceId Indicates the surface id.
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 11
 * @version 1.0
 */
int32_t OH_NativeImage_GetSurfaceId(OH_NativeImage* image, uint64_t* surfaceId);

/**
 * @brief Set the frame available callback.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param listener Indicates the callback function.
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 11
 * @version 1.0
 */
int32_t OH_NativeImage_SetOnFrameAvailableListener(OH_NativeImage* image, OH_OnFrameAvailableListener listener);

/**
 * @brief Unset the frame available callback.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 11
 * @version 1.0
 */
int32_t OH_NativeImage_UnsetOnFrameAvailableListener(OH_NativeImage* image);

/**
 * @brief Destroy the <b>OH_NativeImage</b> created by OH_NativeImage_Create, and the pointer to \n
 * <b>OH_NativeImage</b> will be null after this operation.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> pointer.
 * @since 9
 * @version 1.0
 */
void OH_NativeImage_Destroy(OH_NativeImage** image);

/**
 * @brief Obtains the transform matrix of the texture image by producer transform type.\n
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param matrix Indicates the retrieved 4*4 transform matrix .
 * @return 0 - Success.
 *     40001000 - image is NULL.
 * @since 12
 * @version 1.0
 */
int32_t OH_NativeImage_GetTransformMatrixV2(OH_NativeImage* image, float matrix[16]);

/**
 * @brief Obtains the transform matrix that combines with crop rect.
 *
 * This API returns a transform matrix that combines the crop rect.
 * Note that the matrix will not be updated until <b>OH_NativeImage_UpdateSurfaceImage</b> is called.\n
 * This interface is a non-thread-safe type interface.\n
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param matrix Indicates the retrieved 4*4 transform matrix .
 * @return {@link NATIVE_ERROR_OK} 0 - Success.
 *     {@link NATIVE_ERROR_INVALID_ARGUMENTS} 40001000 - image is NULL.
 *     {@link NATIVE_ERROR_MEM_OPERATION_ERROR} 30001000 - Memory operation error, failed to get transform matrix.
 * @since 14
 * @version 1.0
 */
int32_t OH_NativeImage_GetBufferMatrix(OH_NativeImage* image, float matrix[16]);
/**
 * @brief Acquire an <b>OHNativeWindowBuffer</b> through an <b>OH_NativeImage</b> instance for content consumer.\n
 * This method can not be used at the same time with <b>OH_NativeImage_UpdateSurfaceImage</b>.\n
 * This method will create an <b>OHNativeWindowBuffer</b>.\n
 * If there is a situation when <b>OHNativeWindowBuffer</b> is still used after calling
 * <b>OH_NativeImage_ReleaseNativeWindowBuffer</b>, you must pay attention to the following two points.\n
 * 1) When using <b>OHNativeWindowBuffer</b>, need to increase its reference count
 * by <b>OH_NativeWindow_NativeObjectReference</b>.\n
 * 2) When the <b>OHNativeWindowBuffer</b> is used up, its reference count needs to be decremented
 * by <b>OH_NativeWindow_NativeObjectUnreference</b>.\n
 * This interface needs to be used in conjunction with <b>OH_NativeImage_ReleaseNativeWindowBuffer</b>,
 * otherwise memory leaks will occur.\n
 * When the fenceFd is used up, you need to close it.\n
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param nativeWindowBuffer Indicates the pointer to an <b>OHNativeWindowBuffer</b> point.
 * @param fenceFd Indicates the pointer to a file descriptor handle.
 * @return {@link NATIVE_ERROR_OK} 0 - Success.
 *     {@link NATIVE_ERROR_INVALID_ARGUMENTS} 40001000 - image, nativeWindowBuffer, fenceFd is NULL.
 *     {@link NATIVE_ERROR_NO_BUFFER} 40601000 - No buffer for consume.
 * @since 12
 * @version 1.0
 */
int32_t OH_NativeImage_AcquireNativeWindowBuffer(OH_NativeImage* image,
    OHNativeWindowBuffer** nativeWindowBuffer, int* fenceFd);

/**
 * @brief Release the <b>OHNativeWindowBuffer</b> to the buffer queue through an
 * <b>OH_NativeImage</b> instance for reuse.\n
 * The fenceFd will be closed by system.\n
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param nativeWindowBuffer Indicates the pointer to an <b>OHNativeWindowBuffer</b> instance.
 * @param fenceFd Indicates a file descriptor handle, which is used for timing synchronization.
 * @return {@link NATIVE_ERROR_OK} 0 - Success.
 *     {@link NATIVE_ERROR_INVALID_ARGUMENTS} 40001000 - image, nativeWindowBuffer is NULL.
 *     {@link NATIVE_ERROR_BUFFER_STATE_INVALID} 41207000 - nativeWindowBuffer state invalid.
 *     {@link NATIVE_ERROR_BUFFER_NOT_IN_CACHE} 41210000 - nativeWindowBuffer not in cache.
 * @since 12
 * @version 1.0
 */
int32_t OH_NativeImage_ReleaseNativeWindowBuffer(OH_NativeImage* image,
    OHNativeWindowBuffer* nativeWindowBuffer, int fenceFd);

/**
 * @brief Create a <b>OH_NativeImage</b> as surface consumer. \n
 * This method can not be used at the same time with <b>OH_NativeImage_UpdateSurfaceImage</b>.\n
 * This interface needs to be used in conjunction with <b>OH_NativeImage_Destroy</b>,
 * otherwise memory leaks will occur.\n
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @return Returns the pointer to the <b>OH_NativeImage</b> instance created if the operation is successful, \n
 * returns <b>NULL</b> otherwise.
 * @since 12
 * @version 1.0
 */
OH_NativeImage* OH_ConsumerSurface_Create();

/**
 * @brief Set the default usage of the <b>OH_NativeImage</b>.\n
 * This interface dose not support concurrency.\n
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param usage Indicates the usage of the <b>OH_NativeImage</b>.Refer to the enum <b>OH_NativeBuffer_Usage</b>.
 * @return {@link NATIVE_ERROR_OK} 0 - Success.
 *     {@link NATIVE_ERROR_INVALID_ARGUMENTS} 40001000 - image is NULL.
 * @since 13
 * @version 1.0
 */
int32_t OH_ConsumerSurface_SetDefaultUsage(OH_NativeImage* image, uint64_t usage);

/**
 * @brief Set the default size of the <b>OH_NativeImage</b>.\n
 * This interface dose not support concurrency.\n
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param width Indicates the width of the <b>OH_NativeImage</b>, and it should be greater than 0.
 * @param height Indicates the height of the <b>OH_NativeImage</b>, and it should be greater than 0.
 * @return {@link NATIVE_ERROR_OK} 0 - Success.
 *     {@link NATIVE_ERROR_INVALID_ARGUMENTS} 40001000 - image is NULL or width, height less than or equal to 0.
 * @since 13
 * @version 1.0
 */
int32_t OH_ConsumerSurface_SetDefaultSize(OH_NativeImage* image, int32_t width, int32_t height);

/**
 * @brief Set the rendering in drop buffer mode of the <b>OH_NativeImage</b>.\n
 * In this mode, most of the buffers produced by the producer will be discarded,
 * and the latest buffer will be selected for rending.\n
 * This mode can not simultaneously guarantee high frame rate requirements.\n
 * This interface suggest be called after the <b>OH_NativeImage_Create</b> call immediately.\n
 * This interface is a non-thread-safe type interface.\n
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeImage
 * @param image Indicates the pointer to a <b>OH_NativeImage</b> instance.
 * @param isOpen Indicates the switch of drop buffer mode.
 * @return {@link NATIVE_ERROR_OK} 0 - Success.
 *     {@link NATIVE_ERROR_INVALID_ARGUMENTS} 40001000 - image is NULL.
 * @since 17
 * @version 1.0
 */
int32_t OH_NativeImage_SetDropBufferMode(OH_NativeImage* image, bool isOpen);
#ifdef __cplusplus
}
#endif

/** @} */
#endif