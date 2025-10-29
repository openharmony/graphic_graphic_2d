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
static constexpr size_t PARCEL_MAX_CAPACITY = 2000 * 1024;
constexpr uint32_t MAX_ANIM_DYNAMIC_ITEM_SIZE = 256;
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
    return data;
}

bool RPHgmConfigData::Marshalling(Parcel& parcel) const
{
    parcel.SetMaxCapacity(PARCEL_MAX_CAPACITY);
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
    return success;
}
} // namespace Rosen
} // namespace OHOS