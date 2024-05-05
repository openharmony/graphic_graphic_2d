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
#include "ge_linear_gradient_blur_shader_filter.h"

#include "ge_log.h"
#include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr static float FLOAT_ZERO_THRESHOLD = 0.001f;
constexpr static uint8_t DIRECTION_NUM = 4;

static bool GetMaskLinearBlurEnabled()
{
#ifdef GE_OHOS
    // Determine whether the mask LinearBlur render should be enabled. The default value is 0,
    // which means that it is unenabled.
    static bool enabled =
        std::atoi((system::GetParameter("persist.sys.graphic.maskLinearBlurEnabled", "1")).c_str()) != 0;
    return enabled;
#endif
    return false;
}
} // namespace

std::shared_ptr<Drawing::RuntimeEffect> GELinearGradientBlurShaderFilter::horizontalMeanBlurShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> GELinearGradientBlurShaderFilter::verticalMeanBlurShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> GELinearGradientBlurShaderFilter::maskBlurShaderEffect_ = nullptr;

GELinearGradientBlurShaderFilter::GELinearGradientBlurShaderFilter(
    const Drawing::GELinearGradientBlurShaderFilterParams& params)
{
    geoWidth_ = params.geoWidth;
    geoHeight_ = params.geoHeight;
    auto maskLinearBlur = GetMaskLinearBlurEnabled();
    linearGradientBlurPara_ = std::make_shared<GELinearGradientBlurPara>(
        params.blurRadius, params.fractionStops, static_cast<GEGradientDirection>(params.direction), maskLinearBlur);
    mat_ = params.mat;
    tranX_ = params.tranX;
    tranY_ = params.tranY;
    isOffscreenCanvas_ = params.isOffscreenCanvas;
}

std::shared_ptr<Drawing::Image> GELinearGradientBlurShaderFilter::ProcessImageDDGR(
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image, uint8_t directionBias)
{
    auto& para = linearGradientBlurPara_;
    auto clipIPadding = Drawing::Rect(0, 0, geoWidth_ * imageScale_, geoHeight_ * imageScale_);
    uint8_t direction = static_cast<uint8_t>(para->direction_);
    TransformGradientBlurDirection(direction, directionBias);
    float radius = para->blurRadius_;

    Drawing::Brush brush;
    Drawing::Filter imageFilter;
    Drawing::GradientBlurType blurType;
    if (GetMaskLinearBlurEnabled() && para->useMaskAlgorithm_) {
        blurType = Drawing::GradientBlurType::AlPHA_BLEND;
        radius /= 2; // 2: half radius.
    } else {
        radius -= GELinearGradientBlurPara::ORIGINAL_BASE;
        radius = std::clamp(radius, 0.0f, 60.0f); // 60.0 represents largest blur radius
        blurType = Drawing::GradientBlurType::RADIUS_GRADIENT;
    }
    imageFilter.SetImageFilter(Drawing::ImageFilter::CreateGradientBlurImageFilter(
        radius, para->fractionStops_, static_cast<Drawing::GradientDir>(direction), blurType, nullptr));
    brush.SetFilter(imageFilter);

    canvas.AttachBrush(brush);
    Drawing::Rect rect = clipIPadding;
    rect.Offset(-clipIPadding.GetLeft(), -clipIPadding.GetTop());
    canvas.DrawImageRect(
        *image, rect, clipIPadding, Drawing::SamplingOptions(), Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
    return image;
}

std::shared_ptr<Drawing::Image> GELinearGradientBlurShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    auto& para = linearGradientBlurPara_;
    if (!image || para == nullptr || para->blurRadius_ <= 0) {
        return image;
    }
    LOGD("GELinearGradientBlurShaderFilter::DrawImageRect%{public}f,  %{public}f, %{public}f, %{public}f, %{public}f "
         "%{public}d", para->blurRadius_, geoWidth_, geoHeight_, tranX_, tranY_, (int)isOffscreenCanvas_);

    ComputeScale(dst.GetWidth(), dst.GetHeight(), para->useMaskAlgorithm_);
    auto clipIPadding = Drawing::Rect(0, 0, geoWidth_ * imageScale_, geoHeight_ * imageScale_);
    uint8_t directionBias = 0;
    auto alphaGradientShader = MakeAlphaGradientShader(clipIPadding, para, directionBias);
    if (alphaGradientShader == nullptr) {
        LOGE("GELinearGradientBlurShaderFilter::DrawImageRect alphaGradientShader null");
        return image;
    }

    if (GetMaskLinearBlurEnabled() && para->useMaskAlgorithm_) {
        // use faster LinearGradientBlur if valid
        if (para->LinearGradientBlurFilter_ == nullptr) {
            LOGE("RSPropertiesPainter::DrawLinearGradientBlur blurFilter null");
            return image;
        }

        auto& RSFilter = para->LinearGradientBlurFilter_;
        auto filter = RSFilter;
        return DrawMaskLinearGradientBlur(image, canvas, filter, alphaGradientShader, dst);
    } else {
        // use original LinearGradientBlur
        float radius = para->blurRadius_ - GELinearGradientBlurPara::ORIGINAL_BASE;
        radius = std::clamp(radius, 0.0f, 60.0f); // 60.0 represents largest blur radius
        radius = radius / 2 * imageScale_;        // 2 half blur radius
        MakeHorizontalMeanBlurEffect();
        MakeVerticalMeanBlurEffect();
        DrawMeanLinearGradientBlur(image, canvas, radius, alphaGradientShader, dst);
        return image;
    }
}

