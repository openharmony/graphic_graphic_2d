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
 
#include <gtest/gtest.h>
#include <gmock/gmock.h>
 
#include "display_engine/rs_display_effect_swing_control.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class MockRSDisplayEffectSwingControl : public RSDisplayEffectSwingControlInterface {
public:
    static std::shared_ptr<MockRSDisplayEffectSwingControl> GetInstance()
    {
        static auto instance = std::make_shared<MockRSDisplayEffectSwingControl>();
        return instance;
    }
 
    MOCK_METHOD(bool, Init, (), (override));
    MOCK_METHOD(void, SetSwingEnabled, (ScreenId screenId, bool enable), (override));
    MOCK_METHOD(bool, IsSwingRegistered, (ScreenId screenId), (override));
    MOCK_METHOD(SwingData, GetSwingData, (ScreenId screenId), (override));
};
 
class RSDisplayEffectSwingControlTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void RSDisplayEffectSwingControlTest::SetUpTestCase() {}
void RSDisplayEffectSwingControlTest::TearDownTestCase() {}
void RSDisplayEffectSwingControlTest::SetUp() {}
void RSDisplayEffectSwingControlTest::TearDown() {}
 
/**
 * @tc.name: SwingControl001
 * @tc.desc: Test SwingControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayEffectSwingControlTest, SwingControl001, TestSize.Level1)
{
    ScreenId screenId{};
    auto& swingControl = RSDisplayEffectSwingControl::Get(); 
    swingControl.Init();
    swingControl.SetSwingEnabled(screenId, false);
 
    auto mockRSDisplayEffectSwingControl = MockRSDisplayEffectSwingControl::GetInstance();
    swingControl.swingControlInterface_ = mockRSDisplayEffectSwingControl.get();
    ASSERT_NE(swingControl.swingControlInterface_, nullptr);
    swingControl.SetSwingEnabled(screenId, false);
 
    ASSERT_NE((&swingControl), nullptr);
}
 
/**
 * @tc.name: SwingControl002
 * @tc.desc: Test SwingControl IsSwingRegistered returns false with mock
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayEffectSwingControlTest, SwingControl002, TestSize.Level1)
{
    auto& swingControl = RSDisplayEffectSwingControl::Get();
    auto mockRSDisplayEffectSwingControl = MockRSDisplayEffectSwingControl::GetInstance();
    swingControl.swingControlInterface_ = mockRSDisplayEffectSwingControl.get();
    ASSERT_NE(swingControl.swingControlInterface_, nullptr);
    EXPECT_CALL(*mockRSDisplayEffectSwingControl, IsSwingRegistered(0)).WillOnce(Return(false));
    ASSERT_EQ(swingControl.IsSwingRegistered(0), false);
}
 
/**
 * @tc.name: SwingControl003
 * @tc.desc: Test SwingControl IsSwingRegistered returns true with mock
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayEffectSwingControlTest, SwingControl003, TestSize.Level1)
{
    auto& swingControl = RSDisplayEffectSwingControl::Get();
    auto mockRSDisplayEffectSwingControl = MockRSDisplayEffectSwingControl::GetInstance();
    swingControl.swingControlInterface_ = mockRSDisplayEffectSwingControl.get();
    ASSERT_NE(swingControl.swingControlInterface_, nullptr);
    EXPECT_CALL(*mockRSDisplayEffectSwingControl, IsSwingRegistered(0)).WillOnce(Return(true));
    ASSERT_EQ(swingControl.IsSwingRegistered(0), true);
}
 
/**
 * @tc.name: SwingControl004
 * @tc.desc: Test SwingControl GetSwingData with mock
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayEffectSwingControlTest, SwingControl004, TestSize.Level1)
{
    auto& swingControl = RSDisplayEffectSwingControl::Get();
    auto mockRSDisplayEffectSwingControl = MockRSDisplayEffectSwingControl::GetInstance();
    swingControl.swingControlInterface_ = mockRSDisplayEffectSwingControl.get();
    ASSERT_NE(swingControl.swingControlInterface_, nullptr);
    SwingData swingData  = {0.0f, 0.0f, 1.0f};
    EXPECT_CALL(*mockRSDisplayEffectSwingControl, GetSwingData(0)).WillOnce(Return(swingData));
    ASSERT_EQ(swingControl.GetSwingData(0), swingData);
}
} // namespace OHOS::Rosen
