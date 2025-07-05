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

#include "ui_effect/filter/include/filter_para.h"
#include "ui_effect/filter/include/filter_unmarshalling_singleton.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

bool FilterPara::Marshalling(Parcel& parcel) const
{
    RS_LOGE("[ui_effect] FilterPara Marshalling not implemented, type is %{public}d", type_);
    return false;
}

bool FilterPara::RegisterUnmarshallingCallback(uint16_t type, UnmarshallingFunc func)
{
    if (type == ParaType::NONE || func == nullptr) {
        RS_LOGE("[ui_effect] FilterPara RegisterUnmarshallingCallback type:%{public}d or func is invalid", type);
        return false;
    }
    FilterUnmarshallingSingleton::GetInstance().RegisterCallback(type, func);
    return true;
}

bool FilterPara::Unmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    val.reset();
    uint16_t type = ParaType::NONE;
    if (!parcel.ReadUint16(type)) {
        RS_LOGE("[ui_effect] FilterPara Unmarshalling read type failed");
        return false;
    }
    if (type == ParaType::NONE) {
        RS_LOGE("[ui_effect] FilterPara Unmarshalling read type invalid");
        return false;
    }

    auto func = FilterUnmarshallingSingleton::GetInstance().GetCallback(type);
    if (func != nullptr) {
        return func(parcel, val);
    }
    RS_LOGE("[ui_effect] FilterPara Unmarshalling func invalid, type is %{public}d", type);
    return false;
}

std::shared_ptr<FilterPara> FilterPara::Clone() const
{
    RS_LOGE("[ui_effect] FilterPara Clone not implemented, type is %{public}d", type_);
    return nullptr;
}

} // namespace Rosen
} // namespace OHOS