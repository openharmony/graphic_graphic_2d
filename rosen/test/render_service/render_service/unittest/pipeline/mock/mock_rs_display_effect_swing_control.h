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
 
#ifndef GRAPHIC_RS_DISPLAY_EFFECT_SWING_CONTROL_MOCK_H
#define GRAPHIC_RS_DISPLAY_EFFECT_SWING_CONTROL_MOCK_H
 
#include "display_engine/rs_display_effect_swing_control.h"
#include <gmock/gmock.h>
 
namespace OHOS {
namespace Rosen {
namespace Mock {
class RSDisplayEffectSwingControlInterfaceMock : public RSDisplayEffectSwingControlInterface {
public:
    RSDisplayEffectSwingControlInterfaceMock() = default;
    ~RSDisplayEffectSwingControlInterfaceMock() override = default;
 
    MOCK_METHOD(bool, Init, (), (override));
    MOCK_METHOD(void, SetSwingEnabled, (ScreenId screenId, bool enable), (override));
    MOCK_METHOD(bool, IsSwingRegistered, (ScreenId screenId), (override));
    MOCK_METHOD(SwingData, GetSwingData, (ScreenId screenId), (override));
};
} // namespace Mock
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHIC_RS_DISPLAY_EFFECT_SWING_CONTROL_MOCK_H