void GELinearGradientBlurShaderFilter::ComputeScale(float width, float height, bool useMaskAlgorithm)
{
    if (GetMaskLinearBlurEnabled() && useMaskAlgorithm) {
        imageScale_ = 1.0f;
    } else {
        if (width * height < 10000) { // 10000 for 100 * 100 resolution
            imageScale_ = 0.7f;       // 0.7 for scale
        } else {
            imageScale_ = 0.5f; // 0.5 for scale
        }
    }
}

uint8_t GELinearGradientBlurShaderFilter::CalcDirectionBias(const Drawing::Matrix& mat)
{
    uint8_t directionBias = 0;
    // 1 and 3 represents rotate matrix's index
    if ((mat.Get(1) > FLOAT_ZERO_THRESHOLD) && (mat.Get(3) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 1; // 1 represents rotate 90 degree
        // 0 and 4 represents rotate matrix's index
    } else if ((mat.Get(0) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(4) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 2; // 2 represents rotate 180 degree
        // 1 and 3 represents rotate matrix's index
    } else if ((mat.Get(1) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(3) > FLOAT_ZERO_THRESHOLD)) {
        directionBias = 3; // 3 represents rotate 270 degree
    }
    return directionBias;
}

void GELinearGradientBlurShaderFilter::TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias)
{
    if (direction == static_cast<uint8_t>(GEGradientDirection::LEFT_BOTTOM)) {
        direction += 2; // 2 is used to transtorm diagnal direction.
    } else if (direction == static_cast<uint8_t>(GEGradientDirection::RIGHT_TOP) ||
               direction == static_cast<uint8_t>(GEGradientDirection::RIGHT_BOTTOM)) {
        direction -= 1; // 1 is used to transtorm diagnal direction.
    }
    if (direction <= static_cast<uint8_t>(GEGradientDirection::BOTTOM)) {
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
    if (direction == static_cast<uint8_t>(GEGradientDirection::RIGHT_BOTTOM)) {
        direction -= 2; // 2 is used to restore diagnal direction.
    } else if (direction == static_cast<uint8_t>(GEGradientDirection::LEFT_BOTTOM) ||
               direction == static_cast<uint8_t>(GEGradientDirection::RIGHT_TOP)) {
        direction += 1; // 1 is used to restore diagnal direction.
    }
}

bool GELinearGradientBlurShaderFilter::GetGEGradientDirectionPoints(
    Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GEGradientDirection direction) // 2 size of points
{
    switch (direction) {
        case GEGradientDirection::BOTTOM: {
            pts[0].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetTop()); // 2 middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetBottom()); // 2  middle of width;
            break;
        }
        case GEGradientDirection::TOP: {
            pts[0].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetBottom()); // 2  middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetTop());    // 2  middle of width;
            break;
        }
        case GEGradientDirection::RIGHT: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            pts[1].Set(clipBounds.GetRight(),
                clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            break;
        }
        case GEGradientDirection::LEFT: {
            pts[0].Set(clipBounds.GetRight(),
                clipBounds.GetHeight() / 2 + clipBounds.GetTop());                              // 2  middle of height;
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            break;
        }
        default: {
        }
    }
    return ProcessGradientDirectionPoints(pts, clipBounds, direction);
}

