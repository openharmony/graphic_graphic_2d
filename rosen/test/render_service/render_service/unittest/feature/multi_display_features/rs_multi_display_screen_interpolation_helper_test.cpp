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

#include "gtest/gtest.h"

#include "rs_multi_display_screen_interpolation_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t REAL_WIDTH = 3402;
constexpr uint32_t REAL_HEIGHT = 1080;
constexpr uint32_t PHY_WIDTH = 3402;
constexpr uint32_t PHY_HEIGHT = 1620;
constexpr int32_t MODE_ID_FIRST = 0;
constexpr int32_t MODE_ID_SECOND = 1;
constexpr int32_t MODE_ID_NOT_EXIST = 99;
constexpr uint32_t FRESH_RATE_FIRST = 60;
constexpr uint32_t FRESH_RATE_SECOND = 90;

GraphicDisplayModeInfo MakeMode(int32_t id, uint32_t width, uint32_t height, uint32_t freshRate)
{
    GraphicDisplayModeInfo mode;
    mode.id = id;
    mode.width = width;
    mode.height = height;
    mode.freshRate = freshRate;
    return mode;
}

GraphicDisplayCapability MakeCapability(uint32_t phyWidth, uint32_t phyHeight)
{
    GraphicDisplayCapability capability;
    capability.phyWidth = phyWidth;
    capability.phyHeight = phyHeight;
    return capability;
}

std::vector<GraphicDisplayModeInfo> MakeSupportedModes()
{
    return { MakeMode(MODE_ID_FIRST, PHY_WIDTH, PHY_HEIGHT, FRESH_RATE_FIRST),
        MakeMode(MODE_ID_SECOND, PHY_WIDTH, PHY_HEIGHT, FRESH_RATE_SECOND) };
}
} // namespace

class RSMultiDisplayScreenInterpolationHelperTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: BackupAndModifyDisplayInfo001
 * @tc.desc: Updated capability and supportedModes use mock resolution, id/freshRate unchanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayScreenInterpolationHelperTest, BackupAndModifyDisplayInfo001, TestSize.Level1)
{
    RSMultiDisplayScreenInterpolationHelper helper(REAL_WIDTH, REAL_HEIGHT);
    auto capability = MakeCapability(PHY_WIDTH, PHY_HEIGHT);
    auto supportedModes = MakeSupportedModes();

    helper.BackupAndModifyDisplayInfo(capability, supportedModes);

    ASSERT_EQ(capability.phyWidth, REAL_WIDTH);
    ASSERT_EQ(capability.phyHeight, REAL_HEIGHT);
    ASSERT_EQ(supportedModes.size(), 2UL);
    for (const auto& mode : supportedModes) {
        ASSERT_EQ(mode.width, REAL_WIDTH);
        ASSERT_EQ(mode.height, REAL_HEIGHT);
    }
    ASSERT_EQ(supportedModes[0].id, MODE_ID_FIRST);
    ASSERT_EQ(supportedModes[0].freshRate, FRESH_RATE_FIRST);
    ASSERT_EQ(supportedModes[1].id, MODE_ID_SECOND);
    ASSERT_EQ(supportedModes[1].freshRate, FRESH_RATE_SECOND);
}

/**
 * @tc.name: BackupAndModifyDisplayInfo002
 * @tc.desc: Original capability backup unaffected by modification, GetOriginCapability returns physical value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayScreenInterpolationHelperTest, BackupAndModifyDisplayInfo002, TestSize.Level1)
{
    RSMultiDisplayScreenInterpolationHelper helper(REAL_WIDTH, REAL_HEIGHT);
    auto capability = MakeCapability(PHY_WIDTH, PHY_HEIGHT);
    auto supportedModes = MakeSupportedModes();

    helper.BackupAndModifyDisplayInfo(capability, supportedModes);

    const auto& originCapability = helper.GetOriginCapability();
    ASSERT_EQ(originCapability.phyWidth, PHY_WIDTH);
    ASSERT_EQ(originCapability.phyHeight, PHY_HEIGHT);
}

/**
 * @tc.name: GetOriginModeInfo001
 * @tc.desc: Return original (physical resolution) mode on id match, not the modified spoofed mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayScreenInterpolationHelperTest, GetOriginModeInfo001, TestSize.Level1)
{
    RSMultiDisplayScreenInterpolationHelper helper(REAL_WIDTH, REAL_HEIGHT);
    auto capability = MakeCapability(PHY_WIDTH, PHY_HEIGHT);
    auto supportedModes = MakeSupportedModes();
    helper.BackupAndModifyDisplayInfo(capability, supportedModes);

    auto originMode = helper.GetOriginModeInfo(MODE_ID_SECOND);
    ASSERT_TRUE(originMode.has_value());
    ASSERT_EQ(originMode->id, MODE_ID_SECOND);
    ASSERT_EQ(originMode->width, PHY_WIDTH);
    ASSERT_EQ(originMode->height, PHY_HEIGHT);
    ASSERT_EQ(originMode->freshRate, FRESH_RATE_SECOND);
}

/**
 * @tc.name: GetOriginModeInfo002
 * @tc.desc: Return nullopt for non-existent modeId (null check required before SetActiveMode restores hardware mode)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayScreenInterpolationHelperTest, GetOriginModeInfo002, TestSize.Level1)
{
    RSMultiDisplayScreenInterpolationHelper helper(REAL_WIDTH, REAL_HEIGHT);
    auto capability = MakeCapability(PHY_WIDTH, PHY_HEIGHT);
    auto supportedModes = MakeSupportedModes();
    helper.BackupAndModifyDisplayInfo(capability, supportedModes);

    auto originMode = helper.GetOriginModeInfo(MODE_ID_NOT_EXIST);
    ASSERT_FALSE(originMode.has_value());
}

/**
 * @tc.name: GetOriginModeInfo003
 * @tc.desc: Return nullopt safely when querying if BackupAndModifyDisplayInfo has not been called (empty modes)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayScreenInterpolationHelperTest, GetOriginModeInfo003, TestSize.Level1)
{
    RSMultiDisplayScreenInterpolationHelper helper(REAL_WIDTH, REAL_HEIGHT);
    auto capability = MakeCapability(PHY_WIDTH, PHY_HEIGHT);
    std::vector<GraphicDisplayModeInfo> emptyModes;
    helper.BackupAndModifyDisplayInfo(capability, emptyModes);

    auto originMode = helper.GetOriginModeInfo(MODE_ID_FIRST);
    ASSERT_FALSE(originMode.has_value());
}

/**
 * @tc.name: BackupAndModifyDisplayInfo003
 * @tc.desc: Repeated BackupAndModifyDisplayInfo backs current input (idempotent).
 *           Re-backing spoofed input yields spoofed origin — caller must call once only.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayScreenInterpolationHelperTest, BackupAndModifyDisplayInfo003, TestSize.Level1)
{
    RSMultiDisplayScreenInterpolationHelper helper(REAL_WIDTH, REAL_HEIGHT);
    auto capability = MakeCapability(PHY_WIDTH, PHY_HEIGHT);
    auto supportedModes = MakeSupportedModes();
    helper.BackupAndModifyDisplayInfo(capability, supportedModes);

    helper.BackupAndModifyDisplayInfo(capability, supportedModes);
    const auto& originCapability = helper.GetOriginCapability();
    ASSERT_EQ(originCapability.phyWidth, REAL_WIDTH);
    ASSERT_EQ(originCapability.phyHeight, REAL_HEIGHT);
}
} // namespace OHOS::Rosen
