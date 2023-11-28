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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_OCCLUSION_DATA_H
#define ROSEN_RENDER_SERVICE_BASE_RS_OCCLUSION_DATA_H

#include <vector>
#include <parcel.h>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
enum WINDOW_LAYER_INFO_TYPE : uint32_t {
    ALL_VISIBLE = 0,
    SEMI_VISIBLE,
    INVISIBLE,
    WINDOW_LAYER_DYNAMIC_STATUS,
    WINDOW_LAYER_STATIC_STATUS,
    WINDOW_LAYER_UNKNOWN_TYPE,
};
using VisibleData = std::vector<std::pair<uint64_t, WINDOW_LAYER_INFO_TYPE>>;
class RSB_EXPORT RSOcclusionData : public Parcelable {
public:
    RSOcclusionData() = default;
    RSOcclusionData(VisibleData& vec)
    {
        std::copy(vec.begin(), vec.end(), std::back_inserter(visibleData_));
    }
    RSOcclusionData(RSOcclusionData&& other) : visibleData_(std::move(other.visibleData_)) {}
    ~RSOcclusionData() noexcept;

    VisibleData& GetVisibleData()
    {
        return visibleData_;
    }
    [[nodiscard]] static RSOcclusionData* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

private:
    VisibleData visibleData_;
};
} // namespace Rosen
} // namespace OHOS

#endif
