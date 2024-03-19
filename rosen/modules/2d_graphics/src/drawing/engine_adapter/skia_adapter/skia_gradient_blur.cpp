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

#include<array>
#include<iostream>

#include "skia_gradient_blur.h"
#include "utils/log.h"
#include "effect/runtime_shader_builder.h"
#include "utils/gradient_blur_param.h"
#include "draw/surface.h"
#include "utils/kawase_blur_param.h"
#include "skia_adapter/skia_kawase_blur_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
constexpr static uint8_t DIRECTION_NUM = 4;
} // namespace

std::shared_ptr<Drawing::RuntimeEffect> SkiaGradientBlur::horizontalMeanBlurShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> SkiaGradientBlur::verticalMeanBlurShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> SkiaGradientBlur::maskBlurShaderEffect_ = nullptr;

void SkiaGradientBlur::DrawGradientBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst, Drawing::GradientBlurParam& param)
{
    auto clipIPadding = param.clipIPadding_;
    imageFilterBlurRadius_ = param.imageFilterBlurRadius_;
    ComputeScale(clipIPadding.GetWidth(), clipIPadding.GetHeight(), param);
    imageScale_ = param.imageScale_;
    auto scaledClipIPadding = Drawing::Rect(clipIPadding.GetLeft(), clipIPadding.GetTop(), clipIPadding.GetLeft() +
        clipIPadding.GetWidth() * imageScale_, clipIPadding.GetTop() + clipIPadding.GetHeight() * imageScale_);
    auto alphaGradientShader = MakeAlphaGradientShader(scaledClipIPadding, param);
    if (alphaGradientShader == nullptr) {
        LOGE("SkiaGradientBlur::DrawGradientBlur alphaGradientShader null");
        return;
    }
    if (param.maskLinearBlurEnable_ && param.useMaskAlgorithm_) {
        // use faster LinearGradientBlur if valid
        if (param.blurImageFilter_ == nullptr) {
            LOGE("SkiaGradientBlur::DrawLinearGradientBlur blurFilter null");
            return;
        }
        auto filter = param.blurImageFilter_;
        DrawMaskLinearGradientBlur(image, canvas, filter, alphaGradientShader, dst);
    } else {
        // use original LinearGradientBlur
        float radius = param.blurRadius_ - param.originalBase_;
        radius = std::clamp(radius, 0.0f, 60.0f);  // 60.0 represents largest blur radius
        radius = radius / 2 * imageScale_;         // 2 half blur radius
        MakeHorizontalMeanBlurEffect();
        MakeVerticalMeanBlurEffect();
        DrawMeanLinearGradientBlur(image, canvas, radius, alphaGradientShader, dst);
    }
}

void SkiaGradientBlur::ComputeScale(float width, float height, Drawing::GradientBlurParam& param)
{
    if (param.maskLinearBlurEnable_ && param.useMaskAlgorithm_) {
        param.imageScale_ = 1.f;
    } else {
        if (width * height < 10000) {   // 10000 for 100 * 100 resolution
            param.imageScale_ = 0.7f;         // 0.7 for scale
        } else {
            param.imageScale_ = 0.5f;         // 0.5 for scale
        }
    }
}

void SkiaGradientBlur::TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias)
{
    if (direction == static_cast<uint8_t>(Drawing::GradientDir::LEFT_BOTTOM)) {
        direction += 2; // 2 is used to transtorm diagnal direction.
    } else if (direction == static_cast<uint8_t>(Drawing::GradientDir::RIGHT_TOP) ||
                    direction == static_cast<uint8_t>(Drawing::GradientDir::RIGHT_BOTTOM)) {
        direction -= 1; // 1 is used to transtorm diagnal direction.
    }
    if (direction <= static_cast<uint8_t>(Drawing::GradientDir::BOTTOM)) {
        if (direction < directionBias) {
            direction += DIRECTION_NUM;
        }
        direction -= directionBias;
    } else {
        direction -= DIRECTION_NUM;
        if (direction < directionBias) {
            direction += DIRECTION_NUM;
        }
        direction -= directionBias;
        direction += DIRECTION_NUM;
    }
    if (direction == static_cast<uint8_t>(Drawing::GradientDir::RIGHT_BOTTOM)) {
        direction -= 2; // 2 is used to restore diagnal direction.
    } else if (direction == static_cast<uint8_t>(Drawing::GradientDir::LEFT_BOTTOM) ||
                        direction == static_cast<uint8_t>(Drawing::GradientDir::RIGHT_TOP)) {
        direction += 1; // 1 is used to restore diagnal direction.
    }
}

