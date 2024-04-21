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

#include "render/rs_motion_blur_filter.h"

#include "common/rs_optional_trace.h"
#include "draw/surface.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr static float FLOAT_ZERO_THRESHOLD = 0.01f; // 0.01f threshold
} // namespace

std::shared_ptr<Drawing::RuntimeEffect> RSMotionBlurFilter::motionBlurShaderEffect_ = nullptr;

RSMotionBlurFilter::RSMotionBlurFilter(const std::shared_ptr<MotionBlurParam>& para)
    : RSDrawingFilter(nullptr), motionBlurPara_(para)
{
    type_ = FilterType::MOTION_BLUR;
    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&motionBlurPara_, sizeof(motionBlurPara_), hash_);
}

RSMotionBlurFilter::~RSMotionBlurFilter() = default;

void RSMotionBlurFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    if (!image || image->GetWidth() == 0 || image->GetHeight() == 0) {
        lastRect_ = Drawing::Rect(curRect_.GetLeft(), curRect_.GetTop(), curRect_.GetRight(), curRect_.GetBottom());
        ROSEN_LOGE("RSMotionBlurFilter::image error");
        return;
    }

    auto para = motionBlurPara_;
    if (!RSSystemProperties::GetMotionBlurEnabled() || para == nullptr || para->radius <= 0) {
        lastRect_ = Drawing::Rect(curRect_.GetLeft(), curRect_.GetTop(), curRect_.GetRight(), curRect_.GetBottom());
        OutputOriginalImage(canvas, image, src, dst);
        return;
    }

    if (!RectValid(lastRect_, curRect_)) {
        lastRect_ = Drawing::Rect(curRect_.GetLeft(), curRect_.GetTop(), curRect_.GetRight(), curRect_.GetBottom());
        OutputOriginalImage(canvas, image, src, dst);
        return;
    }

    RS_OPTIONAL_TRACE_NAME("DrawMotionBlur");
    Vector2f rectOffset;
    Vector2f scaleSize;
    rectOffset[0] = curRect_.GetLeft() - lastRect_.GetLeft() +
        (curRect_.GetWidth() - lastRect_.GetWidth()) * para->scaleAnchor[0];
    rectOffset[1] = curRect_.GetTop() - lastRect_.GetTop() +
        (curRect_.GetHeight() - lastRect_.GetHeight()) * para->scaleAnchor[1];
    scaleSize[0] = curRect_.GetWidth() / lastRect_.GetWidth();
    scaleSize[1] = curRect_.GetHeight() / lastRect_.GetHeight();

    if (curRect_.GetWidth() < lastRect_.GetWidth() && curRect_.GetHeight() < lastRect_.GetHeight()) {
        rectOffset[0] = -rectOffset[0];
        rectOffset[1] = -rectOffset[1];
        scaleSize[0] = 1.0 / scaleSize[0];
        scaleSize[1] = 1.0 / scaleSize[1];
    }

    Vector2f scaleAnchorCoord = {para->scaleAnchor[0] * curRect_.GetWidth(),
        para->scaleAnchor[1] * curRect_.GetHeight()};

    lastRect_ = Drawing::Rect(curRect_.GetLeft(), curRect_.GetTop(), curRect_.GetRight(), curRect_.GetBottom());

    Drawing::Matrix inputMatrix;
    inputMatrix.Translate(-src.GetLeft(), -src.GetTop());
    inputMatrix.PostScale(dst.GetWidth() / image->GetWidth(), dst.GetHeight() / image->GetHeight());
    Drawing::Matrix matrix;
    matrix.Translate(dst.GetLeft(), dst.GetTop());
    inputMatrix.PostConcat(matrix);
    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::DECAL,
        Drawing::TileMode::DECAL, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), inputMatrix);
    auto shader = MakeMotionBlurShader(srcImageShader, scaleAnchorCoord, scaleSize, rectOffset, para->radius);

    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::ShaderEffect> RSMotionBlurFilter::MakeMotionBlurShader(
    std::shared_ptr<Drawing::ShaderEffect> srcImageShader, Vector2f& scaleAnchor, Vector2f& scaleSize,
    Vector2f& rectOffset, float radius)
{
    static const char* MotionBlurProg = R"(
        uniform shader srcImageShader;
        uniform float2 scaleAnchor;
        uniform float2 scaleSize;
        uniform float2 rectOffset;
        uniform float radius;

        half4 main(float2 coord)
        {
            const float num = 8.0; // 8.0 sample times
            float2 scaleSizeStep = (scaleSize - 1.0) / num * radius;
            float2 rectOffsetStep = rectOffset / num * radius;
            float2 samplingOffset = (coord - scaleAnchor) * scaleSizeStep + rectOffsetStep;

            half4 color = srcImageShader.eval(coord) * 0.18; // 0.18 alpha
            color += srcImageShader.eval(coord + samplingOffset) * 0.15; // 0.15 alpha
            color += srcImageShader.eval(coord + samplingOffset * 2) * 0.12; // 0.12 alpha, 2 times
            color += srcImageShader.eval(coord + samplingOffset * 3) * 0.09; // 0.09 alpha, 3 times
            color += srcImageShader.eval(coord + samplingOffset * 4) * 0.05; // 0.05 alpha, 4 times
            color += srcImageShader.eval(coord + samplingOffset * 5) * 0.15; // 0.15 alpha, 5 times
            color += srcImageShader.eval(coord + samplingOffset * 6) * 0.12; // 0.12 alpha, 6 times
            color += srcImageShader.eval(coord + samplingOffset * 7) * 0.09; // 0.09 alpha, 7 times
            color += srcImageShader.eval(coord + samplingOffset * 8) * 0.05; // 0.05 alpha, 8 times

            return color;
        }
    )";

    if (motionBlurShaderEffect_ == nullptr) {
        motionBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(MotionBlurProg);
        if (motionBlurShaderEffect_ == nullptr) {
            return nullptr;
        }
    }

    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(motionBlurShaderEffect_);
    builder->SetChild("srcImageShader", srcImageShader);
    builder->SetUniform("scaleAnchor", scaleAnchor[0], scaleAnchor[1]);
    builder->SetUniform("scaleSize", scaleSize[0], scaleSize[1]);
    builder->SetUniform("rectOffset", rectOffset[0], rectOffset[1]);
    builder->SetUniform("radius", radius);
    return builder->MakeShader(nullptr, false);
}

