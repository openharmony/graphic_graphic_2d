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

#include "transaction/rp_hgm_config_data.h"
#include "platform/common/rs_log.h"

namespace {
constexpr uint32_t MAX_ANIM_DYNAMIC_ITEM_SIZE = 256;
constexpr uint32_t MAX_POWER_CONFIG_SIZE = 10;
}

namespace OHOS {
namespace Rosen {
RPHgmConfigData* RPHgmConfigData::Unmarshalling(Parcel& parcel)
{
    auto data = new RPHgmConfigData();
    uint32_t size;
    if (!parcel.ReadFloat(data->ppi_) || !parcel.ReadFloat(data->xDpi_) || !parcel.ReadFloat(data->yDpi_) ||
        !parcel.ReadInt32(data->smallSizeArea_) || !parcel.ReadInt32(data->smallSizeLength_) ||
        !parcel.ReadUint32(size)) {
        RS_LOGE("RPHgmConfigData Unmarshalling read failed");
        delete data;
        return nullptr;
    }
    if (size > MAX_ANIM_DYNAMIC_ITEM_SIZE) {
        RS_LOGE("RPHgmConfigData Unmarshalling Failed read vector, size:%{public}" PRIu32, size);
        delete data;
        return nullptr;
    }
    std::string type;
    std::string name;
    int32_t minSpeed;
    int32_t maxSpeed;
    int32_t preferredFps;
    for (uint32_t i = 0; i < size; i++) {
        if (!parcel.ReadString(type) || !parcel.ReadString(name) || !parcel.ReadInt32(minSpeed) ||
            !parcel.ReadInt32(maxSpeed) || !parcel.ReadInt32(preferredFps)) {
            RS_LOGE("RPHgmConfigData Unmarshalling read data failed");
            delete data;
            return nullptr;
        }
        AnimDynamicItem item = { std::move(type), std::move(name), minSpeed, maxSpeed, preferredFps };
        data->AddAnimDynamicItem(item);
    }

    uint32_t smallSize;
    if (!parcel.ReadUint32(smallSize)) {
        RS_LOGE("RPHgmConfigData Unmarshalling read small failed");
        delete data;
        return nullptr;
    }
    if (smallSize > MAX_ANIM_DYNAMIC_ITEM_SIZE) {
        RS_LOGE("RPHgmConfigData Unmarshalling Failed read small vector, size:%{public}" PRIu32, smallSize);
        delete data;
        return nullptr;
    }
    for (uint32_t i = 0; i < smallSize; i++) {
        if (!parcel.ReadString(type) || !parcel.ReadString(name) || !parcel.ReadInt32(minSpeed) ||
            !parcel.ReadInt32(maxSpeed) || !parcel.ReadInt32(preferredFps)) {
            RS_LOGE("RPHgmConfigData Unmarshalling read small data failed");
            delete data;
            return nullptr;
        }
        AnimDynamicItem item = { std::move(type), std::move(name), minSpeed, maxSpeed, preferredFps };
        data->AddSmallSizeAnimDynamicItem(item);
    }
    if (!data->UnmarshallingComponentPowerConfig(parcel)) {
        RS_LOGE("%{public}s read componentPowerConfig data failed", __func__);
        return data;
    }

    bool isVideoSwitchOn = false;
    if (!parcel.ReadBool(isVideoSwitchOn)) {
        RS_LOGE("%{public}s read isVideoSwitchOn failed", __func__);
        return data;
    }
    data->isVideoSwitchOn_ = isVideoSwitchOn;
    return data;
}

bool RPHgmConfigData::Marshalling(Parcel& parcel) const
{
    bool success = parcel.WriteFloat(ppi_) && parcel.WriteFloat(xDpi_) && parcel.WriteFloat(yDpi_) &&
        parcel.WriteInt32(smallSizeArea_) && parcel.WriteInt32(smallSizeLength_) &&
        parcel.WriteUint32(configData_.size());
    if (!success) {
        RS_LOGE("RPHgmConfigData::Marshalling parse failed");
        return success;
    }

    for (auto& item : configData_) {
        success = parcel.WriteString(item.animType) && parcel.WriteString(item.animName) &&
            parcel.WriteInt32(item.minSpeed) && parcel.WriteInt32(item.maxSpeed) &&
            parcel.WriteInt32(item.preferredFps);
        if (!success) {
            RS_LOGE("RPHgmConfigData::Marshalling parse config item failed");
            return success;
        }
    }

    success = parcel.WriteUint32(smallSizeConfigData_.size());
    if (!success) {
        RS_LOGE("RPHgmConfigData::Marshalling parse small failed");
        return success;
    }

    for (auto& item : smallSizeConfigData_) {
        success = parcel.WriteString(item.animType) && parcel.WriteString(item.animName) &&
            parcel.WriteInt32(item.minSpeed) && parcel.WriteInt32(item.maxSpeed) &&
            parcel.WriteInt32(item.preferredFps);
        if (!success) {
            RS_LOGE("RPHgmConfigData::Marshalling parse small config item failed");
            return success;
        }
    }

    success = MarshallingComponentPowerConfig(parcel);
    if (!success) {
        return success;
    }

    success = parcel.WriteBool(isVideoSwitchOn_);
    if (!success) {
        RS_LOGE("RPHgmConfigData::Marshalling parse isVideoSwitchOn config item failed");
        return success;
    }
    return success;
}

bool RPHgmConfigData::MarshallingComponentPowerConfig(Parcel& parcel) const
{
    uint32_t size = static_cast<uint32_t>(componentPowerConfig_.size());
    if (size > MAX_POWER_CONFIG_SIZE) {
        RS_LOGE("%{public}s energyCommonData Failed size:%{public}" PRIu32, __func__, size);
        if (!parcel.WriteInt32(0)) {
            return false;
        }
        return true;
    }
    if (!parcel.WriteUint32(size)) {
        RS_LOGE("%{public}s parse power config failed", __func__);
        return false;
    }
    for (const auto& item : componentPowerConfig_) {
        if (!parcel.WriteString(item.first) || !parcel.WriteInt32(item.second)) {
            RS_LOGE("%{public}s parse power config item failed", __func__);
            return false;
        }
    }
    return true;
}

bool RPHgmConfigData::UnmarshallingComponentPowerConfig(Parcel& parcel)
{
    uint32_t configSize;
    if (!parcel.ReadUint32(configSize)) {
        RS_LOGE("%{public}s read configSize failed", __func__);
        return false;
    }
    if (configSize > MAX_POWER_CONFIG_SIZE) {
        RS_LOGE("%{public}s Failed read map, size:%{public}" PRIu32, __func__, configSize);
        return false;
    }
    for (uint32_t i = 0; i < configSize; i++) {
        std::string key;
        int32_t value;
        if (!parcel.ReadString(key) || !parcel.ReadInt32(value)) {
            RS_LOGE("%{public}s read map failed", __func__);
            componentPowerConfig_.clear();
            return false;
        }
        componentPowerConfig_.emplace(std::move(key), value);
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS