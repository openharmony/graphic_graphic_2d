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

#include "mask_taihe.h"

#include "pixel_map_taihe.h"
#include "ui_effect_taihe_utils.h"

using namespace ANI::UIEffect;
namespace ANI::UIEffect {
MaskImpl::MaskImpl()
{
    nativeMask_ = std::make_shared<OHOS::Rosen::Mask>();
}

MaskImpl::MaskImpl(std::shared_ptr<OHOS::Rosen::Mask> mask)
{
    nativeMask_ = mask;
}

MaskImpl::~MaskImpl()
{
    Release();
}

int64_t MaskImpl::GetImplPtr()
{
    return reinterpret_cast<uintptr_t>(this);
}

std::shared_ptr<OHOS::Rosen::Mask> MaskImpl::GetNativePtr()
{
    return nativeMask_;
}

void MaskImpl::Release()
{
    if (!isRelease) {
        if (nativeMask_ != nullptr) {
            nativeMask_ = nullptr;
        }
        isRelease = true;
    }
}

Mask CreateRippleMask(uintptr_t center, double radius, double width, optional_view<double> offset)
{
    auto mask = std::make_shared<OHOS::Rosen::Mask>();
    auto rippleMaskPara = std::make_shared<OHOS::Rosen::RippleMaskPara>();

    OHOS::Rosen::Vector2f centerRes;
    if (!ConvertVector2fFromAniPoint(center, centerRes)) {
        UIEFFECT_MASK_LOG_E("CreateRippleMask center ConvertVector2fFromAniPoint failed");
        return make_holder<MaskImpl, Mask>(std::move(mask));
    }
    rippleMaskPara->SetCenter(centerRes);
    rippleMaskPara->SetRadius(static_cast<float>(radius));
    rippleMaskPara->SetWidth(static_cast<float>(width));
    if (offset.has_value()) {
        rippleMaskPara->SetWidthCenterOffset(static_cast<float>(offset.value()));
    }

    mask->SetMaskPara(rippleMaskPara);
    return make_holder<MaskImpl, Mask>(std::move(mask));
}

Mask CreatePixelMapMask(ohos::multimedia::image::image::weak::PixelMap pixelMap, uintptr_t srcRect, uintptr_t dstRect,
    optional_view<Color> fillColor)
{
    auto mask = std::make_shared<OHOS::Rosen::Mask>();
    auto pixelMapPara = std::make_shared<OHOS::Rosen::PixelMapMaskPara>();

    Image::PixelMapImpl* pixelMapImpl = reinterpret_cast<Image::PixelMapImpl*>(pixelMap->GetImplPtr());
    if (pixelMapImpl == nullptr) {
        return make_holder<MaskImpl, Mask>(std::move(mask));
    }
    auto nativePixelMap = pixelMapImpl->GetNativePtr();
    if (nativePixelMap == nullptr) {
        return make_holder<MaskImpl, Mask>(std::move(mask));
    }
    pixelMapPara->SetPixelMap(nativePixelMap);
    if (!pixelMapPara->IsValid()) {
        return make_holder<MaskImpl, Mask>(std::move(mask));
    }

    OHOS::Rosen::Vector4f srcRectRes;
    OHOS::Rosen::Vector4f dstRectRes;
    if (!ConvertVector4fFromAniRect(srcRect, srcRectRes)) {
        UIEFFECT_MASK_LOG_E("CreatePixelMapMask srcRect ConvertVector4fFromAniRect failed");
        return make_holder<MaskImpl, Mask>(std::move(mask));
    }
    if (!ConvertVector4fFromAniRect(dstRect, dstRectRes)) {
        UIEFFECT_MASK_LOG_E("CreatePixelMapMask dstRect ConvertVector4fFromAniRect failed");
        return make_holder<MaskImpl, Mask>(std::move(mask));
    }

    if (fillColor.has_value()) {
        OHOS::Rosen::Vector4f colorRes = OHOS::Rosen::Vector4f(fillColor->red, fillColor->green, fillColor->blue,
            fillColor->alpha);
        ClampVector4f(colorRes, 0.f, 1.f);
        pixelMapPara->SetFillColor(colorRes);
    }

    mask->SetMaskPara(pixelMapPara);
    return make_holder<MaskImpl, Mask>(std::move(mask));
}

Mask CreateWaveGradientMask(uintptr_t center, double width, double propagationRadius, double blurRadius,
    optional_view<double> turbulenceStrength)
{
    auto mask = std::make_shared<OHOS::Rosen::Mask>();
    auto waveGradientMaskPara = std::make_shared<OHOS::Rosen::WaveGradientMaskPara>();

    OHOS::Rosen::Vector2f centerRes;
    if (!ConvertVector2fFromAniPoint(center, centerRes)) {
        UIEFFECT_MASK_LOG_E("CreateWaveGradientMask center ConvertVector2fFromAniPoint failed");
        return make_holder<MaskImpl, Mask>(std::move(mask));
    }
    waveGradientMaskPara->SetWaveCenter(centerRes);

    waveGradientMaskPara->SetWaveWidth(static_cast<float>(width));
    waveGradientMaskPara->SetPropagationRadius(static_cast<float>(propagationRadius));
    waveGradientMaskPara->SetBlurRadius(static_cast<float>(blurRadius));
    if (turbulenceStrength.has_value()) {
        waveGradientMaskPara->SetTurbulenceStrength(static_cast<float>(turbulenceStrength.value()));
    }

    mask->SetMaskPara(waveGradientMaskPara);
    return make_holder<MaskImpl, Mask>(std::move(mask));
}

Mask CreateRadialGradientMask(uintptr_t center, double radiusX, double radiusY, array_view<uintptr_t> gradients)
{
    auto mask = std::make_shared<OHOS::Rosen::Mask>();
    auto radialGradientMaskPara = std::make_shared<OHOS::Rosen::RadialGradientMaskPara>();

    OHOS::Rosen::Vector2f centerRes;
    if (!ConvertVector2fFromAniPoint(center, centerRes)) {
        UIEFFECT_MASK_LOG_E("CreateRadialGradientMask center ConvertVector2fFromAniPoint failed");
        return make_holder<MaskImpl, Mask>(std::move(mask));
    }
    radialGradientMaskPara->SetCenter(centerRes);
    radialGradientMaskPara->SetRadiusX(static_cast<float>(radiusX));
    radialGradientMaskPara->SetRadiusY(static_cast<float>(radiusY));

    std::vector<float> colors;
    std::vector<float> positions;
    ParseRadialGradientValues(gradients, colors, positions);
    radialGradientMaskPara->SetColors(std::move(colors));
    radialGradientMaskPara->SetPositions(std::move(positions));

    mask->SetMaskPara(radialGradientMaskPara);
    return make_holder<MaskImpl, Mask>(std::move(mask));
}
} // namespace ANI::UIEffect

// NOLINTBEGIN
TH_EXPORT_CPP_API_CreateRippleMask(CreateRippleMask);
TH_EXPORT_CPP_API_CreatePixelMapMask(CreatePixelMapMask);
TH_EXPORT_CPP_API_CreateWaveGradientMask(CreateWaveGradientMask);
TH_EXPORT_CPP_API_CreateRadialGradientMask(CreateRadialGradientMask);
// NOLINTEND