bool GELinearGradientBlurShaderFilter::ProcessGradientDirectionPoints(
    Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GEGradientDirection direction)  // 2 size of points
{
    switch (direction) {
        case GEGradientDirection::RIGHT_BOTTOM: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetTop());
            pts[1].Set(clipBounds.GetRight(), clipBounds.GetBottom());
            break;
        }
        case GEGradientDirection::LEFT_TOP: {
            pts[0].Set(clipBounds.GetRight(), clipBounds.GetBottom());
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetTop());
            break;
        }
        case GEGradientDirection::LEFT_BOTTOM: {
            pts[0].Set(clipBounds.GetRight(), clipBounds.GetTop());
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetBottom());
            break;
        }
        case GEGradientDirection::RIGHT_TOP: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetBottom());
            pts[1].Set(clipBounds.GetRight(), clipBounds.GetTop());
            break;
        }
        default: {
        }
    }
    Drawing::Matrix pointsMat = mat_;
    if (isOffscreenCanvas_) {
        pointsMat.PostTranslate(-tranX_, -tranY_);
    }
    std::vector<Drawing::Point> points(pts, pts + 2); // 2 size of pts
    pointsMat.MapPoints(points, points, points.size());
    pts[0].Set(points[0].GetX(), points[0].GetY());
    pts[1].Set(points[1].GetX(), points[1].GetY());
    return true;
}

std::shared_ptr<Drawing::ShaderEffect> GELinearGradientBlurShaderFilter::MakeAlphaGradientShader(
    const Drawing::Rect& clipBounds, const std::shared_ptr<GELinearGradientBlurPara>& para, uint8_t directionBias)
{
    if (para->fractionStops_.size() < 1) {
        return nullptr;
    }

    std::vector<Drawing::ColorQuad> c;
    std::vector<Drawing::scalar> p;
    Drawing::Point pts[2];  // 2 size of points

    uint8_t direction = static_cast<uint8_t>(para->direction_);
    if (directionBias != 0) {
        TransformGradientBlurDirection(direction, directionBias);
    }
    bool result = GetGEGradientDirectionPoints(pts, clipBounds, static_cast<GEGradientDirection>(direction));
    if (!result) {
        return nullptr;
    }
    uint8_t ColorMax = 255; // 255 max number of color
    uint8_t ColorMin = 0;
    if (para->fractionStops_[0].second > 0.01) { // 0.01 represents the fraction bias
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        p.emplace_back(para->fractionStops_[0].second - 0.01); // 0.01 represents the fraction bias
    }
    for (size_t i = 0; i < para->fractionStops_.size(); i++) {
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(
            static_cast<uint8_t>(para->fractionStops_[i].first * ColorMax), ColorMax, ColorMax, ColorMax));
        p.emplace_back(para->fractionStops_[i].second);
    }
    // 0.01 represents the fraction bias
    if (para->fractionStops_.back().second < (1 - 0.01)) {
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        // 0.01 represents the fraction bias
        p.emplace_back(para->fractionStops_.back().second + 0.01);
    }
    return Drawing::ShaderEffect::CreateLinearGradient(pts[0], pts[1], c, p, Drawing::TileMode::CLAMP);
}

void GELinearGradientBlurShaderFilter::MakeHorizontalMeanBlurEffect()
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

void GELinearGradientBlurShaderFilter::MakeVerticalMeanBlurEffect()
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

