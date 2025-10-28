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

#ifndef RP_HGM_CONFIG_DATA_H
#define RP_HGM_CONFIG_DATA_H

#include <vector>
#include <unordered_set>
#include <parcel.h>
#include "rs_hgm_config_data.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RPHgmConfigData : public Parcelable {
public:
    RPHgmConfigData() = default;
    ~RPHgmConfigData() noexcept;

    [[nodiscard]] static RPHgmConfigData* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

    void AddAnimDynamicItem(const AnimDynamicItem& item)
    {
        configData_.emplace_back(item);
    }

    void AddSmallSizeAnimDynamicItem(const AnimDynamicItem& item)
    {
        smallSizeConfigData_.emplace_back(item);
    }

    const std::vector<AnimDynamicItem>& GetConfigData() const
    {
        return configData_;
    }

    const std::vector<AnimDynamicItem>& GetSmallSizeConfigData() const
    {
        return smallSizeConfigData_;
    }

    int32_t GetSmallSizeArea() const
    {
        return smallSizeArea_;
    }

    int32_t GetSmallSizeLength() const
    {
        return smallSizeLength_;
    }

    void SetSmallSizeArea(int32_t smallSizeArea)
    {
        smallSizeArea_ = smallSizeArea;
    }

    void SetSmallSizeLength(int32_t smallSizeLength)
    {
        smallSizeLength_ = smallSizeLength;
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
    int32_t smallSizeArea_ = -1;
    int32_t smallSizeLength_ = -1;
    std::vector<AnimDynamicItem> configData_;
    std::vector<AnimDynamicItem> smallSizeConfigData_;
    float ppi_ = 1.0f;
    float xDpi_ = 1.0f;
    float yDpi_ = 1.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // RP_HGM_CONFIG_DATA_H