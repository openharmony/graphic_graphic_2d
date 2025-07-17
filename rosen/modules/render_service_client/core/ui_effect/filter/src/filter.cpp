/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ui_effect/filter/include/filter.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

Filter::Filter(const Filter& filter)
{
    const auto& filterParas = filter.GetAllPara();
    for (const auto& para : filterParas) {
        bool isInvalid = (para == nullptr || !FilterPara::IsWhitelistPara(static_cast<uint16_t>(para->GetParaType())));
        if (isInvalid) {
            RS_LOGE("[ui_effect] Filter para copy failed");
            continue;
        }
        if (auto paraCopy = para->Clone(); paraCopy) {
            AddPara(paraCopy);
        }
    }
}

bool Filter::Marshalling(Parcel& parcel) const
{
    const auto& filterParas = GetAllPara();
    size_t validParaCount = 0;
    std::vector<std::shared_ptr<FilterPara>> validFilterParas;
    for (auto const& para : filterParas) {
        bool isInvalid = (para == nullptr || !FilterPara::IsWhitelistPara(static_cast<uint16_t>(para->GetParaType())));
        if (!isInvalid) {
            validParaCount++;
            validFilterParas.emplace_back(para);
        }
    }
    if (validParaCount > FilterPara::UNMARSHALLING_MAX_VECTOR_SIZE) {
        RS_LOGE("[ui_effect] Filter Marshalling validParaCount is illegal");
        return false;
    }
    if (!parcel.WriteUint32(validParaCount)) {
        RS_LOGE("[ui_effect] Filter Marshalling write validParaCount failed");
        return false;
    }
    for (const auto& para : validFilterParas) {
        if (!para->Marshalling(parcel)) {
            RS_LOGE("[ui_effect] Filter Marshalling failed, type is %{public}d", para->GetParaType());
            return false;
        }
    }
    return true;
}

bool Filter::Unmarshalling(Parcel& parcel, std::shared_ptr<Filter>& val)
{
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        RS_LOGE("[ui_effect] Filter Unmarshalling read size failed");
        return false;
    }
    if (size > FilterPara::UNMARSHALLING_MAX_VECTOR_SIZE) {
        RS_LOGE("[ui_effect] Filter Unmarshalling read size is illegal");
        return false;
    }

    auto filter = std::make_unique<Filter>();
    for (uint32_t i = 0; i < size; ++i) {
        std::shared_ptr<FilterPara> para = nullptr;
        bool isSuccess = (FilterPara::Unmarshalling(parcel, para) && para != nullptr);
        if (!isSuccess) {
            RS_LOGE("[ui_effect] Filter Unmarshalling para failed");
            return false;
        }
        filter->AddPara(para);
    }
    val = std::move(filter);
    return true;
}

} // namespace Rosen
} // namespace OHOS