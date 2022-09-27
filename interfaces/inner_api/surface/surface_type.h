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
#include <display_type.h>

namespace OHOS {
#define SURFACE_MAX_USER_DATA_COUNT 1000
#define SURFACE_MAX_QUEUE_SIZE 32
#define SURFACE_DEFAULT_QUEUE_SIZE 3
#define SURFACE_MAX_STRIDE_ALIGNMENT 32
#define SURFACE_MIN_STRIDE_ALIGNMENT 4
#define SURFACE_DEFAULT_STRIDE_ALIGNMENT 4
#define SURFACE_MAX_SIZE 58982400 // 8K * 8K

using Rect = struct Rect {
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

using HDRMetaDataSet = struct HDRMetaDataSet {
    HDRMetadataKey key = HDRMetadataKey::MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaData;
};

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

using BufferRequestConfig = struct BufferRequestConfig {
    int32_t width;
    int32_t height;
    int32_t strideAlignment;
    int32_t format; // PixelFormat
    uint64_t usage;
    int32_t timeout;
    ColorGamut colorGamut = ColorGamut::COLOR_GAMUT_SRGB;
    TransformType transform = TransformType::ROTATE_NONE;
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

using BufferFlushConfig = struct BufferFlushConfig {
    Rect damage;
    int64_t timestamp;
};

using SceneType = enum {
    SURFACE_SCENE_TYPE_EGL = 0,
    SURFACE_SCENE_TYPE_MEDIA,
    SURFACE_SCENE_TYPE_CAMERA,
    SURFACE_SCENE_TYPE_CPU,
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_TYPE_H