void GELinearGradientBlurShaderFilter::DrawMeanLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image,
    Drawing::Canvas& canvas, float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
    const Drawing::Rect& dst)
{
    if (!horizontalMeanBlurShaderEffect_ || !verticalMeanBlurShaderEffect_ || !image) {
        return;
    }

    if (imageScale_ < 1e-6) {
        return;
    }

    Drawing::Matrix m;
    Drawing::Matrix blurMatrix;
    blurMatrix.PostScale(imageScale_, imageScale_);
    blurMatrix.PostTranslate(dst.GetLeft(), dst.GetTop());

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);

    auto tmpBlur4 = BuildMeanLinearGradientBlur(image, canvas, radius, alphaGradientShader, blurMatrix);

    float invBlurScale = 1.0f / imageScale_;
    Drawing::Matrix invBlurMatrix;
    invBlurMatrix.PostScale(invBlurScale, invBlurScale);
    auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *tmpBlur4, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, invBlurMatrix);

    Drawing::Brush brush;
    brush.SetShaderEffect(blurShader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::Image> GELinearGradientBlurShaderFilter::BuildMeanLinearGradientBlur(
    const std::shared_ptr<Drawing::Image>& image, Drawing::Canvas& canvas, float radius,
    std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, Drawing::Matrix blurMatrix)
{
    auto width = image->GetWidth();
    auto height = image->GetHeight();
    auto originImageInfo = image->GetImageInfo();
    auto scaledInfo = Drawing::ImageInfo(std::ceil(width * imageScale_), std::ceil(height * imageScale_),
        originImageInfo.GetColorType(), originImageInfo.GetAlphaType(), originImageInfo.GetColorSpace());
    Drawing::Matrix m;
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::RuntimeShaderBuilder hBlurBuilder(horizontalMeanBlurShaderEffect_);
    hBlurBuilder.SetUniform("r", radius);
    auto shader1 = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, blurMatrix);
    hBlurBuilder.SetChild("imageShader", shader1);
    hBlurBuilder.SetChild("gradientShader", alphaGradientShader);
    std::shared_ptr<Drawing::Image> tmpBlur(
        hBlurBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false));

    Drawing::RuntimeShaderBuilder vBlurBuilder(verticalMeanBlurShaderEffect_);
    vBlurBuilder.SetUniform("r", radius);
    auto tmpBlurShader = Drawing::ShaderEffect::CreateImageShader(
        *tmpBlur, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, m);
    vBlurBuilder.SetChild("imageShader", tmpBlurShader);
    vBlurBuilder.SetChild("gradientShader", alphaGradientShader);
    std::shared_ptr<Drawing::Image> tmpBlur2(
        vBlurBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false));

    auto tmpBlur2Shader = Drawing::ShaderEffect::CreateImageShader(
        *tmpBlur2, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, m);
    hBlurBuilder.SetChild("imageShader", tmpBlur2Shader);
    std::shared_ptr<Drawing::Image> tmpBlur3(
        hBlurBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false));

    auto tmpBlur3Shader = Drawing::ShaderEffect::CreateImageShader(
        *tmpBlur3, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, m);
    vBlurBuilder.SetChild("imageShader", tmpBlur3Shader);
    std::shared_ptr<Drawing::Image> tmpBlur4(
        vBlurBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false));
    return tmpBlur4;
}

std::shared_ptr<Drawing::Image> GELinearGradientBlurShaderFilter::DrawMaskLinearGradientBlur(
    const std::shared_ptr<Drawing::Image>& image, Drawing::Canvas& canvas, std::shared_ptr<GEShaderFilter>& blurFilter,
    std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("GELinearGradientBlurShaderFilter::DrawMaskLinearGradientBlur image is null");
        return image;
    }

    auto imageInfo = image->GetImageInfo();
    if (imageInfo.GetWidth() < 1e-6 || imageInfo.GetHeight() < 1e-6) {
        return image;
    }
    auto srcRect = Drawing::Rect(0, 0, imageInfo.GetWidth(), imageInfo.GetHeight());
    auto blurImage = blurFilter->ProcessImage(canvas, image, srcRect, dst);

    Drawing::Matrix matrix;
    Drawing::Matrix inputMatrix;
    inputMatrix.Translate(dst.GetLeft(), dst.GetTop());
    inputMatrix.PostScale(dst.GetWidth() / imageInfo.GetWidth(), dst.GetHeight() / imageInfo.GetHeight());

    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), inputMatrix);
    auto blurImageShader = Drawing::ShaderEffect::CreateImageShader(*blurImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    auto builder = MakeMaskLinearGradientBlurShader(srcImageShader, blurImageShader, alphaGradientShader);
    auto outImageInfo = Drawing::ImageInfo(dst.GetWidth(), dst.GetHeight(), blurImage->GetImageInfo().GetColorType(),
        blurImage->GetImageInfo().GetAlphaType(), blurImage->GetImageInfo().GetColorSpace());
    auto outImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, outImageInfo, false);

    return outImage;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GELinearGradientBlurShaderFilter::MakeMaskLinearGradientBlurShader(
    std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> blurImageShader,
    std::shared_ptr<Drawing::ShaderEffect> gradientShader)
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
    return builder;
}

std::string GELinearGradientBlurShaderFilter::GetDescription()
{
    return "GELinearGradientBlurShaderFilter";
}

std::string GELinearGradientBlurShaderFilter::GetDetailedDescription()
{
    return "GELinearGradientBlurShaderFilterBlur, radius: " + std::to_string(linearGradientBlurPara_->blurRadius_);
}
} // namespace Rosen
} // namespace OHOS
