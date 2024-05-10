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

#include "render/rs_drawing_filter.h"

#include <memory>

#include "common/rs_optional_trace.h"
#include "draw/blend_mode.h"
#include "ge_render.h"
#include "ge_visual_effect.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_aibar_shader_filter.h"
#include "render/rs_grey_shader_filter.h"
#include "render/rs_kawase_blur_shader_filter.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"
#include "render/rs_maskcolor_shader_filter.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int SHADER_FILTERS_SIZE = 10;
const std::map<int, std::string> FILTER_TYPE_MAP {
    { RSFilter::BLUR, "RSBlurFilterBlur" },
    { RSFilter::MATERIAL, "RSMaterialFilterBlur" },
    { RSFilter::AIBAR, "RSAIBarFilterBlur" },
    { RSFilter::LINEAR_GRADIENT_BLUR, "RSLinearGradientBlurFilterBlur" },
};
}
RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter, uint32_t hash)
    : RSFilter(), imageFilter_(imageFilter)
{
    imageFilterHash_ = hash;
    shaderFilters_.reserve(SHADER_FILTERS_SIZE);
}

RSDrawingFilter::RSDrawingFilter(std::shared_ptr<RSShaderFilter> shaderFilter)
{
    hash_ = shaderFilter->Hash();
    shaderFilters_.reserve(SHADER_FILTERS_SIZE);
    shaderFilters_.emplace_back(shaderFilter);
}

RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
    std::shared_ptr<RSShaderFilter> shaderFilter, uint32_t hash) : RSFilter(), imageFilter_(imageFilter)
{
    hash_ = shaderFilter->Hash();
    imageFilterHash_ = hash;
    shaderFilters_.reserve(SHADER_FILTERS_SIZE);
    shaderFilters_.emplace_back(shaderFilter);
}

RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters, uint32_t hash)
    : RSFilter(), imageFilter_(imageFilter)
{
    shaderFilters_ = shaderFilters;
    for (const auto& shaderFilter : shaderFilters_) {
        uint32_t hash = shaderFilter->Hash();
        hash_ = SkOpts::hash(&hash, sizeof(hash), hash_);
    }
    imageFilterHash_ = hash;
}

RSDrawingFilter::~RSDrawingFilter() {};

std::string RSDrawingFilter::GetDescription()
{
    std::string filterString = FILTER_TYPE_MAP.at(RSFilter::type_);
    for (const auto& shaderFilter : shaderFilters_) {
        switch (shaderFilter->GetShaderFilterType()) {
            case RSShaderFilter::KAWASE: {
                auto filter = std::static_pointer_cast<RSKawaseBlurShaderFilter>(shaderFilter);
                int radius = filter->GetRadius();
                filterString = filterString + ", radius: " + std::to_string(radius) + " sigma";
                break;
            }
            case RSShaderFilter::LINEAR_GRADIENT_BLUR: {
                auto filter4 = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(shaderFilter);
                float radius2 = filter4->GetLinearGradientBlurRadius();
                filterString = filterString + ", radius: " + std::to_string(radius2);
                break;
            }
            default:
                break;
        }
    }

    return filterString;
}

std::string RSDrawingFilter::GetDetailedDescription()
{
    std::string filterString = FILTER_TYPE_MAP.at(RSFilter::type_);
    for (const auto& shaderFilter : shaderFilters_) {
        switch (shaderFilter->GetShaderFilterType()) {
            case RSShaderFilter::KAWASE: {
                auto filter = std::static_pointer_cast<RSKawaseBlurShaderFilter>(shaderFilter);
                int radius = filter->GetRadius();
                filterString = filterString + ", radius: " + std::to_string(radius) + " sigma";
                break;
            }
            case RSShaderFilter::GREY: {
                auto filter2 = std::static_pointer_cast<RSGreyShaderFilter>(shaderFilter);
                float greyCoefLow = filter2->GetGreyCoefLow();
                float greyCoefHigh = filter2->GetGreyCoefHigh();
                filterString = filterString + ", greyCoef1: " + std::to_string(greyCoefLow);
                filterString = filterString + ", greyCoef2: " + std::to_string(greyCoefHigh);
                break;
            }
            case RSShaderFilter::MASK_COLOR: {
                auto filter3 = std::static_pointer_cast<RSMaskColorShaderFilter>(shaderFilter);
                int colorMode = filter3->GetColorMode();
                RSColor maskColor = filter3->GetMaskColor();
                char maskColorStr[UINT8_MAX] = { 0 };
                auto ret = memset_s(maskColorStr, UINT8_MAX, 0, UINT8_MAX);
                if (ret != EOK) {
                    return "Failed to memset_s for maskColorStr, ret=" + std::to_string(ret);
                }
                if (sprintf_s(maskColorStr, UINT8_MAX, "%08X", maskColor.AsArgbInt()) != -1) {
                    filterString =
                        filterString + ", maskColor: " + maskColorStr + ", colorMode: " + std::to_string(colorMode);
                }
                break;
            }
            case RSShaderFilter::LINEAR_GRADIENT_BLUR: {
                auto filter4 = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(shaderFilter);
                float radius2 = filter4->GetLinearGradientBlurRadius();
                filterString = filterString + ", radius: " + std::to_string(radius2);
                break;
            }
            default:
                break;
        }
    }

    return filterString;
}