bool SkiaGradientBlur::GetGradientDirectionPoints(
    Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, Drawing::GradientDir direction)
{
    switch (direction) {
        case Drawing::GradientDir::BOTTOM: {
            pts[0].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetTop()); // 2 middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetBottom()); // 2  middle of width;
            break;
        }
        case Drawing::GradientDir::TOP: {
            pts[0].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetBottom()); // 2  middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetTop()); // 2  middle of width;
            break;
        }
        case Drawing::GradientDir::RIGHT: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            pts[1].Set(clipBounds.GetRight(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            break;
        }
        case Drawing::GradientDir::LEFT: {
            pts[0].Set(clipBounds.GetRight(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            break;
        }
        case Drawing::GradientDir::RIGHT_BOTTOM: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetTop());
            pts[1].Set(clipBounds.GetRight(), clipBounds.GetBottom());
            break;
        }
        case Drawing::GradientDir::LEFT_TOP: {
            pts[0].Set(clipBounds.GetRight(), clipBounds.GetBottom());
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetTop());
            break;
        }
        case Drawing::GradientDir::LEFT_BOTTOM: {
            pts[0].Set(clipBounds.GetRight(), clipBounds.GetTop());
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetBottom());
            break;
        }
        case Drawing::GradientDir::RIGHT_TOP: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetBottom());
            pts[1].Set(clipBounds.GetRight(), clipBounds.GetTop());
            break;
        }
        default: {
            return false;
        }
    }
    return true;
}

std::shared_ptr<Drawing::ShaderEffect> SkiaGradientBlur::MakeAlphaGradientShader(
    const Drawing::Rect& clipBounds, Drawing::GradientBlurParam& param)
{
    std::vector<Drawing::ColorQuad> c;
    std::vector<Drawing::scalar> p;
    Drawing::Point pts[2];

    uint8_t direction = static_cast<uint8_t>(param.direction_);
    if (param.directionBias_ != 0) {
        TransformGradientBlurDirection(direction, param.directionBias_);
    }
    bool result = GetGradientDirectionPoints(pts, clipBounds, static_cast<Drawing::GradientDir>(direction));
    if (!result) {
        return nullptr;
    }
    uint8_t ColorMax = 255; // 255: color max
    uint8_t ColorMin = 0;
    if (param.fractionStops_[0].second > 0.01) {  // 0.01 represents the fraction bias
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        p.emplace_back(param.fractionStops_[0].second - 0.01); // 0.01 represents the fraction bias
    }
    for (size_t i = 0; i < param.fractionStops_.size(); i++) {
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(
            static_cast<uint8_t>(param.fractionStops_[i].first * ColorMax), ColorMax, ColorMax, ColorMax));
        p.emplace_back(param.fractionStops_[i].second);
    }
    // 0.01 represents the fraction bias
    if (param.fractionStops_[param.fractionStops_.size() - 1].second < (1 - 0.01)) {
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        // 0.01 represents the fraction bias
        p.emplace_back(param.fractionStops_[param.fractionStops_.size() - 1].second + 0.01);
    }
    return Drawing::ShaderEffect::CreateLinearGradient(pts[0], pts[1], c, p, Drawing::TileMode::CLAMP);
}

void SkiaGradientBlur::DrawMaskLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image,
    Drawing::Canvas& canvas, std::shared_ptr<Drawing::ImageFilter>& blurFilter,
    std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("SkiaGradientBlur::DrawMaskLinearGradientBlur image is null");
        return;
    }

    auto imageInfo = image->GetImageInfo();
    auto srcRect = Drawing::Rect(0, 0, imageInfo.GetWidth(), imageInfo.GetHeight());
    auto blurParam = Drawing::KawaseParameters{srcRect, dst, imageFilterBlurRadius_, nullptr, 1.0f};
    Drawing::KawaseProperties properties;
    SkiaKawaseBlurFilter::GetKawaseBlurFilter()->ApplyKawaseBlur(canvas, image, blurParam, properties);

    Drawing::Matrix inputMatrix;
    inputMatrix.Translate(dst.GetLeft(), dst.GetTop());
    inputMatrix.PostScale(dst.GetWidth() / imageInfo.GetWidth(), dst.GetHeight() / imageInfo.GetHeight());

    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), inputMatrix);
    auto shader = MakeMaskLinearGradientBlurShader(srcImageShader, alphaGradientShader);

    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::ShaderEffect> SkiaGradientBlur::MakeMaskLinearGradientBlurShader(
    std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> gradientShader)
{
    static const char* prog = R"(
        uniform shader srcImageShader;
        uniform shader gradientShader;
        half4 main(float2 coord)
        {
            float gradient = 1 - gradientShader.eval(coord).a;
            return half4(srcImageShader.eval(coord).rgb * gradient, gradient);
        }
    )";

    if (maskBlurShaderEffect_ == nullptr) {
        maskBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (maskBlurShaderEffect_ == nullptr) {
            return nullptr;
        }
    }

    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(maskBlurShaderEffect_);
    builder->SetChild("srcImageShader", srcImageShader);
    builder->SetChild("gradientShader", gradientShader);
    return builder->MakeShader(nullptr, false);
}

