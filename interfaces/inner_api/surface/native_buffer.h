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
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @since 9
 * @version 1.0
 */

/**
 * @file native_buffer.h
 *
 * @brief Defines the functions for obtaining and using a native buffer.
 *
 * @library libnative_buffer.so
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
 * @brief Indicates the usage of a native buffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @since 10
 * @version 1.0
 */
enum OH_NativeBuffer_Usage {
    NATIVEBUFFER_USAGE_CPU_READ = (1ULL << 0),        /// < CPU read buffer */
    NATIVEBUFFER_USAGE_CPU_WRITE = (1ULL << 1),       /// < CPU write memory */
    NATIVEBUFFER_USAGE_MEM_DMA = (1ULL << 3),         /// < Direct memory access (DMA) buffer */
};

/**
 * @brief Indicates the format of a native buffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @since 10
 * @version 1.0
 */
enum OH_NativeBuffer_Format {
    NATIVEBUFFER_PIXEL_FMT_RGB_565 = 3,               /// < RGB565 format */
    NATIVEBUFFER_PIXEL_FMT_RGBA_5658,                 /// < RGBA5658 format */
    NATIVEBUFFER_PIXEL_FMT_RGBX_4444,                 /// < RGBX4444 format */
    NATIVEBUFFER_PIXEL_FMT_RGBA_4444,                 /// < RGBA4444 format */
    NATIVEBUFFER_PIXEL_FMT_RGB_444,                   /// < RGB444 format */
    NATIVEBUFFER_PIXEL_FMT_RGBX_5551,                 /// < RGBX5551 format */
    NATIVEBUFFER_PIXEL_FMT_RGBA_5551,                 /// < RGBA5551 format */
    NATIVEBUFFER_PIXEL_FMT_RGB_555,                   /// < RGB555 format */
    NATIVEBUFFER_PIXEL_FMT_RGBX_8888,                 /// < RGBX8888 format */
    NATIVEBUFFER_PIXEL_FMT_RGBA_8888,                 /// < RGBA8888 format */
    NATIVEBUFFER_PIXEL_FMT_RGB_888,                   /// < RGB888 format */
    NATIVEBUFFER_PIXEL_FMT_BGR_565,                   /// < BGR565 format */
    NATIVEBUFFER_PIXEL_FMT_BGRX_4444,                 /// < BGRX4444 format */
    NATIVEBUFFER_PIXEL_FMT_BGRA_4444,                 /// < BGRA4444 format */
    NATIVEBUFFER_PIXEL_FMT_BGRX_5551,                 /// < BGRX5551 format */
    NATIVEBUFFER_PIXEL_FMT_BGRA_5551,                 /// < BGRA5551 format */
    NATIVEBUFFER_PIXEL_FMT_BGRX_8888,                 /// < BGRX8888 format */
    NATIVEBUFFER_PIXEL_FMT_BGRA_8888,                 /// < BGRA8888 format */
    NATIVEBUFFER_PIXEL_FMT_BUTT = 0X7FFFFFFF          /// < Invalid pixel format */
};

/**
 * @brief Indicates the color space of a native buffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @since 11
 * @version 1.0
 */
