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

#include "rs_slr_scale.h"

#include <cfloat>
#include "graphic_feature_param_manager.h"
#include "rs_trace.h"
#include "pipeline/main_thread/rs_main_thread.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<Drawing::RuntimeEffect> RSSLRScaleFunction::slrFilterShader_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSSLRScaleFunction::laplaceFilterShader_ = nullptr;
std::shared_ptr<Drawing::RuntimeShaderBuilder> RSSLRScaleFunction::slrShaderBuilder_ = nullptr;
std::shared_ptr<Drawing::RuntimeShaderBuilder> RSSLRScaleFunction::laplaceShaderBuilder_ = nullptr;
RSSLRScaleFunction::RSSLRScaleFunction(
    float dstWidth, float dstHeight, float srcWidth, float srcHeight)
    : mirrorWidth_(dstWidth), mirrorHeight_(dstHeight), srcWidth_(srcWidth), srcHeight_(srcHeight)
{
    RefreshScreenData();
}

void RSSLRScaleFunction::CheckOrRefreshScreen(
    float dstWidth, float dstHeight, float srcWidth, float srcHeight)
{
    if (std::fabs(mirrorWidth_ - dstWidth) <= FLT_EPSILON && std::fabs(mirrorHeight_ - dstHeight) <= FLT_EPSILON &&
        std::fabs(srcWidth_ - srcWidth) <= FLT_EPSILON && std::fabs(srcHeight_ - srcHeight) <= FLT_EPSILON) {
        return;
    }
    mirrorWidth_ = dstWidth;
    mirrorHeight_ = dstHeight;
    srcWidth_ = srcWidth;
    srcHeight_ = srcHeight;
    RefreshScreenData();
}

#define PI 3.14159265358979323846
#define SLR_WIN_BOUND 2

float GetSLRFactor(float x, int a)
{
    if (x - static_cast<float>(a) >= FLT_EPSILON ||
        x + static_cast<float>(a) < FLT_EPSILON || a == 0) {
        return 0.0f;
    }
    if (std::abs(x) < 1e-16) {
        return 1.0f;
    }
    x *= PI;
    return a * std::sin(x) * std::sin(x / a) / (x * x);
}

// BIT_BOUND is Weight resolution
const float BIT_BOUND = 32767.0f;
const int SLR_MAX_WIN_SIZE = 4;
const int SLR_TAO_MAX_SIZE = 2;
const float SLR_SCALE_THR_LOW = 0.65f;
const float SLR_ALPHA_LOW = 0.2f;
const float SLR_ALPHA_HIGH = 0.05f;

static std::shared_ptr<uint32_t[]> GetSLRWeights(float coeff, int width, int dstWidth, int minBound)
{
    std::shared_ptr<uint32_t[]> result(new uint32_t[dstWidth * SLR_MAX_WIN_SIZE]);
    float weights[SLR_MAX_WIN_SIZE];
    for (auto i = 0; i < dstWidth; i++) {
        float eta_i = (i + 0.5f) / coeff - 0.5f;
        int k = std::floor(eta_i) - minBound + 1;
        float sum = 0.f;
        for (auto j = 0; j < width; ++j) {
            float f = GetSLRFactor(coeff * (eta_i - (k + j)), minBound);
            weights[j] = f;
            sum += f;
        }
        if (ROSEN_LE(sum, 0.f)) {
            return nullptr;
        }
        for (auto j = 0; j < width; ++j) {
            auto f = weights[j];
            f /= sum;
            f *= BIT_BOUND;
            result[i * SLR_MAX_WIN_SIZE + j] = f + BIT_BOUND;
        }
        for (auto k = width; k < SLR_MAX_WIN_SIZE; k++) {
            result[i * SLR_MAX_WIN_SIZE + k] = BIT_BOUND;
        }
    }
    return result;
}

