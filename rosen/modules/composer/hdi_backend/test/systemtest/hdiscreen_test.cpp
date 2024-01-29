/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_screen.h"
#include "mock_hdi_device_systest.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiScreenSysTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::unique_ptr<HdiScreen> hdiScreen_;
    static inline MockSys::HdiDeviceMock* mockDevice_;
};

void HdiScreenSysTest::SetUpTestCase()
{
    uint32_t screenId = 0;
    hdiScreen_ = HdiScreen::CreateHdiScreen(screenId);
    mockDevice_ = MockSys::HdiDeviceMock::GetInstance();

    EXPECT_CALL(*mockDevice_, GetScreenCapability(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenSupportedModes(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenMode(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenMode(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenOverlayResolution(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenPowerStatus(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenPowerStatus(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenBacklight(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenBacklight(_, _)).WillRepeatedly(testing::Return(0));

    hdiScreen_->SetHdiDevice(mockDevice_);
}

void HdiScreenSysTest::TearDownTestCase() {}

namespace {
/**
 * @tc.name: TestHdiScreen001
 * @tc.desc: Verify the hdiscreen
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiScreenSysTest, TestHdiScreen001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiScreenSysTest::hdiScreen_->Init(), true);

    GraphicDisplayCapability displayCapability = {};
    ASSERT_EQ(HdiScreenSysTest::hdiScreen_->GetScreenCapability(displayCapability), 0);

    std::vector<GraphicDisplayModeInfo> modeInfo = {};
    ASSERT_EQ(HdiScreenSysTest::hdiScreen_->GetScreenSupportedModes(modeInfo), 0);

    uint32_t modeId = 0;
    ASSERT_EQ(HdiScreenSysTest::hdiScreen_->SetScreenMode(modeId), 0);
    uint32_t width = 1080;
    uint32_t height = 1920;
    ASSERT_EQ(HdiScreenSysTest::hdiScreen_->SetScreenOverlayResolution(width, height), 0);
    ASSERT_EQ(HdiScreenSysTest::hdiScreen_->GetScreenMode(modeId), 0);

    GraphicDispPowerStatus dispPowerStatus = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    ASSERT_EQ(HdiScreenSysTest::hdiScreen_->SetScreenPowerStatus(dispPowerStatus), 0);
    ASSERT_EQ(HdiScreenSysTest::hdiScreen_->GetScreenPowerStatus(dispPowerStatus), 0);

    uint32_t level = 0;
    ASSERT_EQ(HdiScreenSysTest::hdiScreen_->SetScreenBacklight(level), 0);
    ASSERT_EQ(HdiScreenSysTest::hdiScreen_->GetScreenBacklight(level), 0);
}
} // namespace
} // namespace Rosen
} // namespace OHOS