enum OH_NativeBuffer_ColorSpace {
    /** None color space */
    OH_COLORSPACE_NONE,
    /** COLORPRIMARIES_BT601_P | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_P << 16) | (RANGE_FULL << 21) */
    OH_COLORSPACE_BT601_EBU_FULL,
    /** COLORPRIMARIES_BT601_N | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_N << 16) | (RANGE_FULL << 21)*/
    OH_COLORSPACE_BT601_SMPTE_C_FULL,
    /** COLORPRIMARIES_BT709 | (TRANSFUNC_BT709 << 8) | (MATRIX_BT709 << 16) | (RANGE_FULL << 21) */
    OH_COLORSPACE_BT709_FULL,
    /** COLORPRIMARIES_BT2020 | (TRANSFUNC_HLG << 8) | (MATRIX_BT2020 << 16) | (RANGE_FULL << 21) */
    OH_COLORSPACE_BT2020_HLG_FULL,
    /** COLORPRIMARIES_BT2020 | (TRANSFUNC_PQ << 8) | (MATRIX_BT2020 << 16) | (RANGE_FULL << 21) */
    OH_COLORSPACE_BT2020_PQ_FULL,
    /** COLORPRIMARIES_BT601_P | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_P << 16) | (RANGE_LIMITED << 21) */
    OH_COLORSPACE_BT601_EBU_LIMIT,
    /** COLORPRIMARIES_BT601_N | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_N << 16) | (RANGE_LIMITED << 21) */
    OH_COLORSPACE_BT601_SMPTE_C_LIMIT,
    /** COLORPRIMARIES_BT709 | (TRANSFUNC_BT709 << 8) | (MATRIX_BT709 << 16) | (RANGE_LIMITED << 21) */
    OH_COLORSPACE_BT709_LIMIT,
    /** COLORPRIMARIES_BT2020 | (TRANSFUNC_HLG << 8) | (MATRIX_BT2020 << 16) | (RANGE_LIMITED << 21) */
    OH_COLORSPACE_BT2020_HLG_LIMIT,
    /** COLORPRIMARIES_BT2020 | (TRANSFUNC_PQ << 8) | (MATRIX_BT2020 << 16) | (RANGE_LIMITED << 21) */
    OH_COLORSPACE_BT2020_PQ_LIMIT,
    /** COLORPRIMARIES_SRGB | (TRANSFUNC_SRGB << 8) | (MATRIX_BT601_N << 16) | (RANGE_FULL << 21) */
    OH_COLORSPACE_SRGB_FULL,
    /** COLORPRIMARIES_P3_D65 | (TRANSFUNC_SRGB << 8) | (MATRIX_P3 << 16) | (RANGE_FULL << 21) */
    OH_COLORSPACE_P3_FULL,
    /** COLORPRIMARIES_P3_D65 | (TRANSFUNC_HLG << 8) | (MATRIX_P3 << 16) | (RANGE_FULL << 21) */
    OH_COLORSPACE_P3_HLG_FULL,
    /** COLORPRIMARIES_P3_D65 | (TRANSFUNC_PQ << 8) | (MATRIX_P3 << 16) | (RANGE_FULL << 21) */
    OH_COLORSPACE_P3_PQ_FULL,
    /** COLORPRIMARIES_ADOBERGB | (TRANSFUNC_ADOBERGB << 8) | (MATRIX_ADOBERGB << 16) | (RANGE_FULL << 21) */
    OH_COLORSPACE_ADOBERGB_FULL,
    /** COLORPRIMARIES_SRGB | (TRANSFUNC_SRGB << 8) | (MATRIX_BT601_N << 16) | (RANGE_LIMITED << 21) */
    OH_COLORSPACE_SRGB_LIMIT,
    /** COLORPRIMARIES_P3_D65 | (TRANSFUNC_SRGB << 8) | (MATRIX_P3 << 16) | (RANGE_LIMITED << 21) */
    OH_COLORSPACE_P3_LIMIT,
    /** COLORPRIMARIES_P3_D65 | (TRANSFUNC_HLG << 8) | (MATRIX_P3 << 16) | (RANGE_LIMITED << 21) */
    OH_COLORSPACE_P3_HLG_LIMIT,
    /** COLORPRIMARIES_P3_D65 | (TRANSFUNC_PQ << 8) | (MATRIX_P3 << 16) | (RANGE_LIMITED << 21) */
    OH_COLORSPACE_P3_PQ_LIMIT,
    /** COLORPRIMARIES_ADOBERGB | (TRANSFUNC_ADOBERGB << 8) | (MATRIX_ADOBERGB << 16) | (RANGE_LIMITED << 21) */
    OH_COLORSPACE_ADOBERGB_LIMIT,
    /** COLORPRIMARIES_SRGB | (TRANSFUNC_LINEAR << 8) */
    OH_COLORSPACE_LINEAR_SRGB,
    /** equal to OH_COLORSPACE_LINEAR_SRGB */
    OH_COLORSPACE_LINEAR_BT709,
    /** COLORPRIMARIES_P3_D65 | (TRANSFUNC_LINEAR << 8) */
    OH_COLORSPACE_LINEAR_P3,
    /** COLORPRIMARIES_BT2020 | (TRANSFUNC_LINEAR << 8) */
    OH_COLORSPACE_LINEAR_BT2020,
    /** equal to OH_COLORSPACE_SRGB_FULL */
    OH_COLORSPACE_DISPLAY_SRGB,
    /** equal to OH_COLORSPACE_P3_FULL */
    OH_COLORSPACE_DISPLAY_P3_SRGB,
    /** equal to OH_COLORSPACE_P3_HLG_FULL */
    OH_COLORSPACE_DISPLAY_P3_HLG,
    /** equal to OH_COLORSPACE_P3_PQ_FULL */
    OH_COLORSPACE_DISPLAY_P3_PQ,
    /** COLORPRIMARIES_BT2020 | (TRANSFUNC_SRGB << 8) | (MATRIX_BT2020 << 16) | (RANGE_FULL << 21) */
    OH_COLORSPACE_DISPLAY_BT2020_SRGB,
    /** equal to OH_COLORSPACE_BT2020_HLG_FULL */
    OH_COLORSPACE_DISPLAY_BT2020_HLG,
    /** equal to OH_COLORSPACE_BT2020_PQ_FULL */
    OH_COLORSPACE_DISPLAY_BT2020_PQ,
};

