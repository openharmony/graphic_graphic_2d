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

#ifndef NDK_INCLUDE_EXTERNAL_HARDWARE_BUFFER_H_
#define NDK_INCLUDE_EXTERNAL_HARDWARE_BUFFER_H_

/**
 * @addtogroup HardwareBuffer
 * @{
 *
 * @brief Provides the hardware buffer capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.HardwareBuffer
 * @since 8
 * @version 2.0
 */

/**
 * @file external_buffer.h
 *
 * @brief Defines the functions for obtaining and using a hardware buffer.
 *
 * @since 8
 * @version 2.0
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct HardwareBuffer;
typedef struct HardwareBuffer OHHardwareBuffer;

/**
 * @brief <b>HardwareBuffer</b> config. \n
 * Used to allocating new <b>HardwareBuffer</b> andquery parameters if existing ones.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.HardwareBuffer
 * @since 8
 * @version 2.0
 */
typedef struct {
    int32_t width;           ///< width in pixels
    int32_t height;          ///< width in pixels
    int32_t strideAlignment; ///< width in pixels
    int32_t format;          ///< width in pixels
    int32_t usage;           ///< width in pixels
} OHHardwareBufferConfig;

/**
 * @brief Alloc a <b>HardwareBuffer</b> that matches the passed BufferRequestConfig. \n
 * A new <b>HardwareBuffer</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.HardwareBuffer
 * @param config Indicates the pointer to a <b>BufferRequestConfig</b> instance.
 * @return Returns the pointer to the <b>HardwareBuffer</b> instance created if the operation is successful, \n
 * returns <b>NULL</b> otherwise.
 * @since 8
 * @version 2.0
 */
OHHardwareBuffer* OH_HardwareBuffer_HardwareBufferAlloc(const OHHardwareBufferConfig* config);

/**
 * @brief Adds the reference count of a HardwareBuffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.HardwareBuffer
 * @param buffer Indicates the pointer to a <b>HardwareBuffer</b> instance.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 2.0
 */
int32_t OH_HardwareBuffer_HardwareBufferReference(OHHardwareBuffer *buffer);

/**
 * @brief Decreases the reference count of a HardwareBuffer and, when the reference count reaches 0, \n
 * destroys this HardwareBuffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.HardwareBuffer
 * @param buffer Indicates the pointer to a <b>HardwareBuffer</b> instance.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 2.0
 */
int32_t OH_HardwareBuffer_HardwareBufferUnreference(OHHardwareBuffer *buffer);

/**
 * @brief Return a config of the OHHardwareBuffer in the passed OHHardwareBufferConfig struct.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.HardwareBuffer
 * @param buffer Indicates the pointer to a <b>HardwareBuffer</b> instance.
 * @param config Indicates the pointer to the <b>HardwareBufferConfig</b> of the buffer.
 * @return Returns the pointer to the <b>BufferRequestConfig</b> instance if the operation is successful, \n
 * returns <b>NULL</b> otherwise.
 * @since 8
 * @version 2.0
 */
void OH_HardwareBuffer_GetHardwareBufferConfig(OHHardwareBuffer *buffer, OHHardwareBufferConfig* config);

/**
 * @brief Provide direct cpu access to the OHHardwareBuffer in the process's address space.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.HardwareBuffer
 * @param buffer Indicates the pointer to a <b>HardwareBuffer</b> instance.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 2.0
 */

int32_t OH_HardwareBuffer_HardwareBufferMap(OHHardwareBuffer *buffer);

/**
 * @brief Remove direct cpu access ability of the HardwareBuffer in the process's address space.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.HardwareBuffer
 * @param buffer Indicates the pointer to a <b>HardwareBuffer</b> instance.
 * @return Returns an error code defined in <b>GSError</b>.
 * @since 8
 * @version 2.0
 */
int32_t OH_HardwareBuffer_HardwareBufferUnmap(OHHardwareBuffer *buffer);

/**
 * @brief Get the systen wide unique sequence number of the HardwareBuffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.HardwareBuffer
 * @param buffer Indicates the pointer to a <b>HardwareBuffer</b> instance.
 * @return Returns the sequence number, which is unique for each HardwareBuffer.
 * @since 8
 * @version 2.0
 */
uint32_t OH_HardwareBuffer_HardwareBufferGetSeqNum(OHHardwareBuffer *buffer);

#ifdef __cplusplus
}
#endif

/** @} */
#endif