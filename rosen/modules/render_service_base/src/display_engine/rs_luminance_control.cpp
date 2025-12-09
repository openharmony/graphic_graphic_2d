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

#include "display_engine/rs_luminance_control.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr float HDR_DEFAULT_SCALER = 1000.0f / 203.0f;

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
    int32_t mode{};
    uint32_t level{};
    auto& luminCtrl = RSLuminanceControl::Get();
    luminCtrl.Init();
    luminCtrl.DimmingIncrease(screenId);
    std::ignore = luminCtrl.IsDimmingOn(screenId);
    std::ignore = luminCtrl.IsHdrOn(screenId);
    std::ignore = luminCtrl.IsNeedUpdateLuminance(screenId);
    std::ignore = luminCtrl.GetSdrDisplayNits(screenId);
    std::ignore = luminCtrl.GetDisplayNits(screenId);
    std::ignore = luminCtrl.GetHdrBrightnessRatio(screenId, mode);
    std::ignore = luminCtrl.GetNewHdrLuminance(screenId);
    luminCtrl.SetNowHdrLuminance(screenId, level);
    luminCtrl.SetSdrLuminance(screenId, level);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::NO_HDR);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::HDR_VIDEO);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::AI_HDR_VIDEO);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::HDR_PHOTO);
    luminCtrl.ForceCloseHdr(screenId, false);

    luminCtrl.initStatus_ = true;
    luminCtrl.DimmingIncrease(screenId);
    std::ignore = luminCtrl.IsDimmingOn(screenId);
    std::ignore = luminCtrl.IsHdrOn(screenId);
    std::ignore = luminCtrl.IsNeedUpdateLuminance(screenId);
    std::ignore = luminCtrl.GetSdrDisplayNits(screenId);
    std::ignore = luminCtrl.GetDisplayNits(screenId);
    std::ignore = luminCtrl.GetHdrBrightnessRatio(screenId, mode);
    std::ignore = luminCtrl.GetNewHdrLuminance(screenId);
    luminCtrl.SetNowHdrLuminance(screenId, level);
    luminCtrl.SetSdrLuminance(screenId, level);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::NO_HDR);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::HDR_VIDEO);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::AI_HDR_VIDEO);
    luminCtrl.SetHdrStatus(screenId, HdrStatus::HDR_PHOTO);
    luminCtrl.ForceCloseHdr(screenId, true);
    
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
    ratio = 1.0f;
    ASSERT_EQ(luminCtrl.CalScaler(maxCll, dyMetadata, ratio), HDR_DEFAULT_SCALER);
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
    ASSERT_EQ(luminCtrl.IsForceCloseHdr(), false);
}
} // namespace OHOS::Rosen