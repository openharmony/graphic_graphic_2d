/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "blender_taihe.h"

#include "effect/include/blender.h"
#include "effect/include/brightness_blender.h"
#include "effect/include/hdr_darken_blender.h"
#include "ui_effect_taihe_utils.h"

using namespace ANI::UIEffect;

namespace ANI::UIEffect {

BlenderImpl::BlenderImpl()
    : nativeBlender_(std::make_shared<OHOS::Rosen::Blender>())
{
}

BlenderImpl::BlenderImpl(std::shared_ptr<OHOS::Rosen::Blender> blender)
    : nativeBlender_(std::move(blender))
{
}

BlenderImpl::~BlenderImpl()
{
    nativeBlender_ = nullptr;
}

bool BlenderImpl::IsBlenderValid() const
{
    return nativeBlender_ != nullptr;
}

int64_t BlenderImpl::getNativePtr()
{
    return reinterpret_cast<int64_t>(nativeBlender_.get());
}

BrightnessBlenderImpl::BrightnessBlenderImpl()
    : BlenderImpl(std::make_shared<OHOS::Rosen::BrightnessBlender>())
{
}

BrightnessBlenderImpl::BrightnessBlenderImpl(std::shared_ptr<OHOS::Rosen::BrightnessBlender> blender)
    : BlenderImpl(std::static_pointer_cast<OHOS::Rosen::Blender>(std::move(blender)))
{
}

BrightnessBlenderImpl::~BrightnessBlenderImpl()
{
}

OHOS::Rosen::BrightnessBlender* BrightnessBlenderImpl::GetBrightnessBlender()
{
    return static_cast<OHOS::Rosen::BrightnessBlender*>(nativeBlender_.get());
}

double BrightnessBlenderImpl::getCubicRate()
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::getCubicRate failed, blender is invalid");
        return 0.0;
    }
    return static_cast<double>(blender->GetCubicRate());
}

void BrightnessBlenderImpl::setCubicRate(double cubicRate)
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::setCubicRate failed, blender is invalid");
        return;
    }
    blender->SetCubicRate(static_cast<float>(cubicRate));
}

double BrightnessBlenderImpl::getQuadraticRate()
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::getQuadraticRate failed, blender is invalid");
        return 0.0;
    }
    return static_cast<double>(blender->GetQuadRate());
}

void BrightnessBlenderImpl::setQuadraticRate(double quadraticRate)
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::setQuadraticRate failed, blender is invalid");
        return;
    }
    blender->SetQuadRate(static_cast<float>(quadraticRate));
}

double BrightnessBlenderImpl::getLinearRate()
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::getLinearRate failed, blender is invalid");
        return 1.0;
    }
    return static_cast<double>(blender->GetLinearRate());
}

void BrightnessBlenderImpl::setLinearRate(double linearRate)
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::setLinearRate failed, blender is invalid");
        return;
    }
    blender->SetLinearRate(static_cast<float>(linearRate));
}

double BrightnessBlenderImpl::getDegree()
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::getDegree failed, blender is invalid");
        return 0.0;
    }
    return static_cast<double>(blender->GetDegree());
}

void BrightnessBlenderImpl::setDegree(double degree)
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::setDegree failed, blender is invalid");
        return;
    }
    blender->SetDegree(static_cast<float>(degree));
}

double BrightnessBlenderImpl::getSaturation()
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::getSaturation failed, blender is invalid");
        return 1.0;
    }
    return static_cast<double>(blender->GetSaturation());
}

void BrightnessBlenderImpl::setSaturation(double saturation)
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::setSaturation failed, blender is invalid");
        return;
    }
    blender->SetSaturation(static_cast<float>(saturation));
}

Tuple3F BrightnessBlenderImpl::getPositiveCoefficient()
{
    Tuple3F result = {0.0, 0.0, 0.0};
    
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::getPositiveCoefficient failed, blender is invalid");
        return result;
    }
    
    const auto& coeff = blender->GetPositiveCoeff();
    result.x = static_cast<double>(coeff[0]);
    result.y = static_cast<double>(coeff[1]);
    result.z = static_cast<double>(coeff[2]); // 2 index for rgb positivecoeff
    
    return result;
}

void BrightnessBlenderImpl::setPositiveCoefficient(Tuple3F const& positiveCoefficient)
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::setPositiveCoefficient failed, blender is invalid");
        return;
    }
    
    OHOS::Rosen::Vector3f coeff(
        static_cast<float>(positiveCoefficient.x),
        static_cast<float>(positiveCoefficient.y),
        static_cast<float>(positiveCoefficient.z));
    blender->SetPositiveCoeff(coeff);
}

