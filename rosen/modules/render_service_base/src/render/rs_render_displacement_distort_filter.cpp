/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "render/rs_render_displacement_distort_filter.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "platform/common/rs_log.h"
#include "render/rs_render_pixel_map_mask.h"
#include "render/rs_render_radial_gradient_mask.h"
#include "render/rs_shader_mask.h"


namespace OHOS {
namespace Rosen {
    void RSRenderDispDistortFilterPara::GetDescription(std::string& out) const
    {
        out += "RSRenderDispDistortFilterPara";
    }

    bool RSRenderDispDistortFilterPara::WriteToParcel(Parcel& parcel)
    {
        ROSEN_LOGD("RSRenderDispDistortFilterPara::WriteToParcel %{public}d %{public}d %{public}d",
            static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
        if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
            !RSMarshallingHelper::Marshalling(parcel, type_) ||
            !RSMarshallingHelper::Marshalling(parcel, modifierType_)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel type Error");
            return false;
        }
        auto factProperty = GetRenderProperty(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR);
        if (factProperty == nullptr) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel empty factor");
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR) ||
            !RSMarshallingHelper::Marshalling(parcel, factProperty)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel factor error");
            return false;
        }
        auto maskProperty = GetRenderProperty(maskType_);
        if (maskProperty == nullptr) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel empty mask");
            return false;
        }
        auto mask = std::static_pointer_cast<RSRenderMaskPara>(maskProperty);
        if (mask == nullptr ||
            !RSMarshallingHelper::Marshalling(parcel, maskType_) ||
            !mask->WriteToParcel(parcel)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel mask error");
            return false;
        }
        return true;
    }

    bool RSRenderDispDistortFilterPara::ReadFromParcel(Parcel& parcel)
    {
        ROSEN_LOGD("RSRenderDispDistortFilterPara::ReadFromParcel %{public}d %{public}d %{public}d",
            static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
        if (!RSMarshallingHelper::Unmarshalling(parcel, id_) ||
            !RSMarshallingHelper::Unmarshalling(parcel, type_) ||
            !RSMarshallingHelper::Unmarshalling(parcel, modifierType_)) {
            return false;
        }
        RSUIFilterType factType = RSUIFilterType::NONE;
        if (!RSMarshallingHelper::Unmarshalling(parcel, factType)) {
            return false;
        }
        std::shared_ptr<RSRenderPropertyBase> factProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>();
        if (RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR != factType ||
            !RSMarshallingHelper::Unmarshalling(parcel, factProperty)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::ReadFromParcel factor error");
            return false;
        }
        Setter(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR, factProperty);
        if (!RSMarshallingHelper::Unmarshalling(parcel, maskType_)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::ReadFromParcel maskType error");
            return false;
        }
        std::shared_ptr<RSRenderFilterParaBase> maskProperty = CreateRenderProperty(maskType_);
        if (maskProperty == nullptr || !maskProperty->ReadFromParcel(parcel)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::ReadFromParcel mask error");
            return false;
        }
        Setter(maskType_, maskProperty);
        return true;
    }

    std::shared_ptr<RSRenderMaskPara> RSRenderDispDistortFilterPara::CreateRenderProperty(RSUIFilterType type)
    {
        switch (type) {
            case RSUIFilterType::RIPPLE_MASK : {
                return std::make_shared<RSRenderRippleMaskPara>(0);
            }
            case RSUIFilterType::PIXEL_MAP_MASK : {
                return std::make_shared<RSRenderPixelMapMaskPara>(0);
            }
            case RSUIFilterType::RADIAL_GRADIENT_MASK : {
                return std::make_shared<RSRenderRadialGradientMaskPara>(0);
            }
            default: {
                ROSEN_LOGD("RSRenderDispDistortFilterPara::CreateRenderProperty mask nullptr");
                return nullptr;
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderDispDistortFilterPara::GetLeafRenderProperties()
    {
        auto factProperty = GetRenderProperty(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR);
        if (factProperty == nullptr) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::GetLeafRenderProperties empty factor");
            return {};
        }
        auto maskProperty = GetRenderProperty(maskType_);
        if (maskProperty == nullptr) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::GetLeafRenderProperties empty mask");
            return {};
        }
        auto mask = std::static_pointer_cast<RSRenderMaskPara>(maskProperty);
        if (mask == nullptr) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::GetLeafRenderProperties empty mask property");
            return {};
        }
        std::vector<std::shared_ptr<RSRenderPropertyBase>> out = mask->GetLeafRenderProperties();
        out.push_back(factProperty);
        return out;
    }

    std::shared_ptr<RSRenderMaskPara> RSRenderDispDistortFilterPara::GetRenderMask()
    {
        auto property = GetRenderProperty(maskType_);
        if (property == nullptr) {
            return nullptr;
        }
        return std::static_pointer_cast<RSRenderMaskPara>(property);
    }

    bool RSRenderDispDistortFilterPara::ParseFilterValues()
    {
        auto displacementDistortFactor = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(
            GetRenderProperty(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR));
        auto maskProperty = std::static_pointer_cast<RSRenderMaskPara>(GetRenderProperty(maskType_));
        if (!displacementDistortFactor || !maskProperty) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::ParseFilterValues GetRenderProperty has nullptr.");
            return false;
        }
        factor_ = displacementDistortFactor->Get();
        mask_ = std::make_shared<RSShaderMask>(maskProperty);
#ifdef USE_M133_SKIA
        const auto hashFunc = SkChecksum::Hash32;
#else
        const auto hashFunc = SkOpts::hash;
#endif
        hash_ = hashFunc(&factor_, sizeof(factor_), hash_);
        auto maskHash = mask_->Hash();
        hash_ = hashFunc(&maskHash, sizeof(maskHash), hash_);
        return true;
    }

    void RSRenderDispDistortFilterPara::GenerateGEVisualEffect(
        std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
    {
        if (!mask_) {
            return;
        }
        auto distortFilter =
            std::make_shared<Drawing::GEVisualEffect>("DISPLACEMENT_DISTORT", Drawing::DrawingPaintType::BRUSH);
        distortFilter->SetParam(GE_FILTER_DISPLACEMENT_DISTORT_FACTOR, std::make_pair(factor_[0], factor_[1]));
        distortFilter->SetParam(GE_FILTER_DISPLACEMENT_DISTORT_MASK, mask_->GenerateGEShaderMask());
        visualEffectContainer->AddToChainedFilter(distortFilter);
    }

    const Vector2f& RSRenderDispDistortFilterPara::GetFactor() const
    {
        return factor_;
    }

    const std::shared_ptr<RSShaderMask>& RSRenderDispDistortFilterPara::GetMask() const
    {
        return mask_;
    }
} // namespace Rosen
} // namespace OHOS