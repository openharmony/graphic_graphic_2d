/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "limit_number.h"
#include "screen_manager/rs_screen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenTest::SetUpTestCase() {}
void RSScreenTest::TearDownTestCase() {}
void RSScreenTest::SetUp() {}
void RSScreenTest::TearDown() {}

/*
 * @tc.name: DisplayDump_001
 * @tc.desc: Test PhysicalScreen DisplayDump
 * @tc.type: FUNC
 * @tc.require: issueI60RFZ
 */
HWTEST_F(RSScreenTest, DisplayDump_001, testing::ext::TestSize.Level2)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    std::string dumpString = "";
    int32_t screenIndex = 0;
    rsScreen->DisplayDump(screenIndex, dumpString);
}

/*
 * @tc.name: IsEnable_001
 * @tc.desc: IsEnable Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, IsEnable_001, testing::ext::TestSize.Level1)
{
    ScreenId id = INVALID_SCREEN_ID;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_EQ(rsScreen->IsEnable(), false);
}

/*
 * @tc.name: SetResolution_001
 * @tc.desc: SetResolution Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetResolution_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    uint32_t width = 100;
    uint32_t height = 100;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    virtualScreen->SetResolution(width, height);
}

/*
 * @tc.name: SetPowerStatus_001
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetPowerStatus_001, testing::ext::TestSize.Level2)
{
    ScreenId id = INVALID_SCREEN_ID;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    rsScreen->SetPowerStatus(static_cast<uint32_t>(1000));
}

/*
 * @tc.name: ScreenTypeDump_001
 * @tc.desc: ScreenTypeDump Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, ScreenTypeDump_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    config.id = static_cast<uint64_t>(1000);
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    std::string dumpString = "";
    virtualScreen->DisplayDump(config.id, dumpString);
}

/*
 * @tc.name: SetScreenBacklight_001
 * @tc.desc: SetScreenBacklight Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenBacklight_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    virtualScreen->SetScreenBacklight(static_cast<uint32_t>(1000));
    ASSERT_EQ(virtualScreen->GetScreenBacklight(), INVALID_BACKLIGHT_VALUE);
}

/*
 * @tc.name: GetScreenSupportedColorGamuts_001
 * @tc.desc: GetScreenSupportedColorGamuts Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, GetScreenSupportedColorGamuts_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    std::vector<ScreenColorGamut> mode;
    virtualScreen->GetScreenSupportedColorGamuts(mode);
}

/*
 * @tc.name: SetScreenColorGamut_001
 * @tc.desc: SetScreenColorGamut Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_001, testing::ext::TestSize.Level1)
{
    ScreenId id = static_cast<uint64_t>(1000);
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    rsScreen->SetScreenColorGamut(static_cast<int32_t>(1000));
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetScreenColorGamut(static_cast<int32_t>(1000)), INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenVsyncEnabled_001
 * @tc.desc: SetScreenVsyncEnabled Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenVsyncEnabled_001, testing::ext::TestSize.Level1)
{
    ScreenId id = static_cast<uint64_t>(1000);
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    rsScreen->SetScreenVsyncEnabled(true);
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    virtualScreen->SetScreenVsyncEnabled(true);
}

/*
 * @tc.name: SetActiveMode_001
 * @tc.desc: SetActiveMode Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetActiveMode_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    uint32_t modeId = static_cast<uint32_t>(1);
    virtualScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetPowerStatus_002
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetPowerStatus_002, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    uint32_t status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    virtualScreen->SetPowerStatus(status);
}

/*
 * @tc.name: GetActiveMode_001
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, GetActiveMode_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->GetActiveMode().has_value(), false);
}

/*
 * @tc.name: GetScreenSupportedMetaDataKeys_001
 * @tc.desc: GetScreenSupportedMetaDataKeys Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, GetScreenSupportedMetaDataKeys_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    std::vector<ScreenHDRMetadataKey> keys;
    ASSERT_EQ(virtualScreen->GetScreenSupportedMetaDataKeys(keys), INVALID_BACKLIGHT_VALUE);
    ScreenId id = static_cast<uint64_t>(1);
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_EQ(rsScreen->GetScreenSupportedMetaDataKeys(keys), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenGamutMap_001
 * @tc.desc: SetScreenGamutMap Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenGamutMap_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ScreenGamutMap map1 = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    auto result1 = virtualScreen->SetScreenGamutMap(map1);
    ASSERT_EQ(result1, StatusCode::SUCCESS);
    ScreenGamutMap map2 = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    const auto result2 = virtualScreen->GetScreenGamutMap(map2);
    ASSERT_EQ(result2, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetActiveModePosByModeIdTest
 * @tc.desc: GetActiveModePosByModeId Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, GetActiveModePosByModeIdTest001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    int32_t modeId = 0;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    auto res = virtualScreen->GetActiveModePosByModeId(modeId);
    ASSERT_EQ(res, -1);
}

/*
 * @tc.name: GetScreenTypeTest
 * @tc.desc: GetScreenType Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, GetScreenTypeTest, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    config.id = static_cast<uint64_t>(1000);
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    auto res = virtualScreen->GetScreenType();
    auto type = RSScreenType::VIRTUAL_TYPE_SCREEN;
    ASSERT_EQ(res, type);
}

/*
 * @tc.name: SetScreenSkipFrameIntervalTest
 * @tc.desc: SetScreenSkipFrameInterval Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenSkipFrameIntervalTest, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    config.id = static_cast<uint64_t>(1000);
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    uint32_t skipFrameInterval = 0;
    virtualScreen->SetScreenSkipFrameInterval(skipFrameInterval);
    auto res = virtualScreen->GetScreenSkipFrameInterval();
    ASSERT_EQ(res, 0);
}
} // namespace OHOS::Rosen
