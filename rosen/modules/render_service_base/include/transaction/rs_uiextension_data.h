/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_UIEXTENSION_DATA_H
#define ROSEN_RENDER_SERVICE_BASE_RS_UIEXTENSION_DATA_H

#include <map>
#include <parcel.h>
#include <vector>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"

namespace OHOS {
namespace Rosen {

struct SecRectInfo {
    RectI relativeCoords = RectI();
    Vector2f scale = {1.f, 1.f};
    Vector2f anchor = {0.f, 0.f};
};

struct SecSurfaceInfo {
    SecRectInfo uiExtensionRectInfo;
    pid_t hostPid = 0;
    pid_t uiExtensionPid = 0;
    uint64_t hostNodeId = INVALID_NODEID;
    uint64_t uiExtensionNodeId = INVALID_NODEID;
    std::vector<SecRectInfo> upperNodes = {};
};

using UIExtensionCallbackData = std::map<NodeId, std::vector<SecSurfaceInfo>>;
class RSB_EXPORT RSUIExtensionData : public Parcelable {
public:
    RSUIExtensionData() = default;
    RSUIExtensionData(UIExtensionCallbackData& data)
    {
        for (UIExtensionCallbackData::iterator iter = data.begin(); iter != data.end(); ++iter) {
            secData_.insert(std::make_pair(iter->first, iter->second));
        }
    }
    RSUIExtensionData(RSUIExtensionData&& other) : secData_(std::move(other.secData_)) {}
    virtual ~RSUIExtensionData() noexcept = default;

    const UIExtensionCallbackData& GetSecData() const
    {
        return secData_;
    }
    [[nodiscard]] static RSUIExtensionData* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

private:
    static bool MarshallingRectInfo(const SecRectInfo& rectInfo, Parcel& parcel);
    static void UnmarshallingRectInfo(SecRectInfo& rectInfo, Parcel& parcel);
    UIExtensionCallbackData secData_;
};
} // namespace Rosen
} // namespace OHOS

#endif