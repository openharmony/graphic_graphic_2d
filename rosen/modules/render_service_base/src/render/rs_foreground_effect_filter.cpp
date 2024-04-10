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
#include "render/rs_foreground_effect_filter.h"

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {
RSForegroundEffectFilter::RSForegroundEffectFilter(float blurRadius)
    : RSDrawingFilter(nullptr)
{
    type_ = FilterType::FOREGROUND_EFFECT;
    MakeForegroundEffect();
    ComputeParamter(blurRadius);

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&blurRadius_, sizeof(blurRadius_), hash_);
}

RSForegroundEffectFilter::~RSForegroundEffectFilter() = default;

std::string RSForegroundEffectFilter::GetDescription()
{
    return "RSForegroundEffectFilter " + std::to_string(blurRadius_);
}

bool RSForegroundEffectFilter::IsValid() const
{
    constexpr float epsilon = 0.999f; // if blur radius less than 1, do not need to draw
    return blurRadius_ > epsilon;
}

void RSForegroundEffectFilter::MakeForegroundEffect()
{
    std::string blurString(
        R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            c += imageInput.eval(float2(in_blurOffset.x + xy.x,
                                        in_blurOffset.y + xy.y));
            c += imageInput.eval(float2(in_blurOffset.x + xy.x,
                                        -in_blurOffset.y + xy.y));
            c += imageInput.eval(float2(-in_blurOffset.x + xy.x,
                                        in_blurOffset.y + xy.y));
            c += imageInput.eval(float2(-in_blurOffset.x + xy.x,
                                        -in_blurOffset.y + xy.y));
            return half4(c.rgba * 0.2);
        }
    )");

    auto blurEffect = Drawing::RuntimeEffect::CreateForShader(blurString);
    if (!blurEffect) {
        ROSEN_LOGE("RSForegroundEffect::RuntimeShader blurEffect create failed");
        return;
    }
    blurEffect_ = std::move(blurEffect);
}

void RSForegroundEffectFilter::ComputeParamter(int radius)
{
    static constexpr int noiseFactor = 3; // 3 : smooth the radius change
    blurRadius_ = radius * 4 / noiseFactor * noiseFactor; // 4 : scale between gauss radius and kawase

    AdjustRadiusAndScale();
    ComputePassesAndUnit();
}

void RSForegroundEffectFilter::AdjustRadiusAndScale()
{
    static constexpr int radiusStep1 = 50; // 50 : radius step1
    static constexpr int radiusStep2 = 150; // 150 : radius step2
    static constexpr int radiusStep3 = 400; // 400 : radius step3
    static constexpr float scaleFactor1 = 0.25f; // 0.25 : downSample scale for step1
    static constexpr float scaleFactor2 = 0.125f; // 0.125 : downSample scale for step2
    static constexpr float scaleFactor3 = 0.0625f; // 0.0625 : downSample scale for step3

    auto radius = static_cast<int>(blurRadius_);
    if (radius > radiusStep3) {
        blurScale_ = scaleFactor3;
    } else if (radius > radiusStep2) {
        blurScale_ = scaleFactor2;
    } else if (radius > radiusStep1) {
        blurScale_ = scaleFactor1;
    } else {
        blurScale_ = BASE_BLUR_SCALE;
    }
}

void RSForegroundEffectFilter::ComputePassesAndUnit()
{
    int maxPasses = MAX_PASSES_LARGE_RADIUS;
    float tmpRadius = static_cast<float>(blurRadius_) / DILATED_CONVOLUTION_LARGE_RADIUS;
    numberOfPasses_ = std::min(maxPasses, std::max(static_cast<int>(ceil(tmpRadius)), 1)); // 1 : min pass num
    if (numberOfPasses_ == 0) {
        numberOfPasses_ = 1;
    }
    radiusByPasses_ = tmpRadius / numberOfPasses_;
    unit_ = std::ceil(radiusByPasses_ * blurScale_);
}

float RSForegroundEffectFilter::GetDirtyExtension()
{
    return std::ceil(EXPAND_UNIT_NUM * unit_ * numberOfPasses_ * 1 / blurScale_);
}

