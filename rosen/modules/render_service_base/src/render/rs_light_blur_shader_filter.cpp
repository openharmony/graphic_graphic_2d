/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "draw/surface.h"
#include "platform/common/rs_log.h"
#include "render/rs_light_blur_shader_filter.h"
namespace OHOS {
namespace Rosen {
namespace {
const int MAX_DOWN_SAMPLE_NUM = 12;
const int DOWN_SAMPLE_BOUNDARY_PIXEL_SIZE = 100;
const int DOWN_SAMPLE_STEP_4 = 4;
const int DOWN_SAMPLE_STEP_2 = 2;
const int MIX_COLOR_FACTOR = 3;
const int COLOR_MAX = 255;
const Drawing::SamplingOptions SMAPLING_OPTIONS(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
const float TWO_FRAME_BEFORE_COLOR_WEIGHT = 0.6f;
const float ONE_FRAME_BEFORE_COLOR_WEIGHT = 0.9f;
const float CUR_FRAME_BEFORE_COLOR_WEIGHT = 1.5f;
const float COLOR_THRESHOLD = 5.f;
}

RSLightBlurShaderFilter::RSLightBlurShaderFilter(int radius) : radius_(radius)
{
    type_ = ShaderFilterType::LIGHT_BLUR;
}

int RSLightBlurShaderFilter::GetRadius() const
{
    return radius_;
}

Drawing::Rect RSLightBlurShaderFilter::GetDownSampleRect(int width, int height) const
{
    if (width + height > DOWN_SAMPLE_BOUNDARY_PIXEL_SIZE) {
        int downSampleWidth = width <= DOWN_SAMPLE_STEP_4 ? 1 : width / DOWN_SAMPLE_STEP_4;
        int downSampleHeight = height <= DOWN_SAMPLE_STEP_4 ? 1 : height / DOWN_SAMPLE_STEP_4;
        return Drawing::Rect(0, 0, downSampleWidth, downSampleHeight);
    }

    int downSampleWidth = width <= DOWN_SAMPLE_STEP_2 ? 1 : width / DOWN_SAMPLE_STEP_2;
    int downSampleHeight = height <= DOWN_SAMPLE_STEP_2 ? 1 : height / DOWN_SAMPLE_STEP_2;
    return Drawing::Rect(0, 0, downSampleWidth, downSampleHeight);
}

std::shared_ptr<Drawing::Image> RSLightBlurShaderFilter::GetDownSampleImage(Drawing::Image& srcImage,
    Drawing::Surface& surface, Drawing::Rect& src, Drawing::Rect& dst) const
{
    int dstWidth = dst.GetWidth();
    int dstHeight = dst.GetHeight();
    if (dstWidth <= 0 || dstHeight <= 0) {
        return nullptr;
    }

    auto offscreenSurface = surface.MakeSurface(dstWidth, dstHeight);
    if (offscreenSurface == nullptr) {
        return nullptr;
    }

    auto offscreenCanvas = offscreenSurface->GetCanvas();
    if (offscreenCanvas == nullptr) {
        return nullptr;
    }

    Drawing::Brush brush;
    offscreenCanvas->AttachBrush(brush);
    offscreenCanvas->DrawImageRect(srcImage, src, dst, SMAPLING_OPTIONS);
    offscreenCanvas->DetachBrush();
    return offscreenSurface->GetImageSnapshot();
}

RSColor RSLightBlurShaderFilter::MixColor(const RSColor& twoFrameBefore,
    const RSColor& oneFrameBefore, const RSColor& curColor) const
{
    int16_t alpha = (twoFrameBefore.GetAlpha() * TWO_FRAME_BEFORE_COLOR_WEIGHT +
        oneFrameBefore.GetAlpha() * ONE_FRAME_BEFORE_COLOR_WEIGHT +
        curColor.GetAlpha() * CUR_FRAME_BEFORE_COLOR_WEIGHT) / MIX_COLOR_FACTOR;
    int16_t red = (twoFrameBefore.GetRed() * TWO_FRAME_BEFORE_COLOR_WEIGHT +
        oneFrameBefore.GetRed() * ONE_FRAME_BEFORE_COLOR_WEIGHT +
        curColor.GetRed() * CUR_FRAME_BEFORE_COLOR_WEIGHT) / MIX_COLOR_FACTOR;
    int16_t green = (twoFrameBefore.GetGreen() * TWO_FRAME_BEFORE_COLOR_WEIGHT +
        oneFrameBefore.GetGreen() * ONE_FRAME_BEFORE_COLOR_WEIGHT +
        curColor.GetGreen() * CUR_FRAME_BEFORE_COLOR_WEIGHT) / MIX_COLOR_FACTOR;
    int16_t blue = (twoFrameBefore.GetBlue() * TWO_FRAME_BEFORE_COLOR_WEIGHT +
        oneFrameBefore.GetBlue() * ONE_FRAME_BEFORE_COLOR_WEIGHT +
        curColor.GetBlue() * CUR_FRAME_BEFORE_COLOR_WEIGHT) / MIX_COLOR_FACTOR;
    return RSColor(red, green, blue, alpha);
}

bool RSLightBlurShaderFilter::GetAverageColorFromImageAndCache(Drawing::Image& image, RSColor& color) const
{
    if (image.GetWidth() != 1 || image.GetHeight() != 1) {
        return false;
    }

    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format;
    format.alphaType = image.GetAlphaType();
    format.colorType = image.GetColorType();
    bitmap.Build(1, 1, format);
    if (!image.ReadPixels(bitmap, 0, 0)) {
        return false;
    }

    auto extraceColor = bitmap.GetColor(0, 0);
    color.SetAlpha(Drawing::Color::ColorQuadGetA(extraceColor));
    color.SetBlue(Drawing::Color::ColorQuadGetB(extraceColor));
    color.SetGreen(Drawing::Color::ColorQuadGetG(extraceColor));
    color.SetRed(Drawing::Color::ColorQuadGetR(extraceColor));

    if (lightBlurResultCache_ != nullptr) {
        const RSColor& colorTwoFrameBefore = lightBlurResultCache_->first;
        const RSColor& colorOneFrameBefore = lightBlurResultCache_->second;
        RSColor mixedColor = MixColor(colorTwoFrameBefore, colorOneFrameBefore, color);
        RSColor colorDist = mixedColor - colorOneFrameBefore;
        auto dist = colorDist.GetRed() * colorDist.GetRed() + colorDist.GetBlue() * colorDist.GetBlue() +
            colorDist.GetGreen() * colorDist.GetGreen() + colorDist.GetAlpha() * colorDist.GetAlpha();
        if (dist > COLOR_THRESHOLD) {
            auto red = static_cast<int>(COLOR_THRESHOLD * colorDist.GetRed() * std::abs(colorDist.GetRed()) / dist);
            auto green =
                static_cast<int>(COLOR_THRESHOLD * colorDist.GetGreen() * std::abs(colorDist.GetGreen()) / dist);
            auto blue = static_cast<int>(COLOR_THRESHOLD * colorDist.GetBlue() * std::abs(colorDist.GetBlue()) / dist);
            auto alpha =
                static_cast<int>(COLOR_THRESHOLD * colorDist.GetAlpha() * std::abs(colorDist.GetAlpha()) / dist);
            color.SetAlpha(std::clamp(colorOneFrameBefore.GetAlpha() + alpha, 0, COLOR_MAX));
            color.SetRed(std::clamp(colorOneFrameBefore.GetRed() + red, 0, COLOR_MAX));
            color.SetGreen(std::clamp(colorOneFrameBefore.GetGreen() + green, 0, COLOR_MAX));
            color.SetBlue(std::clamp(colorOneFrameBefore.GetBlue() + blue, 0, COLOR_MAX));
        } else {
            color = mixedColor;
        }
    }
    return true;
}

void RSLightBlurShaderFilter::ApplyLightBlur(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image, const LightBlurParameter& para)
{
    if (image == nullptr) {
        ROSEN_LOGE("LightBlurFilter apply light blur failed, image is nullptr");
        return;
    }

    auto width = image->GetWidth();
    auto height = image->GetHeight();
    if (width <= 0 || height <= 0) {
        ROSEN_LOGE("LightBlurFilter apply light blur failed, image width or height <= 0");
        return;
    }

    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("LightBlurFilter apply light blur failed, surface is nullptr");
        return;
    }

