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
enum MATERIAL_BLUR_STYLE : int {
    // card blur style
    STYLE_CARD_THIN_LIGHT  = 1,
    STYLE_CARD_LIGHT       = 2,
    STYLE_CARD_THICK_LIGHT = 3,
    STYLE_CARD_THIN_DARK   = 4,
    STYLE_CARD_DARK        = 5,
    STYLE_CARD_THICK_DARK  = 6,

    // background blur style
    STYLE_BACKGROUND_SMALL_LIGHT  = 101,
    STYLE_BACKGROUND_MEDIUM_LIGHT = 102,
    STYLE_BACKGROUND_LARGE_LIGHT  = 103,
    STYLE_BACKGROUND_XLARGE_LIGHT = 104,
    STYLE_BACKGROUND_SMALL_DARK   = 105,
    STYLE_BACKGROUND_MEDIUM_DARK  = 106,
    STYLE_BACKGROUND_LARGE_DARK   = 107,
    STYLE_BACKGROUND_XLARGE_DARK  = 108
};

class RSMaterialFilter : public RSSkiaFilter {
public:
    RSMaterialFilter(int style, float dipScale, BLUR_COLOR_MODE mode);
    ~RSMaterialFilter() override;
    void PreProcess(sk_sp<SkImage> image) override;
    void PostProcess(RSPaintFilterCanvas& canvas) override;

    std::shared_ptr<RSFilter> Add(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Sub(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Multiply(float rhs) override;
    std::shared_ptr<RSFilter> Negate() override;
private:
    float dipScale_;
    MATERIAL_BLUR_STYLE style_;
    BLUR_COLOR_MODE colorMode_;
    SkColor maskColor_;

    sk_sp<SkImageFilter> CreateMaterialStyle(MATERIAL_BLUR_STYLE style, float dipScale);
    sk_sp<SkImageFilter> CreateMaterialFilter(float radius, float sat, SkColor maskColor);
    static float RadiusVp2Sigma(float radiusVp, float dipScale);

    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H