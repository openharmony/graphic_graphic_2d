/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_HGM_CONFIG_DATA_H
#define ROSEN_RENDER_SERVICE_BASE_RS_HGM_CONFIG_DATA_H

#include <vector>
#include <parcel.h>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
struct AnimDynamicItem {
    std::string animType = "";
    std::string animName = "";
    int32_t minSpeed = 0;
    int32_t maxSpeed = 0;
    int32_t preferredFps = 0;
};

class RSB_EXPORT RSHgmConfigData : public Parcelable {
public:
    RSHgmConfigData() = default;
    RSHgmConfigData(RSHgmConfigData&& other) : configData_(std::move(other.configData_)) {}
    ~RSHgmConfigData() noexcept;

    [[nodiscard]] static RSHgmConfigData* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

    void AddAnimDynamicItem(AnimDynamicItem item)
    {
        configData_.emplace_back(item);
    }

    const std::vector<AnimDynamicItem>& GetConfigData() const
    {
        return configData_;
    }

    float GetPpi() const
    {
        return ppi_;
    }

    float GetXDpi() const
    {
        return xDpi_;
    }

    float GetYDpi() const
    {
        return yDpi_;
    }

    void SetPpi(float ppi)
    {
        ppi_ = ppi;
    }

    void SetXDpi(float xDpi)
    {
        xDpi_ = xDpi;
    }

    void SetYDpi(float yDpi)
    {
        yDpi_ = yDpi;
    }

private:
    std::vector<AnimDynamicItem> configData_;
    float ppi_ = 1.0f;
    float xDpi_ = 1.0f;
    float yDpi_ = 1.0f;
};
} // namespace Rosen
} // namespace OHOS

#endif
