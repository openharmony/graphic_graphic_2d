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
#include "render/rs_render_edge_light_filter.h"

#include <memory>

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"
#include "render/rs_render_filter_base.h"
#include "render/rs_render_radial_gradient_mask.h"
#include "render/rs_render_pixel_map_mask.h"
#include "render/rs_shader_mask.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
static const std::vector<RSUIFilterType> FILTER_TYPE_WITHOUT_MASK = {
    RSUIFilterType::EDGE_LIGHT_ALPHA,
    RSUIFilterType::EDGE_LIGHT_BLOOM,
    RSUIFilterType::EDGE_LIGHT_COLOR,
};

void RSRenderEdgeLightFilterPara::CalculateHash()
{
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&alpha_, sizeof(alpha_), hash_);
    hash_ = hashFunc(&bloom_, sizeof(bloom_), hash_);
    hash_ = hashFunc(&color_, sizeof(color_), hash_);
    if (mask_) {
        auto maskHash = mask_->Hash();
        hash_ = hashFunc(&maskHash, sizeof(maskHash), hash_);
    }
    hash_ = hashFunc(&geoWidth_, sizeof(geoWidth_), hash_);
    hash_ = hashFunc(&geoHeight_, sizeof(geoHeight_), hash_);
}

std::shared_ptr<RSRenderFilterParaBase> RSRenderEdgeLightFilterPara::DeepCopy() const
{
    auto copyFilter = std::make_shared<RSRenderEdgeLightFilterPara>(id_);
    copyFilter->type_ = type_;
    copyFilter->alpha_ = alpha_;
    copyFilter->bloom_ = bloom_;
    copyFilter->color_ = color_;
    copyFilter->mask_ = mask_;
    copyFilter->CalculateHash();
    return copyFilter;
}

void RSRenderEdgeLightFilterPara::GetDescription(std::string& out) const
{
    out += "RSRenderEdgeLightFilterPara::[maskType_:" + std::to_string(static_cast<int>(maskType_)) + "]";
}

std::shared_ptr<RSRenderPropertyBase> RSRenderEdgeLightFilterPara::CreateRenderProperty(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::EDGE_LIGHT_ALPHA : {
            return std::make_shared<RSRenderAnimatableProperty<float>>(0.f, 0);
        }
        case RSUIFilterType::EDGE_LIGHT_BLOOM : {
            return std::make_shared<RSRenderProperty<bool>>(true, 0);
        }
        case RSUIFilterType::EDGE_LIGHT_COLOR: {
            return std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(), 0);
        }
        case RSUIFilterType::RIPPLE_MASK: {
            return std::make_shared<RSRenderRippleMaskPara>(0);
        }
        case RSUIFilterType::PIXEL_MAP_MASK : {
            return std::make_shared<RSRenderPixelMapMaskPara>(0);
        }
        case RSUIFilterType::RADIAL_GRADIENT_MASK : {
            return std::make_shared<RSRenderRadialGradientMaskPara>(0);
        }
        default: {
            ROSEN_LOGD("RSRenderEdgeLightFilterPara::CreateRenderProperty nullptr");
            return nullptr;
        }
    }
}

bool RSRenderEdgeLightFilterPara::WriteToParcel(Parcel& parcel)
{
    ROSEN_LOGD("RSRenderEdgeLightFilterPara::WriteToParcel %{public}d %{public}d %{public}d",
        static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
    if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
        !RSMarshallingHelper::Marshalling(parcel, type_) ||
        !RSMarshallingHelper::Marshalling(parcel, modifierType_) ||
        !RSMarshallingHelper::Marshalling(parcel, maskType_)) {
        ROSEN_LOGE("RSRenderEdgeLightFilterPara::WriteToParcel Error");
        return false;
    }

    size_t size = properties_.size();
    if (!RSMarshallingHelper::Marshalling(parcel, size)) {
        ROSEN_LOGE("RSRenderEdgeLightFilterPara::WriteToParcel properties size Error");
        return false;
    }

    for (const auto& [key, value] : properties_) {
        if (key == maskType_) {
            continue;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, key) ||
            !RSMarshallingHelper::Marshalling(parcel, value)) {
            ROSEN_LOGE("RSRenderEdgeLightFilterPara::WriteToParcel Marshalling failed. key: %{public}d",
                static_cast<int>(key));
            return false;
        }
    }

    if (maskType_ == RSUIFilterType::NONE) {
        return true;
    }

    auto maskProperty = GetRenderProperty(maskType_);
    if (maskProperty == nullptr) {
        ROSEN_LOGE("RSRenderEdgeLightFilterPara::WriteToParcel empty mask, maskType: %{public}d",
            static_cast<int>(maskType_));
        return false;
    }
    auto mask = std::static_pointer_cast<RSRenderMaskPara>(maskProperty);
    if (mask == nullptr || !mask->WriteToParcel(parcel)) {
        ROSEN_LOGE("RSRenderEdgeLightFilterPara::WriteToParcel mask error, maskType: %{public}d",
            static_cast<int>(maskType_));
        return false;
    }
    return true;
}

