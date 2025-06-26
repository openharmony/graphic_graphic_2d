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

#include "render/rs_render_dispersion_filter.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "platform/common/rs_log.h"
#include "render/rs_render_mask.h"
#include "render/rs_shader_mask.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {

constexpr RSUIFilterType DISPERSION_FILTER_TYPE[] = {
    RSUIFilterType::DISPERSION_OPACITY,
    RSUIFilterType::DISPERSION_RED_OFFSET,
    RSUIFilterType::DISPERSION_GREEN_OFFSET,
    RSUIFilterType::DISPERSION_BLUE_OFFSET,
};

void RSRenderDispersionFilterPara::CalculateHash()
{
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    if (mask_) {
        auto maskHash = mask_->Hash();
        hash_ = hashFunc(&maskHash, sizeof(maskHash), hash_);
    }
    hash_ = hashFunc(&opacity_, sizeof(opacity_), hash_);
    hash_ = hashFunc(&redOffsetX_, sizeof(redOffsetX_), hash_);
    hash_ = hashFunc(&redOffsetY_, sizeof(redOffsetY_), hash_);
    hash_ = hashFunc(&greenOffsetX_, sizeof(greenOffsetX_), hash_);
    hash_ = hashFunc(&greenOffsetY_, sizeof(greenOffsetY_), hash_);
    hash_ = hashFunc(&blueOffsetX_, sizeof(blueOffsetX_), hash_);
    hash_ = hashFunc(&blueOffsetY_, sizeof(blueOffsetY_), hash_);
}

std::shared_ptr<RSRenderFilterParaBase> RSRenderDispersionFilterPara::DeepCopy() const
{
    auto copyFilter = std::make_shared<RSRenderDispersionFilterPara>(id_);
    copyFilter->type_ = type_;
    copyFilter->opacity_ = opacity_;
    copyFilter->redOffsetX_ = redOffsetX_;
    copyFilter->redOffsetY_ = redOffsetY_;
    copyFilter->greenOffsetX_ = greenOffsetX_;
    copyFilter->greenOffsetY_ = greenOffsetY_;
    copyFilter->blueOffsetX_ = blueOffsetX_;
    copyFilter->blueOffsetY_ = blueOffsetY_;
    copyFilter->mask_ = mask_;
    copyFilter->CalculateHash();
    return copyFilter;
}

void RSRenderDispersionFilterPara::GetDescription(std::string& out) const
{
    out += "RSRenderDispersionFilterPara::[maskType_:" + std::to_string(static_cast<int>(maskType_)) + "]";
}

std::shared_ptr<RSRenderPropertyBase> RSRenderDispersionFilterPara::CreateRenderProperty(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::PIXEL_MAP_MASK : {
            return nullptr;
        }
        case RSUIFilterType::DISPERSION_OPACITY : {
            return std::make_shared<RSRenderAnimatableProperty<float>>();
        }
        case RSUIFilterType::DISPERSION_RED_OFFSET :
        case RSUIFilterType::DISPERSION_GREEN_OFFSET :
        case RSUIFilterType::DISPERSION_BLUE_OFFSET : {
            return std::make_shared<RSRenderAnimatableProperty<Vector2f>>();
        }
        default: {
            ROSEN_LOGD("RSRenderDispersionFilterPara::CreateRenderProperty nullptr");
            return nullptr;
        }
    }
}

bool RSRenderDispersionFilterPara::WriteToParcel(Parcel& parcel)
{
    ROSEN_LOGD("RSRenderDispersionFilterPara::WriteToParcel %{public}d %{public}d %{public}d",
        static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
    if (!RSMarshallingHelper::Marshalling(parcel, id_) || !RSMarshallingHelper::Marshalling(parcel, type_) ||
        !RSMarshallingHelper::Marshalling(parcel, modifierType_)) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::WriteToParcel Error");
        return false;
    }

    auto maskProperty = GetRenderProperty(maskType_);
    if (maskProperty == nullptr) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::WriteToParcel empty mask");
        return false;
    }
    auto mask = std::static_pointer_cast<RSRenderMaskPara>(maskProperty);
    if (mask == nullptr || !RSMarshallingHelper::Marshalling(parcel, maskType_) || !mask->WriteToParcel(parcel)) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::WriteToParcel mask error");
        return false;
    }

    for (auto type : DISPERSION_FILTER_TYPE) {
        auto property = GetRenderProperty(type);
        if (property == nullptr) {
            ROSEN_LOGE("RSRenderDispersionFilterPara::WriteToParcel empty type: %{public}d", static_cast<int>(type));
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, type) ||
            !RSMarshallingHelper::Marshalling(parcel, property)) {
            ROSEN_LOGE("RSRenderDispersionFilterPara::WriteToParcel write error: %{public}d", static_cast<int>(type));
            return false;
        }
    }

    return true;
}

