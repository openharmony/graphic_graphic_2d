/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_FILTER_H

#include "render/rs_skia_filter.h"
#include "render/rs_gradient_blur_para.h"

#ifndef USE_ROSEN_DRAWING
#include "include/effects/SkRuntimeEffect.h"
#else
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#endif

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
class RSB_EXPORT RSLinearGradientBlurFilter : public RSSkiaFilter {
#else
class RSB_EXPORT RSLinearGradientBlurFilter : public RSDrawingFilter {
#endif
public:
    RSLinearGradientBlurFilter(const std::shared_ptr<RSLinearGradientBlurPara>& para, const float geoWidth,
        const float geoHeight);
    RSLinearGradientBlurFilter(const RSLinearGradientBlurFilter&) = delete;
    RSLinearGradientBlurFilter operator=(const RSLinearGradientBlurFilter&) = delete;
    ~RSLinearGradientBlurFilter() override;

    void PostProcess(RSPaintFilterCanvas& canvas) override {};
    std::string GetDescription() override;
    void SetBoundsGeometry(float geoWidth, float geoHeight) override
    {
        geoWidth_ = geoWidth;
        geoHeight_ = geoHeight;
    }

#ifndef USE_ROSEN_DRAWING
    void DrawImageRect(
        SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const override;
    void PreProcess(sk_sp<SkImage> image) override {};
    std::shared_ptr<RSSkiaFilter> Compose(const std::shared_ptr<RSSkiaFilter>& other) const override
    {
        return nullptr;
    }
    void SetCanvasChange(SkMatrix& mat, float surfaceWidth, float surfaceHeight) override
    {
        mat_ = mat;
        surfaceWidth_ = surfaceWidth;
        surfaceHeight_ = surfaceHeight;
    }
#else
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
    void PreProcess(std::shared_ptr<Drawing::Image> image) override {};
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter>& other) const override
    {
        return nullptr;
    }
    void SetCanvasChange(Drawing::Matrix& mat, float surfaceWidth, float surfaceHeight) override
    {
        mat_ = mat;
        surfaceWidth_ = surfaceWidth;
        surfaceHeight_ = surfaceHeight;
    }
#endif

private:
    static void TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias);
    static void ComputeScale(float width, float height, bool useMaskAlgorithm);
    static void MakeHorizontalMeanBlurEffect();
    static void MakeVerticalMeanBlurEffect();

    friend class RSMarshallingHelper;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
    inline static float imageScale_ = 1.f;
    inline static float geoWidth_ = 0.f;
    inline static float geoHeight_ = 0.f;
    inline static float surfaceWidth_ = 0.f;
    inline static float surfaceHeight_ = 0.f;

#ifndef USE_ROSEN_DRAWING
    static SkRect::Rect ComputeRectBeforeClip(const uint8_t directionBias, const SkRect& dst);
    static uint8_t CalcDirectionBias(const SkMatrix& mat);
    static bool GetGradientDirectionPoints(SkPoint (&pts)[2],
                                const SkRect& clipBounds, GradientDirection direction);
    static sk_sp<SkShader> MakeAlphaGradientShader(const SkRect& clipBounds,
                                const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias);
    static void DrawMaskLinearGradientBlur(const sk_sp<SkImage>& image, SkCanvas& canvas,
        std::shared_ptr<RSSkiaFilter>& blurFilter, sk_sp<SkShader> alphaGradientShader, const SkRect& dst);
    static sk_sp<SkShader> MakeMaskLinearGradientBlurShader(sk_sp<SkShader> srcImageShader,
        sk_sp<SkShader> blurImageShader, sk_sp<SkShader> gradientShader);
    static void DrawMeanLinearGradientBlur(const sk_sp<SkImage>& image, SkCanvas& canvas,
        float radius, sk_sp<SkShader> alphaGradientShader, const SkRect& dst);

    static sk_sp<SkRuntimeEffect> horizontalMeanBlurShaderEffect_;
    static sk_sp<SkRuntimeEffect> verticalMeanBlurShaderEffect_;
    static sk_sp<SkRuntimeEffect> maskBlurShaderEffect_;
    inline static SkMatrix mat_;
#else
    static Drawing::Rect ComputeRectBeforeClip(const uint8_t directionBias, const Drawing::Rect& dst);
    static uint8_t CalcDirectionBias(const Drawing::Matrix& mat);
    static bool GetGradientDirectionPoints(
        Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GradientDirection direction);
    static std::shared_ptr<Drawing::ShaderEffect> MakeAlphaGradientShader(const Drawing::Rect& clipBounds,
        const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias);
    static void DrawMaskLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image, Drawing::Canvas& canvas,
        std::shared_ptr<RSDrawingFilter>& blurFilter, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
        const Drawing::Rect& dst);
    static std::shared_ptr<Drawing::ShaderEffect> MakeMaskLinearGradientBlurShader(
        std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> blurImageShader,
        std::shared_ptr<Drawing::ShaderEffect> gradientShader);
    static void DrawMeanLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image, Drawing::Canvas& canvas,
        float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::Rect& dst);

    static std::shared_ptr<Drawing::RuntimeEffect> horizontalMeanBlurShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> verticalMeanBlurShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> maskBlurShaderEffect_;
    inline static Drawing::Matrix mat_;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_FILTER_H