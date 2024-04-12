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
#include "ge_render.h"

#include "ge_aibar_shader_filter.h"
#include "ge_grey_shader_filter.h"
#include "ge_kawase_blur_shader_filter.h"
#include "ge_log.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace GraphicsEffectEngine {

GERender::GERender() {}

GERender::~GERender() {}

void GERender::DrawImageEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
    const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src, const Drawing::Rect& dst,
    const Drawing::SamplingOptions& sampling)
{
    std::vector<std::shared_ptr<GEShaderFilter>> geShaderFilters = GenerateShaderFilter(veContainer);

    for (auto geShaderFilter : geShaderFilters) {
        if (geShaderFilter != nullptr) {
            *image = *(geShaderFilter->ProcessImage(canvas, image, src, dst));
        } else {
            LOGE("GERender::DrawImageRect filter is null");
        }
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*image, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::Image> GERender::ApplyImageEffect(Drawing::Canvas& canvas,
    Drawing::GEVisualEffectContainer& veContainer, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling)
{
    std::vector<std::shared_ptr<GEShaderFilter>> geShaderFilters = GenerateShaderFilter(veContainer);

    for (auto geShaderFilter : geShaderFilters) {
        if (geShaderFilter != nullptr) {
            *image = *(geShaderFilter->ProcessImage(canvas, image, src, dst));
        } else {
            LOGE("GERender::DrawImageRect filter is null");
        }
    }

    return image;
}

std::vector<std::shared_ptr<GEShaderFilter>> GERender::GenerateShaderFilter(
    Drawing::GEVisualEffectContainer& veContainer)
{
    std::vector<std::shared_ptr<GEShaderFilter>> shaderFilters;
    for (auto vef : veContainer.GetFilters()) {
        auto ve = vef->GetImpl();
        std::shared_ptr<GEShaderFilter> shaderFilter;
        switch (ve->GetFilterType()) {
            case Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR: {
                const auto& kawaseParams = ve->GetKawaseParams();
                LOGE("GERender::KAWASE_BLUR %{public}d", kawaseParams->radius);
                shaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(*kawaseParams);
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::AIBAR: {
                const auto& aiBarParams = ve->GetAIBarParams();
                LOGE("GERender::AIBAR %{public}f,", aiBarParams->aiBarLow);
                shaderFilter = std::make_shared<GEAIBarShaderFilter>(*aiBarParams);
                break;
            }
            default:
                break;
        }
        shaderFilters.push_back(shaderFilter);
    }
    return shaderFilters;
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
