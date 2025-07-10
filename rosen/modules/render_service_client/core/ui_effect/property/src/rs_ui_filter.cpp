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
#include "ui_effect/property/include/rs_ui_filter.h"

#include "platform/common/rs_log.h"
#include "render/rs_render_blur_filter.h"
#include "render/rs_render_displacement_distort_filter.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<RSRenderFilter> RSUIFilter::GetRSRenderFilter()
{
    if (propertyTypes_.empty() || paras_.empty()) {
        return nullptr;
    }

    std::shared_ptr<RSRenderFilter> rsRenderFilter = std::make_shared<RSRenderFilter>();
    for (auto type : propertyTypes_) {
        auto it = paras_.find(type);
        if (it != paras_.end()) {
            auto property = it->second;
            if (property == nullptr) {
                continue;
            }
            auto frProperty = property->CreateRSRenderFilter();
            rsRenderFilter->Insert(type, frProperty);
        }
    }
    return rsRenderFilter;
}

std::shared_ptr<RSRenderPropertyBase> RSUIFilter::CreateRenderProperty(const PropertyId& id)
{
    std::shared_ptr<RSRenderFilter> rsRenderFilter = GetRSRenderFilter();
    if (rsRenderFilter == nullptr) {
        return nullptr;
    }
    std::shared_ptr<RSRenderPropertyBase> renderProperty =
        std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(
            rsRenderFilter, id);
    return renderProperty;
}

void RSUIFilter::Insert(const std::shared_ptr<RSUIFilterParaBase>& value)
{
    if (!value) {
        ROSEN_LOGW("RSUIFilter::Insert value is nullptr.");
        return;
    }
    auto type = value->GetType();
    auto it = paras_.find(type);
    if (it == paras_.end()) {
        propertyTypes_.push_back(type);
    }
    paras_[type] = value;
}

bool RSUIFilter::IsStructureSame(const std::shared_ptr<RSUIFilter>& other) const
{
    if (other == nullptr) {
        return false;
    }
    auto aTypes = GetAllTypes();
    auto bTypes = other->GetAllTypes();
    if (aTypes.size() != bTypes.size()) {
        ROSEN_LOGW("RSUIFilter::IsStructureSame size NG %{public}d %{public}d ", static_cast<int>(aTypes.size()),
            static_cast<int>(bTypes.size()));
        return false;
    }
    for (size_t i = 0; i < aTypes.size(); i++) {
        if (aTypes[i] != bTypes[i]) {
            ROSEN_LOGW("RSUIFilter::IsStructureSame type NG %{public}d %{public}d %{public}d", static_cast<int>(i),
                static_cast<int>(aTypes[i]), static_cast<int>(bTypes[i]));
            return false;
        }
        auto oldFilter = GetUIFilterPara(aTypes[i]);
        auto newFilter = other->GetUIFilterPara(bTypes[i]);
        // only pass if all nullptr or type equal
        bool invalid = (oldFilter == nullptr && newFilter != nullptr) ||
                       (oldFilter != nullptr && !oldFilter->Equals(newFilter));
        if (invalid) {
            ROSEN_LOGW("RSUIFilter::IsStructureSame value NG %{public}d %{public}d %{public}d %{public}d %{public}d",
                static_cast<int>(i), static_cast<int>(aTypes[i]), static_cast<int>(bTypes[i]),
                static_cast<int>(oldFilter == nullptr), static_cast<int>(newFilter == nullptr));
            return false;
        }
        if (oldFilter != nullptr && newFilter != nullptr) {
            oldFilter->SetStagingEnableHdrEffect(newFilter->GetStagingEnableHdrEffect());
        }
    }
    return true;
}

void RSUIFilter::SetValue(const std::shared_ptr<RSUIFilter>& other)
{
    if (other == nullptr) {
        ROSEN_LOGE("RSUIFilter::SetValue set nullptr");
        return;
    }
    auto aTypes = other->GetAllTypes();
    for (size_t i = 0; i < aTypes.size(); i++) {
        auto oldFilter = GetUIFilterPara(aTypes[i]);
        auto newFilter = other->GetUIFilterPara(aTypes[i]);
        if (oldFilter != nullptr && newFilter != nullptr) {
            oldFilter->SetProperty(newFilter);
        } else {
            ROSEN_LOGE("RSUIFilter::SetValue set:  %{public}d %{public}d %{public}d", static_cast<int>(aTypes[i]),
                static_cast<int>(oldFilter == nullptr), static_cast<int>(newFilter == nullptr));
        }
    }
}

bool RSUIFilter::GetHdrEffectEnable() const
{
    for (auto type : propertyTypes_) {
        auto it = paras_.find(type);
        if (it != paras_.end()) {
            auto property = it->second;
            if (property != nullptr && property->CheckEnableHdrEffect()) {
                return true;
            }
        }
    }
    return false;
}

std::shared_ptr<RSUIFilterParaBase> RSUIFilter::GetUIFilterPara(RSUIFilterType type) const
{
    auto it = paras_.find(type);
    if (it != paras_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<RSUIFilterType>& RSUIFilter::GetUIFilterTypes()
{
    return propertyTypes_;
}

} // namespace Rosen
} // namespace OHOS