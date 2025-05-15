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

#include <gtest/gtest.h>

#include "display_engine/rs_color_temperature.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorTemperatureTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorTemperatureTest::SetUpTestCase() {}
void RSColorTemperatureTest::TearDownTestCase() {}
void RSColorTemperatureTest::SetUp() {}
void RSColorTemperatureTest::TearDown() {}

/**
 * @tc.name: ColorTemperature001
 * @tc.desc: Test ColorTemperature class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature001, TestSize.Level1)
{
    ScreenId screenId{};
    ScreenPowerStatus status{};
    std::function<void()> refresh = []() -> void {};
    auto& colorTemp = RSColorTemperature::Get();
    colorTemp.Init();
    colorTemp.RegisterRefresh(std::move(refresh));
    colorTemp.UpdateScreenStatus(screenId, status);
    colorTemp.DimmingIncrease(screenId);

    ASSERT_NE((&colorTemp), nullptr);
}

/**
 * @tc.name: ColorTemperature002
 * @tc.desc: Test ColorTemperature class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature002, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    ASSERT_EQ(colorTemp.IsDimmingOn(0), false);
}

/**
 * @tc.name: ColorTemperature003
 * @tc.desc: Test ColorTemperature class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature003, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    std::vector<float> res = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    ASSERT_EQ(colorTemp.GetNewLinearCct(0), res);
}

/**
 * @tc.name: ColorTemperature004
 * @tc.desc: Test ColorTemperature class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature004, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    std::vector<uint8_t> data = {1, 1, 1};
    std::vector<float> res = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    ASSERT_EQ(colorTemp.GetLayerLinearCct(0, data), res);
}

/**
 * @tc.name: ColorTemperature005
 * @tc.desc: Test ColorTemperature class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature005, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    ASSERT_EQ(colorTemp.IsColorTemperatureOn(), false);
}
} // namespace OHOS::Rosen