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

#include "filter_taihe.h"

#include "effect/include/blender.h"
#include "effect/include/brightness_blender.h"
#include "ui_effect_taihe_utils.h"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "mask_taihe.h"

using namespace ANI::UIEffect;

namespace ANI::UIEffect {
FilterImpl::FilterImpl()
{
    nativeFilter_ = std::make_shared<OHOS::Rosen::Filter>();
}

FilterImpl::FilterImpl(std::shared_ptr<OHOS::Rosen::Filter> filter)
{
    nativeFilter_ = filter;
}

FilterImpl::~FilterImpl()
{
    nativeFilter_ = nullptr;
}

Filter FilterImpl::Blur(double blurRadius)
{
    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::blur failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    auto filterBlurPara = std::make_shared<OHOS::Rosen::FilterBlurPara>();
    filterBlurPara->SetRadius(static_cast<float>(blurRadius));

    nativeFilter_->AddPara(filterBlurPara);

    return make_holder<FilterImpl, Filter>(nativeFilter_);
}

Filter FilterImpl::PixelStretch(taihe::array_view<double> stretchSizes, TileMode tileMode)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call pixelStretch failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::pixelStretch failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    auto pixelStretchPara = std::make_shared<OHOS::Rosen::PixelStretchPara>();
    pixelStretchPara->SetTileMode(ConvertTileModeFromTaiheTileMode(tileMode));
    OHOS::Rosen::Vector4f stretchPercent;
    if (!ConvertVector4fFromTaiheArray(stretchPercent, stretchSizes)) {
        UIEFFECT_LOG_E("FilterImpl::pixelStretch ConvertVector4fFromTaiheArray failed");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    pixelStretchPara->SetStretchPercent(stretchPercent);

    nativeFilter_->AddPara(pixelStretchPara);

    return make_holder<FilterImpl, Filter>(nativeFilter_);
}

Filter FilterImpl::Distort(double distortionK)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call distort failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::distort failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::DistortPara>();
    para->SetDistortionK(static_cast<float>(distortionK));

    nativeFilter_->AddPara(para);
    auto filter = nativeFilter_;

    return make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::FlyInFlyOutEffect(double degree, FlyMode flyMode)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call flyInFlyOutEffect failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::flyInFlyOutEffect failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::FlyOutPara>();
    para->SetDegree(static_cast<float>(degree));
    para->SetFlyMode(ConvertUint32FromTaiheFlyMode(flyMode));

    nativeFilter_->AddPara(para);
    auto filter = nativeFilter_;
    return make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::WaterRipple(double progress, int32_t waveCount, double x, double y, WaterRippleMode rippleMode)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call waterRipple failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::waterRipple failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::WaterRipplePara>();
    para->SetProgress(static_cast<float>(progress));
    para->SetWaveCount(static_cast<uint32_t>(waveCount));
    para->SetRippleCenterX(static_cast<float>(x));
    para->SetRippleCenterY(static_cast<float>(y));
    para->SetRippleMode(ConvertUint32FromTaiheWaterRippleMode(rippleMode));

