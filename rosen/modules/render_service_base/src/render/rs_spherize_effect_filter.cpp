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
#include "render/rs_spherize_effect_filter.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {
RSSpherizeEffectFilter::RSSpherizeEffectFilter(float spherizeDegree)
    : RSDrawingFilterOriginal(nullptr), spherizeDegree_(spherizeDegree)
{
    type_ = FilterType::SPHERIZE_EFFECT;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&spherizeDegree_, sizeof(spherizeDegree_), hash_);
}

RSSpherizeEffectFilter::~RSSpherizeEffectFilter() = default;

std::string RSSpherizeEffectFilter::GetDescription()
{
    return "RSSpherizeEffectFilter " + std::to_string(spherizeDegree_);
}

bool RSSpherizeEffectFilter::IsValid() const
{
    constexpr float epsilon = 0.001f;
    return spherizeDegree_ > epsilon;
}

float RSSpherizeEffectFilter::GetSpherizeDegree() const
{
    return spherizeDegree_;
}

Drawing::Brush RSSpherizeEffectFilter::GetBrush(const std::shared_ptr<Drawing::Image>& image) const
{
    Drawing::Brush brush;
    brush.SetBlendMode(Drawing::BlendMode::SRC_OVER);
    Drawing::SamplingOptions samplingOptions;
    Drawing::Matrix scaleMat;
    brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, scaleMat));
    return brush;
}

void RSSpherizeEffectFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    if (!spherizeDegree_ || !image || image->GetWidth() == 0 || image->GetHeight() == 0) {
        ROSEN_LOGE("RSSpherizeEffectFilter::shader error");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("DrawSpherize:%f", spherizeDegree_);
    int width = image->GetWidth();
    int height = image->GetHeight();
    bool isWidthGreater = width > height;

    auto brush = GetBrush(image);
    canvas.AttachBrush(brush);

    const Drawing::Point texCoords[4] = { { 0.0f, 0.0f }, { width, 0.0f }, { width, height }, { 0.0f, height } };
    float offsetSquare = 0.f;
    if (isWidthGreater) {
        offsetSquare = (width - height) * spherizeDegree_ / 2.0; // half of the change distance
        width = width - (width - height) * spherizeDegree_;
    } else {
        offsetSquare = (height - width) * spherizeDegree_ / 2.0; // half of the change distance
        height = height - (height - width) * spherizeDegree_;
    }

    float segmentWidthOne = width / 3.0; // Anchor point of 1 is located at one-third of the image width.
    float segmentWidthTwo = width / 3.0 * 2.0; // Anchor point 2 is located at two-thirds of the image width.
    float segmentHeightOne = height / 3.0; // Anchor point 1 is located at one-third of the height of the image.
    float segmentHeightTwo = height / 3.0 * 2.0; // Anchor point 2 is located at a height of two-thirds of the image.
    // The width moving distance of the four corner anchor points of the image.
    float offsetSphereWidth = width / 6 * spherizeDegree_;
    // The high moving distance of the four corner anchor points of the image.
    float offsetSphereHeight = height / 6  * spherizeDegree_;

    const int PointNum = 12; // 12 anchor points
    Drawing::Point ctrlPoints[PointNum] = {
        // top edge control points
        {0.0f, 0.0f}, {segmentWidthOne, 0.0f}, {segmentWidthTwo, 0.0f}, {width, 0.0f},
        // right edge control points
        {width, segmentHeightOne}, {width, segmentHeightTwo},
        // bottom edge control points
        {width, height}, {segmentWidthTwo, height}, {segmentWidthOne, height}, {0.0f, height},
        // left edge control points
        {0.0f, segmentHeightTwo}, {0.0f, segmentHeightOne}
    };
    ctrlPoints[0].Offset(offsetSphereWidth, offsetSphereHeight); // top left control point
    ctrlPoints[3].Offset(-offsetSphereWidth, offsetSphereHeight); // top right control point
    ctrlPoints[6].Offset(-offsetSphereWidth, -offsetSphereHeight); // bottom right control point
    ctrlPoints[9].Offset(offsetSphereWidth, -offsetSphereHeight); // bottom left control point
    if (isWidthGreater) {
        for (int i = 0; i < PointNum; ++i) {
            ctrlPoints[i].Offset(offsetSquare, 0);
        }
    } else {
        for (int i = 0; i < PointNum; ++i) {
            ctrlPoints[i].Offset(0, offsetSquare);
        }
    }
    Drawing::Path path;
    path.MoveTo(ctrlPoints[0].GetX(), ctrlPoints[0].GetY());
    path.CubicTo(ctrlPoints[1], ctrlPoints[2], ctrlPoints[3]); // upper edge
    path.CubicTo(ctrlPoints[4], ctrlPoints[5], ctrlPoints[6]); // right edge
    path.CubicTo(ctrlPoints[7], ctrlPoints[8], ctrlPoints[9]); // bottom edge
    path.CubicTo(ctrlPoints[10], ctrlPoints[11], ctrlPoints[0]); // left edge
    canvas.ClipPath(path, Drawing::ClipOp::INTERSECT, true);
    canvas.DrawPatch(ctrlPoints, nullptr, texCoords, Drawing::BlendMode::SRC_OVER);
    canvas.DetachBrush();
}
} // namespace Rosen
} // namespace OHOS