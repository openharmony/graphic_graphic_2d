/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "screen_manager/rs_screen_props.h"

namespace OHOS {
namespace Rosen {
RSScreenProps::RSScreenProps(std::string propName, uint32_t propId, uint64_t value)
    : propName_(propName), propId_(propId), value_(value)
{
}

void RSScreenProps::SetPropertyName(const std::string& propName)
{
    propName_ = propName;
}

void RSScreenProps::SetPropId(uint32_t propId)
{
    propId_ = propId;
}

void RSScreenProps::SetValue(uint64_t value)
{
    value_ = value;
}

const std::string& RSScreenProps::GetPropertyName() const
{
    return propName_;
}

uint32_t RSScreenProps::GetPropId() const
{
    return propId_;
}

uint64_t RSScreenProps::GetValue() const
{
    return value_;
}

bool RSScreenProps::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(propName_)) {
        return false;
    }
    if (!parcel.WriteUint32(propId_)) {
        return false;
    }
    if (!parcel.WriteUint64(value_)) {
        return false;
    }
    return true;
}

RSScreenProps* RSScreenProps::Unmarshalling(Parcel &parcel)
{
    std::string propName;
    uint32_t propId;
    uint64_t value;
    if (!parcel.ReadString(propName)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(propId)) {
        return nullptr;
    }
    if (!parcel.ReadUint64(value)) {
        return nullptr;
    }
    RSScreenProps* screenProps = new RSScreenProps(propName, propId, value);
    return screenProps;
}
}
}
