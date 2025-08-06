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
#include "platform/common/rs_log.h"

namespace {
    static constexpr size_t PARCEL_MAX_CAPACITY = 2000 * 1024;
    constexpr int32_t MAX_ANIM_DYNAMIC_ITEM_SIZE = 2048;
    constexpr int32_t MAX_PAGE_NAME_SIZE = 50;
}

namespace OHOS {
namespace Rosen {
RSHgmConfigData::~RSHgmConfigData() noexcept
{
    configData_.clear();
}

RSHgmConfigData* RSHgmConfigData::Unmarshalling(Parcel& parcel)
{
    auto data = new RSHgmConfigData();
    uint32_t size;
    uint32_t pageNameSize;
    if (!parcel.ReadFloat(data->ppi_) || !parcel.ReadFloat(data->xDpi_) || !parcel.ReadFloat(data->yDpi_) ||
        !parcel.ReadUint32(size)) {
        RS_LOGE("RSHgmConfigData Unmarshalling read failed");
        return data;
    }
    size_t readableSize = parcel.GetReadableBytes() / sizeof(uint64_t);
    size_t len = static_cast<size_t>(size);
    if (len > readableSize || len > data->configData_.max_size() || size > MAX_ANIM_DYNAMIC_ITEM_SIZE) {
        RS_LOGE("RSHgmConfigData Unmarshalling Failed read vector, size:%zu, readableSize:%zu", len, readableSize);
        return data;
    }
    std::string type;
    std::string name;
    int32_t minSpeed;
    int32_t maxSpeed;
    int32_t preferredFps;
    for (uint32_t i = 0; i < size; i++) {
        if (!parcel.ReadString(type) || !parcel.ReadString(name) || !parcel.ReadInt32(minSpeed) ||
            !parcel.ReadInt32(maxSpeed) || !parcel.ReadInt32(preferredFps)) {
            RS_LOGE("RSHgmConfigData Unmarshalling read data failed");
            return data;
        }
        AnimDynamicItem item = {type, name, minSpeed, maxSpeed, preferredFps};
        data->AddAnimDynamicItem(item);
    }

    if (!parcel.ReadUint32(pageNameSize)) {
        RS_LOGE("RSHgmConfigData Unmarshalling read data failed");
        return data;
    }
    len = static_cast<size_t>(pageNameSize);
    if (len > readableSize || len > data->configData_.max_size() || pageNameSize > MAX_PAGE_NAME_SIZE) {
        RS_LOGE("RSHgmConfigData Unmarshalling Failed read vector, size:%zu, readableSize:%zu", len, readableSize);
        return data;
    }
    for (uint32_t i = 0; i < pageNameSize; i++) {
        std::string pageName;
        if (!parcel.ReadString(pageName)) {
            RS_LOGE("RSHgmConfigData Unmarshalling read data failed");
            return data;
        }
        data->AddPageName(pageName);
    }
    return data;
}

bool RSHgmConfigData::Marshalling(Parcel& parcel) const
{
    parcel.SetMaxCapacity(PARCEL_MAX_CAPACITY);
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
} // namespace Rosen
} // namespace OHOS
