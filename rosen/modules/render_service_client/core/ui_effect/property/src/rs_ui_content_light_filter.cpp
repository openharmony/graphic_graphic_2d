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
#include "ui_effect/property/include/rs_ui_content_light_filter.h"

namespace OHOS {
namespace Rosen {

bool RSUIContentLightFilterPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::CONTENT_LIGHT) {
        ROSEN_LOGW("RSUIContentLightFilterPara::Equals NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return false;
    }
    return true;
}

void RSUIContentLightFilterPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (!other || other->GetType() != RSUIFilterType::CONTENT_LIGHT) {
        ROSEN_LOGW("RSUIContentLightFilterPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }
    auto contentLightProperty = std::static_pointer_cast<RSUIContentLightFilterPara>(other);
    if (!contentLightProperty) {
        ROSEN_LOGW("RSUIContentLightFilterPara::SetProperty contentLightProperty NG!");
        return;
    }

    auto lightPositionProperty = contentLightProperty->GetRSProperty(RSUIFilterType::LIGHT_POSITION);
    auto lightPositionAnimation = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(lightPositionProperty);
    if (lightPositionAnimation == nullptr) {
        return;
    }
    Setter<RSAnimatableProperty<Vector3f>>(RSUIFilterType::LIGHT_POSITION, lightPositionAnimation->Get());
    auto lightColorProperty = contentLightProperty->GetRSProperty(RSUIFilterType::LIGHT_COLOR);
    auto lightColorAnimation = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(lightColorProperty);
    if (lightColorAnimation == nullptr) {
        return;
    }
    Setter<RSAnimatableProperty<Vector4f>>(RSUIFilterType::LIGHT_COLOR, lightColorAnimation->Get());
    auto lightIntensityProperty = contentLightProperty->GetRSProperty(RSUIFilterType::LIGHT_INTENSITY);
    auto lightIntensityAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(lightIntensityProperty);
    if (lightIntensityAnimation == nullptr) {
        return;
    }
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::LIGHT_INTENSITY, lightIntensityAnimation->Get());
}

void RSUIContentLightFilterPara::SetContentLight(const std::shared_ptr<ContentLightPara>& contentLight)
{
    if (contentLight == nullptr) {
        ROSEN_LOGW("RSUIContentLightFilterPara::SetContentLight contentLight is nullptr!");
        return;
    }
    const Vector3f lightPosition = contentLight->GetLightPosition();
    Setter<RSAnimatableProperty<Vector3f>>(RSUIFilterType::LIGHT_POSITION, lightPosition);
    const Vector4f lightColor = contentLight->GetLightColor();
    Setter<RSAnimatableProperty<Vector4f>>(RSUIFilterType::LIGHT_COLOR, lightColor);
    const float lightIntensity = contentLight->GetLightIntensity();
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::LIGHT_INTENSITY, lightIntensity);
}

std::shared_ptr<RSRenderFilterParaBase> RSUIContentLightFilterPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderContentLightFilterPara>(id_);
    if (!CreateRenderFilterLightPosition(frProperty, RSUIFilterType::LIGHT_POSITION) ||
        !CreateRenderFilterLightColor(frProperty, RSUIFilterType::LIGHT_COLOR) ||
        !CreateRenderFilterLightIntensity(frProperty, RSUIFilterType::LIGHT_INTENSITY)) {
        return nullptr;
    }
    return frProperty;
}

bool RSUIContentLightFilterPara::CreateRenderFilterLightPosition(
    const std::shared_ptr<RSRenderContentLightFilterPara>& frProperty, const RSUIFilterType type)
{
    auto specifiedProperty = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(
        GetRSProperty(type));
    if (specifiedProperty == nullptr) {
        ROSEN_LOGE("RSUIContentLightFilterPara::CreateRenderFilterLightPosition not found %{public}d",
            static_cast<int>(type));
        return false;
    }
    auto renderProperty = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(
        specifiedProperty->Get(), specifiedProperty->GetId());
    frProperty->Setter(type, renderProperty);
    return true;
}

bool RSUIContentLightFilterPara::CreateRenderFilterLightColor(
    const std::shared_ptr<RSRenderContentLightFilterPara>& frProperty, const RSUIFilterType type)
{
    auto specifiedProperty = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
        GetRSProperty(type));
    if (specifiedProperty == nullptr) {
        ROSEN_LOGE("RSUIContentLightFilterPara::CreateRenderFilterLightColor not found %{public}d",
            static_cast<int>(type));
        return false;
    }
    auto renderProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(
        specifiedProperty->Get(), specifiedProperty->GetId());
    frProperty->Setter(type, renderProperty);
    return true;
}

bool RSUIContentLightFilterPara::CreateRenderFilterLightIntensity(
    const std::shared_ptr<RSRenderContentLightFilterPara>& frProperty, const RSUIFilterType type)
{
    auto specifiedProperty = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(type));
    if (specifiedProperty == nullptr) {
        ROSEN_LOGE("RSUIContentLightFilterPara::CreateRenderFilterLightIntensity not found %{public}d",
            static_cast<int>(type));
        return false;
    }
    auto renderProperty = std::make_shared<RSRenderAnimatableProperty<float>>(
        specifiedProperty->Get(), specifiedProperty->GetId());
    frProperty->Setter(type, renderProperty);
    return true;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUIContentLightFilterPara::GetLeafProperties()
{
    std::vector<std::shared_ptr<RSPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}
} // namespace Rosen
} // namespace OHOS
