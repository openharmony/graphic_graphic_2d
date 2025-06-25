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
#include "render/rs_colorful_shadow_filter.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "draw/surface.h"
#include "platform/common/rs_log.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
RSColorfulShadowFilter::RSColorfulShadowFilter(
    float blurRadius, float offsetX, float offsetY, Drawing::Path path, bool isFill)
    : RSForegroundEffectFilter(blurRadius), blurRadius_(blurRadius), offsetX_(offsetX), offsetY_(offsetY), path_(path),
      isFilled_(isFill)
{
    type_ = FilterType::COLORFUL_SHADOW;
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&type_, sizeof(type_), 0);
}

std::string RSColorfulShadowFilter::GetDescription()
{
    return "RSColorfulShadowFilter " + std::to_string(blurRadius_);
}

bool RSColorfulShadowFilter::IsValid() const
{
    constexpr float epsilon = 0.999f;  // if blur radius less than 1, do not need to draw
    return blurRadius_ > epsilon;
}

void RSColorfulShadowFilter::SetShadowColorMask(Color color)
{
    isColorMask_ = true;
    isFilled_ = true;
    color_ = color;
}

std::shared_ptr<Drawing::Image> RSColorfulShadowFilter::DrawImageRectWithColor(Drawing::Canvas &canvas,
    const std::shared_ptr<Drawing::Image> &image) const
{
    bool isInvalid = !image || image->GetWidth() == 0 || image->GetHeight() == 0;
    if (isInvalid) {
        ROSEN_LOGE("RSColorfulShadowFilter::DrawImageRect error");
        return nullptr;
    }

    RS_OPTIONAL_TRACE_NAME("ApplyaShadowColorFilter");
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRenderTarget(canvas.GetGPUContext().get(),
        false, image->GetImageInfo());
    if (!surface) {
        ROSEN_LOGE("Null surface");
        return nullptr;
    }

    Drawing::Brush brush;
    Drawing::Filter filter;
    filter.SetColorFilter(Drawing::ColorFilter::CreateBlendModeColorFilter(color_.AsArgbInt(),
        Drawing::BlendMode::SRC_IN));
    brush.SetFilter(filter);
    auto canvas1 = surface->GetCanvas();
    if (canvas1 == nullptr) {
        return nullptr;
    }
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    canvas1->AttachBrush(brush);
    canvas1->DrawImage(*image, 0.f, 0.f, samplingOptions);
    canvas1->DetachBrush();
    return surface->GetImageSnapshot();
}

void RSColorfulShadowFilter::DrawImageRect(Drawing::Canvas &canvas, const std::shared_ptr<Drawing::Image> &image,
    const Drawing::Rect &src, const Drawing::Rect &dst) const
{
    if (image == nullptr) {
        ROSEN_LOGE("RSColorfulShadowFilter::DrawImageRect error");
        return;
    }

    if (IsValid()) {
        Drawing::AutoCanvasRestore acr(canvas, true);
        if (!isFilled_) {
            canvas.ClipPath(path_, Drawing::ClipOp::DIFFERENCE, true);
        }
        // draw blur image
        canvas.Translate(offsetX_, offsetY_);
        std::shared_ptr<Drawing::Image> imageTemp = image;
        if (isColorMask_) {
            imageTemp = DrawImageRectWithColor(canvas, image);
            imageTemp = imageTemp == nullptr ? image : imageTemp;
        }
        RSForegroundEffectFilter::DrawImageRect(canvas, imageTemp, src, dst);
    }

    // draw clear image
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    canvas.DrawImage(*image, 0.f, 0.f, samplingOptions);
}
}  // namespace Rosen
}  // namespace OHOS
