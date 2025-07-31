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
#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_DRAWING_RS_DRAWING_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_DRAWING_RS_DRAWING_FILTER_H

#include "common/rs_color.h"
#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "effect/color_filter.h"
#include "effect/color_matrix.h"
#include "effect/image_filter.h"
#include "effect/runtime_shader_builder.h"
#include "render/rs_filter.h"
#include "render/rs_kawase_blur.h"
#include "render/rs_hps_blur.h"
#include "render/rs_render_filter_base.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSB_EXPORT RSDrawingFilter : public RSFilter {
public:
    RSDrawingFilter() = default;
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter, uint32_t hash);
    RSDrawingFilter(std::shared_ptr<RSRenderFilterParaBase> shaderFilter);
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
        std::shared_ptr<RSRenderFilterParaBase> shaderFilter, uint32_t hash);
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
        std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters, uint32_t hash);
    ~RSDrawingFilter() override;

    std::string GetDescription() override;
    std::string GetDetailedDescription() override;
    Drawing::Brush GetBrush(float brushAlpha = 1.0f) const;

    struct DrawImageRectParams {
        bool discardCanvas;
        bool offscreenDraw;
    };

    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst, const DrawImageRectParams params = { false, false });
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> GetShaderFilters() const;
    void InsertShaderFilter(std::shared_ptr<RSRenderFilterParaBase> shaderFilter);
    std::shared_ptr<Drawing::ImageFilter> GetImageFilter() const;
    void SetImageFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter);
    std::shared_ptr<RSRenderFilterParaBase> GetShaderFilterWithType(RSUIFilterType type)
    {
        for (const auto& shaderFilter : shaderFilters_) {
            if (shaderFilter->GetType() == type) {
                return shaderFilter;
            }
        }
        return nullptr;
    }
    void OnSync() override;
    bool IsDrawingFilter() const override
    {
        return true;
    }
    uint32_t Hash() const override;
    uint32_t ShaderHash() const;
    uint32_t ImageHash() const;
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter> other) const;
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<Drawing::ImageFilter> other, uint32_t hash) const;
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSRenderFilterParaBase> other) const;
    void SetNGRenderFilter(std::shared_ptr<RSNGRenderFilterBase> filter);

    inline std::shared_ptr<RSNGRenderFilterBase> GetNGRenderFilter() const
    {
        return renderFilter_;
    }

    void GenerateAndUpdateGEVisualEffect();

    void SetGeometry(Drawing::Canvas& canvas, float geoWidth, float geoHeight);
    void SetDisplayHeadroom(float headroom);

    bool CanSkipFrame() const
    {
        return canSkipFrame_;
    }

    void SetSkipFrame(bool canSkipFrame)
    {
        canSkipFrame_ = canSkipFrame;
    }

    static bool CanSkipFrame(float radius);
    void SetSaturationForHPS(float saturationForHPS)
    {
        saturationForHPS_ = saturationForHPS;
    }
    void SetBrightnessForHPS(float brightnessForHPS)
    {
        brightnessForHPS_ = brightnessForHPS;
    }
    void PreProcess(std::shared_ptr<Drawing::Image>& image);
    void PostProcess(Drawing::Canvas& canvas);

    void ApplyColorFilter(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst, float brushAlpha);

    bool NeedForceSubmit() const override;

private:
    struct DrawImageRectAttributes {
        Drawing::Rect src;
        Drawing::Rect dst;
        bool discardCanvas;
        float brushAlpha;
    };
    void DrawImageRectInternal(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const DrawImageRectAttributes& attr);
    float PrepareAlphaForOnScreenDraw(RSPaintFilterCanvas& paintFilterCanvas);
    std::shared_ptr<Drawing::ImageFilter> ProcessImageFilter(float brushAlpha) const;

    bool IsHpsBlurApplied(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& outImage,
        const DrawImageRectAttributes& attr, const Drawing::Brush& brush, float radius);
    /**
     * @brief Apply GE Kawase Blur on image
     *
     * KAWASE filter will be excuted by GE Kawase BLur on image
     *
     * @param canvas                the canvas that draw Kawase filter
     * @param outImage              the image that needed to apply Kawase filter
     * @param attr                  the Attributes of canvas
     * @param brush                 the brush that will be attached on canvas
     * @param kawaseShaderFilter    the Kawase filter that will be applyed
     */
    void DrawKawaseEffect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& outImage,
        const DrawImageRectAttributes& attr, const Drawing::Brush& brush,
        std::shared_ptr<RSRenderFilterParaBase>& kawaseShaderFilter);
    /**
     * @brief Apply HPS Effect on image
     *
     * Try to apply all shaderFilters_ by HPSEffect on image
     * When calling ApplyHpsImageEffect(),
       if outImage == nullptr means:
           HPS draw shaderFilters_ fail, and func will return false,
       if outImage != nullptr and return true means:
           shaderFilters_ contain Kawase or Mesa and HPS draw shaderFilters_ succ,
       if outImage != nullptr and return false means:
           shaderFilters_ not contain Kawase and Mesa and HPS draw shaderFilters_ succ.
     *
     * @param canvas                the canvas that draw all shaderFilters_
     * @param image                 the image that needed to apply all shaderFilters_
     * @param outImage              the image after applying all shaderFilters_
     * @param attr                  the Attributes of canvas
     * @param brush                 the brush that will be attached on canvas
     * @return  true when HPS draw Kawase or Mesa succ, false otherwise.
     */
    bool ApplyHpsImageEffect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        std::shared_ptr<Drawing::Image>& outImage, const DrawImageRectAttributes& attr, Drawing::Brush& brush);
    bool ApplyImageEffectWithLightBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const DrawImageRectAttributes& attr, const Drawing::Brush& brush);
    void ApplyImageEffect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const std::shared_ptr<Drawing::GEVisualEffectContainer>& visualEffectContainer,
        const DrawImageRectAttributes& attr);
    void ProfilerLogImageEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer,
        const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src,
        const std::shared_ptr<Drawing::Image>& outImage);
    std::string GetFilterTypeString() const;
    std::shared_ptr<Drawing::ImageFilter> imageFilter_ = nullptr;
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters_;
    std::shared_ptr<RSNGRenderFilterBase> renderFilter_ = nullptr;
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer_;
    uint32_t imageFilterHash_ = 0;
    uint32_t renderFilterHash_ = 0;
    bool canSkipFrame_ = false;
    bool canSkipMaskColor_ = false;
    float saturationForHPS_ = 1.f;
    float brightnessForHPS_ = 1.f;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RENDER_DRAWING_RS_DRAWING_FILTER_H