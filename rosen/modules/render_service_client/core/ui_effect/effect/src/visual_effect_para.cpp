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

#include "ui_effect/effect/include/visual_effect_para.h"

#include <unordered_set>

#include "ui_effect/effect/include/visual_effect_unmarshalling_singleton.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

bool VisualEffectPara::Marshalling(Parcel& parcel) const
{
    RS_LOGE("[ui_effect] VisualEffectPara do not marshalling non-specified type: %{public}d", type_);
    return false;
}

bool VisualEffectPara::RegisterUnmarshallingCallback(uint16_t type, UnmarshallingFunc func)
{
    bool isInvalid = (func == nullptr || !VisualEffectPara::IsWhitelistPara(type));
    if (isInvalid) {
        RS_LOGE("[ui_effect] VisualEffectPara register unmarshalling type:%{public}hu callback failed", type);
        return false;
    }
    VisualEffectUnmarshallingSingleton::GetInstance().RegisterCallback(type, func);
    return true;
}

bool VisualEffectPara::Unmarshalling(Parcel& parcel, std::shared_ptr<VisualEffectPara>& val)
{
    val.reset();
    uint16_t type = ParaType::NONE;
    if (!parcel.ReadUint16(type)) {
        RS_LOGE("[ui_effect] VisualEffectPara Unmarshalling read type failed");
        return false;
    }
    bool isInvalid = (!VisualEffectPara::IsWhitelistPara(type));
    if (isInvalid) {
        RS_LOGE("[ui_effect] VisualEffectPara Unmarshalling read type invalid");
        return false;
    }

    auto func = VisualEffectUnmarshallingSingleton::GetInstance().GetCallback(type);
    if (func != nullptr) {
        return func(parcel, val);
    }
    RS_LOGE("[ui_effect] VisualEffectPara Unmarshalling func invalid, type is %{public}hu", type);
    return false;
}

std::shared_ptr<VisualEffectPara> VisualEffectPara::Clone() const
{
    RS_LOGE("[ui_effect] VisualEffectPara do not clone non-specified type: %{public}d", type_);
    return nullptr;
}

bool VisualEffectPara::IsWhitelistPara(uint16_t type)
{
    return (type == static_cast<uint16_t>(ParaType::HDS_EFFECT_BEGIN));
}

} // namespace Rosen
} // namespace OHOS