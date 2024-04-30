/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNERKITS_INCLUDE_IMAGE_TYPE_H_
#define INTERFACES_INNERKITS_INCLUDE_IMAGE_TYPE_H_

#include <cinttypes>

namespace OHOS {
namespace Media {
#ifdef _WIN32
#define NATIVEEXPORT __declspec(dllexport)
#else
#define NATIVEEXPORT
#endif

enum class AllocatorType : int32_t {
    // keep same with java AllocatorType
    DEFAULT = 0,
    HEAP_ALLOC = 1,
    SHARE_MEM_ALLOC = 2,
    CUSTOM_ALLOC = 3,  // external
};

enum class ColorSpace : int32_t {
    // unknown color space.
    UNKNOWN = 0,

    // based on SMPTE RP 431-2-2007 & IEC 61966-2.1:1999.
    DISPLAY_P3 = 1,

    // standard Red Green Blue based on IEC 61966-2.1:1999.
    SRGB = 2,

    // SRGB with a linear transfer function based on IEC 61966-2.1:1999.
    LINEAR_SRGB = 3,

    // based on IEC 61966-2-2:2003.
    EXTENDED_SRGB = 4,

    // based on IEC 61966-2-2:2003.
    LINEAR_EXTENDED_SRGB = 5,

    // based on standard illuminant D50 as the white point.
    GENERIC_XYZ = 6,

    // based on CIE XYZ D50 as the profile conversion space.
    GENERIC_LAB = 7,

    // based on SMPTE ST 2065-1:2012.
    ACES = 8,

    // based on Academy S-2014-004.
    ACES_CG = 9,

    // based on Adobe RGB (1998).
    ADOBE_RGB_1998 = 10,

    // based on SMPTE RP 431-2-2007.
    DCI_P3 = 11,

    // based on Rec. ITU-R BT.709-5.
    ITU_709 = 12,

    // based on Rec. ITU-R BT.2020-1.
    ITU_2020 = 13,

    // based on ROMM RGB ISO 22028-2:2013.
    ROMM_RGB = 14,

    // based on 1953 standard.
    NTSC_1953 = 15,

    // based on SMPTE C.
    SMPTE_C = 16,
};

enum class EncodedFormat : int32_t {
    UNKNOWN = 0,
    JPEG = 1,
    PNG = 2,
    GIF = 3,
    HEIF = 4,
};

enum class PixelFormat : int32_t {
    UNKNOWN = 0,
    ARGB_8888 = 1,  // Each pixel is stored on 4 bytes.
    RGB_565 = 2,    // Each pixel is stored on 2 bytes
    RGBA_8888 = 3,
    BGRA_8888 = 4,
    RGB_888 = 5,
    ALPHA_8 = 6,
    RGBA_F16 = 7,
    NV21 = 8,  // Each pixel is sorted on 3/2 bytes.
    NV12 = 9,
    CMYK = 10,
    RGBA_1010102 = 14,
};

enum class AlphaType : int32_t {
    IMAGE_ALPHA_TYPE_UNKNOWN = 0,
    IMAGE_ALPHA_TYPE_OPAQUE = 1,   // image pixels are stored as opaque.
    IMAGE_ALPHA_TYPE_PREMUL = 2,   // image have alpha component, and all pixels have premultiplied by alpha value.
    IMAGE_ALPHA_TYPE_UNPREMUL = 3, // image have alpha component, and all pixels stored without premultiply alpha value.
};

enum class MemoryUsagePreference : int32_t {
    DEFAULT = 0,
    LOW_RAM = 1,  // low memory
};

enum class FinalOutputStep : int32_t {
    NO_CHANGE = 0,
    CONVERT_CHANGE = 1,
    ROTATE_CHANGE = 2,
    SIZE_CHANGE = 3,
    DENSITY_CHANGE = 4
};

struct Position {
    int32_t x = 0;
    int32_t y = 0;
};

struct Rect {
    int32_t left = 0;
    int32_t top = 0;
    int32_t width = 0;
    int32_t height = 0;
};

struct Size {
    int32_t width = 0;
    int32_t height = 0;
};

struct ImageInfo {
    Size size;
    PixelFormat pixelFormat = PixelFormat::UNKNOWN;
    ColorSpace colorSpace = ColorSpace::SRGB;
    AlphaType alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
    int32_t baseDensity = 0;
    int32_t stride = 1;
};

struct DecodeOptions {
    int32_t fitDensity = 0;
    Rect CropRect;
    Size desiredSize;
    Rect desiredRegion;
    float rotateDegrees = 0;
    uint32_t rotateNewDegrees = 0;
    static constexpr uint32_t DEFAULT_SAMPLE_SIZE = 1;
    uint32_t sampleSize = DEFAULT_SAMPLE_SIZE;
    PixelFormat desiredPixelFormat = PixelFormat::UNKNOWN;
    AllocatorType allocatorType = AllocatorType::HEAP_ALLOC;
    ColorSpace desiredColorSpace = ColorSpace::SRGB;
    bool allowPartialImage = true;
    bool editable = false;
    MemoryUsagePreference preference = MemoryUsagePreference::DEFAULT;
};

enum class ScaleMode : int32_t {
    FIT_TARGET_SIZE = 0,
    CENTER_CROP = 1,
};

enum class IncrementalMode { FULL_DATA = 0, INCREMENTAL_DATA = 1 };
} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_IMAGE_TYPE_H_