Drawing::Brush RSDrawingFilter::GetBrush() const
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetImageFilter(imageFilter_);
    brush.SetFilter(filter);
    return brush;
}

bool RSDrawingFilter::CanSkipFrame(float radius)
{
    constexpr float HEAVY_BLUR_THRESHOLD = 25.0f;
    return radius > HEAVY_BLUR_THRESHOLD;
}

uint32_t RSDrawingFilter::Hash() const
{
    auto hash = SkOpts::hash(&imageFilterHash_, sizeof(imageFilterHash_), hash_);
    return hash;
}

uint32_t RSDrawingFilter::ShaderHash() const
{
    return hash_;
}

uint32_t RSDrawingFilter::ImageHash() const
{
    return imageFilterHash_;
}

std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(const std::shared_ptr<RSDrawingFilter> other) const
{
    std::shared_ptr<RSDrawingFilter> result =
        std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_, imageFilterHash_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
    for (auto item : other->GetShaderFilters()) {
        result->InsertShaderFilter(item);
    }
    auto otherShaderHash = other->ShaderHash();
    auto otherImageHash = other->ImageHash();
    result->hash_ = SkOpts::hash(&otherShaderHash, sizeof(otherShaderHash), hash_);
    result->imageFilterHash_ = SkOpts::hash(&otherImageHash, sizeof(otherImageHash), imageFilterHash_);
    return result;
}

std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(const std::shared_ptr<RSShaderFilter> other) const
{
    std::shared_ptr<RSDrawingFilter> result =
        std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_, imageFilterHash_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->InsertShaderFilter(other);
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}

std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(
    const std::shared_ptr<Drawing::ImageFilter> other, uint32_t hash) const
{
    std::shared_ptr<RSDrawingFilter> result =
        std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_, imageFilterHash_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other);
    result->imageFilterHash_ = SkOpts::hash(&hash, sizeof(hash), imageFilterHash_);
    return result;
}

std::shared_ptr<Drawing::ImageFilter> RSDrawingFilter::GetImageFilter() const
{
    return imageFilter_;
}

void RSDrawingFilter::SetImageFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter)
{
    imageFilter_ = imageFilter;
}

std::vector<std::shared_ptr<RSShaderFilter>> RSDrawingFilter::GetShaderFilters() const
{
    return shaderFilters_;
}

void RSDrawingFilter::InsertShaderFilter(std::shared_ptr<RSShaderFilter> shaderFilter)
{
    shaderFilters_.emplace_back(shaderFilter);
}

void RSDrawingFilter::ApplyColorFilter(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    Drawing::Brush brush;
    if (imageFilter_) {
        Drawing::Filter filter;
        filter.SetImageFilter(imageFilter_);
        brush.SetFilter(filter);
    }
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*image, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();

    return;
}

void RSDrawingFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    for (const auto& filter : shaderFilters_) {
        if (filter->GetShaderFilterType() == RSShaderFilter::KAWASE) {
            continue;
        }
        filter->GenerateGEVisualEffect(visualEffectContainer);
    };
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    auto outImage = geRender->ApplyImageEffect(
        canvas, *visualEffectContainer, image, src, src, Drawing::SamplingOptions());
    auto brush = GetBrush();
    std::shared_ptr<RSShaderFilter> kawaseShaderFilter =
        GetShaderFilterWithType(RSShaderFilter::KAWASE);
    if (kawaseShaderFilter != nullptr) {
        auto tmpFilter = std::static_pointer_cast<RSKawaseBlurShaderFilter>(kawaseShaderFilter);
        auto radius = tmpFilter->GetRadius();

        static constexpr float epsilon = 0.999f;
        if (ROSEN_LE(radius, epsilon)) {
            ApplyColorFilter(canvas, outImage, src, dst);
            return;
        }

        if (RSSystemProperties::GetHpsBlurEnabled() && GetFilterType() == RSFilter::MATERIAL &&
            canvas.DrawBlurImage(*outImage, Drawing::HpsBlurParameter(
                src, dst, radius, saturationForHPS_, brightnessForHPS_))) {
            RS_OPTIONAL_TRACE_NAME("ApplyHPSBlur " + std::to_string(radius));
            return;
        } else {
            auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
            tmpFilter->GenerateGEVisualEffect(visualEffectContainer);
            auto blurImage = geRender->ApplyImageEffect(
                canvas, *visualEffectContainer, outImage, src, src, Drawing::SamplingOptions());
            canvas.AttachBrush(brush);
            canvas.DrawImageRect(*blurImage, src, dst, Drawing::SamplingOptions());
            canvas.DetachBrush();
            return;
        }
    }
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*outImage, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

void RSDrawingFilter::PreProcess(std::shared_ptr<Drawing::Image>& image)
{
    std::for_each(shaderFilters_.begin(), shaderFilters_.end(), [&](auto& filter) {
        filter->PreProcess(image);
    });
}

void RSDrawingFilter::PostProcess(Drawing::Canvas& canvas)
{
    std::for_each(shaderFilters_.begin(), shaderFilters_.end(), [&](auto& filter) {
        filter->PostProcess(canvas);
    });
}
} // namespace Rosen
} // namespace OHOS