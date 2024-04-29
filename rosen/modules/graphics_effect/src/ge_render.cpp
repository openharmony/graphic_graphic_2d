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
#include "ge_linear_gradient_blur_shader_filter.h"
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
    if (!image) {
        LOGE("GERender::DrawImageRect image is null");
        return;
    }
    std::vector<std::shared_ptr<GEShaderFilter>> geShaderFilters = GenerateShaderFilter(veContainer);

    auto resImage = ApplyImageEffect(canvas, veContainer, image, src, dst, sampling);
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*resImage, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::Image> GERender::ApplyImageEffect(Drawing::Canvas& canvas,
    Drawing::GEVisualEffectContainer& veContainer, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling)
{
    if (!image) {
        LOGE("GERender::ApplyImageEffect image is null");
        return nullptr;
    }
    std::vector<std::shared_ptr<GEShaderFilter>> geShaderFilters = GenerateShaderFilter(veContainer);
    auto resImage = image;
    for (auto geShaderFilter : geShaderFilters) {
        if (geShaderFilter != nullptr) {
            resImage = geShaderFilter->ProcessImage(canvas, resImage, src, dst);
        } else {
            LOGD("GERender::ApplyImageEffect filter is null");
        }
    }

    return resImage;
}

std::vector<std::shared_ptr<GEShaderFilter>> GERender::GenerateShaderFilter(
    Drawing::GEVisualEffectContainer& veContainer)
{
    LOGD("GERender::shaderFilters %{public}d", (int)veContainer.GetFilters().size());
    std::vector<std::shared_ptr<GEShaderFilter>> shaderFilters;
    for (auto vef : veContainer.GetFilters()) {
        auto ve = vef->GetImpl();
        std::shared_ptr<GEShaderFilter> shaderFilter;
        switch (ve->GetFilterType()) {
            case Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR: {
                const auto& kawaseParams = ve->GetKawaseParams();
                LOGD("GERender::KAWASE_BLUR %{public}d", kawaseParams->radius);
                shaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(*kawaseParams);
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::AIBAR: {
                const auto& aiBarParams = ve->GetAIBarParams();
                LOGD("GERender::AIBAR %{public}f,", aiBarParams->aiBarLow);
                shaderFilter = std::make_shared<GEAIBarShaderFilter>(*aiBarParams);
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::GREY: {
                const auto& greyParams = ve->GetGreyParams();
                LOGD("GERender::GREY %{public}f,", greyParams->greyCoef1);
                shaderFilter = std::make_shared<GEGreyShaderFilter>(*greyParams);
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR: {
                const auto& linearGradientBlurParams = ve->GetLinearGradientBlurParams();
                LOGD("GERender::LINEAR_GRADIENT_BLUR %{public}f,", linearGradientBlurParams->blurRadius);
                shaderFilter = std::make_shared<GELinearGradientBlurShaderFilter>(*linearGradientBlurParams);
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