bool RSMotionBlurFilter::RectValid(const Drawing::Rect& rect1, const Drawing::Rect& rect2)
{
    if (rect1.GetWidth() < 1 || rect1.GetHeight() < 1 || rect2.GetWidth() < 1 || rect2.GetHeight() < 1) {
        return false;
    }

    if (fabs(rect1.GetWidth() - rect2.GetWidth()) <= FLOAT_ZERO_THRESHOLD &&
        fabs(rect1.GetHeight() - rect2.GetHeight()) <= FLOAT_ZERO_THRESHOLD) {
        return false;
    }

    return true;
}

void RSMotionBlurFilter::OutputOriginalImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!image || image->GetWidth() == 0 || image->GetHeight() == 0) {
        ROSEN_LOGE("RSMotionBlurFilter::OutputOriginalImage image error");
        return;
    }
    Drawing::Matrix inputMatrix;
    inputMatrix.Translate(-src.GetLeft(), -src.GetTop());
    inputMatrix.PostScale(dst.GetWidth() / image->GetWidth(), dst.GetHeight() / image->GetHeight());

    Drawing::Matrix matrix;
    matrix.Translate(dst.GetLeft(), dst.GetTop());
    inputMatrix.PostConcat(matrix);
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    const auto inputShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, inputMatrix);

    Drawing::Brush brush;
    brush.SetShaderEffect(inputShader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}

std::string RSMotionBlurFilter::GetDescription()
{
    return "RSMotionBlurFilter";
}
} // namespace Rosen
} // namespace OHOS
