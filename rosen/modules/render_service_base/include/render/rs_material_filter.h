/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MATERIAL_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MATERIAL_FILTER_H

#ifdef NEW_SKIA
#include "include/effects/SkRuntimeEffect.h"
#endif

#include "common/rs_color.h"
#include "render/rs_skia_filter.h"
#include "render/rs_kawase_blur.h"

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkColorFilter.h"
#include "include/core/SkColor.h"
#include "include/effects/SkColorMatrix.h"
#include "include/effects/SkImageFilters.h"
#else
#include "effect/color_filter.h"
#include "draw/color.h"
#include "effect/color_matrix.h"
#include "effect/image_filter.h"
#endif
#include "property/rs_color_picker_cache_task.h"

namespace OHOS {
namespace Rosen {
enum MATERIAL_BLUR_STYLE : int {
    // card blur style
    STYLE_CARD_THIN_LIGHT  = 1,
    STYLE_CARD_LIGHT       = 2,
    STYLE_CARD_THICK_LIGHT = 3,
    STYLE_CARD_THIN_DARK   = 4,
    STYLE_CARD_DARK        = 5,
    STYLE_CARD_THICK_DARK  = 6,

    // background blur style
    STYLE_BACKGROUND_SMALL_LIGHT  = 101,
    STYLE_BACKGROUND_MEDIUM_LIGHT = 102,
    STYLE_BACKGROUND_LARGE_LIGHT  = 103,
    STYLE_BACKGROUND_XLARGE_LIGHT = 104,
    STYLE_BACKGROUND_SMALL_DARK   = 105,
    STYLE_BACKGROUND_MEDIUM_DARK  = 106,
    STYLE_BACKGROUND_LARGE_DARK   = 107,
    STYLE_BACKGROUND_XLARGE_DARK  = 108
};
// material blur style params
struct MaterialParam {
    float radius;
    float saturation;
    float brightness;
    RSColor maskColor;
};
#ifndef USE_ROSEN_DRAWING
class RSB_EXPORT RSMaterialFilter : public RSSkiaFilter {
#else
class RSB_EXPORT RSMaterialFilter : public RSDrawingFilter {
#endif
public:
    RSMaterialFilter(int style, float dipScale, BLUR_COLOR_MODE mode, float ratio);
    RSMaterialFilter(MaterialParam materialParam, BLUR_COLOR_MODE mode);
    RSMaterialFilter(const RSMaterialFilter&) = delete;
    RSMaterialFilter operator=(const RSMaterialFilter&) = delete;
    ~RSMaterialFilter() override;
    std::shared_ptr<RSFilter> TransformFilter(float fraction) const;
    bool IsValid() const override;
#ifndef USE_ROSEN_DRAWING
    void PreProcess(sk_sp<SkImage> image) override;
#else
    void PreProcess(std::shared_ptr<Drawing::Image> image) override;
#endif
    void PostProcess(RSPaintFilterCanvas& canvas) override;
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<RSSkiaFilter> Compose(const std::shared_ptr<RSSkiaFilter>& other) const override;
#else
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter>& other) const override;
#endif
    std::string GetDescription() override;

    std::shared_ptr<RSFilter> Add(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Sub(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Multiply(float rhs) override;
    std::shared_ptr<RSFilter> Negate() override;

#ifndef USE_ROSEN_DRAWING
    void DrawImageRect(
        SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const override;
#else
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
#endif
    float GetRadius() const;
    bool CanSkipFrame() const override;

    bool IsNearEqual(
        const std::shared_ptr<RSFilter>& other, float threshold = std::numeric_limits<float>::epsilon()) const override;
    bool IsNearZero(float threshold = std::numeric_limits<float>::epsilon()) const override;
    void SetGreyCoef(float greyCoef1, float greyCoef2, bool isGreyCoefValid) override;
 
    // color picker subthread
    const std::shared_ptr<RSColorPickerCacheTask>& GetColorPickerCacheTask() const;
    void ReleaseColorPickerFilter();
private:
    BLUR_COLOR_MODE colorMode_;
    float radius_ {};
    float saturation_ = 1.f;
    float brightness_ = 1.f;
    float greyCoef1_ = 0.f;
    float greyCoef2_ = 0.f;
    bool isGreyCoefValid_ = false;
    RSColor maskColor_ = RSColor();

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkColorFilter> GetColorFilter(float sat, float brightness);
    sk_sp<SkImageFilter> CreateMaterialStyle(MATERIAL_BLUR_STYLE style, float dipScale, float ratio);
    sk_sp<SkImageFilter> CreateMaterialFilter(float radius, float sat, float brightness);
#else
    std::shared_ptr<Drawing::ColorFilter> GetColorFilter(float sat, float brightness);
    std::shared_ptr<Drawing::ImageFilter> CreateMaterialStyle(MATERIAL_BLUR_STYLE style, float dipScale, float ratio);
    std::shared_ptr<Drawing::ImageFilter> CreateMaterialFilter(float radius, float sat, float brightness);
#endif
    static float RadiusVp2Sigma(float radiusVp, float dipScale);

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkColorFilter> colorFilter_;
#else
    std::shared_ptr<Drawing::ColorFilter> colorFilter_;
#endif
    std::shared_ptr<RSColorPickerCacheTask> colorPickerTask_;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H