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
#include "render/rs_linear_gradient_blur_filter.h"
#include "src/core/SkOpts.h"
#include "platform/common/rs_log.h"
#include "common/rs_optional_trace.h"

#ifndef USE_ROSEN_DRAWING
#else
    #include "draw/surface.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr static uint8_t DIRECTION_NUM = 4;
} // namespace

#ifndef USE_ROSEN_DRAWING
sk_sp<SkRuntimeEffect> horizontalMeanBlurShaderEffect_ = nullptr;
sk_sp<SkRuntimeEffect> verticalMeanBlurShaderEffect_ = nullptr;
sk_sp<SkRuntimeEffect> maskBlurShaderEffect_ = nullptr;
#else
std::shared_ptr<Drawing::RuntimeEffect> RSLinearGradientBlurFilter::horizontalMeanBlurShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSLinearGradientBlurFilter::verticalMeanBlurShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSLinearGradientBlurFilter::maskBlurShaderEffect_ = nullptr;
#endif

RSLinearGradientBlurFilter::RSLinearGradientBlurFilter(const std::shared_ptr<RSLinearGradientBlurPara>& para)
#ifndef USE_ROSEN_DRAWING
    : RSSkiaFilter(nullptr),
#else
    : RSDrawingFilter(nullptr),
#endif
      linearGradientBlurPara_(para)
{
    type_ = FilterType::LINEAR_GRADIENT_BLUR;
    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&linearGradientBlurPara_, sizeof(linearGradientBlurPara_), hash_);
}

RSLinearGradientBlurFilter::~RSLinearGradientBlurFilter() = default;

