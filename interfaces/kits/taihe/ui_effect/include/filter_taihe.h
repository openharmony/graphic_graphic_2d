/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef TAIHE_INCLUDE_UIEFFECT_FILTER_TAIHE_H
#define TAIHE_INCLUDE_UIEFFECT_FILTER_TAIHE_H

#include "filter/include/filter.h"
#include "ohos.graphics.uiEffect.proj.hpp"
#include "ohos.graphics.uiEffect.impl.hpp"
#include "ohos.graphics.uiEffect.uiEffect.BrightnessBlender.proj.1.hpp"
#include "ohos.graphics.uiEffect.uiEffect.VisualEffect.proj.1.hpp"
#include "ohos.graphics.uiEffect.uiEffect.TileMode.proj.1.hpp"
#include "ohos.graphics.uiEffect.uiEffect.proj.hpp"
#include "ohos.graphics.uiEffect.uiEffect.impl.hpp"
#include "stdexcept"
#include "taihe/array.hpp"
#include "filter/include/filter_bezier_warp_para.h"
#include "filter/include/filter_blur_para.h"
#include "filter/include/filter_color_gradient_para.h"
#include "filter/include/filter_content_light_para.h"
#include "filter/include/filter_direction_light_para.h"
#include "filter/include/filter_dispersion_para.h"
#include "filter/include/filter_displacement_distort_para.h"
#include "filter/include/filter_distort_para.h"
#include "filter/include/filter_edge_light_para.h"
#include "filter/include/filter_fly_out_para.h"
#include "filter/include/filter_hdr_para.h"
#include "filter/include/filter_mask_transition_para.h"
#include "filter/include/filter_pixel_stretch_para.h"
#include "filter/include/filter_radius_gradient_blur_para.h"
#include "filter/include/filter_variable_radius_blur_para.h"
#include "filter/include/filter_water_ripple_para.h"

namespace ANI::UIEffect {
using namespace taihe;
using namespace ohos::graphics::uiEffect;
using namespace ohos::graphics::uiEffect::uiEffect;

class FilterImpl {
public:
    FilterImpl();
    explicit FilterImpl(std::shared_ptr<OHOS::Rosen::Filter> filter);
    ~FilterImpl();

    Filter Blur(double blurRadius);
    Filter PixelStretch(taihe::array_view<double> stretchSizes, TileMode tileMode);
    Filter Distort(double distortionK);
    Filter FlyInFlyOutEffect(double degree, FlyMode flyMode);
    Filter WaterRipple(double progress, int32_t waveCount, double x, double y, WaterRippleMode rippleMode);
    Filter BezierWarp(taihe::array_view<uintptr_t> controlPoints);
    Filter MaskTransition(::ohos::graphics::uiEffect::uiEffect::weak::Mask alphaMask,
        taihe::optional_view<double> factor, taihe::optional_view<bool> inverse);
    Filter DirectionLight(uintptr_t direction, ::ohos::graphics::uiEffect::uiEffect::Color const& color,
        double intensity, taihe::optional_view<::ohos::graphics::uiEffect::uiEffect::Mask> mask,
        taihe::optional_view<double> factor);
    Filter DisplacementDistort(::ohos::graphics::uiEffect::uiEffect::weak::Mask displacementMap,
        taihe::optional_view<uintptr_t> factor);
    Filter VariableRadiusBlur(double radius, ::ohos::graphics::uiEffect::uiEffect::weak::Mask radiusMap);
    Filter HdrBrightnessRatio(double ratio);
    Filter ContentLight(uintptr_t lightPosition, uintptr_t lightColor, double lightIntensity,
        taihe::optional_view<Mask> displacementMap);
    Filter MaskDispersion(::ohos::graphics::uiEffect::uiEffect::weak::Mask dispersionMap, double alpha,
        taihe::optional_view<uintptr_t> rFactor, taihe::optional_view<uintptr_t> gFactor,
        taihe::optional_view<uintptr_t> bFactor);
    Filter EdgeLight(double alpha, taihe::optional_view<::ohos::graphics::uiEffect::uiEffect::Color> color,
        taihe::optional_view<::ohos::graphics::uiEffect::uiEffect::Mask> mask, taihe::optional_view<bool> bloom);
    Filter ColorGradient(taihe::array_view<::ohos::graphics::uiEffect::uiEffect::Color> colors,
        taihe::array_view<uintptr_t> positions, taihe::array_view<double> strengths,
        taihe::optional_view<::ohos::graphics::uiEffect::uiEffect::Mask> alphaMask);

private:
    bool IsFilterValid() const;

    std::shared_ptr<OHOS::Rosen::Filter> nativeFilter_;
};
} // namespace ANI::UIEffect

#endif