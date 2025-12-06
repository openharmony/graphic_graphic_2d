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

#ifndef GRAPHIC_RS_LUMINANCE_CONTROL_MOCK_H
#define GRAPHIC_RS_LUMINANCE_CONTROL_MOCK_H

#include "display_engine/rs_luminance_control.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
namespace Mock {
class RSLuminanceControlInterfaceMock : public RSLuminanceControlInterface {
public:
    RSLuminanceControlInterfaceMock() = default;
    ~RSLuminanceControlInterfaceMock() override = default;

    MOCK_METHOD(bool, Init, (), (override));
    MOCK_METHOD(void, UpdateScreenStatus, (ScreenId screenId, ScreenPowerStatus powerStatus), (override));
    MOCK_METHOD(bool, IsDimmingOn, (ScreenId screenId), (override));
    MOCK_METHOD(void, DimmingIncrease, (ScreenId screenId), (override));
    MOCK_METHOD(void, SetSdrLuminance, (ScreenId screenId, uint32_t level), (override));
    MOCK_METHOD(uint32_t, GetNewHdrLuminance, (ScreenId screenId), (override));
    MOCK_METHOD(void, SetNowHdrLuminance, (ScreenId screenId, uint32_t level), (override));
    MOCK_METHOD(bool, IsNeedUpdateLuminance, (ScreenId screenId), (override));
    MOCK_METHOD(float, GetSdrDisplayNits, (ScreenId screenId), (override));
    MOCK_METHOD(float, GetDisplayNits, (ScreenId screenId), (override));
    MOCK_METHOD(double, GetNonlinearRatio, (ScreenId screenId, uint32_t mode), (override));
    MOCK_METHOD(float, CalScaler, (const float& maxContentLightLevel, const std::vector<uint8_t>& dynamicMetadata,
        const float& ratio, HdrStatus hdrStatus), (override));
    MOCK_METHOD(uint32_t, ConvertScalerFromFloatToLevel, (float& scaler), (override, const));
    MOCK_METHOD(float, ConvertScalerFromLevelToFloat, (uint32_t& level), (override, const));
    using HdrToBrightnessScalerMap = std::unordered_map<HdrStatus, std::unordered_map<uint32_t, uint32_t>>;
    MOCK_METHOD(void, SetCurDisplayHdrBrightnessScaler,
        (ScreenId screenId, HdrToBrightnessScalerMap& curDisplayHdrBrightnessScaler), (override));
    MOCK_METHOD(bool, IsHdrPictureOn, (), (override));
    MOCK_METHOD(bool, IsForceCloseHdr, (), (override, const));
    MOCK_METHOD(void, ForceCloseHdr, (uint32_t closeHdrSceneId, bool forceCloseHdr), (override));
    MOCK_METHOD(bool, IsScreenNoHeadroom, (ScreenId screenId), (override, const));
    MOCK_METHOD(double, GetMaxScaler, (ScreenId screenId), (override, const));
    MOCK_METHOD(BrightnessInfo, GetBrightnessInfo, (ScreenId screenId), (override));
    MOCK_METHOD(bool, IsBrightnessInfoChanged, (ScreenId screenId), (override));
    MOCK_METHOD(void, HandleGamutSpecialRender, (std::vector<ScreenColorGamut>& modes), (override));

    bool IsHardwareHdrDisabled(bool checkBrightnessRatio, ScreenId screenId) override;
    bool SetHdrStatus(ScreenId screenId, HdrStatus curDisplayHdrStatus) override;
    bool IsHdrOn(ScreenId screenId) const override;

    bool isHardwareHdrDisabled_{false};
    bool isHdrOn_{false};
    HdrStatus hdrStatus_{HdrStatus::NO_HDR};
};
} // namespace Mock
} // namespace Rosen
} // namespace OHOS
#endif // GRAPHIC_RS_LUMINANCE_CONTROL_MOCK_H