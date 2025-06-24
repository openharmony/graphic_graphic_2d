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

#include "ui_effect/property/include/rs_ui_edge_light_filter.h"
#include "ui_effect/property/include/rs_ui_radial_gradient_mask.h"
#include "ui_effect/property/include/rs_ui_pixel_map_mask.h"
#include "platform/common/rs_log.h"

#include "render/rs_render_edge_light_filter.h"
namespace OHOS {
namespace Rosen {

bool RSUIEdgeLightFilterPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != GetType()) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::Equals type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return false;
    }

    if (other->GetMaskType() != maskType_) {
        return false;
    }

    auto mask = other->GetRSProperty(maskType_);
    auto oldMask = GetRSProperty(maskType_);
    if ((oldMask == nullptr) != (mask == nullptr)) {
        return false;
    }
    return true;
}

void RSUIEdgeLightFilterPara::Dump(std::string& out) const
{
    out += "RSUIEdgeLightFilterPara:[alpha:";
    auto iter = properties_.find(RSUIFilterType::EDGE_LIGHT_ALPHA);
    if (iter != properties_.end()) {
        auto alpha = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
        if (alpha != nullptr) {
            out += std::to_string(alpha->Get()) + ", ";
        } else {
            out += "nullptr, ";
        }
    }

    out += "bloom:";
    iter = properties_.find(RSUIFilterType::EDGE_LIGHT_BLOOM);
    if (iter != properties_.end()) {
        auto bloom = std::static_pointer_cast<RSProperty<bool>>(iter->second);
        if (bloom != nullptr) {
            out += std::to_string(bloom->Get()) + ", ";
        } else {
            out += "nullptr, ";
        }
    }

    out += "ColorRGBA:(";
    iter = properties_.find(RSUIFilterType::EDGE_LIGHT_COLOR);
    if (iter != properties_.end()) {
        auto color = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second);
        if (color != nullptr) {
            out += std::to_string(color->Get().x_) + ", ";
            out += std::to_string(color->Get().y_) + ", ";
            out += std::to_string(color->Get().z_) + ", ";
            out += std::to_string(color->Get().w_) + ")";
        } else {
            out += "nullptr)";
        }
    }

    out += "]";
}

void RSUIEdgeLightFilterPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != GetType()) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }
    SetStagingEnableHdrEffect(other->GetEnableHdrEffect());

    auto edgeLightProperty = std::static_pointer_cast<RSUIEdgeLightFilterPara>(other);
    auto alpha = edgeLightProperty->GetPropertyWithFilterType<RSAnimatableProperty<float>>(
        RSUIFilterType::EDGE_LIGHT_ALPHA);
    if (alpha == nullptr) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::SetProperty alpha is null NG!");
        return;
    }
    SetAlpha(alpha->Get());

    auto bloom = edgeLightProperty->GetPropertyWithFilterType<RSProperty<bool>>(
        RSUIFilterType::EDGE_LIGHT_BLOOM);
    if (bloom == nullptr) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::SetProperty bloom is null NG!");
        return;
    }
    SetBloom(bloom->Get());

    auto color = edgeLightProperty->GetPropertyWithFilterType<RSAnimatableProperty<Vector4f>>(
        RSUIFilterType::EDGE_LIGHT_COLOR);
    if (color != nullptr) {
        SetColor(color->Get());
    }

    auto mask = edgeLightProperty->GetRSProperty(maskType_);
    if (mask == nullptr) {
        return;
    }
    auto uiMaskPara = std::static_pointer_cast<RSUIMaskPara>(mask);

    auto oldMask = GetRSProperty(maskType_);
    if (oldMask == nullptr) {
        SetMask(uiMaskPara);
    } else {
        auto oldUIMaskPara = std::static_pointer_cast<RSUIMaskPara>(oldMask);
        oldUIMaskPara->SetProperty(uiMaskPara);
    }
}

void RSUIEdgeLightFilterPara::SetEdgeLight(const std::shared_ptr<EdgeLightPara>& edgeLight)
{
    if (edgeLight == nullptr) {
        return;
    }

    SetAlpha(edgeLight->GetAlpha());
    SetBloom(edgeLight->GetBloom());

    auto colorPara = edgeLight->GetColor();
    if (colorPara.has_value()) {
        SetColor(colorPara.value());
    }

    auto maskPara = edgeLight->GetMask();
    if (maskPara == nullptr) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::SetEdgeLight maskPara nullptr.");
        return;
    }

    maskType_ = RSUIMaskPara::ConvertMaskType(maskPara->GetMaskParaType());
    std::shared_ptr<RSUIMaskPara> maskProperty = CreateMask(maskType_);
    if (maskProperty == nullptr) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::SetEdgeLight maskType:%{public}d not support NG!",
            static_cast<int>(maskType_));
        return;
    }
    maskProperty->SetPara(maskPara);
    SetMask(maskProperty);
}

void RSUIEdgeLightFilterPara::SetAlpha(float alpha)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::EDGE_LIGHT_ALPHA, alpha);
}

