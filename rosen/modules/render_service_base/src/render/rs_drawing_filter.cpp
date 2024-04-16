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
#include "draw/blend_mode.h"
#include "ge_render.h"
#include "ge_visual_effect.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int SHADER_FILTERS_SIZE = 10;
}
RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter)
    : RSFilter(), imageFilter_(imageFilter)
{
    hash_ = SkOpts::hash(&imageFilter_, sizeof(imageFilter_), 0);
    shaderFilters_.reserve(SHADER_FILTERS_SIZE);
}

RSDrawingFilter::RSDrawingFilter(std::shared_ptr<RSShaderFilter> shaderFilter)
{
    hash_ = shaderFilter->Hash();
    shaderFilters_.reserve(SHADER_FILTERS_SIZE);
    shaderFilters_.emplace_back(shaderFilter);
}

RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
    std::shared_ptr<RSShaderFilter> shaderFilter) : RSFilter(), imageFilter_(imageFilter)
{
    hash_ = shaderFilter->Hash();
    hash_ = SkOpts::hash(&imageFilter_, sizeof(imageFilter_), hash_);
    shaderFilters_.reserve(SHADER_FILTERS_SIZE);
    shaderFilters_.emplace_back(shaderFilter);
}

RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters)
    : RSFilter(), imageFilter_(imageFilter)
{
    shaderFilters_ = shaderFilters;
    hash_ = SkOpts::hash(&imageFilter_, sizeof(imageFilter_), hash_);
    hash_ = SkOpts::hash(&shaderFilters_, sizeof(shaderFilters_), hash_);
}

RSDrawingFilter::~RSDrawingFilter() {};

Drawing::Brush RSDrawingFilter::GetBrush() const
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetImageFilter(imageFilter_);
    brush.SetFilter(filter);
    return brush;
}

bool RSDrawingFilter::CanSkipFrame(float radius) const
{
    constexpr float HEAVY_BLUR_THRESHOLD = 25.0f;
    return radius > HEAVY_BLUR_THRESHOLD;
}

std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(const std::shared_ptr<RSDrawingFilter> other) const
{
    std::shared_ptr<RSDrawingFilter> result = std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
    for (auto item : other->GetShaderFilters()) {
        result->InsertShaderFilter(item);
    }
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}

std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(const std::shared_ptr<RSShaderFilter> other) const
{
    std::shared_ptr<RSDrawingFilter> result = std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->InsertShaderFilter(other);
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}

std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(const std::shared_ptr<Drawing::ImageFilter> other) const
{
    std::shared_ptr<RSDrawingFilter> result = std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other);
    result->hash_ = SkOpts::hash(&other, sizeof(other), hash_);
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

void RSDrawingFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    std::for_each(shaderFilters_.begin(), shaderFilters_.end(), [&](auto& filter) {
        filter->GenerateGEVisualEffect(visualEffectContainer);
    });
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    auto outImage = geRender->ApplyImageEffect(
        canvas, *visualEffectContainer, image, src, src, Drawing::SamplingOptions());
    auto brush = GetBrush();
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*outImage, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

void RSDrawingFilter::CaclMaskColor(std::shared_ptr<Drawing::Image>& image)
{
    if (colorMode_ == AVERAGE && image != nullptr) {
        // update maskColor while persevere alpha
        auto colorPicker = RSPropertiesPainter::CalcAverageColor(image);
        maskColor_ = RSColor(Drawing::Color::ColorQuadGetR(colorPicker), Drawing::Color::ColorQuadGetG(colorPicker),
            Drawing::Color::ColorQuadGetB(colorPicker), maskColor_.GetAlpha());
    }
    if (colorMode_ == FASTAVERAGE && RSColorPickerCacheTask::ColorPickerPartialEnabled &&
        image != nullptr) {
        RSColor color;
        if (colorPickerTask_->GetWaitRelease()) {
            if (colorPickerTask_->GetColor(color) && colorPickerTask_->GetFirstGetColorFinished()) {
                maskColor_ = RSColor(color.GetRed(), color.GetGreen(), color.GetBlue(), maskColor_.GetAlpha());
            }
            return;
        }
        if (RSColorPickerCacheTask::PostPartialColorPickerTask(colorPickerTask_, image)) {
            if (colorPickerTask_->GetColor(color)) {
                maskColor_ = RSColor(color.GetRed(), color.GetGreen(), color.GetBlue(), maskColor_.GetAlpha());
            }
            colorPickerTask_->SetStatus(CacheProcessStatus::WAITING);
        }
    }
}

void RSDrawingFilter::PreProcess(std::shared_ptr<Drawing::Image>& image)
{
    if (needMaskColor_) {
        CaclMaskColor(image);
    }
}

void RSDrawingFilter::PostProcess(RSPaintFilterCanvas& canvas)
{
    if (needMaskColor_) {
        Drawing::Brush brush;
        brush.SetColor(maskColor_.AsArgbInt());
        canvas.DrawBackground(brush);
    }
}

const std::shared_ptr<RSColorPickerCacheTask>& RSDrawingFilter::GetColorPickerCacheTask() const
{
    return colorPickerTask_;
}

void RSDrawingFilter::ReleaseColorPickerFilter()
{
    if (colorPickerTask_ == nullptr) {
        return;
    }
    colorPickerTask_->ReleaseColorPicker();
}
} // namespace Rosen
} // namespace OHOS
