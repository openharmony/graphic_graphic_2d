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

#ifndef EFFECT_IMAGE_RENDER_H
#define EFFECT_IMAGE_RENDER_H

#include "effect_image_chain.h"

namespace OHOS::Rosen {
class EffectImageFilter {
public:
    virtual ~EffectImageFilter() = default;

    static std::shared_ptr<EffectImageFilter> Blur(float radius, Drawing::TileMode tileMode = Drawing::TileMode::DECAL);
    static std::shared_ptr<EffectImageFilter> Brightness(float degree);
    static std::shared_ptr<EffectImageFilter> Grayscale();
    static std::shared_ptr<EffectImageFilter> Invert();
    static std::shared_ptr<EffectImageFilter> ApplyColorMatrix(const Drawing::ColorMatrix& colorMatrix);

    virtual DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) = 0;
};

class EffectImageDrawingFilter : public EffectImageFilter {
public:
    EffectImageDrawingFilter(const std::shared_ptr<Drawing::ImageFilter>& filter) : filter_(filter) {}
    ~EffectImageDrawingFilter() override = default;

    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;

private:
    std::shared_ptr<Drawing::ImageFilter> filter_ = nullptr;
};

class EffectImageBlurFilter : public EffectImageFilter {
public:
    EffectImageBlurFilter(float radius, const Drawing::TileMode& tileMode) : radius_(radius), tileMode_(tileMode) {}
    ~EffectImageBlurFilter() override = default;

    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;

private:
    float radius_;
    Drawing::TileMode tileMode_;
};

class EffectImageRender {
public:
    EffectImageRender() = default;
    ~EffectImageRender() = default;

    DrawingError Render(const std::shared_ptr<Media::PixelMap>& srcPixelMap,
        const std::vector<std::shared_ptr<EffectImageFilter>>& effectFilters, bool forceCPU,
        std::shared_ptr<Media::PixelMap>& dstPixelMap);
};
} // namespace OHOS::Rosen
#endif // EFFECT_IMAGE_RENDER_H