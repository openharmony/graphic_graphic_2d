/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "transaction/rs_occlusion_data.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSOcclusionData::~RSOcclusionData() noexcept
{
}

RSOcclusionData* RSOcclusionData::Unmarshalling(Parcel& parcel)
{
    auto data = new RSOcclusionData();
    auto size = parcel.ReadUint32();
    for (uint32_t i = 0; i < size; i++) {
        uint64_t id = parcel.ReadUint64();
        data->visibleData_.emplace_back(id);
    }
    return data;
}

bool RSOcclusionData::Marshalling(Parcel& parcel) const
{
    parcel.WriteUint32(visibleData_.size());
    for (auto& data : visibleData_) {
        parcel.WriteUint64(data);
    }

    return true;
}
} // namespace Rosen
} // namespace OHOS