    auto curImage = image;
    for (int i = 0; i < MAX_DOWN_SAMPLE_NUM; ++i) {
        if (width == 1 && height == 1) {
            break;
        }
        Drawing::Rect srcRect(0, 0, width, height);
        Drawing::Rect dstRect = GetDownSampleRect(width, height);
        auto downsampleImage = GetDownSampleImage(*curImage, *surface, srcRect, dstRect);
        if (downsampleImage == nullptr) {
            break;
        }
        curImage = downsampleImage;
        width = dstRect.GetWidth();
        height = dstRect.GetHeight();
    }

    RSColor color;
    if (!GetAverageColorFromImageAndCache(*curImage, color)) {
        canvas.AttachBrush(para.brush);
        canvas.DrawImageRect(*curImage, para.dst, SMAPLING_OPTIONS);
        canvas.DetachBrush();
        return;
    }

    UpdateLightBlurResultCache(color);
    auto brush = para.brush;
    brush.SetColor(color.AsArgbInt());
    canvas.AttachBrush(brush);
    canvas.DrawRect(para.dst);
    canvas.DetachBrush();
}

void RSLightBlurShaderFilter::UpdateLightBlurResultCache(const RSColor& color)
{
    if (lightBlurResultCache_ == nullptr) {
        lightBlurResultCache_ = std::make_unique<std::pair<RSColor, RSColor>>(color, color);
        return;
    }
    lightBlurResultCache_->first = lightBlurResultCache_->second;
    lightBlurResultCache_->second = color;
}
} // namespace Rosen
} // namespace OHOS