/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifdef ROSEN_OHOS
#include <parcel.h>
#endif

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
class RSOcclusionData : public Parcelable {
#else
class RSOcclusionData {
#endif
public:
    RSOcclusionData() = default;
    RSOcclusionData(std::vector<std::pair<uint64_t, bool>>& vec)
    {
        copy(vec.begin(), vec.end(), back_inserter(visibleData_));
    }
    RSOcclusionData(RSOcclusionData&& other) : visibleData_(std::move(other.visibleData_)) {}
    ~RSOcclusionData() noexcept;

    const std::vector<std::pair<uint64_t, bool>>& GetVisibleData() const
    {
        return visibleData_;
    }
#ifdef ROSEN_OHOS
    static RSOcclusionData* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;
#endif

private:
    std::vector<std::pair<uint64_t, bool>> visibleData_;
};
} // namespace Rosen
} // namespace OHOS

#endif
