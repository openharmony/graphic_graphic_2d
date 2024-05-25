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
constexpr static float FLOAT_SCALE_THRESHOLD = 1.1f; // 1.1f scale threshold
constexpr static float FLOAT_IMAGE_SCALE = 0.5f;    // 0.5f image scale
} // namespace

std::shared_ptr<Drawing::RuntimeEffect> RSMotionBlurFilter::motionBlurShaderEffect_ = nullptr;

RSMotionBlurFilter::RSMotionBlurFilter(const std::shared_ptr<MotionBlurParam>& para)
    : RSDrawingFilterOriginal(nullptr), motionBlurPara_(para)
{
    type_ = FilterType::MOTION_BLUR;
    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&motionBlurPara_, sizeof(motionBlurPara_), hash_);
}

RSMotionBlurFilter::~RSMotionBlurFilter() = default;

void RSMotionBlurFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    RS_OPTIONAL_TRACE_NAME("RSMotionBlurFilter::MotionBlur");
    Drawing::Matrix mat = canvas.GetTotalMatrix();
    Drawing::Rect rect = canvas.GetDeviceClipBounds();
    mat.MapRect(rect, rect);
    curRect_ = Drawing::Rect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());

    if (!image || image->GetWidth() == 0 || image->GetHeight() == 0) {
        lastRect_ = Drawing::Rect(curRect_.GetLeft(), curRect_.GetTop(), curRect_.GetRight(), curRect_.GetBottom());
        ROSEN_LOGE("RSMotionBlurFilter::image error");
        return;
    }

    if (!RSSystemProperties::GetMotionBlurEnabled() || motionBlurPara_ == nullptr || motionBlurPara_->radius <= 0) {
        lastRect_ = Drawing::Rect(0.f, 0.f, 0.f, 0.f);
        OutputOriginalImage(canvas, image, src, dst);
        return;
    }

    if (!RectValid(lastRect_, curRect_)) {
        lastRect_ = Drawing::Rect(curRect_.GetLeft(), curRect_.GetTop(), curRect_.GetRight(), curRect_.GetBottom());
        OutputOriginalImage(canvas, image, src, dst);
        return;
    }

    DrawMotionBlur(canvas, image, src, dst);
}

void RSMotionBlurFilter::DrawMotionBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    if (motionBlurPara_ == nullptr) {
        lastRect_ = Drawing::Rect(0.f, 0.f, 0.f, 0.f);
        OutputOriginalImage(canvas, image, src, dst);
        return;
    }

    RS_OPTIONAL_TRACE_NAME("DrawMotionBlur");
    Vector2f rectOffset;
    Vector2f scaleSize;
    Vector2f scaleAnchorCoord;
    CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);

    Drawing::Matrix inputMatrix;
    inputMatrix.Translate(-src.GetLeft(), -src.GetTop());
    inputMatrix.PostScale(FLOAT_IMAGE_SCALE, FLOAT_IMAGE_SCALE);
    Drawing::Matrix matrix;
    matrix.Translate(dst.GetLeft(), dst.GetTop());
    inputMatrix.PostConcat(matrix);
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::DECAL,
        Drawing::TileMode::DECAL, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), inputMatrix);
    auto builder = MakeMotionBlurShader(imageShader, scaleAnchorCoord, scaleSize, rectOffset, motionBlurPara_->radius);

    auto originImageInfo = image->GetImageInfo();
    auto scaledInfo = Drawing::ImageInfo(std::ceil(image->GetWidth() * FLOAT_IMAGE_SCALE),
        std::ceil(image->GetHeight() * FLOAT_IMAGE_SCALE), originImageInfo.GetColorType(),
        originImageInfo.GetAlphaType(), originImageInfo.GetColorSpace());
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    std::shared_ptr<Drawing::Image> tmpBlur(builder->MakeImage(
        canvas.GetGPUContext().get(), nullptr, scaledInfo, false));

    float invBlurScale = 1.0f / FLOAT_IMAGE_SCALE;
    Drawing::Matrix invBlurMatrix;
    invBlurMatrix.PostScale(invBlurScale, invBlurScale);
    auto shader = Drawing::ShaderEffect::CreateImageShader(*tmpBlur, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, invBlurMatrix);

    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSMotionBlurFilter::MakeMotionBlurShader(
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
            const float num = 6.0; // 6.0 sample times
            float2 scaleSizeStep = (scaleSize - 1.0) / num * radius;
            float2 rectOffsetStep = rectOffset / num * radius;
            float2 samplingOffset = (coord - scaleAnchor) * scaleSizeStep + rectOffsetStep;

            float2 coord1 = coord + samplingOffset;
            float2 coord2 = coord + samplingOffset * 2; // 2 times
            float2 coord3 = coord + samplingOffset * 3; // 3 times
            float2 coord4 = coord + samplingOffset * 4; // 4 times
            float2 coord5 = coord + samplingOffset * 5; // 5 times
            float2 coord6 = coord + samplingOffset * 6; // 6 times
            half4 color = srcImageShader.eval(coord) * 0.11; // 0.11 alpha
            color += srcImageShader.eval(coord1) * 0.12; // 0.12 alpha
            color += srcImageShader.eval(coord2) * 0.13; // 0.13 alpha
            color += srcImageShader.eval(coord3) * 0.14; // 0.14 alpha
            color += srcImageShader.eval(coord4) * 0.15; // 0.15 alpha
            color += srcImageShader.eval(coord5) * 0.16; // 0.16 alpha
            color += srcImageShader.eval(coord6) * 0.19; // 0.19 alpha

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
    return builder;
}

