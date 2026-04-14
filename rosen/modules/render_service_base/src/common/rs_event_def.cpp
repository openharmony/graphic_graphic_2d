/*
* Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "common/rs_event_def.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSExposedEventDataBase* RSExposedEventDataBase::Unmarshalling(Parcel& parcel)
{
    auto data = new RSExposedEventDataBase();
    uint32_t typeValue = static_cast<uint32_t>(RSExposedEventType::EXPOSED_EVENT_INVALID);
    if (!parcel.ReadUint32(typeValue)) {
        RS_LOGE("RSExposedEventDataBase Unmarshalling read failed");
        return data;
    }
    if (typeValue >= static_cast<uint32_t>(RSExposedEventType::EXPOSED_EVENT_INVALID)) {
        RS_LOGE("RSExposedEventDataBase Unmarshalling read invalid type: %{public}u", typeValue);
        return data;
    }
    data->type_ = static_cast<RSExposedEventType>(typeValue);
    return data;
}

bool RSExposedEventDataBase::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint32(static_cast<uint32_t>(type_))) {
        RS_LOGE("RSExposedEventDataBase Marshalling parse type_ failed");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS