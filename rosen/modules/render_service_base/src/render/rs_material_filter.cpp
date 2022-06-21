/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "render/rs_material_filter.h"

#include "render/rs_blur_filter.h"

namespace OHOS {
namespace Rosen {

constexpr float BLUR_SIGMA_SCALE = 0.57735f;

RSMaterialFilter::RSMaterialFilter(int style, float dipScale)
    : RSSkiaFilter(RSMaterialFilter::createMaterialStyle(style, dipScale))
{
    type_ = FilterType::MATERIAL;
}

RSMaterialFilter::~RSMaterialFilter() {}

float RSMaterialFilter::vp2sigma(float radiusVp, float dipScale) const
{
    float radiusPx = radiusVp * dipScale;

    return radiusPx > 0.0f ? BLUR_SIGMA_SCALE * radiusPx + SK_ScalarHalf : 0.0f;
}

sk_sp<SkColorFilter> RSMaterialFilter::maskColorFilter(SkColor maskColor)
{
    SkColor4f maskColor4f = SkColor4f::FromColor(maskColor);

    SkScalar colorMatrix[20] = { 0 };

    colorMatrix[0] = 1 - maskColor4f.fA;
    colorMatrix[6] = 1 - maskColor4f.fA;
    colorMatrix[12] = 1 - maskColor4f.fA;

    colorMatrix[4] = maskColor4f.fR * maskColor4f.fA;
    colorMatrix[9] = maskColor4f.fG * maskColor4f.fA;
    colorMatrix[14] = maskColor4f.fB * maskColor4f.fA;

    colorMatrix[18] = 1;
    return SkColorFilters::Matrix(colorMatrix);
}

sk_sp<SkImageFilter> RSMaterialFilter::createMaterialfilter(float radius, float sat, SkColor maskColor)
{
    sk_sp<SkImageFilter> blurFilter =
        std::static_pointer_cast<RSBlurFilter>(RSFilter::CreateBlurFilter(radius, radius))->GetBlurFilter(); // blur
    SkColorMatrix cm;
    cm.setSaturation(sat);
    sk_sp<SkColorFilter> satFilter = SkColorFilters::Matrix(cm);                    // saturation
    sk_sp<SkColorFilter> maskFilter = RSMaterialFilter::maskColorFilter(maskColor); // mask
    sk_sp<SkColorFilter> filterCompose = SkColorFilters::Compose(maskFilter, satFilter);

    return SkImageFilters::ColorFilter(filterCompose, blurFilter);
}

sk_sp<SkImageFilter> RSMaterialFilter::createMaterialStyle(int style, float dipScale)
{
    switch (style) {
        case 1:
            // cardThinLight
            return RSMaterialFilter::createMaterialfilter(RSMaterialFilter::vp2sigma(109, dipScale), 1.22, 0x6BF0F0F0);
        case 2:
            // cardLight
            return RSMaterialFilter::createMaterialfilter(RSMaterialFilter::vp2sigma(103, dipScale), 2.4, 0xB8FAFAFA);
        case 3:
            // cardThickLight
            return RSMaterialFilter::createMaterialfilter(RSMaterialFilter::vp2sigma(109, dipScale), 2.4, 0xB8FAFAFA);
        default:
            break;
    }
    return nullptr;
}

std::shared_ptr<RSFilter> RSMaterialFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    return shared_from_this();
    ;
}

std::shared_ptr<RSFilter> RSMaterialFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    return shared_from_this();
    ;
}

std::shared_ptr<RSFilter> RSMaterialFilter::Multiply(float rhs)
{
    return shared_from_this();
    ;
}

std::shared_ptr<RSFilter> RSMaterialFilter::Negate()
{
    return shared_from_this();
    ;
}
} // namespace Rosen
} // namespace OHOS