std::shared_ptr<Drawing::ShaderEffect> RSSLRScaleFunction::GetSLRShaderEffect(float coeff, int dstWidth)
{
    float tao = 1.0f / coeff;
    int minBound = std::min(std::max(SLR_TAO_MAX_SIZE, static_cast<int>(std::floor(tao))), SLR_WIN_BOUND);
 
    int width = SLR_MAX_WIN_SIZE;
    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_OPAQUE };
   
    Drawing::Bitmap bitmap;
    bitmap.Build(width, dstWidth, format, width * sizeof(uint32_t));
 
    auto weightW = GetSLRWeights(coeff, 2 * minBound, dstWidth, minBound);
    if (weightW == nullptr) {
        return nullptr;
    }
    Drawing::ImageInfo imageInfo(width, dstWidth,
        Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_OPAQUE, imageColorSpace_);
    bitmap.InstallPixels(imageInfo, weightW.get(), width * sizeof(uint32_t));
    
    Drawing::Image imagew;
    imagew.BuildFromBitmap(bitmap);
    
    Drawing::SamplingOptions sampling(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    auto shader = Drawing::ShaderEffect::CreateImageShader(imagew, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, sampling, Drawing::Matrix());
    return shader;
}

void RSSLRScaleFunction::RefreshScreenData()
{
    if (ROSEN_LE(mirrorWidth_, 0.f) || ROSEN_LE(srcWidth_, 0.f) ||
        ROSEN_LE(mirrorHeight_, 0.f) || ROSEN_LE(srcHeight_, 0.f)) {
        isSLRCopy_ = false;
        return;
    }
    scaleNum_ = std::min(mirrorWidth_ / srcWidth_, mirrorHeight_ / srcHeight_);
    dstWidth_ = scaleNum_ * srcWidth_;
    dstHeight_ = scaleNum_ * srcHeight_;
    
    alpha_ = scaleNum_ > SLR_SCALE_THR_LOW ? SLR_ALPHA_HIGH : SLR_ALPHA_LOW;
    float tao = 1.0f / scaleNum_;
    kernelSize_ = std::min(std::max(SLR_TAO_MAX_SIZE, static_cast<int>(std::floor(tao))), SLR_WIN_BOUND);

    RefreshColorSpace(imageColorSpace_ ? colorGamut_ : GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    isSLRCopy_ = scaleNum_ < SLR_SCALE_THR_HIGH && widthEffect_ && heightEffect_ &&
        MultiScreenParam::IsSlrScaleEnabled();
}

void RSSLRScaleFunction::RefreshColorSpace(GraphicColorGamut colorGamut)
{
    colorGamut_ = colorGamut;
    Drawing::CMSMatrixType colorMatrixType;
    switch (colorGamut_) {
        case GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB:
            colorMatrixType = Drawing::CMSMatrixType::SRGB;
            break;
        case GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3:
            colorMatrixType = Drawing::CMSMatrixType::DCIP3;
            break;
        default:
            RS_LOGE("RSSLRScaleFunction::RefreshColorSpace error colorSpace is %{public}d.", colorGamut_);
            colorMatrixType = Drawing::CMSMatrixType::SRGB;
            break;
    }
    imageColorSpace_ = Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, colorMatrixType);
    widthEffect_ = GetSLRShaderEffect(scaleNum_, dstWidth_);
    heightEffect_ = GetSLRShaderEffect(scaleNum_, dstHeight_);
}

void RSSLRScaleFunction::CanvasScale(RSPaintFilterCanvas& canvas)
{
    canvas.Translate((mirrorWidth_ - dstWidth_) / 2, (mirrorHeight_ - dstHeight_) / 2);
    canvas.Save();
    canvas.Scale(scaleNum_, scaleNum_);
    scaleMatrix_ = canvas.GetTotalMatrix();
    canvas.Restore();
}

void RSSLRScaleFunction::ProcessCacheImage(RSPaintFilterCanvas& canvas, Drawing::Image& cacheImageProcessed)
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    auto image = ProcessSLRImage(canvas, cacheImageProcessed);
    if (image == nullptr) {
        RS_LOGE("RSSLRScaleFunction::ProcessCacheImage image is nullptr");
        return;
    }
    auto sampling = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    canvas.DrawImage(*image, 0, 0, sampling);
    canvas.DetachBrush();
}

