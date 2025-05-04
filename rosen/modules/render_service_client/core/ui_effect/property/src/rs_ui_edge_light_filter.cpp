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
#include "platform/common/rs_log.h"
#include "render/rs_render_edge_light_filter.h"
namespace OHOS {
namespace Rosen {

bool RSUIEdgeLightFilterPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::EDGE_LIGHT) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::Equals type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return false;
    }

    // check variables
    return true;
}

void RSUIEdgeLightFilterPara::Dump(std::string& out) const
{
    out += "RSUIEdgeLightFilterPara: [";
    char buffer[UINT8_MAX] = { 0 };
    auto iter = properties_.find(RSUIFilterType::EDGE_LIGHT_DETECT_COLOR);
    if (iter != properties_.end()) {
        auto detectColor = std::static_pointer_cast<RSProperty<uint32_t>>(iter->second);
        if (detectColor) {
            sprintf_s(buffer, UINT8_MAX, "[detectColor: %d]", detectColor->Get());
            out.append(buffer);
        } else {
            out += "[detectColor: nullptr]";
        }
    }

    out += "]";
}

void RSUIEdgeLightFilterPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::EDGE_LIGHT) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }

    auto edgeLightProperty = std::static_pointer_cast<RSUIEdgeLightFilterPara>(other);
    if (edgeLightProperty == nullptr) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::SetProperty edgeLightProperty NG!");
        return;
    }

    auto detectColor = edgeLightProperty->GetPropertyWithFilterType<RSProperty<Color>>(
        RSUIFilterType::EDGE_LIGHT_DETECT_COLOR);
    auto color = edgeLightProperty->GetPropertyWithFilterType<RSProperty<Color>>(RSUIFilterType::EDGE_LIGHT_COLOR);
    auto edgeThreshold = edgeLightProperty->GetPropertyWithFilterType<RSProperty<float>>(
        RSUIFilterType::EDGE_LIGHT_EDGE_THRESHOLD);
    auto edgeIntensity = edgeLightProperty->GetPropertyWithFilterType<RSAnimatableProperty<float>>(
        RSUIFilterType::EDGE_LIGHT_EDGE_INTENSITY);
    auto softThreshold = edgeLightProperty->GetPropertyWithFilterType<RSProperty<float>>(
        RSUIFilterType::EDGE_LIGHT_EDGE_SOFT_THRESHOLD);
    auto bloomLevel = edgeLightProperty->GetPropertyWithFilterType<RSProperty<int>>(
        RSUIFilterType::EDGE_LIGHT_BLOOM_LEVEL);
    auto useRawColor = edgeLightProperty->GetPropertyWithFilterType<RSProperty<bool>>(
        RSUIFilterType::EDGE_LIGHT_USE_RAW_COLOR);
    auto gradient = edgeLightProperty->GetPropertyWithFilterType<RSProperty<bool>>(
        RSUIFilterType::EDGE_LIGHT_GRADIENT);
    auto alphaProgress = edgeLightProperty->GetPropertyWithFilterType<RSAnimatableProperty<float>>(
        RSUIFilterType::EDGE_LIGHT_ALPHA_PROGRESS);
    auto addImage = edgeLightProperty->GetPropertyWithFilterType<RSProperty<bool>>(
        RSUIFilterType::EDGE_LIGHT_ADD_IMAGE);
    if (detectColor == nullptr || color == nullptr || edgeThreshold == nullptr ||
        edgeIntensity == nullptr || softThreshold == nullptr || bloomLevel == nullptr ||
        useRawColor == nullptr || gradient == nullptr || alphaProgress == nullptr ||
        addImage == nullptr) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::SetProperty null NG!");
        return;
    }

    SetDetectColor(detectColor->Get());
    SetColor(color->Get());
    SetEdgeThreshold(edgeThreshold->Get());
    SetEdgeIntensity(edgeIntensity->Get());
    SetEdgeSoftThreshold(softThreshold->Get());
    SetBloomLevel(bloomLevel->Get());
    SetUseRawColor(useRawColor->Get());
    SetGradient(gradient->Get());
    SetAlphaProgress(alphaProgress->Get());
    SetAddImage(addImage->Get());
}

