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
#include "render/rs_distortion_shader_filter.h"

#include "common/rs_optional_trace.h"
#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
// 0.24 means a mapping between the external input and the actual calculated value.
static const float DISTORTION_SCALE = 0.24f;

RSDistortionFilter::RSDistortionFilter(float distortionK)
    : RSDrawingFilterOriginal(nullptr), distortionK_(distortionK)
{
    type_ = FilterType::DISTORT;

#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&type_, sizeof(type_), 0);
    hash_ = hashFunc(&distortionK_, sizeof(distortionK_), hash_);
    hash_ = hashFunc(&scaleCoord_, sizeof(scaleCoord_), hash_);
    hash_ = hashFunc(&offsetX_, sizeof(offsetX_), hash_);
    hash_ = hashFunc(&offsetY_, sizeof(offsetY_), hash_);
}

RSDistortionFilter::~RSDistortionFilter() = default;

std::string RSDistortionFilter::GetDescription()
{
    return "DistortionEffect k: " + std::to_string(distortionK_) +
        ", scaleCoord: " + std::to_string(scaleCoord_) + ", offsetX: " + std::to_string(offsetX_) +
        ", offsetY: " + std::to_string(offsetY_);
}

bool RSDistortionFilter::IsValid() const
{
    if (distortionK_ < -1 || distortionK_ > 1) {
        return false;
    }
    if (ROSEN_EQ(scaleCoord_, 0.0f)) {
        return false;
    }
    // -0.5f means the calculation coefficient of scale
    if (ROSEN_LNE(distortionK_ * DISTORTION_SCALE, -0.5f / (scaleCoord_ * scaleCoord_))) {
        return false;
    }
    return true;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSDistortionFilter::MakeDistortionShader()
{
    static std::shared_ptr<Drawing::RuntimeEffect> distortEffect = nullptr;
    std::string distortString(
        R"(
        uniform shader imageInput;
        uniform float2 iResolution;
        uniform float distortionK;
        uniform float scale; // coord: 0_1 -> -scale_scale
        uniform float2 offset;

        half4 main(float2 xy) {
            float2 uv = xy / iResolution.xy;
            uv = 2.0 * scale * uv - float2(scale, scale) - offset;
            float rr = uv.x * uv.x +  uv.y * uv.y;
            float2 finalUV = uv / (1.0 + distortionK * rr);
            finalUV = finalUV + float2(scale, scale) + offset;
            finalUV = finalUV / (2 * scale);
            half4 c = imageInput.eval(finalUV * iResolution.xy);
            return half4(c.rgba);
        }
    )");

    if (distortEffect == nullptr) {
        distortEffect = Drawing::RuntimeEffect::CreateForShader(distortString);
        if (distortEffect == nullptr) {
            ROSEN_LOGE("RSDistortionFilter::RuntimeShader distortEffect create failed");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> distortBuilder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(distortEffect);
    return distortBuilder;
}

void RSDistortionFilter::DrawDistortion(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    auto distortBuilder = MakeDistortionShader();
    if (!distortBuilder || !IsValid()) {
        ROSEN_LOGE("RSDistortionFilter::shader error");
        return;
    }
    RS_OPTIONAL_TRACE_NAME("DrawDistortion");

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    distortBuilder->SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::DECAL,
        Drawing::TileMode::DECAL, linear, Drawing::Matrix()));
    distortBuilder->SetUniform("iResolution", image->GetWidth(), image->GetHeight());
    distortBuilder->SetUniform("distortionK", distortionK_ * DISTORTION_SCALE);
    distortBuilder->SetUniform("scale", scaleCoord_);
    distortBuilder->SetUniform("offset", offsetX_, offsetY_);

    std::shared_ptr<Drawing::ShaderEffect> distortShader = distortBuilder->MakeShader(nullptr, false);
    Drawing::Brush brush;
    brush.SetShaderEffect(distortShader);
    if (distortionK_ < 0) {
        canvas.AttachBrush(brush);
        canvas.DrawRect(dst);
        canvas.DetachBrush();
        return;
    }
    canvas.DrawBackground(brush);
}
void RSDistortionFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    if (!image || image->GetWidth() <= 0 || image->GetHeight() <= 0) {
        ROSEN_LOGE("RSDistortionFilter::image error");
        return;
    }
    DrawDistortion(canvas, image, src, dst);
}
} // namespace Rosen
} // namespace OHOS