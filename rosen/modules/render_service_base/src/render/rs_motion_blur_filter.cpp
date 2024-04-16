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
#include "src/core/SkOpts.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "common/rs_optional_trace.h"
#include "draw/surface.h"

namespace OHOS {
namespace Rosen {

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
    if (!RSSystemProperties::GetMotionBlurEnabled()) {
        OutputOriginalImage(canvas, image, src, dst);
        return;
    }

    auto para = motionBlurPara_;
    if (!image || para == nullptr || para->radius <= 0) {
        OutputOriginalImage(canvas, image, src, dst);
        return;
    }

    RS_OPTIONAL_TRACE_NAME("DrawMotionBlur");
    if (!RectValid(lastRect_, curRect_)) {
        lastRect_ = Drawing::Rect(curRect_.GetLeft(), curRect_.GetTop(), curRect_.GetRight(), curRect_.GetBottom());
        OutputOriginalImage(canvas, image, src, dst);
        return;
    }

    Vector2f rectOffset;
    Vector2f scaleSize;
    rectOffset[0] = curRect_.GetLeft() - lastRect_.GetLeft() +
        (curRect_.GetWidth() - lastRect_.GetWidth()) * para->scaleAnchor[0];
    rectOffset[1] = curRect_.GetTop() - lastRect_.GetTop() +
        (curRect_.GetHeight() - lastRect_.GetHeight()) * para->scaleAnchor[1];
    scaleSize[0] = curRect_.GetWidth() / lastRect_.GetWidth();
    scaleSize[1] = curRect_.GetHeight() / lastRect_.GetHeight();

    if (curRect_.GetWidth() < lastRect_.GetWidth()) {
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
            const float num = 7.0;
            float2 scaleSizeStep = (scaleSize - 1.0) / num * radius;
            float2 rectOffsetStep = rectOffset / num * radius;

            float2 samplingOffset = (coord - scaleAnchor) * scaleSizeStep  + rectOffsetStep;
            half4 color = srcImageShader.eval(coord) * 0.09;
            color += srcImageShader.eval(coord + samplingOffset) * 0.13;
            color += srcImageShader.eval(coord + samplingOffset * 2) * 0.17;
            color += srcImageShader.eval(coord + samplingOffset * 3) * 0.22;
            color += srcImageShader.eval(coord + samplingOffset * 4) * 0.17;
            color += srcImageShader.eval(coord + samplingOffset * 5) * 0.13;
            color += srcImageShader.eval(coord + samplingOffset * 6) * 0.09;
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

    if (fabs(rect1.GetWidth() - rect2.GetWidth()) <= 1 && fabs(rect1.GetHeight() - rect2.GetHeight()) <= 1) {
        return false;
    }

    return true;
}

void RSMotionBlurFilter::OutputOriginalImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
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
