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

#include "screen_manager/rs_screen_data.h"

namespace OHOS {
namespace Rosen {
RSScreenData::RSScreenData(RSScreenCapability capability, RSScreenModeInfo activityModeInfo,
    const std::vector<RSScreenModeInfo>& supportModeInfo, ScreenPowerStatus powerStatus)
    : capability_(capability), activityModeInfo_(activityModeInfo),
    supportModeInfo_(supportModeInfo), powerStatus_(powerStatus)
{
}

void RSScreenData::SetCapability(const RSScreenCapability& capability)
{
    capability_ = capability;
}

void RSScreenData::SetActivityModeInfo(const RSScreenModeInfo& activityModeInfo)
{
    activityModeInfo_ = activityModeInfo;
}

void RSScreenData::SetSupportModeInfo(const std::vector<RSScreenModeInfo>& supportModeInfo)
{
    supportModeInfo_ = supportModeInfo;
}

void RSScreenData::SetPowerStatus(ScreenPowerStatus powerStatus)
{
    powerStatus_ = powerStatus;
}


RSScreenCapability RSScreenData::GetCapability() const
{
    return capability_;
}

RSScreenModeInfo RSScreenData::GetActivityModeInfo() const
{
    return activityModeInfo_;
}

const std::vector<RSScreenModeInfo>& RSScreenData::GetSupportModeInfo() const
{
    return supportModeInfo_;
}

ScreenPowerStatus RSScreenData::GetPowerStatus() const
{
    return powerStatus_;
}

bool RSScreenData::WriteVector(const std::vector<RSScreenModeInfo> &supportModes, Parcel &parcel) const
{
    for (uint32_t modeIndex = 0; modeIndex < supportModes.size(); modeIndex++) {
        if (!parcel.WriteParcelable(&supportModes[modeIndex])) {
            return false;
        }
    }
    return true;
}

bool RSScreenData::ReadVector(std::vector<RSScreenModeInfo> &unmarsupportModes, uint32_t unmarModeCount, Parcel &parcel)
{
    for (uint32_t modeIndex = 0; modeIndex < unmarModeCount; modeIndex++) {
        sptr<RSScreenModeInfo> itemMode = parcel.ReadParcelable<RSScreenModeInfo>();
        if (itemMode == nullptr) {
            return false;
        } else {
            unmarsupportModes.push_back(*itemMode);
        }
    }
    return true;
}

bool RSScreenData::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteParcelable(&capability_)) {
        return false;
    }
    if (!parcel.WriteParcelable(&activityModeInfo_)) {
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(supportModeInfo_.size()))) {
        return false;
    }
    if (!WriteVector(supportModeInfo_, parcel)) {
        return false;
    }
    if (!parcel.WriteUint8(static_cast<uint8_t>(powerStatus_))) {
        return false;
    }
    return true;
}

RSScreenData* RSScreenData::Unmarshalling(Parcel &parcel)
{
    sptr<RSScreenCapability> capability;
    sptr<RSScreenModeInfo> activityModeInfo;
    uint32_t modeCount;
    std::vector<RSScreenModeInfo> supportModeInfo;
    uint8_t powerStatus;
    capability = parcel.ReadParcelable<RSScreenCapability>();
    if (capability == nullptr) {
        return nullptr;
    }
    activityModeInfo = parcel.ReadParcelable<RSScreenModeInfo>();
    if (activityModeInfo == nullptr) {
        return nullptr;
    }
    if (!parcel.ReadUint32(modeCount)) {
        return nullptr;
    }
    if (!ReadVector(supportModeInfo, modeCount, parcel)) {
        return nullptr;
    }
    if (!parcel.ReadUint8(powerStatus)) {
        return nullptr;
    }
    RSScreenData* screenData = new RSScreenData(*capability, *activityModeInfo,
        supportModeInfo, static_cast<ScreenPowerStatus>(powerStatus));
    return screenData;
}
}
}