bool RSRenderEdgeLightFilterPara::ReadFromParcel(Parcel& parcel)
{
    ROSEN_LOGD("RSRenderEdgeLightFilterPara::ReadFromParcel %{public}d %{public}d %{public}d",
        static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
    if (!RSMarshallingHelper::UnmarshallingPidPlusId(parcel, id_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, type_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, modifierType_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, maskType_)) {
        ROSEN_LOGE("RSRenderEdgeLightFilterPara::ReadFromParcel Error");
        return false;
    }

    size_t size = 0;
    if (!RSMarshallingHelper::Unmarshalling(parcel, size)) {
        ROSEN_LOGE("RSRenderEdgeLightFilterPara::ReadFromParcel properties size Error");
        return false;
    }

    if (size > static_cast<size_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
        ROSEN_LOGE("RSRenderEdgeLightFilterPara::ReadFromParcel properties size exceed");
        return false;
    }

    properties_.clear();
    for (size_t i = (maskType_ == RSUIFilterType::NONE ? 0 : 1); i < size; ++i) {
        RSUIFilterType key;
        if (!RSMarshallingHelper::Unmarshalling(parcel, key)) {
            ROSEN_LOGE("RSRenderEdgeLightFilterPara::ReadFromParcel unmarshal key failed, filterType: %{public}d",
                static_cast<int>(key));
            return false;
        }

        std::shared_ptr<RSRenderPropertyBase> value = CreateRenderProperty(key);
        if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
            ROSEN_LOGE("RSRenderEdgeLightFilterPara::ReadFromParcel unmarshall value failed, filterType: %{public}d",
                static_cast<int>(key));
            return false;
        }
        Setter(key, value);
    }

    if (maskType_ == RSUIFilterType::NONE) {
        return true;
    }

    auto property = CreateRenderProperty(maskType_);
    if (property == nullptr) {
        ROSEN_LOGE("RSRenderEdgeLightFilterPara::ReadFromParcel empty mask property, maskType: %{public}d",
            static_cast<int>(maskType_));
        return false;
    }

    std::shared_ptr<RSRenderFilterParaBase> maskProperty =
        std::static_pointer_cast<RSRenderFilterParaBase>(property);
    if (maskProperty == nullptr || !maskProperty->ReadFromParcel(parcel)) {
        ROSEN_LOGE("RSRenderEdgeLightFilterPara::ReadFromParcel mask error, maskType: %{public}d",
            static_cast<int>(maskType_));
        return false;
    }
    Setter(maskType_, maskProperty);
    return true;
}

std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderEdgeLightFilterPara::GetLeafRenderProperties()
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
    if (maskType_ != RSUIFilterType::NONE) {
        auto mask = std::static_pointer_cast<RSRenderMaskPara>(GetRenderProperty(maskType_));
        if (mask == nullptr) {
            ROSEN_LOGE("RSRenderEdgeLightFilterPara::GetLeafRenderProperties mask not found, maskType: %{public}d",
                static_cast<int>(maskType_));
            return {};
        }
        out = mask->GetLeafRenderProperties();
    }
    for (const auto& filterType : FILTER_TYPE_WITHOUT_MASK) {
        auto value = GetRenderProperty(filterType);
        if (value == nullptr) {
            continue;
        }
        out.emplace_back(value);
    }
    return out;
}

bool RSRenderEdgeLightFilterPara::ParseFilterValues()
{
    auto edgeLightAlpha = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
        GetRenderProperty(RSUIFilterType::EDGE_LIGHT_ALPHA));
    if (!edgeLightAlpha) {
        ROSEN_LOGE("RSRenderEdgeLightFilterPara::ParseFilterValues alpha is null.");
        return false;
    }
    alpha_ = edgeLightAlpha->Get();

    // bloom
    auto edgeLightBloom = std::static_pointer_cast<RSRenderProperty<bool>>(
        GetRenderProperty(RSUIFilterType::EDGE_LIGHT_BLOOM));
    if (edgeLightBloom == nullptr) {
        return false;
    }
    bloom_ = edgeLightBloom->Get();

    // color
    auto edgeLightColor = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(
        GetRenderProperty(RSUIFilterType::EDGE_LIGHT_COLOR));
    if (edgeLightColor != nullptr) {
        color_ = edgeLightColor->Get();
    }
    // mask
    if (maskType_ != RSUIFilterType::NONE) {
        auto edgeLightMask = std::static_pointer_cast<RSRenderMaskPara>(GetRenderProperty(maskType_));
        if (edgeLightMask == nullptr) {
            ROSEN_LOGE("RSRenderEdgeLightFilterPara::ParseFilterValues mask is null, maskType: %{public}d.",
                static_cast<int>(maskType_));
            return false;
        }
        mask_ = std::make_shared<RSShaderMask>(edgeLightMask);
    }
    return true;
}

void RSRenderEdgeLightFilterPara::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    if (visualEffectContainer == nullptr) {
        return;
    }

    Vector4f color;
    if (color_.has_value()) {
        color = color_.value();
    }

    auto edgeLightShaderFilter = std::make_shared<Drawing::GEVisualEffect>(
        Drawing::GE_FILTER_EDGE_LIGHT, Drawing::DrawingPaintType::BRUSH, GetFilterCanvasInfo());
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_ALPHA, alpha_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_BLOOM, bloom_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_COLOR_R, color.x_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_COLOR_G, color.y_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_COLOR_B, color.z_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_USE_RAW_COLOR, !color_.has_value());
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_MASK,
        mask_ != nullptr ? mask_->GenerateGEShaderMask() : nullptr);
    visualEffectContainer->AddToChainedFilter(edgeLightShaderFilter);
}

} // namespace Rosen
} // namespace OHOS