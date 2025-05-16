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

#include "ui_effect/property/include/rs_ui_blur_filter.h"

#include "platform/common/rs_log.h"
#include "render/rs_render_blur_filter.h"

namespace OHOS {
namespace Rosen {

bool RSUIBlurFilterPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::BLUR) {
        return false;
    }

    return true;
}

void RSUIBlurFilterPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::BLUR) {
        ROSEN_LOGW("RSBlurFilterPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }
    auto blurProperty = std::static_pointer_cast<RSUIBlurFilterPara>(other);
    if (blurProperty == nullptr) {
        ROSEN_LOGW("RSBlurFilterPara::SetProperty blurProperty is nullptr!");
        return;
    }

    auto radiusX = blurProperty->GetRSProperty(RSUIFilterType::BLUR_RADIUS_X);
    auto radiusY = blurProperty->GetRSProperty(RSUIFilterType::BLUR_RADIUS_Y);
    if (radiusX == nullptr || radiusY == nullptr) {
        ROSEN_LOGW("RSBlurFilterPara::SetProperty radiusX or radiusY is nullptr!");
        return;
    }

    auto x = std::static_pointer_cast<RSAnimatableProperty<float>>(radiusX);
    auto y = std::static_pointer_cast<RSAnimatableProperty<float>>(radiusY);
    if (x == nullptr || y == nullptr) {
        ROSEN_LOGW("RSBlurFilterPara::SetProperty x or y is nullptr!");
        return;
    }

    SetRadiusX(x->Get());
    SetRadiusY(y->Get());
}

void RSUIBlurFilterPara::SetBlurPara(std::shared_ptr<FilterBlurPara>& blurPara)
{
    if (blurPara == nullptr) {
        return;
    }
    auto blurRadius = blurPara->GetRadius();
    SetRadiusX(blurRadius);
    SetRadiusY(blurRadius);
}

void RSUIBlurFilterPara::SetRadiusX(float radiusX)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::BLUR_RADIUS_X, radiusX);
}

void RSUIBlurFilterPara::SetRadiusY(float radiusY)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::BLUR_RADIUS_Y, radiusY);
}

std::shared_ptr<RSRenderFilterParaBase> RSUIBlurFilterPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderBlurFilterPara>(id_);
    auto rx = std::static_pointer_cast<RSAnimatableProperty<float>>(GetRSProperty(RSUIFilterType::BLUR_RADIUS_X));
    if (rx == nullptr) {
        ROSEN_LOGI("RSUIBlurFilterPara::CreateRSRenderFilter not found radius x");
        return nullptr;
    }
    std::shared_ptr<RSRenderPropertyBase> x = std::make_shared<RSRenderAnimatableProperty<float>>(
        rx->Get(), rx->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::BLUR_RADIUS_X, x);

    auto ry = std::static_pointer_cast<RSAnimatableProperty<float>>(GetRSProperty(RSUIFilterType::BLUR_RADIUS_Y));
    if (ry == nullptr) {
        ROSEN_LOGI("RSUIBlurFilterPara::CreateRSRenderFilter not found radius y");
        return nullptr;
    }
    std::shared_ptr<RSRenderPropertyBase> y = std::make_shared<RSRenderAnimatableProperty<float>>(
        ry->Get(), ry->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::BLUR_RADIUS_Y, y);
    return frProperty;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUIBlurFilterPara::GetLeafProperties()
{
    std::vector<std::shared_ptr<RSPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}
} // namespace Rosen
} // namespace OHOS