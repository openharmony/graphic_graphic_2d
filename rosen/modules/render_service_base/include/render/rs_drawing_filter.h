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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_DRAWING_RS_DRAWING_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_DRAWING_RS_DRAWING_FILTER_H

#include "common/rs_color.h"
#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "effect/color_filter.h"
#include "effect/color_matrix.h"
#include "effect/image_filter.h"
#include "effect/runtime_shader_builder.h"
#include "property/rs_color_picker_cache_task.h"
#include "render/rs_filter.h"
#include "render/rs_kawase_blur.h"
#include "render/rs_shader_filter.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSDrawingFilter : public RSFilter {
public:
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter, uint32_t hash);
    RSDrawingFilter(std::shared_ptr<RSShaderFilter> shaderFilter);
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
        std::shared_ptr<RSShaderFilter> shaderFilter, uint32_t hash);
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
        std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters, uint32_t hash);
    ~RSDrawingFilter() override;

    Drawing::Brush GetBrush() const;
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst);
    std::vector<std::shared_ptr<RSShaderFilter>> GetShaderFilters() const;
    void InsertShaderFilter(std::shared_ptr<RSShaderFilter> shaderFilter);
    std::shared_ptr<Drawing::ImageFilter> GetImageFilter() const;
    void SetImageFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter);
    std::shared_ptr<RSShaderFilter> GetShaderFilterWithType(RSShaderFilter::ShaderFilterType type)
    {
        for (const auto& shaderFilter : shaderFilters_) {
            if (shaderFilter->GetShaderFilterType() == type) {
                return shaderFilter;
            }
        }
        return nullptr;
    }

    uint32_t Hash() const override;
    uint32_t ShaderHash() const;
    uint32_t ImageHash() const;
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter> other) const;
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<Drawing::ImageFilter> other, uint32_t hash) const;
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSShaderFilter> other) const;
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

private:
    std::shared_ptr<Drawing::ImageFilter> imageFilter_ = nullptr;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters_;
    uint32_t imageFilterHash_;
    bool canSkipFrame_ = false;
    float saturationForHPS_ = 1.f;
    float brightnessForHPS_ = 1.f;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_DRAWING_RS_DRAWING_FILTER_H