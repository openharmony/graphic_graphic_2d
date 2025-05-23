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
#include "render/rs_hdr_ui_brightness_filter.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "display_engine/rs_luminance_control.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float DEFAULT_HDR_UI_BRIGHTNESS = 1.0f;
constexpr float GAMMA2_2 = 2.2f;
}

RSHDRUIBrightnessFilter::RSHDRUIBrightnessFilter(float hdrUIBrightness)
    : RSDrawingFilterOriginal(nullptr), hdrUIBrightness_(hdrUIBrightness)
{
    type_ = FilterType::HDR_UI_BRIGHTNESS;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&hdrUIBrightness_, sizeof(hdrUIBrightness_), hash_);
}

RSHDRUIBrightnessFilter::~RSHDRUIBrightnessFilter() = default;

std::string RSHDRUIBrightnessFilter::GetDescription()
{
    return "RSHDRUIBrightnessFilter " + std::to_string(hdrUIBrightness_);
}

float RSHDRUIBrightnessFilter::GetHDRUIBrightness() const
{
    return hdrUIBrightness_;
}

void RSHDRUIBrightnessFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    if (!image) {
        ROSEN_LOGE("RSHDRUIBrightnessFilter::DrawImageRect image is nullptr");
        return;
    }
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    Drawing::ColorMatrix luminanceMatrix;
    RSPaintFilterCanvas& rscanvas = static_cast<RSPaintFilterCanvas&>(canvas);
    float hdrBrightnessRatio = RSLuminanceControl::Get().GetHdrBrightnessRatio(rscanvas.GetScreenId(), 0);
    float hdrUIBrightness = rscanvas.GetHdrOn() ? GetHDRUIBrightness() : DEFAULT_HDR_UI_BRIGHTNESS;
    float headroom = 1.0f / std::pow(hdrBrightnessRatio, GAMMA2_2);
    // clamp ratio from 1.0 to current headroom and convert ratio from linear domain to nonlinear domain
    hdrUIBrightness = std::pow(std::clamp(hdrUIBrightness, DEFAULT_HDR_UI_BRIGHTNESS, headroom), 1.0f / GAMMA2_2);
    RS_OPTIONAL_TRACE_NAME_FMT("RSHDRUIBrightnessFilter::DrawImageRect canvasHdrOn:%d, headroom:%f, "
        "nonLinearHDRUIBrightness:%f, linearHDRUIBrightness:%f",
        rscanvas.GetHdrOn(), headroom, hdrUIBrightness, hdrUIBrightness_);
    luminanceMatrix.SetScale(hdrUIBrightness, hdrUIBrightness, hdrUIBrightness, 1.0f);
    auto luminanceColorFilter = Drawing::ColorFilter::CreateMatrixColorFilter(luminanceMatrix,
        Drawing::Clamp::NO_CLAMP);
    filter.SetColorFilter(luminanceColorFilter);
    brush.SetFilter(filter);
    canvas.AttachBrush(brush);
    Drawing::SamplingOptions options(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    canvas.DrawImageRect(*image, src, dst, options);
    canvas.DetachBrush();
}
} // namespace Rosen
} // namespace OHOS