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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "display_engine/rs_luminance_control.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr float AI_HDR_SCALER = 2.5f;
constexpr float HDR_DEFAULT_SCALER = 1000.0f / 203.0f;

class MockRSLuminanceControl : public RSLuminanceControlInterface {
public:
    static std::shared_ptr<MockRSLuminanceControl> GetInstance()
    {
        static auto instance = std::make_shared<MockRSLuminanceControl>();
        return instance;
    }

    MOCK_METHOD(bool, Init, (), (override));
    MOCK_METHOD(void, UpdateScreenStatus, (ScreenId screenId, ScreenPowerStatus powerStatus), (override));
    MOCK_METHOD(bool, SetHdrStatus, (ScreenId screenId, HdrStatus hdrStatus), (override));
    MOCK_METHOD(bool, IsHdrOn, (ScreenId screenId), (override, const));
    MOCK_METHOD(bool, IsDimmingOn, (ScreenId screenId), (override));
    MOCK_METHOD(void, DimmingIncrease, (ScreenId screenId), (override));
    MOCK_METHOD(void, SetSdrLuminance, (ScreenId screenId, uint32_t level), (override));
    MOCK_METHOD(uint32_t, GetNewHdrLuminance, (ScreenId screenId), (override));
    MOCK_METHOD(void, SetNowHdrLuminance, (ScreenId screenId, uint32_t level), (override));
    MOCK_METHOD(bool, IsNeedUpdateLuminance, (ScreenId screenId), (override));
    MOCK_METHOD(float, GetSdrDisplayNits, (ScreenId screenId), (override));
    MOCK_METHOD(float, GetDisplayNits, (ScreenId screenId), (override));
    MOCK_METHOD(double, GetNonlinearRatio, (ScreenId screenId, uint32_t mode), (override));
    MOCK_METHOD(bool, IsHdrPictureOn, (), (override));
    MOCK_METHOD(bool, IsForceCloseHdr, (), (override, const));
    MOCK_METHOD(void, ForceCloseHdr, (uint32_t closeHdrSceneId, bool forceCloseHdr), (override));
    MOCK_METHOD(bool, IsCloseHardwareHdr, (), (override, const));
    MOCK_METHOD(bool, IsScreenNoHeadroom, (ScreenId screenId), (override, const));
    MOCK_METHOD(double, GetMaxScaler, (ScreenId screenId), (override, const));
    MOCK_METHOD(BrightnessInfo, GetBrightnessInfo, (ScreenId screenId), (override));
    MOCK_METHOD(bool, IsBrightnessInfoChanged, (ScreenId screenId), (override));
    MOCK_METHOD(void, HandleGamutSpecialRender, (std::vector<ScreenColorGamut>& modes), (override));

    float CalScaler(const float& maxContentLightLevel,
        const std::vector<uint8_t>& dynamicMetadata, const float& ratio, HdrStatus hdrStatus) override;
};

float MockRSLuminanceControl::CalScaler(const float& maxContentLightLevel,
    const std::vector<uint8_t>& dynamicMetadata, const float& ratio, HdrStatus hdrStatus)
{
    if (hdrStatus == HdrStatus::AI_HDR_VIDEO_GAINMAP || hdrStatus == HdrStatus::AI_HDR_VIDEO_GTM) {
        return AI_HDR_SCALER;
    } else {
        return HDR_DEFAULT_SCALER * ratio;
    }
}

class RSLuminanceControlTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLuminanceControlTest::SetUpTestCase() {}
void RSLuminanceControlTest::TearDownTestCase() {}
void RSLuminanceControlTest::SetUp() {}
void RSLuminanceControlTest::TearDown() {}

