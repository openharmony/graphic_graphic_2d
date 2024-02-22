/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNERKITS_SURFACE_SURFACE_TYPE_H
#define INTERFACES_INNERKITS_SURFACE_SURFACE_TYPE_H

#include <cstdint>
#include <string>
#include <vector>
#include <graphic_common.h>

namespace OHOS {
#define SURFACE_MAX_USER_DATA_COUNT 1000
#define SURFACE_MAX_QUEUE_SIZE 32
#define SURFACE_DEFAULT_QUEUE_SIZE 3
#define SURFACE_MAX_STRIDE_ALIGNMENT 32
#define SURFACE_MIN_STRIDE_ALIGNMENT 4
#define SURFACE_DEFAULT_STRIDE_ALIGNMENT 4
#define SURFACE_MAX_SIZE 58982400 // 8K * 8K

constexpr uint32_t SURFACE_PARCEL_SIZE_LIMIT = 1024;

/*
 * @brief Enumerates return values of the functions.
 */
using GraphicDispErrCode = enum {
    GRAPHIC_DISPLAY_SUCCESS = 0,           /**< Success */
    GRAPHIC_DISPLAY_FAILURE = -1,          /**< Failure */
    GRAPHIC_DISPLAY_FD_ERR = -2,           /**< File handle (FD) error */
    GRAPHIC_DISPLAY_PARAM_ERR = -3,        /**< Parameter error */
    GRAPHIC_DISPLAY_NULL_PTR = -4,         /**< Null pointer */
    GRAPHIC_DISPLAY_NOT_SUPPORT = -5,      /**< Unsupported feature */
    GRAPHIC_DISPLAY_NOMEM = -6,            /**< Insufficient memory */
    GRAPHIC_DISPLAY_SYS_BUSY = -7,         /**< System busy */
    GRAPHIC_DISPLAY_NOT_PERM = -8          /**< Forbidden operation */
};

using GraphicCompositionType = enum {
    GRAPHIC_COMPOSITION_CLIENT,       /**< Client composition type. The composer should be the CPU or GPU. */
    GRAPHIC_COMPOSITION_DEVICE,       /**< Device composition type. The composer should be the hardware. */
    GRAPHIC_COMPOSITION_CURSOR,       /**< Cursor composition type, used for cursor. */
    GRAPHIC_COMPOSITION_VIDEO,        /**< Video composition type, used for video. */
    GRAPHIC_COMPOSITION_DEVICE_CLEAR, /**< Device clear composition type, the device will clear the target region. */
    GRAPHIC_COMPOSITION_CLIENT_CLEAR, /**< Client clear composition type, the service will clear the target region. */
    GRAPHIC_COMPOSITION_TUNNEL,       /**< Tunnel composition type, used for tunnel. */
    GRAPHIC_COMPOSITION_BUTT,
    GRAPHIC_COMPOSITION_SOLID_COLOR   /**< used for SetLayerColor. */
};

using GraphicLayerAlpha = struct {
    bool enGlobalAlpha;   /**< Global alpha enable bit */
    bool enPixelAlpha;    /**< Pixel alpha enable bit */
    uint8_t alpha0;       /**< Alpha0 value, ranging from 0 to 255 */
    uint8_t alpha1;       /**< Alpha1 value, ranging from 0 to 255 */
    uint8_t gAlpha;       /**< Global alpha value, ranging from 0 to 255 */
};

using GraphicBlendType = enum {
    GRAPHIC_BLEND_NONE = 0,         /**< No blending */
    GRAPHIC_BLEND_CLEAR,            /**< CLEAR blending */
    GRAPHIC_BLEND_SRC,              /**< SRC blending */
    GRAPHIC_BLEND_SRCOVER,          /**< SRC_OVER blending */
    GRAPHIC_BLEND_DSTOVER,          /**< DST_OVER blending */
    GRAPHIC_BLEND_SRCIN,            /**< SRC_IN blending */
    GRAPHIC_BLEND_DSTIN,            /**< DST_IN blending */
    GRAPHIC_BLEND_SRCOUT,           /**< SRC_OUT blending */
    GRAPHIC_BLEND_DSTOUT,           /**< DST_OUT blending */
    GRAPHIC_BLEND_SRCATOP,          /**< SRC_ATOP blending */
    GRAPHIC_BLEND_DSTATOP,          /**< DST_ATOP blending */
    GRAPHIC_BLEND_ADD,              /**< ADD blending */
    GRAPHIC_BLEND_XOR,              /**< XOR blending */
    GRAPHIC_BLEND_DST,              /**< DST blending */
    GRAPHIC_BLEND_AKS,              /**< AKS blending */
    GRAPHIC_BLEND_AKD,              /**< AKD blending */
    GRAPHIC_BLEND_BUTT              /**< Null operation */
};

/*
 * @brief Enumeration values are indicated for external consistency.
 */
using GraphicPixelFormat = enum {
    GRAPHIC_PIXEL_FMT_CLUT8 = 0,                    /**< CLUT8 format */
    GRAPHIC_PIXEL_FMT_CLUT1 = 1,                    /**< CLUT1 format */
    GRAPHIC_PIXEL_FMT_CLUT4 = 2,                    /**< CLUT4 format */
    GRAPHIC_PIXEL_FMT_RGB_565 = 3,                  /**< RGB565 format */
    GRAPHIC_PIXEL_FMT_RGBA_5658 = 4,                /**< RGBA5658 format */
    GRAPHIC_PIXEL_FMT_RGBX_4444 = 5,                /**< RGBX4444 format */
    GRAPHIC_PIXEL_FMT_RGBA_4444 = 6,                /**< RGBA4444 format */
    GRAPHIC_PIXEL_FMT_RGB_444 = 7,                  /**< RGB444 format */
    GRAPHIC_PIXEL_FMT_RGBX_5551 = 8,                /**< RGBX5551 format */
    GRAPHIC_PIXEL_FMT_RGBA_5551 = 9,                /**< RGBA5551 format */
    GRAPHIC_PIXEL_FMT_RGB_555 = 10,                 /**< RGB555 format */
    GRAPHIC_PIXEL_FMT_RGBX_8888 = 11,               /**< RGBX8888 format */
    GRAPHIC_PIXEL_FMT_RGBA_8888 = 12,               /**< RGBA8888 format */
    GRAPHIC_PIXEL_FMT_RGB_888 = 13,                 /**< RGB888 format */
    GRAPHIC_PIXEL_FMT_BGR_565 = 14,                 /**< BGR565 format */
    GRAPHIC_PIXEL_FMT_BGRX_4444 = 15,               /**< BGRX4444 format */
    GRAPHIC_PIXEL_FMT_BGRA_4444 = 16,               /**< BGRA4444 format */
    GRAPHIC_PIXEL_FMT_BGRX_5551 = 17,               /**< BGRX5551 format */
    GRAPHIC_PIXEL_FMT_BGRA_5551 = 18,               /**< BGRA5551 format */
    GRAPHIC_PIXEL_FMT_BGRX_8888 = 19,               /**< BGRX8888 format */
    GRAPHIC_PIXEL_FMT_BGRA_8888 = 20,               /**< BGRA8888 format */
    GRAPHIC_PIXEL_FMT_YUV_422_I = 21,               /**< YUV422 interleaved format */
    GRAPHIC_PIXEL_FMT_YCBCR_422_SP = 22,            /**< YCBCR422 semi-planar format */
    GRAPHIC_PIXEL_FMT_YCRCB_422_SP = 23,            /**< YCRCB422 semi-planar format */
    GRAPHIC_PIXEL_FMT_YCBCR_420_SP = 24,            /**< YCBCR420 semi-planar format */
    GRAPHIC_PIXEL_FMT_YCRCB_420_SP = 25,            /**< YCRCB420 semi-planar format */
    GRAPHIC_PIXEL_FMT_YCBCR_422_P = 26,             /**< YCBCR422 planar format */
    GRAPHIC_PIXEL_FMT_YCRCB_422_P = 27,             /**< YCRCB422 planar format */
    GRAPHIC_PIXEL_FMT_YCBCR_420_P = 28,             /**< YCBCR420 planar format */
    GRAPHIC_PIXEL_FMT_YCRCB_420_P = 29,             /**< YCRCB420 planar format */
    GRAPHIC_PIXEL_FMT_YUYV_422_PKG = 30,            /**< YUYV422 packed format */
    GRAPHIC_PIXEL_FMT_UYVY_422_PKG = 31,            /**< UYVY422 packed format */
    GRAPHIC_PIXEL_FMT_YVYU_422_PKG = 32,            /**< YVYU422 packed format */
    GRAPHIC_PIXEL_FMT_VYUY_422_PKG = 33,            /**< VYUY422 packed format */
    GRAPHIC_PIXEL_FMT_RGBA_1010102 = 34,            /**< RGBA_1010102 packed format */
    GRAPHIC_PIXEL_FMT_YCBCR_P010 = 35,              /**< YCBCR420 semi-planar 10bit packed format */
    GRAPHIC_PIXEL_FMT_YCRCB_P010 = 36,              /**< YCRCB420 semi-planar 10bit packed format */
    GRAPHIC_PIXEL_FMT_RAW10 = 37,                   /**< Raw 10bit packed format */
    GRAPHIC_PIXEL_FMT_BLOB = 38,                    /**< BLOB format */
    GRAPHIC_PIXEL_FMT_VENDER_MASK = 0X7FFF0000,     /**< vendor mask format */
    GRAPHIC_PIXEL_FMT_BUTT = 0X7FFFFFFF             /**< Invalid pixel format */
};

using GraphicLayerType = enum {
    GRAPHIC_LAYER_TYPE_GRAPHIC,         /**< Graphic layer */
    GRAPHIC_LAYER_TYPE_OVERLAY,         /**< Overlay layer */
    GRAPHIC_LAYER_TYPE_SDIEBAND,        /**< Sideband layer */
    GRAPHIC_LAYER_TYPE_CURSOR,          /**< Cursor Layer */
    GRAPHIC_LAYER_TYPE_BUTT             /**< Empty layer */
};

using GraphicLayerInfo = struct {
    int32_t width;                /**< Layer width */
    int32_t height;               /**< Layer height */
    GraphicLayerType type;        /**< Layer type, which can be a graphic layer, overlay layer, or sideband layer */
    int32_t bpp;                  /**< Number of bits occupied by each pixel */
    GraphicPixelFormat pixFormat; /**< Pixel format of the layer */
};

typedef struct {
    int32_t x;      /**< Start X coordinate of the rectangle */
    int32_t y;      /**< Start Y coordinate of the rectangle */
    int32_t w;      /**< Width of the rectangle */
    int32_t h;      /**< Height of the rectangle */
} GraphicIRect;

typedef struct {
    float scaleX;   /* horizontal scale factor */
    float skewX;    /* horizontal skew factor */
    float transX;   /* horizontal translation */
    float skewY;    /* vertical scale factor */
    float scaleY;   /* vertical skew factor */
    float transY;   /* vertical translation */
    float pers0;    /* input x-axis perspective factor */
    float pers1;    /* input y-axis perspective factor */
    float pers2;    /* perspective scale factor */
} GraphicMatrix;

using BufferAllocInfo = struct {
    uint32_t width;                 /**< Width of the requested memory */
    uint32_t height;                /**< Height of the requested memory */
    uint64_t usage;                 /**< Usage of the requested memory */
    GraphicPixelFormat format;      /**< Format of the requested memory */
    uint32_t expectedSize;          /**< Size assigned by memory requester */
};


using BufferVerifyAllocInfo = struct {
    uint32_t width;               /**< Width of the memory to allocate */
    uint32_t height;              /**< Height of the memory to allocate */
    uint64_t usage;               /**< Usage of the memory */
    GraphicPixelFormat format;    /**< Format of the memory to allocate */
};

using GraphicPresentTimestampType = enum {
    GRAPHIC_DISPLAY_PTS_UNSUPPORTED = 0,        /**< Unsupported */
    GRAPHIC_DISPLAY_PTS_DELAY = 1 << 0,         /**< Delay */
    GRAPHIC_DISPLAY_PTS_TIMESTAMP = 1 << 1,     /**< Timestamp */
};

using GraphicPresentTimestamp = struct {
    GraphicPresentTimestampType type;     /**< Present timestamp type */
    int64_t time;                         /**< Present timestamp value */
};

using Rect = struct {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
};

using ScalingMode = enum {
    SCALING_MODE_FREEZE = 0,
    SCALING_MODE_SCALE_TO_WINDOW,
    SCALING_MODE_SCALE_CROP,
    SCALING_MODE_NO_SCALE_CROP,
};

using HDRMetaDataType = enum {
    HDR_NOT_USED = 0,
    HDR_META_DATA,
    HDR_META_DATA_SET,
};

using GraphicHDRMetadataKey = enum {
    GRAPHIC_MATAKEY_RED_PRIMARY_X = 0,
    GRAPHIC_MATAKEY_RED_PRIMARY_Y = 1,
    GRAPHIC_MATAKEY_GREEN_PRIMARY_X = 2,
    GRAPHIC_MATAKEY_GREEN_PRIMARY_Y = 3,
    GRAPHIC_MATAKEY_BLUE_PRIMARY_X = 4,
    GRAPHIC_MATAKEY_BLUE_PRIMARY_Y = 5,
    GRAPHIC_MATAKEY_WHITE_PRIMARY_X = 6,
    GRAPHIC_MATAKEY_WHITE_PRIMARY_Y = 7,
    GRAPHIC_MATAKEY_MAX_LUMINANCE = 8,
    GRAPHIC_MATAKEY_MIN_LUMINANCE = 9,
    GRAPHIC_MATAKEY_MAX_CONTENT_LIGHT_LEVEL = 10,
    GRAPHIC_MATAKEY_MAX_FRAME_AVERAGE_LIGHT_LEVEL = 11,
    GRAPHIC_MATAKEY_HDR10_PLUS = 12,
    GRAPHIC_MATAKEY_HDR_VIVID = 13,
};

using GraphicHDRMetaDataSet = struct GraphicHDRMetaDataSet {
    GraphicHDRMetadataKey key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaData;
};

typedef struct {
    GraphicHDRMetadataKey key;
    float value;
} GraphicHDRMetaData;

using SurfaceBufferUsage = enum {
    BUFFER_USAGE_CPU_READ = (1ULL << 0),        /**< CPU read buffer */
    BUFFER_USAGE_CPU_WRITE = (1ULL << 1),       /**< CPU write memory */
    BUFFER_USAGE_MEM_MMZ = (1ULL << 2),         /**< Media memory zone (MMZ) */
    BUFFER_USAGE_MEM_DMA = (1ULL << 3),         /**< Direct memory access (DMA) buffer */
    BUFFER_USAGE_MEM_SHARE = (1ULL << 4),       /**< Shared memory buffer*/
    BUFFER_USAGE_MEM_MMZ_CACHE = (1ULL << 5),   /**< MMZ with cache*/
    BUFFER_USAGE_MEM_FB = (1ULL << 6),          /**< Framebuffer */
    BUFFER_USAGE_ASSIGN_SIZE = (1ULL << 7),     /**< Memory assigned */
    BUFFER_USAGE_HW_RENDER = (1ULL << 8),       /**< For GPU write case */
    BUFFER_USAGE_HW_TEXTURE = (1ULL << 9),      /**< For GPU read case */
    BUFFER_USAGE_HW_COMPOSER = (1ULL << 10),    /**< For hardware composer */
    BUFFER_USAGE_PROTECTED = (1ULL << 11),      /**< For safe buffer case, such as DRM */
    BUFFER_USAGE_CAMERA_READ = (1ULL << 12),    /**< For camera read case */
    BUFFER_USAGE_CAMERA_WRITE = (1ULL << 13),   /**< For camera write case */
    BUFFER_USAGE_VIDEO_ENCODER = (1ULL << 14),  /**< For encode case */
    BUFFER_USAGE_VIDEO_DECODER = (1ULL << 15),  /**< For decode case */
    BUFFER_USAGE_CPU_READ_OFTEN = (1ULL << 16), /**< CPU read often buffer */
    BUFFER_USAGE_CPU_HW_BOTH = (1ULL << 17), /**< CPU read often buffer */
    BUFFER_USAGE_VENDOR_PRI0 = (1ULL << 44),    /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI1 = (1ULL << 45),    /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI2 = (1ULL << 46),    /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI3 = (1ULL << 47),    /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI4 = (1ULL << 48),    /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI5 = (1ULL << 49),    /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI6 = (1ULL << 50),    /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI7 = (1ULL << 51),    /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI8 = (1ULL << 52),    /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI9 = (1ULL << 53),    /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI10 = (1ULL << 54),   /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI11 = (1ULL << 55),   /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI12 = (1ULL << 56),   /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI13 = (1ULL << 57),   /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI14 = (1ULL << 58),   /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI15 = (1ULL << 59),   /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI16 = (1ULL << 60),   /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI17 = (1ULL << 61),   /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI18 = (1ULL << 62),   /**< Reserverd for vendor */
    BUFFER_USAGE_VENDOR_PRI19 = (1ULL << 63),   /**< Reserverd for vendor */
};

using GraphicColorGamut = enum {
    GRAPHIC_COLOR_GAMUT_INVALID = -1,            /**< Invalid */
    GRAPHIC_COLOR_GAMUT_NATIVE = 0,              /**< Native or default */
    GRAPHIC_COLOR_GAMUT_STANDARD_BT601 = 1,      /**< Standard BT601 */
    GRAPHIC_COLOR_GAMUT_STANDARD_BT709 = 2,      /**< Standard BT709 */
    GRAPHIC_COLOR_GAMUT_DCI_P3 = 3,              /**< DCI P3 */
    GRAPHIC_COLOR_GAMUT_SRGB = 4,                /**< SRGB */
    GRAPHIC_COLOR_GAMUT_ADOBE_RGB = 5,           /**< Adobe RGB */
    GRAPHIC_COLOR_GAMUT_DISPLAY_P3 = 6,          /**< display P3 */
    GRAPHIC_COLOR_GAMUT_BT2020 = 7,              /**< BT2020 */
    GRAPHIC_COLOR_GAMUT_BT2100_PQ = 8,           /**< BT2100 PQ */
    GRAPHIC_COLOR_GAMUT_BT2100_HLG = 9,          /**< BT2100 HLG */
    GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020 = 10,     /**< Display BT2020 */
};

using GraphicCM_ColorSpaceType = enum {
    GRAPHIC_CM_COLORSPACE_NONE,

    /* COLORPRIMARIES_BT601_P | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_P << 16) | (RANGE_FULL << 21) */
    GRAPHIC_CM_BT601_EBU_FULL      = 2 | (1 << 8) | (2 << 16) | (1 << 21),
    /* COLORPRIMARIES_BT601_N | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_N << 16) | (RANGE_FULL << 21) */
    GRAPHIC_CM_BT601_SMPTE_C_FULL  = 3 | (1 << 8) | (3 << 16) | (1 << 21),
    /* COLORPRIMARIES_BT709   | (TRANSFUNC_BT709 << 8) | (MATRIX_BT709   << 16) | (RANGE_FULL << 21) */
    GRAPHIC_CM_BT709_FULL          = 1 | (1 << 8) | (1 << 16) | (1 << 21),
    /* COLORPRIMARIES_BT2020  | (TRANSFUNC_HLG   << 8) | (MATRIX_BT2020  << 16) | (RANGE_FULL << 21) */
    GRAPHIC_CM_BT2020_HLG_FULL     = 4 | (5 << 8) | (4 << 16) | (1 << 21),
    /* COLORPRIMARIES_BT2020  | (TRANSFUNC_PQ    << 8) | (MATRIX_BT2020  << 16) | (RANGE_FULL << 21) */
    GRAPHIC_CM_BT2020_PQ_FULL      = 4 | (4 << 8) | (4 << 16) | (1 << 21),

    /* COLORPRIMARIES_BT601_P | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_P << 16) | (RANGE_LIMITED << 21) */
    GRAPHIC_CM_BT601_EBU_LIMIT     = 2 | (1 << 8) | (2 << 16) | (2 << 21),
    /* COLORPRIMARIES_BT601_N | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_N << 16) | (RANGE_LIMITED << 21) */
    GRAPHIC_CM_BT601_SMPTE_C_LIMIT = 3 | (1 << 8) | (3 << 16) | (2 << 21),
    /* COLORPRIMARIES_BT709   | (TRANSFUNC_BT709 << 8) | (MATRIX_BT709   << 16) | (RANGE_LIMITED << 21) */
    GRAPHIC_CM_BT709_LIMIT         = 1 | (1 << 8) | (1 << 16) | (2 << 21),
    /* COLORPRIMARIES_BT2020  | (TRANSFUNC_HLG   << 8) | (MATRIX_BT2020  << 16) | (RANGE_LIMITED << 21) */
    GRAPHIC_CM_BT2020_HLG_LIMIT    = 4 | (5 << 8) | (4 << 16) | (2 << 21),
    /* COLORPRIMARIES_BT2020  | (TRANSFUNC_PQ    << 8) | (MATRIX_BT2020  << 16) | (RANGE_LIMITED << 21) */
    GRAPHIC_CM_BT2020_PQ_LIMIT     = 4 | (4 << 8) | (4 << 16) | (2 << 21),

    /* COLORPRIMARIES_SRGB     | (TRANSFUNC_SRGB     << 8) | (MATRIX_BT601_N  << 16) | (RANGE_FULL << 21) */
    GRAPHIC_CM_SRGB_FULL           = 1 | (2 << 8) | (3 << 16) | (1 << 21),
    /* COLORPRIMARIES_P3_D65   | (TRANSFUNC_SRGB     << 8) | (MATRIX_P3       << 16) | (RANGE_FULL << 21) */
    GRAPHIC_CM_P3_FULL             = 6 | (2 << 8) | (3 << 16) | (1 << 21),
    /* COLORPRIMARIES_P3_D65   | (TRANSFUNC_HLG      << 8) | (MATRIX_P3       << 16) | (RANGE_FULL << 21) */
    GRAPHIC_CM_P3_HLG_FULL         = 6 | (5 << 8) | (3 << 16) | (1 << 21),
    /* COLORPRIMARIES_P3_D65   | (TRANSFUNC_PQ       << 8) | (MATRIX_P3       << 16) | (RANGE_FULL << 21) */
    GRAPHIC_CM_P3_PQ_FULL          = 6 | (4 << 8) | (3 << 16) | (1 << 21),
    /* COLORPRIMARIES_ADOBERGB | (TRANSFUNC_ADOBERGB << 8) | (MATRIX_ADOBERGB << 16) | (RANGE_FULL << 21) */
    GRAPHIC_CM_ADOBERGB_FULL       = 23 | (6 << 8) | (0 << 16) | (1 << 21),

    /* COLORPRIMARIES_SRGB     | (TRANSFUNC_SRGB     << 8) | (MATRIX_BT601_N  << 16) | (RANGE_LIMITED << 21) */
    GRAPHIC_CM_SRGB_LIMIT          = 1 | (2 << 8) | (3 << 16) | (2 << 21),
    /* COLORPRIMARIES_P3_D65   | (TRANSFUNC_SRGB     << 8) | (MATRIX_P3       << 16) | (RANGE_LIMITED << 21) */
    GRAPHIC_CM_P3_LIMIT            = 6 | (2 << 8) | (3 << 16) | (2 << 21),
    /* COLORPRIMARIES_P3_D65   | (TRANSFUNC_HLG      << 8) | (MATRIX_P3       << 16) | (RANGE_LIMITED << 21) */
    GRAPHIC_CM_P3_HLG_LIMIT        = 6 | (5 << 8) | (3 << 16) | (2 << 21),
    /* COLORPRIMARIES_P3_D65   | (TRANSFUNC_PQ       << 8) | (MATRIX_P3       << 16) | (RANGE_LIMITED << 21) */
    GRAPHIC_CM_P3_PQ_LIMIT         = 6 | (4 << 8) | (3 << 16) | (2 << 21),
    /* COLORPRIMARIES_ADOBERGB | (TRANSFUNC_ADOBERGB << 8) | (MATRIX_ADOBERGB << 16) | (RANGE_LIMITED << 21) */
    GRAPHIC_CM_ADOBERGB_LIMIT      = 23 | (6 << 8) | (0 << 16) | (2 << 21),

    /* COLORPRIMARIES_SRGB   | (TRANSFUNC_LINEAR << 8) */
    GRAPHIC_CM_LINEAR_SRGB         = 1 | (3 << 8),
    /* equal to GRAPHIC_CM_LINEAR_SRGB */
    GRAPHIC_CM_LINEAR_BT709        = 1 | (3 << 8),
    /* COLORPRIMARIES_P3_D65 | (TRANSFUNC_LINEAR << 8) */
    GRAPHIC_CM_LINEAR_P3           = 6 | (3 << 8),
    /* COLORPRIMARIES_BT2020 | (TRANSFUNC_LINEAR << 8) */
    GRAPHIC_CM_LINEAR_BT2020       = 4 | (3 << 8),

    /* equal to GRAPHIC_CM_SRGB_FULL */
    GRAPHIC_CM_DISPLAY_SRGB        = 1 | (2 << 8) | (3 << 16) | (1 << 21),
    /* equal to GRAPHIC_CM_P3_FULL */
    GRAPHIC_CM_DISPLAY_P3_SRGB     = 6 | (2 << 8) | (3 << 16) | (1 << 21),
    /* equal to GRAPHIC_CM_P3_HLG_FULL */
    GRAPHIC_CM_DISPLAY_P3_HLG      = 6 | (5 << 8) | (3 << 16) | (1 << 21),
    /* equal to GRAPHIC_CM_P3_PQ_FULL */
    GRAPHIC_CM_DISPLAY_P3_PQ       = 6 | (4 << 8) | (3 << 16) | (1 << 21),
    /* COLORPRIMARIES_BT2020   | (TRANSFUNC_SRGB << 8)     | (MATRIX_BT2020 << 16)   | (RANGE_FULL << 21) */
    GRAPHIC_CM_DISPLAY_BT2020_SRGB = 4 | (2 << 8) | (4 << 16) | (1 << 21),
    /* equal to GRAPHIC_CM_BT2020_HLG_FULL */
    GRAPHIC_CM_DISPLAY_BT2020_HLG  = 4 | (5 << 8) | (4 << 16) | (1 << 21),
    /* equal to GRAPHIC_CM_BT2020_PQ_FULL */
    GRAPHIC_CM_DISPLAY_BT2020_PQ   = 4 | (4 << 8) | (4 << 16) | (1 << 21)
};

using GraphicColorDataSpace = enum {
    GRAPHIC_COLOR_DATA_SPACE_UNKNOWN = 0,
    GRAPHIC_GAMUT_BT601 = 0x00000001,
    GRAPHIC_GAMUT_BT709 = 0x00000002,
    GRAPHIC_GAMUT_DCI_P3 = 0x00000003,
    GRAPHIC_GAMUT_SRGB = 0x00000004,
    GRAPHIC_GAMUT_ADOBE_RGB = 0x00000005,
    GRAPHIC_GAMUT_DISPLAY_P3 = 0x00000006,
    GRAPHIC_GAMUT_BT2020 = 0x00000007,
    GRAPHIC_GAMUT_BT2100_PQ = 0x00000008,
    GRAPHIC_GAMUT_BT2100_HLG = 0x00000009,
    GRAPHIC_GAMUT_DISPLAY_BT2020 = 0x0000000a,
    GRAPHIC_TRANSFORM_FUNC_UNSPECIFIED = 0x00000100,
    GRAPHIC_TRANSFORM_FUNC_LINEAR = 0x00000200,
    GRAPHIC_TRANSFORM_FUNC_SRGB = 0x00000300,
    GRAPHIC_TRANSFORM_FUNC_SMPTE_170M = 0x00000400,
    GRAPHIC_TRANSFORM_FUNC_GM2_2 = 0x00000500,
    GRAPHIC_TRANSFORM_FUNC_GM2_6 = 0x00000600,
    GRAPHIC_TRANSFORM_FUNC_GM2_8 = 0x00000700,
    GRAPHIC_TRANSFORM_FUNC_ST2084 = 0x00000800,
    GRAPHIC_TRANSFORM_FUNC_HLG = 0x00000900,
    GRAPHIC_PRECISION_UNSPECIFIED = 0x00010000,
    GRAPHIC_PRECISION_FULL = 0x00020000,
    GRAPHIC_PRESION_LIMITED = 0x00030000,
    GRAPHIC_PRESION_EXTENDED = 0x00040000,
    GRAPHIC_BT601_SMPTE170M_FULL = GRAPHIC_GAMUT_BT601 | GRAPHIC_TRANSFORM_FUNC_SMPTE_170M | GRAPHIC_PRECISION_FULL,
    GRAPHIC_BT601_SMPTE170M_LIMITED = GRAPHIC_GAMUT_BT601 | GRAPHIC_TRANSFORM_FUNC_SMPTE_170M |
        GRAPHIC_PRESION_LIMITED,
    GRAPHIC_BT709_LINEAR_FULL = GRAPHIC_GAMUT_BT709 | GRAPHIC_TRANSFORM_FUNC_LINEAR | GRAPHIC_PRECISION_FULL,
    GRAPHIC_BT709_LINEAR_EXTENDED = GRAPHIC_GAMUT_BT709 | GRAPHIC_TRANSFORM_FUNC_LINEAR | GRAPHIC_PRESION_EXTENDED,
    GRAPHIC_BT709_SRGB_FULL = GRAPHIC_GAMUT_BT709 | GRAPHIC_TRANSFORM_FUNC_SRGB | GRAPHIC_PRECISION_FULL,
    GRAPHIC_BT709_SRGB_EXTENDED = GRAPHIC_GAMUT_BT709 | GRAPHIC_TRANSFORM_FUNC_SRGB | GRAPHIC_PRESION_EXTENDED,
    GRAPHIC_BT709_SMPTE170M_LIMITED = GRAPHIC_GAMUT_BT709 | GRAPHIC_TRANSFORM_FUNC_SMPTE_170M |
        GRAPHIC_PRESION_LIMITED,
    GRAPHIC_DCI_P3_LINEAR_FULL = GRAPHIC_GAMUT_DCI_P3 | GRAPHIC_TRANSFORM_FUNC_LINEAR | GRAPHIC_PRECISION_FULL,
    GRAPHIC_DCI_P3_GAMMA26_FULL = GRAPHIC_GAMUT_DCI_P3 | GRAPHIC_TRANSFORM_FUNC_GM2_6 | GRAPHIC_PRECISION_FULL,
    GRAPHIC_DISPLAY_P3_LINEAR_FULL = GRAPHIC_GAMUT_DISPLAY_P3 | GRAPHIC_TRANSFORM_FUNC_LINEAR | GRAPHIC_PRECISION_FULL,
    GRAPHIC_DCI_P3_SRGB_FULL = GRAPHIC_GAMUT_DCI_P3 | GRAPHIC_TRANSFORM_FUNC_SRGB | GRAPHIC_PRECISION_FULL,
    GRAPHIC_ADOBE_RGB_GAMMA22_FULL = GRAPHIC_GAMUT_ADOBE_RGB | GRAPHIC_TRANSFORM_FUNC_GM2_2 | GRAPHIC_PRECISION_FULL,
    GRAPHIC_BT2020_LINEAR_FULL = GRAPHIC_GAMUT_BT2020 | GRAPHIC_TRANSFORM_FUNC_LINEAR | GRAPHIC_PRECISION_FULL,
    GRAPHIC_BT2020_SRGB_FULL = GRAPHIC_GAMUT_BT2020 | GRAPHIC_TRANSFORM_FUNC_SRGB | GRAPHIC_PRECISION_FULL,
    GRAPHIC_BT2020_SMPTE170M_FULL = GRAPHIC_GAMUT_BT2020 | GRAPHIC_TRANSFORM_FUNC_SMPTE_170M | GRAPHIC_PRECISION_FULL,
    GRAPHIC_BT2020_ST2084_FULL = GRAPHIC_GAMUT_BT2020 | GRAPHIC_TRANSFORM_FUNC_ST2084 | GRAPHIC_PRECISION_FULL,
    GRAPHIC_BT2020_HLG_FULL = GRAPHIC_GAMUT_BT2020 | GRAPHIC_TRANSFORM_FUNC_HLG | GRAPHIC_PRECISION_FULL,
    GRAPHIC_BT2020_ST2084_LIMITED = GRAPHIC_GAMUT_BT2020 | GRAPHIC_TRANSFORM_FUNC_ST2084 | GRAPHIC_PRESION_LIMITED,
};

using GraphicTransformType = enum {
    GRAPHIC_ROTATE_NONE = 0,        /**< No rotation */
    GRAPHIC_ROTATE_90,              /**< Rotation by 90 degrees */
    GRAPHIC_ROTATE_180,             /**< Rotation by 180 degrees */
    GRAPHIC_ROTATE_270,             /**< Rotation by 270 degrees */
    GRAPHIC_FLIP_H,                 /**< Flip horizontally */
    GRAPHIC_FLIP_V,                 /**< Flip vertically */
    GRAPHIC_FLIP_H_ROT90,           /**< Flip horizontally and rotate 90 degrees */
    GRAPHIC_FLIP_V_ROT90,           /**< Flip vertically and rotate 90 degrees */
    GRAPHIC_FLIP_H_ROT180,          /**< Flip horizontally and rotate 180 degrees */
    GRAPHIC_FLIP_V_ROT180,          /**< Flip vertically and rotate 180 degrees */
    GRAPHIC_FLIP_H_ROT270,          /**< Flip horizontally and rotate 270 degrees */
    GRAPHIC_FLIP_V_ROT270,          /**< Flip vertically and rotate 270 degrees */
    GRAPHIC_ROTATE_BUTT            /**< Invalid operation */
};

using BufferRequestConfig = struct BufferRequestConfig {
    int32_t width;
    int32_t height;
    int32_t strideAlignment; // output parameter, system components can ignore it
    int32_t format; // GraphicPixelFormat
    uint64_t usage;
    int32_t timeout;
    GraphicColorGamut colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    bool operator ==(const struct BufferRequestConfig &config) const
    {
        return width == config.width &&
               height == config.height &&
               strideAlignment == config.strideAlignment &&
               format == config.format &&
               usage == config.usage &&
               timeout == config.timeout &&
               colorGamut == config.colorGamut &&
               transform == config.transform;
    }
    bool operator != (const struct BufferRequestConfig &config) const
    {
        return !(*this == config);
    }
};

using BufferFlushConfig = struct {
    Rect damage;
    int64_t timestamp;
};

using BufferFlushConfigWithDamages = struct BufferFlushConfigWithDamages {
    std::vector<Rect> damages = {};
    int64_t timestamp;
};

using SceneType = enum {
    SURFACE_SCENE_TYPE_EGL = 0,
    SURFACE_SCENE_TYPE_MEDIA,
    SURFACE_SCENE_TYPE_CAMERA,
    SURFACE_SCENE_TYPE_CPU,
};

using GraphicExtDataHandle = struct {
    /**< Handle fd, -1 if not supported */
    int32_t fd;
    /**< the number of reserved integer value */
    uint32_t reserveInts;
    /**< the reserved data */
    int32_t reserve[0];
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_TYPE_H