/**
 * @brief <b>OH_NativeBuffer</b> config. \n
 * Used to allocating new <b>OH_NativeBuffer</b> andquery parameters if existing ones.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @since 9
 * @version 1.0
 */
typedef struct {
    int32_t width;           ///< Width in pixels
    int32_t height;          ///< Height in pixels
    int32_t format;          ///< One of PixelFormat
    int32_t usage;           ///< Combination of buffer usage
    int32_t stride;          ///< the stride of memory
} OH_NativeBuffer_Config;

/**
 * @brief Alloc a <b>OH_NativeBuffer</b> that matches the passed BufferRequestConfig. \n
 * A new <b>OH_NativeBuffer</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
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
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeBuffer_Reference(OH_NativeBuffer *buffer);

/**
 * @brief Decreases the reference count of a OH_NativeBuffer and, when the reference count reaches 0, \n
 * destroys this OH_NativeBuffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeBuffer_Unreference(OH_NativeBuffer *buffer);

/**
 * @brief Return a config of the OH_NativeBuffer in the passed OHNativeBufferConfig struct.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @param config Indicates the pointer to the <b>NativeBufferConfig</b> of the buffer.
 * @return <b>void</b>
 * @since 9
 * @version 1.0
 */
void OH_NativeBuffer_GetConfig(OH_NativeBuffer *buffer, OH_NativeBuffer_Config* config);

/**
 * @brief Provide direct cpu access to the OH_NativeBuffer in the process's address space.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @param virAddr Indicates the address of the <b>OH_NativeBuffer</b> in virtual memory.
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 9
 * @version 1.0
 */

int32_t OH_NativeBuffer_Map(OH_NativeBuffer *buffer, void **virAddr);

/**
 * @brief Remove direct cpu access ability of the OH_NativeBuffer in the process's address space.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @return Returns an error code, 0 is success, otherwise, failed.
 * @since 9
 * @version 1.0
 */
int32_t OH_NativeBuffer_Unmap(OH_NativeBuffer *buffer);

/**
 * @brief Get the systen wide unique sequence number of the OH_NativeBuffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @return Returns the sequence number, which is unique for each OH_NativeBuffer.
 * @since 9
 * @version 1.0
 */
uint32_t OH_NativeBuffer_GetSeqNum(OH_NativeBuffer *buffer);

/**
 * @brief Set the color space of the OH_NativeBuffer.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeBuffer
 * @param buffer Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
 * @param colorSpace Indicates the color space of native buffer, see <b>OH_NativeBuffer_ColorSpace</b>.
 * @return Returns the sequence number, which is unique for each OH_NativeBuffer.
 * @since 11
 * @version 1.0
 */
int32_t OH_NativeBuffer_SetColorSpace(OH_NativeBuffer *buffer, OH_NativeBuffer_ColorSpace colorSpace);
#ifdef __cplusplus
}
#endif

/** @} */
#endif