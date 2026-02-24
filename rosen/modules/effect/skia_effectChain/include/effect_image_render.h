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
    static std::shared_ptr<EffectImageFilter> Blur(float radius, float angle,
        Drawing::TileMode tileMode = Drawing::TileMode::DECAL);
    static std::shared_ptr<EffectImageFilter> Brightness(float degree);
    static std::shared_ptr<EffectImageFilter> MapColorByBrightness(const std::vector<Vector4f>& colors,
        const std::vector<float>& positions);
    static std::shared_ptr<EffectImageFilter> GammaCorrection(float gamma);
    static std::shared_ptr<EffectImageFilter> Grayscale();
    static std::shared_ptr<EffectImageFilter> CreateSDF(int spreadFactor, bool generateDerivs);
    static std::shared_ptr<EffectImageFilter> Invert();
    static std::shared_ptr<EffectImageFilter> ApplyColorMatrix(const Drawing::ColorMatrix& colorMatrix);
    static std::shared_ptr<EffectImageFilter> EllipticalGradientBlur(float blurRadius, float center_x, float center_y,
        float mask_radius_x, float mask_radius_y, const std::vector<float> &positions,
        const std::vector<float> &degrees);
    static std::shared_ptr<EffectImageFilter> MaskTransition(const std::shared_ptr<Media::PixelMap>& topLayerMap,
        const std::shared_ptr<Drawing::GEShaderMask>& mask, float factor, bool inverse);
    static std::shared_ptr<EffectImageFilter> WaterDropletTransition(
        const std::shared_ptr<OHOS::Media::PixelMap>& topLayerMap,
        const std::shared_ptr<Drawing::GEWaterDropletTransitionFilterParams>& geWaterDropletParams);
    static std::shared_ptr<EffectImageFilter> WaterGlass(
        const std::shared_ptr<Drawing::GEWaterGlassDataParams>& params);
    static std::shared_ptr<EffectImageFilter> ReededGlass(
        const std::shared_ptr<Drawing::GEReededGlassDataParams>& params);
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
    EffectImageBlurFilter(float radius, float angle, const Drawing::TileMode& tileMode)
        : radius_(radius), tileMode_(tileMode), angle_(angle)
    {
        isDirectionBlur_ = true;
    }
    ~EffectImageBlurFilter() override = default;

    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;

private:
    float radius_;
    Drawing::TileMode tileMode_;
    bool isDirectionBlur_ = false;
    float angle_ = 0;
};

class EffectImageEllipticalGradientBlurFilter : public EffectImageFilter {
public:
    EffectImageEllipticalGradientBlurFilter(float blurRadius, float centerX, float centerY, float maskRadiusX,
        float maskRadiusY, const std::vector<float> &positions, const std::vector<float> &degrees)
        : blurRadius_(blurRadius), centerX_(centerX), centerY_(centerY), maskRadiusX_(maskRadiusX),
          maskRadiusY_(maskRadiusY), positions_(positions), degrees_(degrees)
    {}

    ~EffectImageEllipticalGradientBlurFilter() override = default;

    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;

private:
    float blurRadius_;
    float centerX_;
    float centerY_;
    float maskRadiusX_;
    float maskRadiusY_;
    std::vector<float> positions_;
    std::vector<float> degrees_;
};

class EffectImageMapColorByBrightnessFilter : public EffectImageFilter {
public:
    EffectImageMapColorByBrightnessFilter(const std::vector<Vector4f>& colors,
        const std::vector<float>& positions) : colors_(colors), positions_(positions) {}
    ~EffectImageMapColorByBrightnessFilter() override = default;

    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;
private:
    std::vector<Vector4f> colors_;
    std::vector<float> positions_;
};

class EffectImageGammaCorrectionFilter : public EffectImageFilter {
public:
    EffectImageGammaCorrectionFilter(float gamma) : gamma_(gamma) {}
    ~EffectImageGammaCorrectionFilter() override = default;

    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;
private:
    float gamma_ = 0.0f;
};

class EffectImageSdfFromImageFilter : public EffectImageFilter {
public:
    EffectImageSdfFromImageFilter(int spreadFactor, bool generateDerivs)
        : spreadFactor_(spreadFactor), generateDerivs_(generateDerivs) {}
    ~EffectImageSdfFromImageFilter() override = default;

    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;

private:
    int spreadFactor_;
    bool generateDerivs_;
};

class EffectImageMaskTransitionFilter : public EffectImageFilter {
public:
    EffectImageMaskTransitionFilter(const std::shared_ptr<Media::PixelMap>& topLayerMap,
        const std::shared_ptr<Drawing::GEShaderMask>& mask, float factor, bool inverse)
        : topLayerMap_(topLayerMap), mask_(mask), factor_(factor), inverse_(inverse)
        {}
    ~EffectImageMaskTransitionFilter() override = default;
 
    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;
 
private:
    std::shared_ptr<Media::PixelMap> topLayerMap_ = nullptr;
    std::shared_ptr<Drawing::GEShaderMask> mask_ = nullptr;
    float factor_;
    bool inverse_;
};
 
class EffectImageWaterDropletTransitionFilter : public EffectImageFilter {
public:
    EffectImageWaterDropletTransitionFilter(const std::shared_ptr<Media::PixelMap>& topLayerMap,
        const std::shared_ptr<Drawing::GEWaterDropletTransitionFilterParams>& waterDropletParams)
        : topLayerMap_(topLayerMap), waterDropletParams_(waterDropletParams)
        {}
    ~EffectImageWaterDropletTransitionFilter() override = default;
 
    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;
 
private:
    std::shared_ptr<Media::PixelMap> topLayerMap_ = nullptr;
    std::shared_ptr<Drawing::GEWaterDropletTransitionFilterParams> waterDropletParams_ = nullptr;
};

class EffectImageWaterGlassFilter : public EffectImageFilter {
public:
    EffectImageWaterGlassFilter(const std::shared_ptr<Drawing::GEWaterGlassDataParams>& waterGlassData)
        : waterGlassData_(waterGlassData)
    {}

    ~EffectImageWaterGlassFilter() override = default;

    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;

private:
    std::shared_ptr<Drawing::GEWaterGlassDataParams> waterGlassData_;
};

class EffectImageReededGlassFilter : public EffectImageFilter {
public:
    EffectImageReededGlassFilter(const std::shared_ptr<Drawing::GEReededGlassDataParams>& reededGlassData)
        : reededGlassData_(reededGlassData)
    {}
    ~EffectImageReededGlassFilter() override = default;

    DrawingError Apply(const std::shared_ptr<EffectImageChain>& image) override;

private:
    std::shared_ptr<Drawing::GEReededGlassDataParams> reededGlassData_;
};

class EffectImageRender {
public:
    EffectImageRender() = default;
    ~EffectImageRender() = default;

    DrawingError Render(const std::shared_ptr<Media::PixelMap>& srcPixelMap,
        const std::vector<std::shared_ptr<EffectImageFilter>>& effectFilters, bool forceCPU,
        std::shared_ptr<Media::PixelMap>& dstPixelMap);
    DrawingError RenderDstNative(const std::shared_ptr<Media::PixelMap>& srcPixelMap,
        std::shared_ptr<OH_NativeBuffer>& dstNativeBuffer,
        const std::vector<std::shared_ptr<EffectImageFilter>>& effectFilters, bool forceCPU);
};
} // namespace OHOS::Rosen
#endif // EFFECT_IMAGE_RENDER_H