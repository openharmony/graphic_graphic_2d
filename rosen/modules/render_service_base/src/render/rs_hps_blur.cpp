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

#include "render/rs_hps_blur.h"

#include "common/rs_optional_trace.h"
#include "draw/surface.h"
#include "effect/runtime_shader_builder.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"


namespace OHOS {
namespace Rosen {

static constexpr uint32_t MAX_SURFACE_SIZE = 10000;

Drawing::Matrix HpsBlurFilter::GetShaderTransform(const Drawing::Rect& blurRect, float scaleW, float scaleH)
{
    Drawing::Matrix matrix;
    matrix.SetScale(scaleW, scaleH);
    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(blurRect.GetLeft(), blurRect.GetTop());
    matrix.PostConcat(translateMatrix);
    return matrix;
}

bool HpsBlurFilter::ApplyHpsBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::HpsBlurParameter& param, float alpha, std::shared_ptr<Drawing::ColorFilter> colorFilter) const
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr || image == nullptr) {
        return false;
    }
    std::array<int, 2> dimension = canvas.CalcHpsBluredImageDimension(param); // There are 2 variables
    if (dimension[0] <= 0 || dimension[1] <= 0 || dimension[0] >= static_cast<int>(MAX_SURFACE_SIZE)
        || dimension[1] >= static_cast<int>(MAX_SURFACE_SIZE)) {
        ROSEN_LOGD("HpsBlurFilter::ApplyHpsBlur CalcHpsBluredImageDimension error");
        return false;
    }
    std::shared_ptr<Drawing::Surface> offscreenSurface = surface->MakeSurface(dimension[0], dimension[1]);
    if (offscreenSurface == nullptr) {
        return false;
    }

    std::shared_ptr<Drawing::Canvas> offscreenCanvas = offscreenSurface->GetCanvas();
    if (offscreenCanvas == nullptr) {
        return false;
    }
    Drawing::Rect dimensionRect = {0, 0, dimension[0], dimension[1]};
    auto offscreenHpsParam = Drawing::HpsBlurParameter(param.src, dimensionRect, param.sigma,
        param.saturation, param.brightness);
    if (!offscreenCanvas->DrawBlurImage(*image, offscreenHpsParam)) {
        return false;
    }

    auto imageCache = offscreenSurface->GetImageSnapshot();
    if (imageCache == nullptr) {
        return false;
    }
    auto dst = param.dst;
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    const auto blurMatrix = GetShaderTransform(dst, dst.GetWidth() / imageCache->GetWidth(),
        dst.GetHeight() / imageCache->GetHeight());
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(*imageCache, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, blurMatrix);
    Drawing::Brush brush;
    if (colorFilter != nullptr) {
        Drawing::Filter filter;
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);
    }
    brush.SetAlphaF(alpha);
    brush.SetShaderEffect(blurShader);
    brush.SetForceBrightnessDisable(true);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
    return true;
}
} // namespace Rosen
} // namespace OHOS