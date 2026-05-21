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
#include <gmock/gmock.h>

#include "display_engine/rs_color_temperature.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class MockRSColorTemperature : public RSColorTemperatureInterface {
public:
    static std::shared_ptr<MockRSColorTemperature> GetInstance()
    {
        static auto instance = std::make_shared<MockRSColorTemperature>();
        return instance;
    }

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

    auto mockRSColorTemperature = MockRSColorTemperature::GetInstance();
    colorTemp.rSColorTemperatureInterface_ = mockRSColorTemperature.get();
    ASSERT_NE(colorTemp.rSColorTemperatureInterface_, nullptr);
    colorTemp.RegisterRefresh(std::move(refresh));
    colorTemp.UpdateScreenStatus(screenId, status);
    colorTemp.DimmingIncrease(screenId);

    ASSERT_NE((&colorTemp), nullptr);
}

/**
 * @tc.name: ColorTemperature002
 * @tc.desc: Test ColorTemperature IsDimmingOn returns false with mock
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature002, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    auto mockRSColorTemperature = MockRSColorTemperature::GetInstance();
    colorTemp.rSColorTemperatureInterface_ = mockRSColorTemperature.get();
    ASSERT_NE(colorTemp.rSColorTemperatureInterface_, nullptr);
    ASSERT_EQ(colorTemp.IsDimmingOn(0), false);
}

/**
 * @tc.name: ColorTemperature003
 * @tc.desc: Test ColorTemperature IsDimmingOn returns true with mock
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature003, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    auto mockRSColorTemperature = MockRSColorTemperature::GetInstance();
    colorTemp.rSColorTemperatureInterface_ = mockRSColorTemperature.get();
    ASSERT_NE(colorTemp.rSColorTemperatureInterface_, nullptr);
    EXPECT_CALL(*mockRSColorTemperature, IsDimmingOn(0)).WillOnce(Return(true));
    ASSERT_EQ(colorTemp.IsDimmingOn(0), true);
}

/**
 * @tc.name: ColorTemperature004
 * @tc.desc: Test ColorTemperature GetNewLinearCct with mock
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature004, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    auto mockRSColorTemperature = MockRSColorTemperature::GetInstance();
    colorTemp.rSColorTemperatureInterface_ = mockRSColorTemperature.get();
    ASSERT_NE(colorTemp.rSColorTemperatureInterface_, nullptr);
    std::vector<float> expectedRes = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    EXPECT_CALL(*mockRSColorTemperature, GetNewLinearCct(0)).WillOnce(Return(expectedRes));
    ASSERT_EQ(colorTemp.GetNewLinearCct(0), expectedRes);
}

/**
 * @tc.name: ColorTemperature005
 * @tc.desc: Test ColorTemperature GetNewLinearCct returns default matrix when interface is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature005, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    colorTemp.rSColorTemperatureInterface_ = nullptr;
    std::vector<float> res = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    ASSERT_EQ(colorTemp.GetNewLinearCct(0), res);
}

/**
 * @tc.name: ColorTemperature006
 * @tc.desc: Test ColorTemperature GetLayerLinearCct with mock
 * @tc.type: FUNC
 * @tc.require:
 */
#ifndef ROSEN_CROSS_PLATFORM
HWTEST_F(RSColorTemperatureTest, ColorTemperature006, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    auto mockRSColorTemperature = MockRSColorTemperature::GetInstance();
    colorTemp.rSColorTemperatureInterface_ = mockRSColorTemperature.get();
    ASSERT_NE(colorTemp.rSColorTemperatureInterface_, nullptr);
    std::vector<uint8_t> data = {1, 1, 1};
    std::vector<float> expectedRes = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    EXPECT_CALL(*mockRSColorTemperature, GetLayerLinearCct(0, data, CM_Matrix::MATRIX_P3))
        .WillOnce(Return(expectedRes));
    ASSERT_EQ(colorTemp.GetLayerLinearCct(0, data), expectedRes);
}
#endif

/**
 * @tc.name: ColorTemperature007
 * @tc.desc: Test ColorTemperature IsColorTemperatureOn returns false with mock
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature007, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    auto mockRSColorTemperature = MockRSColorTemperature::GetInstance();
    colorTemp.rSColorTemperatureInterface_ = mockRSColorTemperature.get();
    ASSERT_NE(colorTemp.rSColorTemperatureInterface_, nullptr);
    EXPECT_CALL(*mockRSColorTemperature, IsColorTemperatureOn()).WillOnce(Return(false));
    ASSERT_EQ(colorTemp.IsColorTemperatureOn(), false);
}

/**
 * @tc.name: ColorTemperature008
 * @tc.desc: Test ColorTemperature IsColorTemperatureOn returns true with mock
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTemperatureTest, ColorTemperature008, TestSize.Level1)
{
    auto& colorTemp = RSColorTemperature::Get();
    auto mockRSColorTemperature = MockRSColorTemperature::GetInstance();
    colorTemp.rSColorTemperatureInterface_ = mockRSColorTemperature.get();
    ASSERT_NE(colorTemp.rSColorTemperatureInterface_, nullptr);
    EXPECT_CALL(*mockRSColorTemperature, IsColorTemperatureOn()).WillOnce(Return(true));
    ASSERT_EQ(colorTemp.IsColorTemperatureOn(), true);
}
} // namespace OHOS::Rosen