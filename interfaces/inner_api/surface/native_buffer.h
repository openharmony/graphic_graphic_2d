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

#ifndef NDK_INCLUDE_NATIVE_BUFFER_H_
#define NDK_INCLUDE_NATIVE_BUFFER_H_

/**
 * @addtogroup OH_NativeBuffer
 * @{
 *
 * @brief Provides the native buffer capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeBuffer
 * @since 9
 * @version 1.0
 */

/**
 * @file native_buffer.h
 *
 * @brief Defines the functions for obtaining and using a native buffer.
 *
 * @since 9
 * @version 1.0
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct OH_NativeBuffer;
typedef struct OH_NativeBuffer OH_NativeBuffer;

/**
 * @brief <b>OH_NativeBuffer</b> config. \n
 * Used to allocating new <b>OH_NativeBuffer</b> andquery parameters if existing ones.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeBuffer
 * @since 9
 * @version 1.0
 */
typedef struct {
    int32_t width;           ///< Width in pixels
    int32_t height;          ///< Height in pixels
    int32_t format;          ///< One of PixelFormat
    int32_t usage;           ///< Combination of buffer usage
} OH_NativeBuffer_Config;

/**
 * @brief Alloc a <b>OH_NativeBuffer</b> that matches the passed BufferRequestConfig. \n
 * A new <b>OH_NativeBuffer</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeBuffer
 * @param config Indicates the pointer to a <b>BufferRequestConfig</b> instance.
 * @return Returns the pointer to the <b>OH_NativeBuffer</b> instance created if the operation is successful, \n
 * returns <b>NULL</b> otherwise.
 * @since 9
 * @version 1.0
 */
OH_NativeBuffer* OH_NativeBuffer_Alloc(const OH_NativeBuffer_Config* config);

/**
 * @brief Adds the reference count of a OH_NativeBuffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeBuffer_Reference(OH_NativeBuffer *buffer);

/**
 * @brief Decreases the reference count of a OH_NativeBuffer and, when the reference count reaches 0, \n
 * destroys this OH_NativeBuffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeBuffer_Unreference(OH_NativeBuffer *buffer);

/**
 * @brief Return a config of the OH_NativeBuffer in the passed OHNativeBufferConfig struct.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @param config Indicates the pointer to the <b>NativeBufferConfig</b> of the buffer.
 * @return Returns the pointer to the <b>BufferRequestConfig</b> instance if the operation is successful, \n
 * returns <b>NULL</b> otherwise.
 * @since 9
 * @version 1.0
 */
void OH_NativeBuffer_GetConfig(OH_NativeBuffer *buffer, OH_NativeBuffer_Config* config);

/**
 * @brief Provide direct cpu access to the OH_NativeBuffer in the process's address space.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @param virAddr Indicates the address of the <b>OH_NativeBuffer</b> in virtual memory.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 9
 * @version 1.0
 */

int32_t OH_NativeBuffer_Map(OH_NativeBuffer *buffer, void **virAddr);

/**
 * @brief Remove direct cpu access ability of the OH_NativeBuffer in the process's address space.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeBuffer_Unmap(OH_NativeBuffer *buffer);

/**
 * @brief Get the systen wide unique sequence number of the OH_NativeBuffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.OH_NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @return Returns the sequence number, which is unique for each OH_NativeBuffer.
 * @since 9
 * @version 1.0
 */
uint32_t OH_NativeBuffer_GetSeqNum(OH_NativeBuffer *buffer);

#ifdef __cplusplus
}
#endif

/** @} */
#endif