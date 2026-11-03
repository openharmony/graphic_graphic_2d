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
    constexpr uint32_t MAX_ANIM_DYNAMIC_ITEM_SIZE = 256;
    constexpr uint32_t MAX_PAGE_NAME_SIZE = 64;
    constexpr uint32_t MAX_APP_BUFFER_SIZE = 64;
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
    if (!parcel.ReadFloat(data->ppi_) || !parcel.ReadFloat(data->xDpi_) || !parcel.ReadFloat(data->yDpi_) ||
        !parcel.ReadUint32(size)) {
        RS_LOGE("RSHgmConfigData Unmarshalling read base Failed");
        delete data;
        return nullptr;
    }
    if (size > MAX_ANIM_DYNAMIC_ITEM_SIZE) {
        RS_LOGE("RSHgmConfigData Unmarshalling read failed, dynamic size:%{public}u", size);
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
            RS_LOGE("RSHgmConfigData Unmarshalling read data failed");
            delete data;
            return nullptr;
        }
        AnimDynamicItem item = {type, name, minSpeed, maxSpeed, preferredFps};
        data->AddAnimDynamicItem(item);
    }

    if (!UnmarshallingAppBufferList(parcel, *data)) {
        RS_LOGE("%{public}s: UnmarshallingAppBufferList failed", __func__);
        delete data;
        return nullptr;
    }
    if (!UnmarshallingPageNameList(parcel, *data)) {
        RS_LOGE("%{public}s: UnmarshallingPageNameList failed", __func__);
        delete data;
        return nullptr;
    }
    return data;
}

bool RSHgmConfigData::UnmarshallingAppBufferList(Parcel& parcel, RSHgmConfigData& data)
{
    uint32_t appBufferSize;
    if (!parcel.ReadUint32(appBufferSize)) {
        RS_LOGE("%{public}s: read appBufferSize failed", __func__);
        return false;
    }
    if (appBufferSize > MAX_APP_BUFFER_SIZE) {
        RS_LOGE("%{public}s: read vector failed, size:%{public}" PRIu32 ", maxSize:%{public}" PRIu32,
            __func__, appBufferSize, MAX_APP_BUFFER_SIZE);
        return false;
    }
    for (uint32_t i = 0; i < appBufferSize; ++i) {
        std::string appBuffer;
        if (!parcel.ReadString(appBuffer)) {
            RS_LOGE("%{public}s: read app buffer failed", __func__);
            return false;
        }
        if (!appBuffer.empty()) {
            data.AddAppBuffer(std::move(appBuffer));
        }
    }
    return true;
}

bool RSHgmConfigData::UnmarshallingPageNameList(Parcel& parcel, RSHgmConfigData& data)
{
    uint32_t pageNameSize;
    if (!parcel.ReadUint32(pageNameSize)) {
        RS_LOGE("%{public}s: read pageNameSize failed", __func__);
        return false;
    }
    if (pageNameSize > MAX_PAGE_NAME_SIZE) {
        RS_LOGE("%{public}s: page size:%{public}" PRIu32 " exceeds max", __func__, pageNameSize);
        return false;
    }
    for (uint32_t i = 0; i < pageNameSize; i++) {
        std::string pageName;
        if (!parcel.ReadString(pageName)) {
            RS_LOGE("%{public}s: read pageName failed", __func__);
            return false;
        }
        data.AddPageName(pageName);
    }
    return true;
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

    if (!parcel.WriteUint32(appBufferList_.size())) {
        RS_LOGE("%{public}s: write appBufferList size failed", __func__);
        return false;
    }
    for (const auto& item : appBufferList_) {
        if (!parcel.WriteString(item)) {
            RS_LOGE("%{public}s: write app buffer failed", __func__);
            return false;
        }
    }

    if (!parcel.WriteUint32(pageNameList_.size())) {
        return false;
    }
    for (auto& item : pageNameList_) {
        if (!parcel.WriteString(item)) {
            return false;
        }
    }

    return flag;
}
} // namespace Rosen
} // namespace OHOS
