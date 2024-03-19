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

#include "draw/surface.h"
#include "utils/gradient_blur_utils.h"
#include "render/rs_blur_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr static float FLOAT_ZERO_THRESHOLD = 0.001f;
} // namespace

RSLinearGradientBlurFilter::RSLinearGradientBlurFilter(const std::shared_ptr<RSLinearGradientBlurPara>& para,
    const float geoWidth, const float geoHeight)
    : RSDrawingFilter(nullptr),
      linearGradientBlurPara_(para)
{
    geoWidth_ = geoWidth;
    geoHeight_ = geoHeight;
    type_ = FilterType::LINEAR_GRADIENT_BLUR;
    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&linearGradientBlurPara_, sizeof(linearGradientBlurPara_), hash_);
}

RSLinearGradientBlurFilter::~RSLinearGradientBlurFilter() = default;

void RSLinearGradientBlurFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    auto& para = linearGradientBlurPara_;
    if (!image || para == nullptr || para->blurRadius_ <= 0) {
        return;
    }

    RS_OPTIONAL_TRACE_NAME("DrawLinearGradientBlur");
    uint8_t directionBias = CalcDirectionBias(mat_);
    auto clipIPadding = ComputeRectBeforeClip(directionBias, dst);
    // constructive gradient blur param
    Drawing::GradientBlurParam gradientBlurParam;
    gradientBlurParam.blurRadius_ = para->blurRadius_;
    gradientBlurParam.fractionStops_ = para->fractionStops_;
    gradientBlurParam.direction_ = static_cast<Drawing::GradientDir>(static_cast<uint8_t>(para->direction_));
    gradientBlurParam.useMaskAlgorithm_ = para->useMaskAlgorithm_;
    gradientBlurParam.originalBase_ = para->originalBase_;
    auto& rsFilter = para->LinearGradientBlurFilter_;
    if (rsFilter) {
        auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter)->GetImageFilter();
        if (filter) {
            gradientBlurParam.imageFilterBlurRadius_ =
                std::static_pointer_cast<RSBlurFilter>(rsFilter)->GetBlurRadiusX();
        }
        gradientBlurParam.blurImageFilter_ = filter;
    }
    gradientBlurParam.clipIPadding_ = clipIPadding;
    gradientBlurParam.directionBias_ = directionBias;
    gradientBlurParam.imageScale_ = imageScale_;
    gradientBlurParam.maskLinearBlurEnable_ = RSSystemProperties::GetMaskLinearBlurEnabled();

    Drawing::GradientBlurUtils gradientBlurUtils;
    gradientBlurUtils.DrawGradientBlur(canvas, image, src, dst, gradientBlurParam);
}

Drawing::Rect RSLinearGradientBlurFilter::ComputeRectBeforeClip(const uint8_t directionBias, const Drawing::Rect& dst)
{
    auto clipIPadding = dst;
    float width = geoWidth_;
    float height = geoHeight_;
    if (directionBias == 1 || directionBias == 3) { // 1 3: rotation 0 270
        width = geoHeight_;
        height = geoWidth_;
    }

    if (height - dst.GetHeight() > 1) {
        if (mat_.Get(Drawing::Matrix::TRANS_Y) > surfaceHeight_ / 2) {  // 2 half of height
            clipIPadding = Drawing::Rect(dst.GetLeft(), dst.GetTop(), dst.GetRight(), dst.GetTop() + height);
        } else {
            clipIPadding = Drawing::Rect(dst.GetLeft(), dst.GetBottom() - height, dst.GetRight(), dst.GetBottom());
        }
    }
    if (width - dst.GetWidth() > 1) {
        if (mat_.Get(Drawing::Matrix::TRANS_X) > surfaceWidth_ / 2) {   // 2 half of width
            clipIPadding = Drawing::Rect(clipIPadding.GetLeft(), clipIPadding.GetTop(), clipIPadding.GetLeft() + width,
                clipIPadding.GetBottom());
        } else {
            clipIPadding = Drawing::Rect(clipIPadding.GetRight() - width, clipIPadding.GetTop(),
                clipIPadding.GetRight(), clipIPadding.GetBottom());
        }
    }
    return clipIPadding;
}

uint8_t RSLinearGradientBlurFilter::CalcDirectionBias(const Drawing::Matrix& mat)
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

std::string RSLinearGradientBlurFilter::GetDescription()
{
    return "RSLinearGradientBlurFilter";
}
} // namespace Rosen
} // namespace OHOS
