/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef EFFECT_IMAGE_CHAIN_H
#define EFFECT_IMAGE_CHAIN_H

#include <memory>
#include <mutex>

#include "include/draw/canvas.h"
#include "include/draw/surface.h"
#include "include/effect/image_filter.h"
#include "pixel_map.h"

namespace OHOS::Rosen {
enum class DrawingError {
    ERR_OK = 0,
    ERR_NOT_PREPARED,
    ERR_SURFACE,
    ERR_IMAGE_NULL,
    ERR_PIXEL_READ,
    ERR_ILLEGAL_INPUT,
    ERR_MEMORY,
    ERR_FAST_BLUR
};

class ImageUtil {
public:
    static Drawing::ColorType PixelFormatToDrawingColorType(const Media::PixelFormat& pixelFormat);

    static Drawing::AlphaType AlphaTypeToDrawingAlphaType(const Media::AlphaType& alphaType);
};

class EffectImageChain {
public:
    EffectImageChain() = default;
    ~EffectImageChain() = default;

    DrawingError Prepare(const std::shared_ptr<Media::PixelMap>& srcPixelMap, bool forceCPU);

    DrawingError ApplyDrawingFilter(const std::shared_ptr<Drawing::ImageFilter>& filter);
    DrawingError ApplyBlur(float radius, const Drawing::TileMode& tileMode);

    DrawingError Draw();

    std::shared_ptr<Media::PixelMap> GetPixelMap();

private:
    bool CheckPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap);
    DrawingError InitWithoutCanvas(const std::shared_ptr<Media::PixelMap>& srcPixelMap);
    std::shared_ptr<Drawing::Surface> CreateSurface(bool forceCPU);
    DrawingError ApplyMesaBlur(float radius, const Drawing::TileMode& tileMode);
    DrawingError ApplyHpsBlur(float radius);
    void DrawOnFilter();

    std::mutex apiMutex_;

    bool prepared_ = false;
    bool forceCPU_ = true;

    Drawing::ImageInfo imageInfo_ = {};
    std::shared_ptr<Drawing::Image> image_ = nullptr;
    std::shared_ptr<Drawing::ImageFilter> filters_ = nullptr;

    std::shared_ptr<Media::PixelMap> srcPixelMap_ = nullptr;
    std::shared_ptr<Media::PixelMap> dstPixelMap_ = nullptr;

    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // EFFECT_IMAGE_CHAIN_H