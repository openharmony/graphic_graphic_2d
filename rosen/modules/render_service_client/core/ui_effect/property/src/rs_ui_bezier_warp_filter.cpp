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
#include "ui_effect/property/include/rs_ui_bezier_warp_filter.h"

namespace OHOS {
namespace Rosen {

bool RSUIBezierWarpFilterPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::BEZIER_WARP) {
        ROSEN_LOGW("RSUIBezierWarpFilterPara::Equals NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return false;
    }
    return true;
}

void RSUIBezierWarpFilterPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (!other || other->GetType() != RSUIFilterType::BEZIER_WARP) {
        ROSEN_LOGW("RSUIBezierWarpFilterPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }
    auto bezierWarpProperty = std::static_pointer_cast<RSUIBezierWarpFilterPara>(other);
    if (!bezierWarpProperty) {
        ROSEN_LOGW("RSUIBezierWarpFilterPara::SetProperty bezierWarpProperty NG!");
        return;
    }

    for (size_t i = 0; i < BEZIER_WARP_POINT_NUM; ++i) {
        auto ctrlPointsProperty = bezierWarpProperty->GetRSProperty(bezierCtrlPointsType_[i]);
        auto ctrlPointAnimation = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(ctrlPointsProperty);
        if (ctrlPointAnimation == nullptr) {
            return;
        }
        Setter<RSAnimatableProperty<Vector2f>>(bezierCtrlPointsType_[i], ctrlPointAnimation->Get());
    }
}

void RSUIBezierWarpFilterPara::SetBezierWarp(const std::shared_ptr<BezierWarpPara>& bezierWarp)
{
    if (bezierWarp == nullptr) {
        ROSEN_LOGW("RSUIBezierWarpFilterPara::SetBezierWarp bezierWarp is nullptr!");
        return;
    }
    const std::array<Vector2f, BEZIER_WARP_POINT_NUM>& bezierWarpParas = bezierWarp->GetBezierControlPoints();
    for (size_t i = 0; i < BEZIER_WARP_POINT_NUM; ++i) {
        Setter<RSAnimatableProperty<Vector2f>>(bezierCtrlPointsType_[i], bezierWarpParas[i]);
    }
}

std::shared_ptr<RSRenderFilterParaBase> RSUIBezierWarpFilterPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderBezierWarpFilterPara>(id_);
    for (size_t i = 0; i < BEZIER_WARP_POINT_NUM; ++i) {
        if (!CreateRenderFilterCtrlPoints(frProperty, bezierCtrlPointsType_[i])) {
            return nullptr;
        }
    }
    return frProperty;
}

bool RSUIBezierWarpFilterPara::CreateRenderFilterCtrlPoints(
    const std::shared_ptr<RSRenderBezierWarpFilterPara>& frProperty, const RSUIFilterType type)
{
    auto specifiedProperty = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
        GetRSProperty(type));
    if (specifiedProperty == nullptr) {
        ROSEN_LOGE("RSUIBezierWarpFilterPara::CreateRenderFilterCtrlPoints not found %{public}d",
            static_cast<int>(type));
        return false;
    }
    auto renderProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
        specifiedProperty->Get(), specifiedProperty->GetId());
    frProperty->Setter(type, renderProperty);
    return true;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUIBezierWarpFilterPara::GetLeafProperties()
{
    std::vector<std::shared_ptr<RSPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}
} // namespace Rosen
} // namespace OHOS