    nativeFilter_->AddPara(para);
    auto filter = nativeFilter_;
    return make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::BezierWarp(taihe::array_view<uintptr_t> controlPoints)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call bezierWarp failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::bezierWarp failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::BezierWarpPara>();
    size_t length = controlPoints.size();
    if (length != NUM_12) {
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    std::array<OHOS::Rosen::Vector2f, NUM_12> bezierPoints;
    for (size_t i = 0; i < NUM_12; ++i) {
        if (!ConvertVector2fFromAniPoint(controlPoints[i], bezierPoints[i])) {
            return make_holder<FilterImpl, Filter>(nativeFilter_);
        }
    }
    para->SetBezierControlPoints(bezierPoints);
    nativeFilter_->AddPara(para);
    auto filter = nativeFilter_;
    return make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::MaskTransition(::ohos::graphics::uiEffect::uiEffect::weak::Mask alphaMask,
    taihe::optional_view<double> factor, taihe::optional_view<bool> inverse)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call maskTransition failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::maskTransition failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::MaskTransitionPara>();
    MaskImpl* maskImpl = reinterpret_cast<MaskImpl*>(alphaMask->GetImplPtr());
    if (maskImpl && maskImpl->GetNativePtr()) {
        para->SetMask(maskImpl->GetNativePtr()->GetMaskPara());
    }
    if (factor.has_value()) {
        para->SetFactor(factor.value());
    }
    if (inverse.has_value()) {
        para->SetFactor(inverse.value());
    }
    nativeFilter_->AddPara(para);
    auto filter = nativeFilter_;
    return taihe::make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::DirectionLight(uintptr_t direction, ::ohos::graphics::uiEffect::uiEffect::Color const& color,
    double intensity, taihe::optional_view<Mask> mask, taihe::optional_view<double> factor)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call directionLight failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::directionLight failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::DirectionLightPara>();
    ani_env *env = get_env();
    ani_object ani_obj = reinterpret_cast<ani_object>(direction);
    ani_double directionX { 0.0 };
    ani_double directionY { 0.0 };
    ani_double directionZ { 0.0 };
    if (env->Object_GetPropertyByName_Double(ani_obj, "x", &directionX) == ANI_OK &&
        env->Object_GetPropertyByName_Double(ani_obj, "y", &directionY) == ANI_OK &&
        env->Object_GetPropertyByName_Double(ani_obj, "z", &directionZ) == ANI_OK) {
        OHOS::Rosen::Vector3f lightDirection = OHOS::Rosen::Vector3f(directionX, directionY, directionZ);
        para->SetLightDirection(lightDirection);
    }
    OHOS::Rosen::Vector4f lightColor = OHOS::Rosen::Vector4f(color.red, color.green, color.blue, color.alpha);
    para->SetLightColor(lightColor);
    if (mask.has_value()) {
        MaskImpl* maskImpl = reinterpret_cast<MaskImpl*>(mask.value()->GetImplPtr());
        if (maskImpl && maskImpl->GetNativePtr()) {
            para->SetMask(maskImpl->GetNativePtr()->GetMaskPara());
        }
    }
    if (factor.has_value()) {
        para->SetMaskFactor(static_cast<float>(factor.value()));
    }
    nativeFilter_->AddPara(para);
    auto filter = nativeFilter_;
    return taihe::make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::DisplacementDistort(::ohos::graphics::uiEffect::uiEffect::weak::Mask displacementMap,
    taihe::optional_view<uintptr_t> factor)

{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call displacementDistort failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::displacementDistort failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::DisplacementDistortPara>();
    MaskImpl* maskImpl = reinterpret_cast<MaskImpl*>(displacementMap->GetImplPtr());
    if (maskImpl && maskImpl->GetNativePtr()) {
        para->SetMask(maskImpl->GetNativePtr()->GetMaskPara());
    }
    if (factor.has_value()) {
        OHOS::Rosen::Vector2f factors;
        if (ConvertVector2fFromAniTuple(factors, factor.value())) {
            para->SetFactor(factors);
        }
    }
    nativeFilter_->AddPara(para);
    auto filter = nativeFilter_;
    return taihe::make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::VariableRadiusBlur(double radius, ::ohos::graphics::uiEffect::uiEffect::weak::Mask radiusMap)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call variableRadiusBlur failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::variableRadiusBlur failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::VariableRadiusBlurPara>();
    para->SetBlurRadius(static_cast<float>(radius));
    MaskImpl* maskImpl = reinterpret_cast<MaskImpl*>(radiusMap->GetImplPtr());
    if (maskImpl && maskImpl->GetNativePtr()) {
        para->SetMask(maskImpl->GetNativePtr()->GetMaskPara());
    }
    nativeFilter_->AddPara(para);
    auto filter = nativeFilter_;
    return taihe::make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::HdrBrightnessRatio(double ratio)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call hdrBrightnessRatio failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::hdrBrightnessRatio failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto hdrPara = std::make_shared<OHOS::Rosen::HDRBrightnessRatioPara>();
    hdrPara->SetBrightnessRatio(static_cast<float>(ratio));

    nativeFilter_->AddPara(hdrPara);
    auto filter = nativeFilter_;

    return make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::ContentLight(uintptr_t lightPosition, uintptr_t lightColor, double lightIntensity,
    taihe::optional_view<Mask> displacementMap)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call contentLight failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::contentLight failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::ContentLightPara>();
    OHOS::Rosen::Vector3f lightPositionRes;
    OHOS::Rosen::Vector4f lightColorRes;

    if (!ConvertVector3fFromAniPoint3D(lightPosition, lightPositionRes)) {
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    if (!ConvertVector4fFromAniColor(lightColor, lightColorRes)) {
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    para->SetLightPosition(lightPositionRes);
    para->SetLightColor(lightColorRes);
    if (displacementMap.has_value()) {
    }
    para->SetLightIntensity(static_cast<float>(lightIntensity));

    nativeFilter_->AddPara(para);
    auto filter = nativeFilter_;
    return make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::MaskDispersion(::ohos::graphics::uiEffect::uiEffect::weak::Mask dispersionMap, double alpha,
    ::taihe::optional_view<uintptr_t> rFactor, ::taihe::optional_view<uintptr_t> gFactor,
    ::taihe::optional_view<uintptr_t> bFactor)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call MaskDispersion failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::MaskDispersion failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::DispersionPara>();
    MaskImpl* maskImpl = reinterpret_cast<MaskImpl*>(dispersionMap->GetImplPtr());
    if (maskImpl && maskImpl->GetNativePtr()) {
        para->SetMask(maskImpl->GetNativePtr()->GetMaskPara());
    }
    para->SetOpacity(static_cast<float>(alpha));
    if (rFactor.has_value()) {
        OHOS::Rosen::Vector2f rFactors;
        if (ConvertVector2fFromAniTuple(rFactors, rFactor.value())) {
            para->SetRedOffset(rFactors);
        }
    }
    if (gFactor.has_value()) {
        OHOS::Rosen::Vector2f gFactors;
        if (ConvertVector2fFromAniTuple(gFactors, gFactor.value())) {
            para->SetRedOffset(gFactors);
        }
    }
    if (bFactor.has_value()) {
        OHOS::Rosen::Vector2f bFactors;
        if (ConvertVector2fFromAniTuple(bFactors, bFactor.value())) {
            para->SetRedOffset(bFactors);
        }
    }
    auto filter = nativeFilter_;
    nativeFilter_->AddPara(para);
    return make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::EdgeLight(double alpha, taihe::optional_view<::ohos::graphics::uiEffect::uiEffect::Color> color,
    taihe::optional_view<::ohos::graphics::uiEffect::uiEffect::Mask> mask, taihe::optional_view<bool> bloom)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call EdgeLight failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::EdgeLight failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::EdgeLightPara>();
    para->SetAlpha(std::clamp(static_cast<float>(alpha), 0.f, 1.f));
    if (color.has_value()) {
        OHOS::Rosen::Vector4f it =
            OHOS::Rosen::Vector4f(color.value().red, color.value().green, color.value().blue, color.value().alpha);
        para->SetColor(it);
    } else {
        para->SetUseRawColor(true);
    }
    if (mask.has_value()) {
        MaskImpl* maskImpl = reinterpret_cast<MaskImpl*>(mask.value()->GetImplPtr());
        if (maskImpl && maskImpl->GetNativePtr()) {
            para->SetMask(maskImpl->GetNativePtr()->GetMaskPara());
        }
    }
    if (bloom.has_value()) {
        para->SetBloom(static_cast<bool>(bloom.value()));
    }
    auto filter = nativeFilter_;
    nativeFilter_->AddPara(para);
    return make_holder<FilterImpl, Filter>(std::move(filter));
}

Filter FilterImpl::ColorGradient(taihe::array_view<::ohos::graphics::uiEffect::uiEffect::Color> colors,
    taihe::array_view<uintptr_t> positions, taihe::array_view<double> strengths,
    taihe::optional_view<::ohos::graphics::uiEffect::uiEffect::Mask> alphaMask)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call ColorGradient failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::ColorGradient failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    auto para = std::make_shared<OHOS::Rosen::ColorGradientPara>();
    std::vector<float> colorValue;
    std::vector<float> posValue;
    std::vector<float> strengthValue;
    size_t colorsLen = colors.size();
    for (size_t i = 0; i < colorsLen; ++i) {
        colorValue.push_back(static_cast<float>(colors[i].red));
        colorValue.push_back(static_cast<float>(colors[i].green));
        colorValue.push_back(static_cast<float>(colors[i].blue));   // 2 element of color
        colorValue.push_back(static_cast<float>(colors[i].alpha));  // 3 element of color
    }
    size_t positionsLen = positions.size();
    for (size_t i = 0; i < positionsLen; ++i) {
        OHOS::Rosen::Vector2f vc2f;
        if (!ConvertVector2fFromAniPoint(positions[i], vc2f)) {
            return make_holder<FilterImpl, Filter>(nativeFilter_);
        }
        posValue.push_back(static_cast<float>(vc2f[0]));
        posValue.push_back(static_cast<float>(vc2f[1]));
    }
    size_t strengthsLen = strengths.size();
    for (size_t i = 0; i < strengthsLen; ++i) {
        strengthValue.push_back(static_cast<float>(strengths[i]));
    }
    para->SetColors(colorValue);
    para->SetPositions(posValue);
    para->SetStrengths(strengthValue);

    if (alphaMask.has_value()) {
        MaskImpl* maskImpl = reinterpret_cast<MaskImpl*>(alphaMask.value()->GetImplPtr());
        if (maskImpl && maskImpl->GetNativePtr()) {
            para->SetMask(maskImpl->GetNativePtr()->GetMaskPara());
        }
    }
    auto filter = nativeFilter_;
    nativeFilter_->AddPara(para);
    return make_holder<FilterImpl, Filter>(std::move(filter));
}

BrightnessBlender CreateBrightnessBlender(BrightnessBlenderParam const& param)
{
    BrightnessBlender brightnessBlender;
    brightnessBlender.cubicRate = param.cubicRate;
    brightnessBlender.quadraticRate = param.quadraticRate;
    brightnessBlender.linearRate = param.linearRate;
    brightnessBlender.degree = param.degree;
    brightnessBlender.saturation = param.saturation;
    brightnessBlender.positiveCoefficient = param.positiveCoefficient;
    brightnessBlender.negativeCoefficient = param.negativeCoefficient;
    brightnessBlender.fraction = param.fraction;
    return brightnessBlender;
}

HdrBrightnessBlender CreateHdrBrightnessBlender(BrightnessBlenderParam const& param)
{
    HdrBrightnessBlender hdrBrightnessBlender;
    hdrBrightnessBlender.brightnessBlender.cubicRate = param.cubicRate;
    hdrBrightnessBlender.brightnessBlender.quadraticRate = param.quadraticRate;
    hdrBrightnessBlender.brightnessBlender.linearRate = param.linearRate;
    hdrBrightnessBlender.brightnessBlender.degree = param.degree;
    hdrBrightnessBlender.brightnessBlender.saturation = param.saturation;
    hdrBrightnessBlender.brightnessBlender.positiveCoefficient = param.positiveCoefficient;
    hdrBrightnessBlender.brightnessBlender.negativeCoefficient = param.negativeCoefficient;
    hdrBrightnessBlender.brightnessBlender.fraction = param.fraction;
    return hdrBrightnessBlender;
}

bool FilterImpl::IsFilterValid() const
{
    return nativeFilter_ != nullptr;
}

Filter CreateFilter()
{
    return make_holder<FilterImpl, Filter>();
}
} // namespace ANI::UIEffect

// NOLINTBEGIN
TH_EXPORT_CPP_API_CreateFilter(CreateFilter);
TH_EXPORT_CPP_API_CreateHdrBrightnessBlender(CreateHdrBrightnessBlender);
TH_EXPORT_CPP_API_CreateBrightnessBlender(CreateBrightnessBlender);
// NOLINTEND