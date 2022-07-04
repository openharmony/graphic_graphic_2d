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

#include "include/core/SkColorFilter.h"
#include "include/core/SkColor.h"
#include "include/effects/SkColorMatrix.h"
#include "include/effects/SkImageFilters.h"

namespace OHOS {
namespace Rosen {
struct MaterialParam {
    float radius;
    float saturation;
    SkColor maskColor;
};

class RSMaterialFilter : public RSSkiaFilter {
public:
    RSMaterialFilter(int style, float dipScale);
    ~RSMaterialFilter() override;
    std::shared_ptr<RSFilter> Add(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Sub(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Multiply(float rhs) override;
    std::shared_ptr<RSFilter> Negate() override;
private:
    static constexpr int STYLE_CARD_THIN_LIGHT = 1;
    static constexpr int STYLE_CARD_LIGHT = 2;
    static constexpr int STYLE_CARD_THICK_LIGHT = 3;
    static constexpr MaterialParam CARDTHINLIGHT = {109.0f, 1.22, 0x6BF0F0F0};
    static constexpr MaterialParam CARDLIGHT = {103.0f, 2.4, 0xB8FAFAFA};
    static constexpr MaterialParam CARDTHICKLIGHT = {109.0f, 2.4, 0xB8FAFAFA};
    sk_sp<SkImageFilter> CreateMaterialStyle(int style, float dipScale);
    float RadiusVp2Sigma(float radiusVp, float dipScale) const;
    sk_sp<SkColorFilter> MaskColorFilter(SkColor maskColor);
    sk_sp<SkImageFilter> CreateMaterialFilter(float radius, float sat, SkColor maskColor);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H