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

#include "ui_effect/property/include/rs_ui_filter_para_base.h"

namespace OHOS {
namespace Rosen {
RSUIFilterType RSUIFilterParaBase::GetType() const
{
    return type_;
}

RSUIFilterType RSUIFilterParaBase::GetMaskType() const
{
    return maskType_;
}

bool RSUIFilterParaBase::IsValid() const
{
return type_ != RSUIFilterType::NONE;
}

void RSUIFilterParaBase::Dump(std::string& out) const
{
}

std::map<RSUIFilterType, std::shared_ptr<RSPropertyBase>> RSUIFilterParaBase::GetFilterProperties() const
{
    return properties_;
}

std::shared_ptr<RSRenderPropertyBase> RSUIFilterParaBase::GetRSRenderProperty(RSUIFilterType type) const
{
    auto it = properties_.find(type);
    if (it == properties_.end()) {
        return nullptr;
    }
    auto property = it->second;
    if (!property) {
        return nullptr;
    }
    return property->GetRenderProperty();
}

void RSUIFilterParaBase::Clear()
{
    properties_.clear();
}

std::vector<std::shared_ptr<RSPropertyBase>>  RSUIFilterParaBase::GetLeafProperties()
{
    return {};
}

} // namespace Rosen
} // namespace OHOS