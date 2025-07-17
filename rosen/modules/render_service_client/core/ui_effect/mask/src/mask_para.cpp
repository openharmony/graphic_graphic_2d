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

#include "ui_effect/mask/include/mask_para.h"

#include <unordered_set>

#include "ui_effect/mask/include/mask_unmarshalling_singleton.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

bool MaskPara::Marshalling(Parcel& parcel) const
{
    RS_LOGE("[ui_effect] MaskPara do not marshalling non-specified type: %{public}d", type_);
    return false;
}

bool MaskPara::RegisterUnmarshallingCallback(uint16_t type, UnmarshallingFunc func)
{
    bool isInvalid = (func == nullptr || !MaskPara::IsWhitelistPara(type));
    if (isInvalid) {
        RS_LOGE("[ui_effect] MaskPara register unmarshalling type:%{public}hu callback failed", type);
        return false;
    }
    MaskUnmarshallingSingleton::GetInstance().RegisterCallback(type, func);
    return true;
}

bool MaskPara::Unmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val)
{
    val.reset();
    uint16_t type = Type::NONE;
    if (!parcel.ReadUint16(type)) {
        RS_LOGE("[ui_effect] MaskPara Unmarshalling read type failed");
        return false;
    }
    bool isInvalid = (!MaskPara::IsWhitelistPara(type));
    if (isInvalid) {
        RS_LOGE("[ui_effect] MaskPara Unmarshalling read type invalid");
        return false;
    }

    auto func = MaskUnmarshallingSingleton::GetInstance().GetCallback(type);
    if (func != nullptr) {
        return func(parcel, val);
    }
    RS_LOGE("[ui_effect] MaskPara Unmarshalling func invalid, type is %{public}hu", type);
    return false;
}

std::shared_ptr<MaskPara> MaskPara::Clone() const
{
    RS_LOGE("[ui_effect] MaskPara do not clone non-specified type: %{public}d", type_);
    return nullptr;
}

bool MaskPara::IsWhitelistPara(uint16_t type)
{
    static const std::unordered_set<uint16_t> whitelist = {
        static_cast<uint16_t>(Type::RADIAL_GRADIENT_MASK),
        static_cast<uint16_t>(Type::PIXEL_MAP_MASK)
    };

    auto find = whitelist.find(type);
    if (find != whitelist.end()) {
        return true;
    }

    RS_LOGE("[ui_effect] MaskPara This %{public}hu para is not allowed to be operated", type);
    return false;
}

} // namespace Rosen
} // namespace OHOS