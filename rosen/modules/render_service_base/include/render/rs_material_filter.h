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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MATERIAL_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MATERIAL_FILTER_H

#include "render/rs_skia_filter.h"

#include "include/effects/SkImageFilters.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkColor.h"
#include "include/effects/SkColorMatrix.h"

namespace OHOS {
namespace Rosen {
class RSMaterialFilter : public RSSkiaFilter {
public:
    public static final int STYLE_CARD_THIN_LIGHT = 1;
    public static final int STYLE_CARD_LIGHT = 2;
    public static final int STYLE_CARD_THICK_LIGHT = 3;
    constexpr int RADIUSVP_THIN_LIGHT = 109;
    constexpr float SATURATION_THIN_LIGHT= 1.2;
    constexpr SkColor MASKCOLOR_THIN_LIGHT = 0x6BF0F0F0;
    constexpr int RADIUSVP_LIGHT = 103;
    constexpr float SATURATION_LIGHT = 2.4;
    constexpr SkColor MASKCOLOR_LIGHT = 0xB8FAFAFA;
    constexpr int RADIUSVP_THICK_LIGHT = 109;
    constexpr float SATURATION_THICK_LIGHT = 2.4;
    constexpr SkColor MASKCOLOR_THICK_LIGHT = 0xB8FAFAFA;
    RSMaterialFilter(int style, float dipScale);
    ~RSMaterialFilter() override;
    
    float vp2sigma(float radiusVp, float dipScale) const;
    sk_sp<SkColorFilter> maskColorFilter(SkColor maskColor);
    sk_sp<SkImageFilter> createMaterialfilter(float radius, float sat, SkColor maskColor);
    sk_sp<SkImageFilter> createMaterialStyle(int style, float dipScale);
    std::shared_ptr<RSFilter> Add(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Sub(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Multiply(float rhs) override;
    std::shared_ptr<RSFilter> Negate() override;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H