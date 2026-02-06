/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "transaction/rs_hgm_config_data.h"

#include <memory>
#include "platform/common/rs_log.h"

namespace {
    constexpr uint32_t MAX_ANIM_DYNAMIC_ITEM_SIZE = 256;
    constexpr uint32_t MAX_PAGE_NAME_SIZE = 64;
}

namespace OHOS {
namespace Rosen {
RSHgmConfigData::~RSHgmConfigData() noexcept
{
    configData_.clear();
}

RSHgmConfigData* RSHgmConfigData::Unmarshalling(Parcel& parcel)
{
    auto data = std::make_unique<RSHgmConfigData>();
    if (!parcel.ReadBool(data->isSyncConfig_)) {
        RS_LOGE("RSHgmConfigData Unmarshalling read isSyncConfig failed");
        return nullptr;
    }
    if (data->isSyncConfig_) {
        if (!data->UnmarshallingConfig(parcel)) {
            RS_LOGE("RSHgmConfigData Unmarshalling read config failed");
            return nullptr;
        }
    }

    if (!parcel.ReadBool(data->isSyncEnergyData_)) {
        RS_LOGE("RSHgmConfigData Unmarshalling read isSyncEnergyData failed");
        return nullptr;
    }
    if (data->isSyncEnergyData_) {
        if (!data->UnmarshallingEnergyData(parcel)) {
            RS_LOGE("RSHgmConfigData Unmarshalling read EnergyData failed");
            return nullptr;
        }
    }
    return data.release();
}

bool RSHgmConfigData::Marshalling(Parcel& parcel) const
{
    bool flag = parcel.WriteBool(isSyncConfig_);
    if (!flag) {
        RS_LOGE("RSHgmConfigData::Marshalling parse isSyncConfig failed");
        return flag;
    }
    if (isSyncConfig_) {
        flag = MarshallingConfig(parcel);
        if (!flag) {
            RS_LOGE("RSHgmConfigData::Marshalling parse config failed");
            return flag;
        }
    }

    flag = parcel.WriteBool(isSyncEnergyData_);
    if (!flag) {
        RS_LOGE("RSHgmConfigData::Marshalling parse isSyncEnergyData failed");
        return flag;
    }
    if (isSyncEnergyData_) {
        flag = MarshallingEnergyData(parcel);
        if (!flag) {
            RS_LOGE("RSHgmConfigData::Marshalling parse energyData failed");
            return flag;
        }
    }
    return flag;
}

bool RSHgmConfigData::UnmarshallingConfig(Parcel& parcel)
{
    uint32_t size;
    if (!parcel.ReadFloat(ppi_) || !parcel.ReadFloat(xDpi_) || !parcel.ReadFloat(yDpi_) || !parcel.ReadUint32(size)) {
        RS_LOGE("RSHgmConfigData UnmarshallingConfig read failed");
        return false;
    }
    size_t readableSize = parcel.GetReadableBytes() / sizeof(uint64_t);
    size_t len = static_cast<size_t>(size);
    if (size > MAX_ANIM_DYNAMIC_ITEM_SIZE || len > readableSize) {
        RS_LOGE(
            "RSHgmConfigData UnmarshallingConfig Failed read vector, size:%zu, readableSize:%zu", len, readableSize);
        return false;
    }
    std::string type;
    std::string name;
    int32_t minSpeed;
    int32_t maxSpeed;
    int32_t preferredFps;
    for (uint32_t i = 0; i < size; i++) {
        if (!parcel.ReadString(type) || !parcel.ReadString(name) || !parcel.ReadInt32(minSpeed) ||
            !parcel.ReadInt32(maxSpeed) || !parcel.ReadInt32(preferredFps)) {
            RS_LOGE("RSHgmConfigData UnmarshallingConfig read data failed");
            return false;
        }
        AnimDynamicItem item = { type, name, minSpeed, maxSpeed, preferredFps };
        AddAnimDynamicItem(item);
    }

    uint32_t pageNameSize;
    if (!parcel.ReadUint32(pageNameSize)) {
        RS_LOGE("RSHgmConfigData UnmarshallingConfig read data failed");
        return false;
    }
    len = static_cast<size_t>(pageNameSize);
    if (pageNameSize > MAX_PAGE_NAME_SIZE || len > readableSize) {
        RS_LOGE("RSHgmConfigData UnmarshallingConfig Failed read vector, size:%zu, readableSize:%zu",
            len, readableSize);
        return false;
    }
    for (uint32_t i = 0; i < pageNameSize; i++) {
        std::string pageName;
        if (!parcel.ReadString(pageName)) {
            RS_LOGE("RSHgmConfigData UnmarshallingConfig read data failed");
            return false;
        }
        AddPageName(pageName);
    }
    return true;
}

bool RSHgmConfigData::MarshallingConfig(Parcel& parcel) const
{
    bool flag = parcel.WriteFloat(ppi_) && parcel.WriteFloat(xDpi_) && parcel.WriteFloat(yDpi_) &&
        parcel.WriteUint32(configData_.size());
    if (!flag) {
        RS_LOGE("RSHgmConfigData::Marshalling parse dpi failed");
        return flag;
    }

    for (auto& item : configData_) {
        flag = parcel.WriteString(item.animType) && parcel.WriteString(item.animName) &&
               parcel.WriteInt32(item.minSpeed) && parcel.WriteInt32(item.maxSpeed) &&
               parcel.WriteInt32(item.preferredFps);
        if (!flag) {
            RS_LOGE("RSHgmConfigData::Marshalling parse config item failed");
            return flag;
        }
    }

    parcel.WriteUint32(pageNameList_.size());
    for (auto& item : pageNameList_) {
        parcel.WriteString(item);
    }

    return flag;
}

bool RSHgmConfigData::UnmarshallingEnergyData(Parcel& parcel)
{
    if (!parcel.ReadString(energyInfo_.componentName) || !parcel.ReadInt32(energyInfo_.componentDefaultFps)) {
        RS_LOGE("RSHgmConfigData UnmarshallingEnergyData read data failed");
        return false;
    }
    return true;
}

bool RSHgmConfigData::MarshallingEnergyData(Parcel& parcel) const
{
    if (!parcel.WriteString(energyInfo_.componentName) || !parcel.WriteInt32(energyInfo_.componentDefaultFps)) {
        RS_LOGE("RSHgmConfigData::MarshallingEnergyData parse data failed");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