Tuple3F BrightnessBlenderImpl::getNegativeCoefficient()
{
    Tuple3F result = {0.0, 0.0, 0.0};
    
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::getNegativeCoefficient failed, blender is invalid");
        return result;
    }
    
    const auto& coeff = blender->GetNegativeCoeff();
    result.x = static_cast<double>(coeff[0]);
    result.y = static_cast<double>(coeff[1]);
    result.z = static_cast<double>(coeff[2]); // 2 index for rgb negativecoefficient
    
    return result;
}

void BrightnessBlenderImpl::setNegativeCoefficient(Tuple3F const& negativeCoefficient)
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::setNegativeCoefficient failed, blender is invalid");
        return;
    }
    
    OHOS::Rosen::Vector3f coeff(
        static_cast<float>(negativeCoefficient.x),
        static_cast<float>(negativeCoefficient.y),
        static_cast<float>(negativeCoefficient.z));
    blender->SetNegativeCoeff(coeff);
}

double BrightnessBlenderImpl::getFraction()
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::getFraction failed, blender is invalid");
        return 1.0;
    }
    return static_cast<double>(blender->GetFraction());
}

void BrightnessBlenderImpl::setFraction(double fraction)
{
    auto* blender = GetBrightnessBlender();
    if (!blender) {
        UIEFFECT_LOG_E("BrightnessBlenderImpl::setFraction failed, blender is invalid");
        return;
    }
    blender->SetFraction(static_cast<float>(fraction));
}

HdrBrightnessBlenderImpl::HdrBrightnessBlenderImpl()
    : BrightnessBlenderImpl(std::make_shared<OHOS::Rosen::BrightnessBlender>())
{
    auto* blender = GetBrightnessBlender();
    if (blender) {
        blender->SetHdr(true);
    }
}

HdrBrightnessBlenderImpl::HdrBrightnessBlenderImpl(std::shared_ptr<OHOS::Rosen::BrightnessBlender> blender)
    : BrightnessBlenderImpl(blender)
{
    auto* b = GetBrightnessBlender();
    if (b) {
        b->SetHdr(true);
    }
}

HdrBrightnessBlenderImpl::~HdrBrightnessBlenderImpl()
{
}

namespace {
void ApplyBrightnessBlenderParam(OHOS::Rosen::BrightnessBlender* blender, BrightnessBlenderParam const& param)
{
    if (!blender) {
        return;
    }
    blender->SetCubicRate(static_cast<float>(param.cubicRate));
    blender->SetQuadRate(static_cast<float>(param.quadraticRate));
    blender->SetLinearRate(static_cast<float>(param.linearRate));
    blender->SetDegree(static_cast<float>(param.degree));
    blender->SetSaturation(static_cast<float>(param.saturation));
    
    OHOS::Rosen::Vector3f positiveCoeff;
    if (ANI::UIEffect::ConvertVector3fFromAniTuple(positiveCoeff, param.positiveCoefficient)) {
        blender->SetPositiveCoeff(positiveCoeff);
    } else {
        blender->SetPositiveCoeff({0.f, 0.f, 0.f});
    }

    OHOS::Rosen::Vector3f negativeCoeff;
    if (ANI::UIEffect::ConvertVector3fFromAniTuple(negativeCoeff, param.negativeCoefficient)) {
        blender->SetNegativeCoeff(negativeCoeff);
    } else {
        blender->SetNegativeCoeff({0.f, 0.f, 0.f});
    }
    
    blender->SetFraction(static_cast<float>(param.fraction));
}
} // namespace

Blender CreateBlender()
{
    return make_holder<BlenderImpl, Blender>();
}

BrightnessBlender CreateBrightnessBlender(BrightnessBlenderParam const& param)
{
    auto blender = std::make_shared<OHOS::Rosen::BrightnessBlender>();
    ApplyBrightnessBlenderParam(blender.get(), param);
    return make_holder<BrightnessBlenderImpl, BrightnessBlender>(std::move(blender));
}

HdrBrightnessBlender CreateHdrBrightnessBlender(BrightnessBlenderParam const& param)
{
    auto blender = std::make_shared<OHOS::Rosen::BrightnessBlender>();
    ApplyBrightnessBlenderParam(blender.get(), param);
    blender->SetHdr(true);
    return make_holder<HdrBrightnessBlenderImpl, HdrBrightnessBlender>(std::move(blender));
}

HdrDarkenBlenderImpl::HdrDarkenBlenderImpl()
    : BlenderImpl(std::make_shared<OHOS::Rosen::HdrDarkenBlender>())
{
}

HdrDarkenBlenderImpl::HdrDarkenBlenderImpl(std::shared_ptr<OHOS::Rosen::HdrDarkenBlender> blender)
    : BlenderImpl(std::static_pointer_cast<OHOS::Rosen::Blender>(std::move(blender)))
{
}

HdrDarkenBlenderImpl::~HdrDarkenBlenderImpl()
{
}

OHOS::Rosen::HdrDarkenBlender* HdrDarkenBlenderImpl::GetHdrDarkenBlender()
{
    return static_cast<OHOS::Rosen::HdrDarkenBlender*>(nativeBlender_.get());
}

double HdrDarkenBlenderImpl::getHdrBrightnessRatio()
{
    auto* blender = GetHdrDarkenBlender();
    if (!blender) {
        UIEFFECT_LOG_E("HdrDarkenBlenderImpl::getHdrBrightnessRatio failed, blender is invalid");
        return 1.0;
    }
    return static_cast<double>(blender->GetHdrBrightnessRatio());
}

void HdrDarkenBlenderImpl::setHdrBrightnessRatio(double hdrBrightnessRatio)
{
    auto* blender = GetHdrDarkenBlender();
    if (!blender) {
        UIEFFECT_LOG_E("HdrDarkenBlenderImpl::setHdrBrightnessRatio failed, blender is invalid");
        return;
    }
    blender->SetHdrBrightnessRatio(static_cast<float>(hdrBrightnessRatio));
}

taihe::optional<Tuple3F> HdrDarkenBlenderImpl::getGrayscaleFactor()
{
    auto* blender = GetHdrDarkenBlender();
    if (!blender) {
        UIEFFECT_LOG_E("HdrDarkenBlenderImpl::getGrayscaleFactor failed, blender is invalid");
        return taihe::optional<Tuple3F>();
    }
    
    const auto& factor = blender->GetGrayscaleFactor();
    Tuple3F result;
    result.x = static_cast<double>(factor[0]);
    result.y = static_cast<double>(factor[1]);
    result.z = static_cast<double>(factor[2]); // 2 index for rgb grayscale factor
    return taihe::optional<Tuple3F>(std::in_place, result);
}

void HdrDarkenBlenderImpl::setGrayscaleFactor(taihe::optional<Tuple3F> const& grayscaleFactor)
{
    auto* blender = GetHdrDarkenBlender();
    if (!blender) {
        UIEFFECT_LOG_E("HdrDarkenBlenderImpl::setGrayscaleFactor failed, blender is invalid");
        return;
    }
    
    if (grayscaleFactor.has_value()) {
        auto& factor = grayscaleFactor.value();
        OHOS::Rosen::Vector3f vector3f(
            static_cast<float>(factor.x),
            static_cast<float>(factor.y),
            static_cast<float>(factor.z));
        blender->SetGrayscaleFactor(vector3f);
    }
}

HdrDarkenBlender CreateHdrDarkenBlender(double ratio, taihe::optional<Tuple3F> factors)
{
    auto blender = std::make_shared<OHOS::Rosen::HdrDarkenBlender>();
    blender->SetHdrBrightnessRatio(static_cast<float>(ratio));
    
    if (factors.has_value()) {
        auto& factor = factors.value();
        OHOS::Rosen::Vector3f vector3f(
            static_cast<float>(factor.x),
            static_cast<float>(factor.y),
            static_cast<float>(factor.z));
        blender->SetGrayscaleFactor(vector3f);
    }
    
    return make_holder<HdrDarkenBlenderImpl, HdrDarkenBlender>(std::move(blender));
}

} // namespace ANI::UIEffect

// NOLINTBEGIN
TH_EXPORT_CPP_API_CreateBlender(CreateBlender);
TH_EXPORT_CPP_API_CreateBrightnessBlender(CreateBrightnessBlender);
TH_EXPORT_CPP_API_CreateHdrBrightnessBlender(CreateHdrBrightnessBlender);
TH_EXPORT_CPP_API_CreateHdrDarkenBlender(CreateHdrDarkenBlender);
// NOLINTEND