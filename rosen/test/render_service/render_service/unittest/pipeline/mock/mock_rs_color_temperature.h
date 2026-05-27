/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_RS_COLOR_TEMPERATURE_MOCK_H
#define GRAPHIC_RS_COLOR_TEMPERATURE_MOCK_H

#include "display_engine/rs_color_temperature.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
namespace Mock {

class RSColorTemperatureInterfaceMock : public RSColorTemperatureInterface {
public:
    RSColorTemperatureInterfaceMock() = default;
    ~RSColorTemperatureInterfaceMock() override = default;

    MOCK_METHOD(bool, Init, (), (override));
    MOCK_METHOD(void, RegisterRefresh, (std::function<void()>&& refreshFunc), (override));
    MOCK_METHOD(void, UpdateScreenStatus, (ScreenId screenId, ScreenPowerStatus powerStatus), (override));
    MOCK_METHOD(bool, IsDimmingOn, (ScreenId screenId), (override));
    MOCK_METHOD(void, DimmingIncrease, (ScreenId screenId), (override));
    MOCK_METHOD(bool, IsColorTemperatureOn, (), (override, const));
    MOCK_METHOD(std::vector<float>, GetNewLinearCct, (ScreenId screenId), (override));
#ifndef ROSEN_CROSS_PLATFORM
    MOCK_METHOD(std::vector<float>, GetLayerLinearCct, (ScreenId screenId,
        const std::vector<uint8_t>& dynamicMetadata, const CM_Matrix srcColorMatrix), (override));
#endif
};

} // namespace Mock
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHIC_RS_COLOR_TEMPERATURE_MOCK_H