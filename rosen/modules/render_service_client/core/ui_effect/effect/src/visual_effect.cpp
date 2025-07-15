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

#include "ui_effect/effect/include/visual_effect.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

VisualEffect::VisualEffect(const VisualEffect& visualEffect)
{
    const auto& visualEffectParas = visualEffect.GetAllPara();
    for (const auto& para : visualEffectParas) {
        bool isInvalid =
            (para == nullptr || !VisualEffectPara::IsWhitelistPara(static_cast<uint16_t>(para->GetParaType())));
        if (isInvalid) {
            RS_LOGE("[ui_effect] VisualEffect para copy failed");
            continue;
        }
        if (auto paraCopy = para->Clone(); paraCopy) {
            AddPara(paraCopy);
        }
    }
}

bool VisualEffect::Marshalling(Parcel& parcel) const
{
    const auto& visualEffectParas = GetAllPara();
    size_t validParaCount = 0;
    std::vector<std::shared_ptr<VisualEffectPara>> validVisualEffectParas;
    for (const auto& para : visualEffectParas) {
        bool isInvalid =
            (para == nullptr || !VisualEffectPara::IsWhitelistPara(static_cast<uint16_t>(para->GetParaType())));
        if (!isInvalid) {
            validParaCount++;
            validVisualEffectParas.emplace_back(para);
        }
    }
    if (validParaCount > VisualEffectPara::UNMARSHALLING_MAX_VECTOR_SIZE) {
        RS_LOGE("[ui_effect] VisualEffect Marshalling validParaCount is illegal");
        return false;
    }
    if (!parcel.WriteUint32(validParaCount)) {
        RS_LOGE("[ui_effect] VisualEffect Marshalling write validParaCount failed");
        return false;
    }
    for (const auto& visualEffectPara : validVisualEffectParas) {
        if (!visualEffectPara->Marshalling(parcel)) {
            RS_LOGE("[ui_effect] VisualEffect Marshalling failed, type is %{public}d", visualEffectPara->GetParaType());
            return false;
        }
    }
    return true;
}

bool VisualEffect::Unmarshalling(Parcel& parcel, std::shared_ptr<VisualEffect>& val)
{
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        RS_LOGE("[ui_effect] VisualEffect Unmarshalling read size failed");
        return false;
    }
    if (size > VisualEffectPara::UNMARSHALLING_MAX_VECTOR_SIZE) {
        RS_LOGE("[ui_effect] VisualEffect Unmarshalling read size is illegal");
        return false;
    }

    auto visualEffect = std::make_shared<VisualEffect>();
    for (uint32_t i = 0; i < size; ++i) {
        std::shared_ptr<VisualEffectPara> para = nullptr;
        bool isSuccess = (VisualEffectPara::Unmarshalling(parcel, para) && para != nullptr);
        if (!isSuccess) {
            RS_LOGE("[ui_effect] VisualEffect Unmarshalling para failed");
            return false;
        }
        visualEffect->AddPara(para);
    }
    val = std::move(visualEffect);
    return true;
}

} // namespace Rosen
} // namespace OHOS