void RSMotionBlurFilter::CaculateRect(Vector2f& rectOffset, Vector2f& scaleSize, Vector2f& scaleAnchorCoord) const
{
    rectOffset[0] = curRect_.GetLeft() - lastRect_.GetLeft() +
        (curRect_.GetWidth() - lastRect_.GetWidth()) * motionBlurPara_->scaleAnchor[0];
    rectOffset[1] = curRect_.GetTop() - lastRect_.GetTop() +
        (curRect_.GetHeight() - lastRect_.GetHeight()) * motionBlurPara_->scaleAnchor[1];
    scaleSize[0] = curRect_.GetWidth() / lastRect_.GetWidth();
    scaleSize[1] = curRect_.GetHeight() / lastRect_.GetHeight();

    if (curRect_.GetWidth() < lastRect_.GetWidth() && curRect_.GetHeight() < lastRect_.GetHeight()) {
        rectOffset[0] = -rectOffset[0];
        rectOffset[1] = -rectOffset[1];
        scaleSize[0] = 1.0 / scaleSize[0];
        scaleSize[1] = 1.0 / scaleSize[1];
    }

    if (scaleSize[0] > FLOAT_SCALE_THRESHOLD || scaleSize[1] > FLOAT_SCALE_THRESHOLD) {
        rectOffset[0] = rectOffset[0] * FLOAT_SCALE_THRESHOLD / scaleSize[0];
        rectOffset[1] = rectOffset[1] * FLOAT_SCALE_THRESHOLD / scaleSize[1];
        scaleSize[0] = FLOAT_SCALE_THRESHOLD;
        scaleSize[1] = FLOAT_SCALE_THRESHOLD;
    }
    rectOffset[0] = rectOffset[0] * FLOAT_IMAGE_SCALE;
    rectOffset[1] = rectOffset[1] * FLOAT_IMAGE_SCALE;

    scaleAnchorCoord = {motionBlurPara_->scaleAnchor[0] * curRect_.GetWidth() * FLOAT_IMAGE_SCALE,
        motionBlurPara_->scaleAnchor[1] * curRect_.GetHeight() * FLOAT_IMAGE_SCALE};

    lastRect_ = Drawing::Rect(curRect_.GetLeft(), curRect_.GetTop(), curRect_.GetRight(), curRect_.GetBottom());
}

bool RSMotionBlurFilter::RectValid(const Drawing::Rect& rect1, const Drawing::Rect& rect2)
{
    if (rect1.GetWidth() < 1 || rect1.GetHeight() < 1 || rect2.GetWidth() < 1 || rect2.GetHeight() < 1) {
        return false;
    }

    if (rect1 == rect2) {
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