bool RSRenderDispersionFilterPara::ReadFromParcel(Parcel& parcel)
{
    ROSEN_LOGD("RSRenderDispersionFilterPara::ReadFromParcel %{public}d %{public}d %{public}d",
        static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
    if (!RSMarshallingHelper::UnmarshallingPidPlusId(parcel, id_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, type_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, modifierType_)) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel Error");
        return false;
    }

    if (!RSMarshallingHelper::Unmarshalling(parcel, maskType_)) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel maskType error");
        return false;
    }
    auto property = CreateRenderProperty(maskType_);
    if (property == nullptr) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel empty mask, maskType: %{public}d",
            static_cast<int>(maskType_));
        return false;
    }
    std::shared_ptr<RSRenderFilterParaBase> maskProperty = std::static_pointer_cast<RSRenderFilterParaBase>(property);
    if (maskProperty == nullptr || !maskProperty->ReadFromParcel(parcel)) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel mask error");
        return false;
    }
    Setter(maskType_, maskProperty);

    for (auto type : DISPERSION_FILTER_TYPE) {
        RSUIFilterType realType = RSUIFilterType::NONE;
        if (!RSMarshallingHelper::Unmarshalling(parcel, realType)) {
            ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel empty type: %{public}d", static_cast<int>(type));
            return false;
        }
        auto property = CreateRenderProperty(type);
        if (type != realType || !RSMarshallingHelper::Unmarshalling(parcel, property)) {
            ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel read error: %{public}d", static_cast<int>(type));
            return false;
        }
        Setter(type, property);
    }

    return true;
}

std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderDispersionFilterPara::GetLeafRenderProperties()
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
    if (maskType_ != RSUIFilterType::NONE) {
        auto mask = std::static_pointer_cast<RSRenderMaskPara>(GetRenderProperty(maskType_));
        if (mask == nullptr) {
            ROSEN_LOGE("RSRenderDispersionFilterPara::GetLeafRenderProperties mask not found, maskType: %{public}d",
                static_cast<int>(maskType_));
            return {};
        }
        out = mask->GetLeafRenderProperties();
    }
    for (const auto& filterType : DISPERSION_FILTER_TYPE) {
        auto value = GetRenderProperty(filterType);
        if (value == nullptr) {
            continue;
        }
        out.emplace_back(value);
    }
    return out;
}

std::shared_ptr<RSRenderMaskPara> RSRenderDispersionFilterPara::GetRenderMask()
{
    auto property = GetRenderProperty(maskType_);
    if (property == nullptr) {
        return nullptr;
    }
    return std::static_pointer_cast<RSRenderMaskPara>(property);
}

bool RSRenderDispersionFilterPara::ParseFilterValues()
{
    auto dispersionMask = std::static_pointer_cast<RSRenderMaskPara>(GetRenderProperty(maskType_));
    auto dispersionOpacity =
        std::static_pointer_cast<RSRenderProperty<float>>(GetRenderProperty(RSUIFilterType::DISPERSION_OPACITY));
    auto dispersionRedOffset =
        std::static_pointer_cast<RSRenderProperty<Vector2f>>(GetRenderProperty(RSUIFilterType::DISPERSION_RED_OFFSET));
    auto dispersionGreenOffset = std::static_pointer_cast<RSRenderProperty<Vector2f>>(
        GetRenderProperty(RSUIFilterType::DISPERSION_GREEN_OFFSET));
    auto dispersionBlueOffset =
        std::static_pointer_cast<RSRenderProperty<Vector2f>>(GetRenderProperty(RSUIFilterType::DISPERSION_BLUE_OFFSET));

    bool hasNull = !dispersionOpacity || !dispersionRedOffset || !dispersionGreenOffset || !dispersionBlueOffset;
    if (hasNull) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::ParseFilterValues GetRenderProperty has some nullptr.");
        return false;
    }
    opacity_ = dispersionOpacity->Get();
    auto redOffset = dispersionRedOffset->Get();
    redOffsetX_ = redOffset[0];
    redOffsetY_ = redOffset[1];
    auto greenOffset = dispersionGreenOffset->Get();
    greenOffsetX_ = greenOffset[0];
    greenOffsetY_ = greenOffset[1];
    auto blueOffset = dispersionBlueOffset->Get();
    blueOffsetX_ = blueOffset[0];
    blueOffsetY_ = blueOffset[1];
    mask_ = dispersionMask ? std::make_shared<RSShaderMask>(dispersionMask) : nullptr;
    return true;
}

void RSRenderDispersionFilterPara::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    if (visualEffectContainer == nullptr) {
        return;
    }
    if (mask_ == nullptr) {
        ROSEN_LOGW("RSRenderDispersionFilterPara::GenerateGEVisualEffect mask_ is nullptr.");
    }

    auto dispersionShaderFilter = std::make_shared<Drawing::GEVisualEffect>(
        Drawing::GE_FILTER_DISPERSION, Drawing::DrawingPaintType::BRUSH);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_MASK,
        mask_ != nullptr ? mask_->GenerateGEShaderMask() : nullptr);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_OPACITY, opacity_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_RED_OFFSET_X, redOffsetX_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_RED_OFFSET_Y, redOffsetY_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_GREEN_OFFSET_X, greenOffsetX_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_GREEN_OFFSET_Y, greenOffsetY_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_BLUE_OFFSET_X, blueOffsetX_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_BLUE_OFFSET_Y, blueOffsetY_);
    visualEffectContainer->AddToChainedFilter(dispersionShaderFilter);
}

const std::shared_ptr<Rosen::RSShaderMask>& RSRenderDispersionFilterPara::GetMask() const
{
    return mask_;
}

} // namespace Rosen
} // namespace OHOS