void RSUIEdgeLightFilterPara::SetEdgeLight(const std::shared_ptr<EdgeLightPara>& edgeLight)
{
    if (edgeLight == nullptr) {
        return;
    }

    auto argb_detect_color = Color::FromArgbInt(edgeLight->GetDetectColor());
    SetDetectColor(argb_detect_color);
    auto argb_color = Color::FromArgbInt(edgeLight->GetColor());
    SetColor(argb_color);
    SetEdgeThreshold(edgeLight->GetEdgeThreshold());
    SetEdgeIntensity(edgeLight->GetEdgeIntensity());
    SetEdgeSoftThreshold(edgeLight->GetEdgeSoftThreshold());
    SetBloomLevel(edgeLight->GetBloomLevel());
    SetUseRawColor(edgeLight->GetUseRawColor());
    SetGradient(edgeLight->GetGradient());
    SetAlphaProgress(edgeLight->GetAlphaProgress());
    SetAddImage(edgeLight->GetAddImage());
}

void RSUIEdgeLightFilterPara::SetDetectColor(RSColor detectColor)
{
    Setter<RSProperty<Color>>(RSUIFilterType::EDGE_LIGHT_DETECT_COLOR, detectColor);
}

void RSUIEdgeLightFilterPara::SetColor(RSColor color)
{
    Setter<RSProperty<Color>>(RSUIFilterType::EDGE_LIGHT_COLOR, color);
}

void RSUIEdgeLightFilterPara::SetEdgeThreshold(float edgeThreshold)
{
    Setter<RSProperty<float>>(RSUIFilterType::EDGE_LIGHT_EDGE_THRESHOLD, edgeThreshold);
}

void RSUIEdgeLightFilterPara::SetEdgeIntensity(float edgeIntensity)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::EDGE_LIGHT_EDGE_INTENSITY, edgeIntensity);
}

void RSUIEdgeLightFilterPara::SetEdgeSoftThreshold(float edgeSoftThreshold)
{
    Setter<RSProperty<float>>(RSUIFilterType::EDGE_LIGHT_EDGE_SOFT_THRESHOLD, edgeSoftThreshold);
}

void RSUIEdgeLightFilterPara::SetBloomLevel(int bloomLevel)
{
    Setter<RSProperty<int>>(RSUIFilterType::EDGE_LIGHT_BLOOM_LEVEL, bloomLevel);
}

void RSUIEdgeLightFilterPara::SetUseRawColor(bool useRawColor)
{
    Setter<RSProperty<bool>>(RSUIFilterType::EDGE_LIGHT_USE_RAW_COLOR, useRawColor);
}

void RSUIEdgeLightFilterPara::SetGradient(bool gradient)
{
    Setter<RSProperty<bool>>(RSUIFilterType::EDGE_LIGHT_GRADIENT, gradient);
}

void RSUIEdgeLightFilterPara::SetAlphaProgress(float alphaProgress)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::EDGE_LIGHT_ALPHA_PROGRESS, alphaProgress);
}

void RSUIEdgeLightFilterPara::SetAddImage(bool addImage)
{
    Setter<RSProperty<bool>>(RSUIFilterType::EDGE_LIGHT_ADD_IMAGE, addImage);
}

void RSUIEdgeLightFilterPara::SetMask(std::shared_ptr<RSUIMaskPara> mask)
{
    if (mask == nullptr) {
        ROSEN_LOGW("RSUIEdgeLightFilterPara::SetMask mask nullptr");
        return;
    }
    properties_[mask->GetType()] = mask;
}