void RSSLRScaleFunction::ProcessOffscreenImage(RSPaintFilterCanvas& canvas, Drawing::Image& offscreenImage)
{
    auto image = ProcessSLRImage(canvas, offscreenImage);
    if (image == nullptr) {
        RS_LOGE("RSSLRScaleFunction::ProcessOffscreenImage image is nullptr");
        return;
    }
    auto sampling = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    canvas.DrawImage(*image, 0, 0, sampling);
}

std::shared_ptr<Rosen::Drawing::Image> RSSLRScaleFunction::ProcessSLRImage(RSPaintFilterCanvas& canvas,
    Drawing::Image& cacheImageProcessed)
{
    auto builder = SLRImageShaderBuilder(cacheImageProcessed);
    if (builder == nullptr) {
        RS_LOGE("RSSLRScaleFunction::ProcessSLRImage SLRImageShaderBuilder is nullptr");
        return nullptr;
    }
    auto originImageInfo = cacheImageProcessed.GetImageInfo();
    auto ScreenInfo = Drawing::ImageInfo(std::ceil(dstWidth_),
        std::ceil(dstHeight_), originImageInfo.GetColorType(),
        originImageInfo.GetAlphaType(), imageColorSpace_);
    std::shared_ptr<Drawing::Image> tmpImage(builder->MakeImage(
        canvas.GetGPUContext().get(), nullptr, ScreenInfo, false));
    if (tmpImage == nullptr) {
        RS_LOGE("RSSLRScaleFunction::ProcessSLRImage tmpImage is nullptr");
        return nullptr;
    }
    auto tempImageInfo = tmpImage->GetImageInfo();
    auto slrImageInfo = Drawing::ImageInfo(std::ceil(dstWidth_),
        std::ceil(dstHeight_), tempImageInfo.GetColorType(),
        tempImageInfo.GetAlphaType(), tempImageInfo.GetColorSpace());
    auto lapLaceBuilder = LaplaceShaderBuilder(*tmpImage);
    if (lapLaceBuilder == nullptr) {
        RS_LOGE("RSSLRScaleFunction::ProcessSLRImage lapLaceBuilder is nullptr");
        return nullptr;
    }
    return lapLaceBuilder->MakeImage(
        canvas.GetGPUContext().get(), nullptr, slrImageInfo, false);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSSLRScaleFunction::SLRImageShaderBuilder(
    const Drawing::Image& image)
{
    if (!slrShaderBuilder_) {
        if (!slrFilterShader_) {
            slrFilterShader_ = RSSLRScaleFunction::MakeSLRShaderEffect();
        }
        if (!slrFilterShader_) {
            RS_LOGE("RSSLRScaleFunction::SLRImageShader slrFilterShader_ is null");
            return nullptr;
        }
        slrShaderBuilder_ = std::make_shared<Drawing::RuntimeShaderBuilder>(slrFilterShader_);
    }
    if (!slrShaderBuilder_) {
        RS_LOGE("RSSLRScaleFunction::SLRImageShader slrShaderBuilder_ is null");
        return nullptr;
    }
    Drawing::SamplingOptions sampling(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, sampling, Drawing::Matrix());
    if (!imageShader) {
        RS_LOGE("RSSLRScaleFunction::SLRImageShader imageShader is null");
        return nullptr;
    }
    slrShaderBuilder_->SetChild("imageShader", imageShader);
    slrShaderBuilder_->SetChild("widthShader", widthEffect_);
    slrShaderBuilder_->SetChild("heightShader", heightEffect_);
   
    float tao = 1.0f / scaleNum_;
    int a = std::min(std::max(SLR_TAO_MAX_SIZE, static_cast<int>(std::floor(tao))), SLR_WIN_BOUND);

    slrShaderBuilder_->SetUniform("slr_a", a, a);
    slrShaderBuilder_->SetUniform("slr_coeff", scaleNum_, scaleNum_);
    slrShaderBuilder_->SetUniform("slr_coeff_tao", tao, tao);
    return slrShaderBuilder_;
}

std::shared_ptr<Drawing::RuntimeEffect> RSSLRScaleFunction::MakeSLRShaderEffect()
{
    static const std::string SLRFilterString(R"(
        uniform shader imageShader;
        uniform shader widthShader;
        uniform shader heightShader;
        uniform vec2 slr_a;
        uniform vec2 slr_coeff;
        uniform vec2 slr_coeff_tao;
        const int MAX_SIZE = 4;
        const float factor = 0.000030518509476;
        const float bit_bound = 32767.0;

        half4 main(float2 fcoord) {
            float2 eta = fcoord * slr_coeff_tao - vec2(0.5);
            half2 coord_s = floor(eta) - slr_a + half2(1.5, 1.5);
            half4 c = vec4(0, 0, 0, 0);
            for (int i = 0; i < MAX_SIZE; ++i) {
                half4 ws = widthShader.eval(half2(half(i) + 0.5, fcoord.x)) * 255;
                float w_i = ws.x + ws.y * 256.0 - bit_bound;
                w_i *= factor;
    
                half4 t = vec4(0, 0, 0, 0);
                for (int j = 0; j < MAX_SIZE; ++j) {
                    half4 hs = heightShader.eval(half2(half(j) + 0.5, fcoord.y)) * 255;
                    float w_j = hs.x + hs.y * 256.0 - bit_bound;
                    w_j *= factor;
                    half2 coord = coord_s + half2(half(i), half(j));
                    half4 tc = imageShader.eval(coord);
                    t += w_j * tc;
                }
                c += t * w_i;
            }
            return c;
        }
    )");

    std::shared_ptr<Drawing::RuntimeEffect> SLRFilterShader =
        Drawing::RuntimeEffect::CreateForShader(SLRFilterString);
    if (!SLRFilterShader) {
        return nullptr;
    }
    slrFilterShader_ = std::move(SLRFilterShader);
    return slrFilterShader_;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSSLRScaleFunction::LaplaceShaderBuilder(const Drawing::Image& image)
{
    if (!laplaceShaderBuilder_) {
        if (!laplaceFilterShader_) {
            laplaceFilterShader_ = RSSLRScaleFunction::MakeLaplaceShaderEffect();
        }
        laplaceShaderBuilder_ = std::make_shared<Drawing::RuntimeShaderBuilder>(laplaceFilterShader_);
    }
    Drawing::SamplingOptions sampling(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, sampling, Drawing::Matrix());
    if (!imageShader) {
        RS_LOGE("RSSLRScaleFunction::SLRImageShader imageShader is null");
        return nullptr;
    }
    laplaceShaderBuilder_->SetChild("imageShader", imageShader);
    laplaceShaderBuilder_->SetUniform("alpha", alpha_);
    return laplaceShaderBuilder_;
}

std::shared_ptr<Drawing::RuntimeEffect> RSSLRScaleFunction::MakeLaplaceShaderEffect()
{
    static const std::string lapFilterString(R"(
        uniform shader imageShader;
        uniform float alpha;
        vec4 main(vec2 coord) {
            vec4 c = imageShader.eval(coord);
            vec4 top = imageShader.eval(coord + half2(0.0, 1.0));
            vec4 bottom = imageShader.eval(coord - half2(0.0, 1.0));
            vec4 left = imageShader.eval(coord - half2(1.0, 0.0));
            vec4 right = imageShader.eval(coord + half2(1.0, 0.0));
            return clamp(c + alpha * (4.0 * c - top - bottom - left - right), 0.0, 1.0);
        }
    )");

    std::shared_ptr<Drawing::RuntimeEffect> lapFilterShader =
        Drawing::RuntimeEffect::CreateForShader(lapFilterString);
    if (!lapFilterShader) {
        return nullptr;
    }
    laplaceFilterShader_ = std::move(lapFilterShader);
    return laplaceFilterShader_;
}

} // namespace Rosen
} // namespace OHOS