void RSUIEdgeLightFilterPara::SetBloom(bool bloom)
{
    Setter<RSProperty<bool>>(RSUIFilterType::EDGE_LIGHT_BLOOM, bloom);
}

void RSUIEdgeLightFilterPara::SetColor(const Vector4f& color)
{
    Setter<RSAnimatableProperty<Vector4f>>(RSUIFilterType::EDGE_LIGHT_COLOR, color);
}

void RSUIEdgeLightFilterPara::SetMask(const std::shared_ptr<RSUIMaskPara>& mask)
{
    properties_[mask->GetType()] = mask;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterAlpha(
    const std::shared_ptr<RSRenderEdgeLightFilterPara>& frProperty)
{
    // para:EDGE_LIGHT_ALPHA
    auto alpha = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_ALPHA));
    if (alpha == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterAlpha not found EDGE_LIGHT_ALPHA");
        return false;
    }

    auto renderAlpha = std::make_shared<RSRenderAnimatableProperty<float>>(
        alpha->Get(), alpha->GetId());
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_ALPHA, renderAlpha);
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterBloom(
    const std::shared_ptr<RSRenderEdgeLightFilterPara>& frProperty)
{
    // para:EDGE_LIGHT_BLOOM
    auto bloom = std::static_pointer_cast<RSProperty<bool>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_BLOOM));
    if (bloom == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterBloom not found EDGE_LIGHT_BLOOM");
        return false;
    }

    auto renderBloom = std::make_shared<RSRenderProperty<bool>>(bloom->Get(), bloom->GetId());
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_BLOOM, renderBloom);
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterColor(
    const std::shared_ptr<RSRenderEdgeLightFilterPara>& frProperty)
{
    // para:EDGE_LIGHT_COLOR
    auto color = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_COLOR));
    if (color == nullptr) {
        return true;
    }

    auto renderColor = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(
        color->Get(), color->GetId());
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_COLOR, renderColor);
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterMask(
    const std::shared_ptr<RSRenderEdgeLightFilterPara>& frProperty)
{
    if (maskType_ == RSUIFilterType::NONE) {
        return true;
    }
    auto mask = std::static_pointer_cast<RSUIMaskPara>(GetRSProperty(maskType_));
    if (mask == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterMask mask not found, maskType: %{public}d",
            static_cast<int>(maskType_));
        return false;
    }
    auto maskProperty = mask->CreateRSRenderFilter();
    if (maskProperty == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterMask create render mask failed, maskType: %{public}d",
            static_cast<int>(maskType_));
        return false;
    }
    frProperty->Setter(maskType_, maskProperty);
    return true;
}

std::shared_ptr<RSRenderFilterParaBase> RSUIEdgeLightFilterPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderEdgeLightFilterPara>(id_, maskType_);
    if (!CreateRSRenderFilterAlpha(frProperty) ||
        !CreateRSRenderFilterBloom(frProperty) ||
        !CreateRSRenderFilterColor(frProperty) ||
        !CreateRSRenderFilterMask(frProperty)) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilter create render filter failed, "
            "id: %{public}d, maskType: %{public}d", static_cast<int>(id_), static_cast<int>(maskType_));
        return nullptr;
    }

    return frProperty;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUIEdgeLightFilterPara::GetLeafProperties()
{
    std::vector<std::shared_ptr<RSPropertyBase>> out;
    if (maskType_ != RSUIFilterType::NONE) {
        auto mask = std::static_pointer_cast<RSUIMaskPara>(GetRSProperty(maskType_));
        if (mask == nullptr) {
            ROSEN_LOGE("RSUIEdgeLightFilterPara::GetLeafProperties mask not found, maskType: %{public}d",
                static_cast<int>(maskType_));
            return {};
        }
        out = mask->GetLeafProperties();
    }

    for (auto& [k, v] : properties_) {
        if (k == maskType_) {
            continue;
        }
        out.emplace_back(v);
    }

    return out;
}

bool RSUIEdgeLightFilterPara::CheckEnableHdrEffect()
{
    auto color = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_COLOR));
    if (color == nullptr) {
        return false;
    }

    Vector4f c = color->Get();
    enableHdrEffect_ =
        ROSEN_GNE(c.x_, 1.0f) || ROSEN_GNE(c.y_, 1.0f) || ROSEN_GNE(c.z_, 1.0f) || ROSEN_GNE(c.w_, 1.0f);
    return enableHdrEffect_ || stagingEnableHdrEffect_;
}

std::shared_ptr<RSUIMaskPara> RSUIEdgeLightFilterPara::CreateMask(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::RIPPLE_MASK: {
            return std::make_shared<RSUIRippleMaskPara>();
        }
        case RSUIFilterType::PIXEL_MAP_MASK: {
            return std::make_shared<RSUIPixelMapMaskPara>();
        }
        case RSUIFilterType::RADIAL_GRADIENT_MASK: {
            return std::make_shared<RSUIRadialGradientMaskPara>();
        }
        default:
            return nullptr;
    }
}
} // namespace Rosen
} // namespace OHOS