bool RSUIEdgeLightFilterPara::SetRSRenderPropertyWithUIFilterType(RSRenderEdgeLightFilterPara& renderPara,
    RSUIFilterType uiFilterType) const
{
    auto property = GetRSProperty(uiFilterType);
    if (property == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::SetRSRenderPropertyWithUIFilterType:GetRSProperty nullptr.");
        return false;
    }
    auto renderProperty = property->GetRenderProperty();
    if (renderProperty == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::SetRSRenderPropertyWithUIFilterType:GetRenderProperty nullptr.");
        return false;
    }
    renderPara.Setter(uiFilterType, renderProperty);
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty)
{
    // para:EDGE_LIGHT_DETECT_COLOR
    auto ft_detect_color = std::static_pointer_cast<RSProperty<Color>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_DETECT_COLOR));
    if (ft_detect_color == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor not found EDGE_LIGHT_DETECT_COLOR");
        return false;
    } else {
        // do nothing;
    }
    auto fp_detect_color = std::make_shared<RSRenderProperty<Color>>(
        ft_detect_color->Get(), ft_detect_color->GetId(), RSRenderPropertyType::PROPERTY_COLOR);
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_DETECT_COLOR, fp_detect_color);
    ROSEN_LOGI("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor EDGE_LIGHT_DETECT_COLOR success.");
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterColor(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty)
{
    // para:EDGE_LIGHT_COLOR
    auto ft_color = std::static_pointer_cast<RSProperty<Color>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_COLOR));
    if (ft_color == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor not found EDGE_LIGHT_COLOR");
        return false;
    } else {
        // do nothing;
    }
    auto fp_color = std::make_shared<RSRenderProperty<Color>>(
        ft_color->Get(), ft_color->GetId(), RSRenderPropertyType::PROPERTY_COLOR);
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_COLOR, fp_color);
    ROSEN_LOGI("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor EDGE_LIGHT_COLOR success.");
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterThreshold(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty)
{
    // para:EDGE_LIGHT_EDGE_THRESHOLD
    auto ft_threshold = std::static_pointer_cast<RSProperty<float>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_EDGE_THRESHOLD));
    if (ft_threshold == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor not found EDGE_LIGHT_EDGE_THRESHOLD");
        return false;
    } else {
        // do nothing;
    }
    auto fp_threshold = std::make_shared<RSRenderProperty<float>>(
        ft_threshold->Get(), ft_threshold->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_EDGE_THRESHOLD, fp_threshold);
    ROSEN_LOGI("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor EDGE_LIGHT_EDGE_THRESHOLD success.");
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterIntensity(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty)
{
    // para:EDGE_LIGHT_EDGE_INTENSITY
    auto ft_intensity = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_EDGE_INTENSITY));
    if (ft_intensity == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor not found EDGE_LIGHT_EDGE_INTENSITY");
        return false;
    } else {
        // do nothing;
    }
    auto fp_intensity = std::make_shared<RSRenderAnimatableProperty<float>>(
        ft_intensity->Get(), ft_intensity->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_EDGE_INTENSITY, fp_intensity);
    ROSEN_LOGI("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor EDGE_LIGHT_EDGE_INTENSITY success.");
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterSoftThreshold(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty)
{
    // para:EDGE_LIGHT_EDGE_SOFT_THRESHOLD
    auto ft_soft_threshold = std::static_pointer_cast<RSProperty<float>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_EDGE_SOFT_THRESHOLD));
    if (ft_soft_threshold == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor not found EDGE_LIGHT_EDGE_SOFT_THRESHOLD");
        return false;
    } else {
        // do nothing;
    }
    auto fp_soft_threshold = std::make_shared<RSRenderProperty<float>>(
        ft_soft_threshold->Get(), ft_soft_threshold->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_EDGE_SOFT_THRESHOLD, fp_soft_threshold);
    ROSEN_LOGI("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor EDGE_LIGHT_EDGE_SOFT_THRESHOLD success.");
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterBloomLevel(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty)
{
    // para:EDGE_LIGHT_BLOOM_LEVEL
    auto ft_bloom_level = std::static_pointer_cast<RSProperty<float>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_BLOOM_LEVEL));
    if (ft_bloom_level == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor not found EDGE_LIGHT_BLOOM_LEVEL");
        return false;
    } else {
        // do nothing;
    }
    auto fp_bloom_level = std::make_shared<RSRenderProperty<float>>(
        ft_bloom_level->Get(), ft_bloom_level->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_BLOOM_LEVEL, fp_bloom_level);
    ROSEN_LOGI("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor EDGE_LIGHT_BLOOM_LEVEL success.");
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterUseRawColor(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty)
{
    // para:EDGE_LIGHT_USE_RAW_COLOR
    auto ft_use_raw_color = std::static_pointer_cast<RSProperty<bool>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_USE_RAW_COLOR));
    if (ft_use_raw_color == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor not found EDGE_LIGHT_USE_RAW_COLOR");
        return false;
    } else {
        // do nothing;
    }
    auto fp_use_raw_color = std::make_shared<RSRenderProperty<bool>>(
        ft_use_raw_color->Get(), ft_use_raw_color->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_USE_RAW_COLOR, fp_use_raw_color);
    ROSEN_LOGI("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor EDGE_LIGHT_USE_RAW_COLOR success.");
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterGradient(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty)
{
    // para:EDGE_LIGHT_GRADIENT
    auto ft_gradient = std::static_pointer_cast<RSProperty<bool>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_GRADIENT));
    if (ft_gradient == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor not found EDGE_LIGHT_GRADIENT");
        return false;
    } else {
        // do nothing;
    }
    auto fp_gradient = std::make_shared<RSRenderProperty<bool>>(
        ft_gradient->Get(), ft_gradient->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_GRADIENT, fp_gradient);
    ROSEN_LOGI("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor EDGE_LIGHT_GRADIENT success.");
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterProgress(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty)
{
    // para:EDGE_LIGHT_ALPHA_PROGRESS
    auto ft_progress = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_ALPHA_PROGRESS));
    if (ft_progress == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor not found EDGE_LIGHT_ALPHA_PROGRESS");
        return false;
    } else {
        // do nothing;
    }
    auto fp_progress = std::make_shared<RSRenderAnimatableProperty<float>>(
        ft_progress->Get(), ft_progress->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_ALPHA_PROGRESS, fp_progress);
    ROSEN_LOGI("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor EDGE_LIGHT_ALPHA_PROGRESS success.");
    return true;
}