/**
 * @tc.name: LuminanceControl001
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl001, TestSize.Level1)
{
    ScreenId screenId{};
    uint32_t level{};
    std::vector<ScreenColorGamut> mode{};
    auto& luminCtrl = RSLuminanceControl::Get();
    luminCtrl.Init();
    luminCtrl.initStatus_ = true;
    luminCtrl.DimmingIncrease(screenId);
    luminCtrl.SetNowHdrLuminance(screenId, level);
    luminCtrl.SetSdrLuminance(screenId, level);
    luminCtrl.ForceCloseHdr(screenId, true);
    luminCtrl.GetBrightnessInfo(screenId);
    luminCtrl.HandleGamutSpecialRender(mode);

    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    luminCtrl.UpdateScreenStatus(screenId, POWER_STATUS_ON);
    luminCtrl.DimmingIncrease(screenId);
    luminCtrl.SetNowHdrLuminance(screenId, level);
    luminCtrl.SetSdrLuminance(screenId, level);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::NO_HDR);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::HDR_VIDEO);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::AI_HDR_VIDEO_GTM);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::HDR_PHOTO);
    luminCtrl.ForceCloseHdr(screenId, true);
    luminCtrl.GetBrightnessInfo(screenId);
    luminCtrl.HandleGamutSpecialRender(mode);
    
    ASSERT_NE((&luminCtrl), nullptr);
}

/**
 * @tc.name: LuminanceControl002
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl002, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    std::ignore = luminCtrl.LoadLibrary();
    luminCtrl.CloseLibrary();

    int32_t extLibHandle = 0;
    luminCtrl.extLibHandle_ = &extLibHandle;
    std::ignore = luminCtrl.LoadLibrary();
    luminCtrl.CloseLibrary();
    luminCtrl.extLibHandle_ = nullptr;

    ASSERT_NE((&luminCtrl), nullptr);
}

/**
 * @tc.name: LuminanceControl003
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl003, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    float maxCll = 1000.0f;
    std::vector<uint8_t> dyMetadata = {1, 2, 3};
    float ratio = 0.0f;
    ASSERT_EQ(luminCtrl.CalScaler(maxCll, dyMetadata, ratio), 0.0f);
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ratio = 0.5f;
    ASSERT_EQ(luminCtrl.CalScaler(maxCll, dyMetadata, ratio), HDR_DEFAULT_SCALER * ratio);
    ASSERT_EQ(luminCtrl.CalScaler(maxCll, dyMetadata, ratio, HdrStatus::AI_HDR_VIDEO_GAINMAP), AI_HDR_SCALER);
    ASSERT_EQ(luminCtrl.CalScaler(maxCll, dyMetadata, ratio, HdrStatus::AI_HDR_VIDEO_GTM), AI_HDR_SCALER);
}

/**
 * @tc.name: LuminanceControl004
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl004, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.IsHdrPictureOn(), false);
}

/**
 * @tc.name: LuminanceControl005
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl005, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.IsForceCloseHdr(), false);
}

/**
 * @tc.name: LuminanceControl006
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl006, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.IsCloseHardwareHdr(), false);
}

/**
 * @tc.name: LuminanceControl007
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl007, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.IsScreenNoHeadroom(0), false);
}

/**
 * @tc.name: LuminanceControl008
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl008, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.GetMaxScaler(0), 0.0);
}

/**
 * @tc.name: LuminanceControl009
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl009, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.IsHdrOn(0), false);
}

/**
 * @tc.name: LuminanceControl010
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl010, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.IsDimmingOn(0), false);
}

/**
 * @tc.name: LuminanceControl011
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl011, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.IsNeedUpdateLuminance(0), false);
}

/**
 * @tc.name: LuminanceControl012
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl012, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.GetSdrDisplayNits(0), 0.0f);
}

/**
 * @tc.name: LuminanceControl013
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl013, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.GetDisplayNits(0), 0.0f);
}

/**
 * @tc.name: LuminanceControl014
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl014, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.GetHdrBrightnessRatio(0, 0), 0.0);
}

/**
 * @tc.name: LuminanceControl015
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl015, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.GetNewHdrLuminance(0), 0);
}

/**
 * @tc.name: LuminanceControl016
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlTest, LuminanceControl016, TestSize.Level1)
{
    auto& luminCtrl = RSLuminanceControl::Get();
    auto mockRSLuminanceControl = MockRSLuminanceControl::GetInstance();
    luminCtrl.rSLuminanceControlInterface_ = mockRSLuminanceControl.get();
    ASSERT_NE(luminCtrl.rSLuminanceControlInterface_, nullptr);
    ASSERT_EQ(luminCtrl.IsBrightnessInfoChanged(0), false);
}
} // namespace OHOS::Rosen