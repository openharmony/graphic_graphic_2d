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
    data->ppi_ = parcel.ReadFloat();
    data->xDpi_ = parcel.ReadFloat();
    data->yDpi_ = parcel.ReadFloat();
    auto size = parcel.ReadUint32();
    size_t readableSize = parcel.GetReadableBytes() / sizeof(uint64_t);
    size_t len = static_cast<size_t>(size);
    if (len > readableSize || len > data->configData_.max_size()) {
        RS_LOGE("RSHgmConfigData Unmarshalling Failed read vector, size:%zu, readableSize:%zu", len, readableSize);
        return data;
    }
    for (uint32_t i = 0; i < size; i++) {
        std::string type = parcel.ReadString();
        std::string name = parcel.ReadString();
        auto minSpeed = parcel.ReadInt32();
        auto maxSpeed = parcel.ReadInt32();
        auto preferredFps = parcel.ReadInt32();
        AnimDynamicItem item = {type, name, minSpeed, maxSpeed, preferredFps};
        data->AddAnimDynamicItem(item);
    }
    return data;
}

bool RSHgmConfigData::Marshalling(Parcel& parcel) const
{
    parcel.SetMaxCapacity(PARCEL_MAX_CAPACITY);
    parcel.WriteFloat(ppi_);
    parcel.WriteFloat(xDpi_);
    parcel.WriteFloat(yDpi_);
    parcel.WriteUint32(configData_.size());

    for (auto& item : configData_) {
        parcel.WriteString(item.animType);
        parcel.WriteString(item.animName);
        parcel.WriteInt32(item.minSpeed);
        parcel.WriteInt32(item.maxSpeed);
        parcel.WriteInt32(item.preferredFps);
    }

    return true;
}
} // namespace Rosen
} // namespace OHOS