bool RSUIEdgeLightFilterPara::CreateRSRenderFilterAddImage(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty)
{
    // para:EDGE_LIGHT_ADD_IMAGE
    auto ft_add_image = std::static_pointer_cast<RSProperty<bool>>(
        GetRSProperty(RSUIFilterType::EDGE_LIGHT_ADD_IMAGE));
    if (ft_add_image == nullptr) {
        ROSEN_LOGE("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor not found EDGE_LIGHT_ADD_IMAGE");
        return false;
    } else {
        // do nothing;
    }
    auto fp_add_image = std::make_shared<RSRenderProperty<bool>>(
        ft_add_image->Get(), ft_add_image->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::EDGE_LIGHT_ADD_IMAGE, fp_add_image);
    ROSEN_LOGI("RSUIEdgeLightFilterPara::CreateRSRenderFilterDetectColor EDGE_LIGHT_ADD_IMAGE success.");
    return true;
}

std::shared_ptr<RSRenderFilterParaBase> RSUIEdgeLightFilterPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderEdgeLightFilterPara>(id_, maskType_);
    if (!CreateRSRenderFilterDetectColor(frProperty) ||
        !CreateRSRenderFilterColor(frProperty) ||
        !CreateRSRenderFilterThreshold(frProperty) ||
        !CreateRSRenderFilterIntensity(frProperty) ||
        !CreateRSRenderFilterSoftThreshold(frProperty) ||
        !CreateRSRenderFilterBloomLevel(frProperty) ||
        !CreateRSRenderFilterUseRawColor(frProperty) ||
        !CreateRSRenderFilterGradient(frProperty) ||
        !CreateRSRenderFilterProgress(frProperty) ||
        !CreateRSRenderFilterAddImage(frProperty)) {
        return nullptr;
    } else {
        // do nothing;
    }
    return frProperty;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUIEdgeLightFilterPara::GetLeafProperties()
{
    std::vector<std::shared_ptr<RSPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}

std::shared_ptr<RSUIMaskPara> RSUIEdgeLightFilterPara::CreateMask(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::RIPPLE_MASK: {
            return std::make_shared<RSUIRippleMaskPara>();
        }
        default:
            return nullptr;
    }
}
} // namespace Rosen
} // namespace OHOS