void SkiaGradientBlur::MakeHorizontalMeanBlurEffect()
{
    static const std::string HorizontalBlurString(
        R"(
        uniform half r;
        uniform shader imageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord, half radius)
        {
            half4 sum = vec4(0.0);
            half div = 0;
            for (half x = -30.0; x < 30.0; x += 1.0) {
                if (x > radius) {
                    break;
                }
                if (abs(x) < radius) {
                    div += 1;
                    sum += imageShader.eval(coord + float2(x, 0));
                }
            }
            return half4(sum.xyz / div, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a - 0) < 0.001) {
                return imageShader.eval(coord);
            }
            float val = clamp(r * gradientShader.eval(coord).a, 1.0, r);
            return meanFilter(coord, val);
        }
    )");

    if (horizontalMeanBlurShaderEffect_ == nullptr) {
        horizontalMeanBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(HorizontalBlurString);
    }
}

void SkiaGradientBlur::MakeVerticalMeanBlurEffect()
{
    static const std::string VerticalBlurString(
        R"(
        uniform half r;
        uniform shader imageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord, half radius)
        {
            half4 sum = vec4(0.0);
            half div = 0;
            for (half y = -30.0; y < 30.0; y += 1.0) {
                if (y > radius) {
                    break;
                }
                if (abs(y) < radius) {
                    div += 1;
                    sum += imageShader.eval(coord + float2(0, y));
                }
            }
            return half4(sum.xyz / div, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a - 0) < 0.001) {
                return imageShader.eval(coord);
            }
            float val = clamp(r * gradientShader.eval(coord).a, 1.0, r);
            return meanFilter(coord, val);
        }
    )");

    if (verticalMeanBlurShaderEffect_ == nullptr) {
        verticalMeanBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(VerticalBlurString);
    }
}

void SkiaGradientBlur::DrawMeanLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image,
    Drawing::Canvas& canvas, float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
    const Drawing::Rect& dst)
{
    if (!horizontalMeanBlurShaderEffect_ || !verticalMeanBlurShaderEffect_ || !image) {
        LOGE("SkiaGradientBlur::DrawMeanLinearGradientBlur shader error.");
        return;
    }

    Drawing::Matrix m;
    Drawing::Matrix blurMatrix;
    blurMatrix.PostScale(imageScale_, imageScale_);
    blurMatrix.PostTranslate(dst.GetLeft(), dst.GetTop());

    auto width = image->GetWidth();
    auto height = image->GetHeight();
    auto originImageInfo = image->GetImageInfo();
    auto scaledInfo = Drawing::ImageInfo(std::ceil(width * imageScale_), std::ceil(height * imageScale_),
        originImageInfo.GetColorType(), originImageInfo.GetAlphaType(), originImageInfo.GetColorSpace());
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);

    Drawing::RuntimeShaderBuilder hBlurBuilder(horizontalMeanBlurShaderEffect_);
    hBlurBuilder.SetUniform("r", radius);
    hBlurBuilder.SetChild("imageShader", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, blurMatrix));
    hBlurBuilder.SetChild("gradientShader", alphaGradientShader);
    std::shared_ptr<Drawing::Image> tmpBlur(hBlurBuilder.MakeImage(
        canvas.GetGPUContext().get(), nullptr, scaledInfo, false));

    Drawing::RuntimeShaderBuilder vBlurBuilder(verticalMeanBlurShaderEffect_);
    vBlurBuilder.SetUniform("r", radius);
    vBlurBuilder.SetChild("imageShader", Drawing::ShaderEffect::CreateImageShader(*tmpBlur, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, m));
    vBlurBuilder.SetChild("gradientShader", alphaGradientShader);
    std::shared_ptr<Drawing::Image> tmpBlur2(vBlurBuilder.MakeImage(
        canvas.GetGPUContext().get(), nullptr, scaledInfo, false));

    hBlurBuilder.SetChild("imageShader", Drawing::ShaderEffect::CreateImageShader(*tmpBlur2, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, m));
    std::shared_ptr<Drawing::Image> tmpBlur3(hBlurBuilder.MakeImage(
        canvas.GetGPUContext().get(), nullptr, scaledInfo, false));

    vBlurBuilder.SetChild("imageShader", Drawing::ShaderEffect::CreateImageShader(*tmpBlur3, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, m));
    std::shared_ptr<Drawing::Image> tmpBlur4(vBlurBuilder.MakeImage(
        canvas.GetGPUContext().get(), nullptr, scaledInfo, false));

    float invBlurScale = 1.0f / imageScale_;
    Drawing::Matrix invBlurMatrix;
    invBlurMatrix.PostScale(invBlurScale, invBlurScale);
    auto blurShader = Drawing::ShaderEffect::CreateImageShader(*tmpBlur4, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, invBlurMatrix);

    Drawing::Brush brush;
    brush.SetShaderEffect(blurShader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