void RSForegroundEffectFilter::ApplyForegroundEffect(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image, const ForegroundEffectParam& param) const
{
    if (!blurEffect_ || !image || image->GetWidth() == 0 || image->GetHeight() == 0) {
        ROSEN_LOGE("RSForegroundEffectFilter::shader error");
        return;
    }

    auto src = param.src;
    auto dst = param.dst;

    RS_OPTIONAL_TRACE_BEGIN("ApplyForegroundEffect");
    ROSEN_LOGD("ForegroundEffect::kawase radius : %{public}f, scale : %{public}f, pass num : %{public}d",
        blurRadius_, blurScale_, numberOfPasses_);

    auto width = std::max(static_cast<int>(std::ceil(dst.GetWidth())), image->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst.GetHeight())), image->GetHeight());
    auto imgInfo = image->GetImageInfo();

    Drawing::Matrix blurMatrix;
    float scaleW = static_cast<float>(std::ceil(width * blurScale_)) / image->GetWidth();
    float scaleH = static_cast<float>(std::ceil(height * blurScale_)) / image->GetHeight();
    blurMatrix.PostScale(scaleW, scaleH);
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);

    Drawing::RuntimeShaderBuilder blurBuilder(blurEffect_);
    blurBuilder.SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::DECAL,
        Drawing::TileMode::DECAL, linear, blurMatrix));
    blurBuilder.SetUniform("in_blurOffset", radiusByPasses_ * blurScale_, radiusByPasses_ * blurScale_);

    auto scaledInfoGeo = Drawing::ImageInfo(std::ceil(width * blurScale_) + EXPAND_UNIT_NUM * unit_,
        std::ceil(height * blurScale_) + EXPAND_UNIT_NUM * unit_, imgInfo.GetColorType(),
        imgInfo.GetAlphaType(), imgInfo.GetColorSpace());
    Drawing::Matrix blurMatrixGeo;
    blurMatrixGeo.Translate(MOVE_UNIT_NUM * unit_, MOVE_UNIT_NUM * unit_);

    std::shared_ptr<Drawing::Image> tmpBlur(blurBuilder.MakeImage(
        canvas.GetGPUContext().get(), &blurMatrixGeo, scaledInfoGeo, false));
    // And now we'll build our chain of scaled blur stages
    for (auto i = 1; i < numberOfPasses_; i++) {
        auto scaledInfoGeoIter = Drawing::ImageInfo(tmpBlur->GetWidth() + EXPAND_UNIT_NUM * unit_,
            tmpBlur->GetHeight() + EXPAND_UNIT_NUM * unit_,
            imgInfo.GetColorType(), imgInfo.GetAlphaType(), imgInfo.GetColorSpace());
        Drawing::Matrix matrixGeoIter;
        matrixGeoIter.Translate(MOVE_UNIT_NUM * unit_, MOVE_UNIT_NUM * unit_);

        const float stepScale = static_cast<float>(i) * blurScale_;
        blurBuilder.SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(*tmpBlur, Drawing::TileMode::DECAL,
            Drawing::TileMode::DECAL, linear, Drawing::Matrix()));
        blurBuilder.SetUniform("in_blurOffset", radiusByPasses_ * stepScale, radiusByPasses_ * stepScale);

        tmpBlur = blurBuilder.MakeImage(canvas.GetGPUContext().get(), &matrixGeoIter, scaledInfoGeoIter, false);
    }

    Drawing::Matrix blurMatrixInv;
    blurMatrixInv.Translate(-MOVE_UNIT_NUM * unit_ * numberOfPasses_, -MOVE_UNIT_NUM * unit_ * numberOfPasses_);
    blurMatrixInv.PostScale(1/scaleW, 1/scaleH);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(*tmpBlur, Drawing::TileMode::DECAL,
        Drawing::TileMode::DECAL, linear, blurMatrixInv);
    Drawing::Brush brush;
    brush.SetShaderEffect(blurShader);
    canvas.DrawBackground(brush);
    canvas.DetachBrush();
}

void RSForegroundEffectFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    auto brush = GetBrush();
    ForegroundEffectParam param = ForegroundEffectParam(src, dst);
    ApplyForegroundEffect(canvas, image, param);
}
} // namespace Rosen
} // namespace OHOS