#ifndef USE_ROSEN_DRAWING
void RSLinearGradientBlurFilter::DrawImageRect(
    SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const
#else
void RSLinearGradientBlurFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
#endif
{
    auto& para = linearGradientBlurPara_;
    if (!image || para == nullptr || para->blurRadius_ <= 0) {
        return;
    }

    RS_OPTIONAL_TRACE_NAME("DrawLinearGradientBlur");
    auto clipIPadding = dst;
#ifndef USE_ROSEN_DRAWING
    ComputeScale(clipIPadding.width(), clipIPadding.height(), para->useMaskAlgorithm_);
    auto scaledClipIPadding = SkRect::MakeLTRB(clipIPadding.GetLeft(), clipIPadding.GetTop(), clipIPadding.GetLeft() +
        clipIPadding.GetWidth() * imageScale_, clipIPadding.GetTop() + clipIPadding.GetHeight() * imageScale_);
    auto alphaGradientShader = MakeAlphaGradientShader(SkRect::Make(scaledClipIPadding), para, directionBias_);
#else
    ComputeScale(clipIPadding.GetWidth(), clipIPadding.GetHeight(), para->useMaskAlgorithm_);
    auto scaledClipIPadding = Drawing::Rect(clipIPadding.GetLeft(), clipIPadding.GetTop(), clipIPadding.GetLeft() +
        clipIPadding.GetWidth() * imageScale_, clipIPadding.GetTop() + clipIPadding.GetHeight() * imageScale_);
    auto alphaGradientShader = MakeAlphaGradientShader(scaledClipIPadding, para, directionBias_);
#endif
    if (alphaGradientShader == nullptr) {
        ROSEN_LOGE("RSLinearGradientBlurFilter::DrawImageRect alphaGradientShader null");
        return;
    }

    if (RSSystemProperties::GetMaskLinearBlurEnabled() && para->useMaskAlgorithm_) {
        // use faster LinearGradientBlur if valid
        RS_OPTIONAL_TRACE_NAME("LinearGradientBlur_mask");
        if (para->LinearGradientBlurFilter_ == nullptr) {
            ROSEN_LOGE("RSPropertiesPainter::DrawLinearGradientBlur blurFilter null");
            return;
        }
        auto& RSFilter = para->LinearGradientBlurFilter_;
#ifndef USE_ROSEN_DRAWING
        auto filter = std::static_pointer_cast<RSSkiaFilter>(RSFilter);
#else
        auto filter = std::static_pointer_cast<RSDrawingFilter>(RSFilter);
#endif
        DrawMaskLinearGradientBlur(image, canvas, filter, alphaGradientShader, dst);
    } else {
        // use original LinearGradientBlur
        RS_OPTIONAL_TRACE_NAME("LinearGradientBlur_radius");
        float radius = para->blurRadius_ - para->originalBase_;
        radius = std::clamp(radius, 0.0f, 60.0f);  // 60.0 represents largest blur radius
        radius = radius / 2 * imageScale_;         // 2 half blur radius
        MakeHorizontalMeanBlurEffect();
        MakeVerticalMeanBlurEffect();
        DrawMeanLinearGradientBlur(image, canvas, radius, alphaGradientShader, dst);
    }
}

void RSLinearGradientBlurFilter::ComputeScale(float width, float height, bool useMaskAlgorithm)
{
    if (RSSystemProperties::GetMaskLinearBlurEnabled() && useMaskAlgorithm) {
        useMaskLinearGradientBlur_ = true;
        imageScale_ = 1.f;
    } else {
        useMaskLinearGradientBlur_ = false;
        if (width * height < 10000) {   // 10000 for 100 * 100 resolution
            imageScale_ = 0.7f;         // 0.7 for scale
        } else {
            imageScale_ = 0.5f;         // 0.5 for scale
        }
    }
}

void RSLinearGradientBlurFilter::TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias)
{
    if (direction == static_cast<uint8_t>(GradientDirection::LEFT_BOTTOM)) {
        direction += 2; // 2 is used to transtorm diagnal direction.
    } else if (direction == static_cast<uint8_t>(GradientDirection::RIGHT_TOP) ||
                    direction == static_cast<uint8_t>(GradientDirection::RIGHT_BOTTOM)) {
        direction -= 1; // 1 is used to transtorm diagnal direction.
    }
    if (direction <= static_cast<uint8_t>(GradientDirection::BOTTOM)) {
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
    if (direction == static_cast<uint8_t>(GradientDirection::RIGHT_BOTTOM)) {
        direction -= 2; // 2 is used to restore diagnal direction.
    } else if (direction == static_cast<uint8_t>(GradientDirection::LEFT_BOTTOM) ||
                        direction == static_cast<uint8_t>(GradientDirection::RIGHT_TOP)) {
        direction += 1; // 1 is used to restore diagnal direction.
    }
}

#ifndef USE_ROSEN_DRAWING
bool RSLinearGradientBlurFilter::GetGradientDirectionPoints(
    SkPoint (&pts)[2], const SkRect& clipBounds, GradientDirection direction)
{
    switch (direction) {
        case GradientDirection::BOTTOM: {
            pts[0].set(clipBounds.width() / 2 + clipBounds.left(), clipBounds.top()); // 2  middle of width;
            pts[1].set(clipBounds.width() / 2 + clipBounds.left(), clipBounds.bottom()); // 2  middle of width;
            break;
        }
        case GradientDirection::TOP: {
            pts[0].set(clipBounds.width() / 2 + clipBounds.left(), clipBounds.bottom()); // 2  middle of width;
            pts[1].set(clipBounds.width() / 2 + clipBounds.left(), clipBounds.top()); // 2  middle of width;
            break;
        }
        case GradientDirection::RIGHT: {
            pts[0].set(clipBounds.left(), clipBounds.height() / 2 + clipBounds.top()); // 2  middle of height;
            pts[1].set(clipBounds.right(), clipBounds.height() / 2 + clipBounds.top()); // 2  middle of height;
            break;
        }
        case GradientDirection::LEFT: {
            pts[0].set(clipBounds.right(), clipBounds.height() / 2 + clipBounds.top()); // 2  middle of height;
            pts[1].set(clipBounds.left(), clipBounds.height() / 2 + clipBounds.top()); // 2  middle of height;
            break;
        }
        case GradientDirection::RIGHT_BOTTOM: {
            pts[0].set(clipBounds.left(), clipBounds.top());
            pts[1].set(clipBounds.right(), clipBounds.bottom());
            break;
        }
        case GradientDirection::LEFT_TOP: {
            pts[0].set(clipBounds.right(), clipBounds.bottom());
            pts[1].set(clipBounds.left(), clipBounds.top());
            break;
        }
        case GradientDirection::LEFT_BOTTOM: {
            pts[0].set(clipBounds.right(), clipBounds.top());
            pts[1].set(clipBounds.left(), clipBounds.bottom());
            break;
        }
        case GradientDirection::RIGHT_TOP: {
            pts[0].set(clipBounds.left(), clipBounds.bottom());
            pts[1].set(clipBounds.right(), clipBounds.top());
            break;
        }
        default: {
            return false;
        }
    }
    return true;
}
#else
bool RSLinearGradientBlurFilter::GetGradientDirectionPoints(
    Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GradientDirection direction)
{
    switch (direction) {
        case GradientDirection::BOTTOM: {
            pts[0].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetTop()); // 2 middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetBottom()); // 2  middle of width;
            break;
        }
        case GradientDirection::TOP: {
            pts[0].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetBottom()); // 2  middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetTop()); // 2  middle of width;
            break;
        }
        case GradientDirection::RIGHT: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            pts[1].Set(clipBounds.GetRight(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            break;
        }
        case GradientDirection::LEFT: {
            pts[0].Set(clipBounds.GetRight(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            break;
        }
        case GradientDirection::RIGHT_BOTTOM: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetTop());
            pts[1].Set(clipBounds.GetRight(), clipBounds.GetBottom());
            break;
        }
        case GradientDirection::LEFT_TOP: {
            pts[0].Set(clipBounds.GetRight(), clipBounds.GetBottom());
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetTop());
            break;
        }
        case GradientDirection::LEFT_BOTTOM: {
            pts[0].Set(clipBounds.GetRight(), clipBounds.GetTop());
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetBottom());
            break;
        }
        case GradientDirection::RIGHT_TOP: {
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
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSLinearGradientBlurFilter::MakeAlphaGradientShader(
    const SkRect& clipBounds, const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias)
{
    std::vector<SkColor> c;
    std::vector<SkScalar> p;
    SkPoint pts[2];

    uint8_t direction = static_cast<uint8_t>(para->direction_);
    if (directionBias != 0) {
        TransformGradientBlurDirection(direction, directionBias);
    }
    bool result = GetGradientDirectionPoints(pts, clipBounds, static_cast<GradientDirection>(direction));
    if (!result) {
        return nullptr;
    }
    uint8_t ColorMax = 255;
    uint8_t ColorMin = 0;
    if (para->fractionStops_[0].second > 0.01) {  // 0.01 represents the fraction bias
        c.emplace_back(SkColorSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        p.emplace_back(para->fractionStops_[0].second - 0.01); // 0.01 represents the fraction bias
    }
    for (size_t i = 0; i < para->fractionStops_.size(); i++) {
        c.emplace_back(SkColorSetARGB(
            static_cast<uint8_t>(para->fractionStops_[i].first * ColorMax), ColorMax, ColorMax, ColorMax));
        p.emplace_back(para->fractionStops_[i].second);
    }
    // 0.01 represents the fraction bias
    if (para->fractionStops_[para->fractionStops_.size() - 1].second < (1 - 0.01)) {
        c.emplace_back(SkColorSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        // 0.01 represents the fraction bias
        p.emplace_back(para->fractionStops_[para->fractionStops_.size() - 1].second + 0.01);
    }
    auto shader = SkGradientShader::MakeLinear(pts, &c[0], &p[0], p.size(), SkTileMode::kClamp);
    return shader;
}
#else
std::shared_ptr<Drawing::ShaderEffect> RSLinearGradientBlurFilter::MakeAlphaGradientShader(
    const Drawing::Rect& clipBounds, const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias)
{
    std::vector<Drawing::ColorQuad> c;
    std::vector<Drawing::scalar> p;
    Drawing::Point pts[2];

    uint8_t direction = static_cast<uint8_t>(para->direction_);
    if (directionBias != 0) {
        TransformGradientBlurDirection(direction, directionBias);
    }
    bool result = GetGradientDirectionPoints(pts, clipBounds, static_cast<GradientDirection>(direction));
    if (!result) {
        return nullptr;
    }
    uint8_t ColorMax = 255;
    uint8_t ColorMin = 0;
    if (para->fractionStops_[0].second > 0.01) {  // 0.01 represents the fraction bias
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        p.emplace_back(para->fractionStops_[0].second - 0.01); // 0.01 represents the fraction bias
    }
    for (size_t i = 0; i < para->fractionStops_.size(); i++) {
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(
            static_cast<uint8_t>(para->fractionStops_[i].first * ColorMax), ColorMax, ColorMax, ColorMax));
        p.emplace_back(para->fractionStops_[i].second);
    }
    // 0.01 represents the fraction bias
    if (para->fractionStops_[para->fractionStops_.size() - 1].second < (1 - 0.01)) {
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        // 0.01 represents the fraction bias
        p.emplace_back(para->fractionStops_[para->fractionStops_.size() - 1].second + 0.01);
    }
    return Drawing::ShaderEffect::CreateLinearGradient(pts[0], pts[1], c, p, Drawing::TileMode::CLAMP);
}
#endif

void RSLinearGradientBlurFilter::MakeHorizontalMeanBlurEffect()
{
#ifndef USE_ROSEN_DRAWING
    static const SkString HorizontalBlurString(
#else
    static const std::string HorizontalBlurString(
#endif
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

#ifndef USE_ROSEN_DRAWING
    if (horizontalMeanBlurShaderEffect_ == nullptr) {
        auto [horizontalMeanBlurShaderEffect, error] = SkRuntimeEffect::MakeForShader(HorizontalBlurString);
        if (!horizontalMeanBlurShaderEffect) {
            ROSEN_LOGE("RSLinearGradientBlurFilter::RuntimeShader horizontalEffect error: %{public}s\n", error.c_str());
            return;
        }
        horizontalMeanBlurShaderEffect_ = std::move(horizontalMeanBlurShaderEffect);
    }
#else
    if (horizontalMeanBlurShaderEffect_ == nullptr) {
        horizontalMeanBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(HorizontalBlurString);
    }
#endif
}

void RSLinearGradientBlurFilter::MakeVerticalMeanBlurEffect()
{
#ifndef USE_ROSEN_DRAWING
    static const SkString VerticalBlurString(
#else
    static const std::string VerticalBlurString(
#endif
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

#ifndef USE_ROSEN_DRAWING
    if (verticalMeanBlurShaderEffect_ == nullptr) {
        auto [verticalMeanBlurShaderEffect, error2] = SkRuntimeEffect::MakeForShader(VerticalBlurString);
        if (!verticalMeanBlurShaderEffect) {
            ROSEN_LOGE("KawaseBlurFilter::RuntimeShader verticalEffect error: %s{public}\n", error2.c_str());
            return;
        }
        verticalMeanBlurShaderEffect_ = std::move(verticalMeanBlurShaderEffect);
    }
#else
    if (verticalMeanBlurShaderEffect_ == nullptr) {
        verticalMeanBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(VerticalBlurString);
    }
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSLinearGradientBlurFilter::DrawMeanLinearGradientBlur(const sk_sp<SkImage>& image, SkCanvas& canvas,
    float radius, sk_sp<SkShader> alphaGradientShader, const SkRect& dst)
{
    if (!horizontalMeanBlurShaderEffect_ || !verticalMeanBlurShaderEffect_ || !image) {
        ROSEN_LOGE("RSLinearGradientBlurFilter::shader error.");
        return;
    }

    SkMatrix blurMatrix = SkMatrix::Scale(imageScale_, imageScale_);
    auto width = image->width();
    auto height = image->height();
    SkImageInfo scaledInfo = image->imageInfo().makeWH(std::ceil(width * imageScale_), std::ceil(height * imageScale_));

    SkRuntimeShaderBuilder hBlurBuilder(horizontalMeanBlurShaderEffect_);
    hBlurBuilder.uniform("r") = radius;
    hBlurBuilder.child("imageShader") = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear), blurMatrix);
    hBlurBuilder.child("gradientShader") = alphaGradientShader;
    sk_sp<SkImage> tmpBlur(hBlurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false));

    SkRuntimeShaderBuilder vBlurBuilder(verticalMeanBlurShaderEffect_);
    vBlurBuilder.uniform("r") = radius;
    vBlurBuilder.child("imageShader") = tmpBlur->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    vBlurBuilder.child("gradientShader") = alphaGradientShader;
    sk_sp<SkImage> tmpBlur2(vBlurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false));

    hBlurBuilder.child("imageShader") = tmpBlur2->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    sk_sp<SkImage> tmpBlur3(hBlurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false));

    vBlurBuilder.child("imageShader") = tmpBlur3->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    sk_sp<SkImage> tmpBlur4(vBlurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false));

    float invBlurScale = 1.0f / imageScale_;
    SkMatrix invBlurMatrix = SkMatrix::Scale(invBlurScale, invBlurScale);
    auto blurShader = tmpBlur4->makeShader(SkSamplingOptions(SkFilterMode::kLinear),  &invBlurMatrix);

    SkPaint paint;
    paint.setShader(blurShader);
    canvas.drawRect(SkRect::Make(dst.makeOffset(-dst.left(), -dst.top())), paint);
}
#else
void RSLinearGradientBlurFilter::DrawMeanLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image,
    Drawing::Canvas& canvas, float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
    const Drawing::Rect& dst)
{
    if (!horizontalMeanBlurShaderEffect_ || !verticalMeanBlurShaderEffect_ ||!image) { return; }

    Drawing::Matrix m;
    Drawing::Matrix blurMatrix;
    blurMatrix.PostScale(imageScale_, imageScale_);
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
    Drawing::Rect rect = dst;
    rect.Offset(-dst.GetLeft(), -dst.GetTop());
    canvas.DrawRect(rect);
    canvas.DetachBrush();
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSLinearGradientBlurFilter::DrawMaskLinearGradientBlur(const sk_sp<SkImage>& image, SkCanvas& canvas,
    std::shared_ptr<RSSkiaFilter>& blurFilter, sk_sp<SkShader> alphaGradientShader, const SkRect& dst)
{
    if (image == nullptr) {
        ROSEN_LOGE("RSLinearGradientBlurFilter::DrawMaskLinearGradientBlur image is null");
        return;
    }

    blurFilter->DrawImageRect(canvas, image, SkRect::Make(image->bounds().makeOutset(-1, -1)), dst);
    auto offscreenSurface = canvas.GetSurface();
    if (offscreenSurface == nullptr) {
        return;
    }
    auto filteredSnapshot = offscreenSurface->makeImageSnapshot();
    auto srcImageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto blurImageShader = filteredSnapshot->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = MakeMaskLinearGradientBlurShader(srcImageShader, blurImageShader, alphaGradientShader);

    SkPaint paint;
    paint.setShader(shader);
    canvas.drawRect(SkRect::Make(dst.makeOffset(-dst.left(), -dst.top())), paint);
}
#else
void RSLinearGradientBlurFilter::DrawMaskLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image,
    Drawing::Canvas& canvas, std::shared_ptr<RSDrawingFilter>& blurFilter,
    std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        ROSEN_LOGE("RSLinearGradientBlurFilter::DrawMaskLinearGradientBlur image is null");
        return;
    }

    auto imageInfo = image->GetImageInfo();
    auto srcRect = Drawing::Rect(0, 0, imageInfo.GetWidth(), imageInfo.GetHeight());
    blurFilter->DrawImageRect(canvas, image, srcRect, dst);
    auto offscreenSurface = canvas.GetSurface();
    if (offscreenSurface == nullptr) {
        return;
    }
    std::shared_ptr<Drawing::Image> filteredSnapshot = offscreenSurface->GetImageSnapshot();
    Drawing::Matrix matrix;
    Drawing::Matrix matrix2;
    matrix2.Translate(dst.GetLeft(), dst.GetTop());
    Drawing::Matrix inputMatrix;
    inputMatrix.Translate(-srcRect.GetLeft(), -srcRect.GetTop());
    inputMatrix.PostConcat(matrix2);

    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), inputMatrix);
    auto blurImageShader = Drawing::ShaderEffect::CreateImageShader(*filteredSnapshot, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    auto shader = MakeMaskLinearGradientBlurShader(srcImageShader, blurImageShader, alphaGradientShader);

    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSLinearGradientBlurFilter::MakeMaskLinearGradientBlurShader(
    sk_sp<SkShader> srcImageShader, sk_sp<SkShader> blurImageShader, sk_sp<SkShader> gradientShader)
#else
std::shared_ptr<Drawing::ShaderEffect> RSLinearGradientBlurFilter::MakeMaskLinearGradientBlurShader(
    std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> blurImageShader,
    std::shared_ptr<Drawing::ShaderEffect> gradientShader)
#endif
{
    static const char* prog = R"(
        uniform shader srcImageShader;
        uniform shader blurImageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord)
        {
            vec3 srcColor = vec3(srcImageShader.eval(coord).r,
                srcImageShader.eval(coord).g, srcImageShader.eval(coord).b);
            vec3 blurColor = vec3(blurImageShader.eval(coord).r,
                blurImageShader.eval(coord).g, blurImageShader.eval(coord).b);
            float gradient = gradientShader.eval(coord).a;

            vec3 color = blurColor * gradient + srcColor * (1 - gradient);
            return vec4(color, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a) < 0.001) {
                return srcImageShader.eval(coord);
            }

            if (abs(gradientShader.eval(coord).a) > 0.999) {
                return blurImageShader.eval(coord);
            }

            return meanFilter(coord);
        }
    )";

#ifndef USE_ROSEN_DRAWING
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    if (!effect) {
        return nullptr;
    }

    SkRuntimeShaderBuilder builder(effect);
    builder.child("srcImageShader") = srcImageShader;
    builder.child("blurImageShader") = blurImageShader;
    builder.child("gradientShader") = gradientShader;
    return builder.makeShader(nullptr, false);
#else
    if (maskBlurShaderEffect_ == nullptr) {
        maskBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (maskBlurShaderEffect_ == nullptr) {
            return nullptr;
        }
    }

    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(maskBlurShaderEffect_);
    builder->SetChild("srcImageShader", srcImageShader);
    builder->SetChild("blurImageShader", blurImageShader);
    builder->SetChild("gradientShader", gradientShader);
    return builder->MakeShader(nullptr, false);
#endif
}

std::string RSLinearGradientBlurFilter::GetDescription()
{
    return "RSLinearGradientBlurFilter";
}
} // namespace Rosen
} // namespace OHOS
