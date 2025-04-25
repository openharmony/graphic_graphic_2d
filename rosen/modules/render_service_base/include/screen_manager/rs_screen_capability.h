/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RS_SCREEN_CAPABILITY
#define RS_SCREEN_CAPABILITY

#include <cstdint>
#include <parcel.h>
#include <refbase.h>
#include <string>
#include <vector>

#include "common/rs_macros.h"
#include "screen_manager/rs_screen_props.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSScreenCapability : public Parcelable {
public:
    RSScreenCapability() = default;
    RSScreenCapability(std::string name, ScreenInterfaceType type, uint32_t phyWidth,
        uint32_t phyHeight, uint32_t supportLayers, uint32_t virtualDispCount,
        bool supportWriteBack, const std::vector<RSScreenProps>& props);
    ~RSScreenCapability() = default;
    [[nodiscard]] static RSScreenCapability* Unmarshalling(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;

    void SetName(const std::string& name);
    void SetType(ScreenInterfaceType type);
    void SetPhyWidth(uint32_t phyWidth);
    void SetPhyHeight(uint32_t phyHeight);
    void SetSupportLayers(uint32_t supportLayers);
    void SetVirtualDispCount(uint32_t virtualDispCount);
    void SetSupportWriteBack(bool supportWriteBack);
    void SetProps(std::vector<RSScreenProps> props);

    const std::string& GetName() const;
    ScreenInterfaceType GetType() const;
    uint32_t GetPhyWidth() const;
    uint32_t GetPhyHeight() const;
    uint32_t GetSupportLayers() const;
    uint32_t GetVirtualDispCount() const;
    bool GetSupportWriteBack() const;
    const std::vector<RSScreenProps>& GetProps() const;

private:
    bool WriteVector(const std::vector<RSScreenProps>& props, Parcel &parcel) const;
    static bool ReadVector(std::vector<RSScreenProps>& unmarProps, uint32_t unmarPropCount, Parcel &parcel);
    std::string name_;
    ScreenInterfaceType type_ = DISP_INVALID;
    uint32_t phyWidth_ = 0;
    uint32_t phyHeight_ = 0;
    uint32_t supportLayers_ = 0;
    uint32_t virtualDispCount_ = 0;
    bool supportWriteBack_ = false;
    std::vector<RSScreenProps> props_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN_CAPABILITY