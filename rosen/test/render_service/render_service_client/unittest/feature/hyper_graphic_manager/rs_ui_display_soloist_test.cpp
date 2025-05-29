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

#include "gtest/gtest.h"
#include "feature/hyper_graphic_manager/rs_ui_display_soloist.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    constexpr uint32_t OLED_MIN_HZ = 0;
    constexpr uint32_t OLED_120_HZ = 120;
    constexpr uint32_t OLED_90_HZ = 90;
    constexpr uint32_t OLED_60_HZ = 60;
    constexpr uint32_t OLED_30_HZ = 30;
    constexpr uint32_t INTERVAL_TIME = 50000; // us
    static const std::vector<int32_t> REFRESH_RATE_LIST1{30, 60, 60};
    static const std::vector<int32_t> REFRESH_RATE_LIST2{30, 120, 60};
    struct NodeInfo {
        int32_t vsyncRate;
        int32_t targetRate;
        std::vector<int32_t> refreshRateList;
        int32_t result;
    };
}
class RSUIDisplaySoloistTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIDisplaySoloistTest::SetUpTestCase() {}
void RSUIDisplaySoloistTest::TearDownTestCase() {}
void RSUIDisplaySoloistTest::SetUp() {}
void RSUIDisplaySoloistTest::TearDown() {}

/**
 * @tc.name: InsertAndRemoveSoloistTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplaySoloistTest, InsertAndRemoveSoloistTest001, TestSize.Level1)
{
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();

    std::shared_ptr<SoloistId> soloistIdObj = OHOS::Rosen::SoloistId::Create();
    int32_t soloistId = soloistIdObj->GetId();
    size_t size = 0;
    EXPECT_EQ(size, soloistManager.GetIdToSoloistMap().size());
    soloistManager.InsertUseExclusiveThreadFlag(soloistId, true);
    size_t size2 = 1;
    EXPECT_EQ(size2, soloistManager.GetIdToSoloistMap().size());

    FrameRateRange frameRateRange(OLED_MIN_HZ, OLED_120_HZ, OLED_60_HZ);
    EXPECT_TRUE(frameRateRange.IsValid());
    soloistManager.Start(soloistId);
    soloistManager.OnVsyncTimeOut();
    soloistManager.RemoveSoloist(soloistId);
    usleep(INTERVAL_TIME);
}

/**
 * @tc.name: InsertAndRemoveSoloistTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplaySoloistTest, InsertAndRemoveSoloistTest002, TestSize.Level1)
{
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();

    std::shared_ptr<SoloistId> soloistIdObj = OHOS::Rosen::SoloistId::Create();
    int32_t soloistId = soloistIdObj->GetId();
    soloistManager.InsertUseExclusiveThreadFlag(soloistId, false);
    size_t size = 1;
    EXPECT_EQ(size, soloistManager.GetIdToSoloistMap().size());

    int32_t count = 0;
    soloistManager.InsertOnVsyncCallback(soloistId, [&count](long long timestamp, long long targetTimestamp, void*) {
        count = 1;
    }, nullptr);

    soloistManager.idToSoloistMap_[soloistId]->TriggerCallback();

    FrameRateRange frameRateRange(OLED_MIN_HZ, OLED_120_HZ, OLED_120_HZ);
    soloistManager.InsertFrameRateRange(soloistId, frameRateRange);

    soloistManager.Start(soloistId);

    soloistManager.Stop(soloistId);
    soloistManager.RemoveSoloist(soloistId);
    usleep(INTERVAL_TIME);
}

/**
 * @tc.name: SetFrameRateLinkerEnable001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplaySoloistTest, SetFrameRateLinkerEnable001, TestSize.Level1)
{
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();

    std::shared_ptr<SoloistId> soloistIdObj = OHOS::Rosen::SoloistId::Create();
    int32_t soloistId = soloistIdObj->GetId();
    soloistManager.InsertUseExclusiveThreadFlag(soloistId, false);
    EXPECT_NE(0, soloistManager.GetIdToSoloistMap().size());
    EXPECT_FALSE(soloistManager.GetIdToSoloistMap()[soloistId]->useExclusiveThread_);

    soloistManager.Start(soloistId);

    bool isLinkerEnabled = false;
    soloistManager.SetMainFrameRateLinkerEnable(isLinkerEnabled);
    EXPECT_EQ(ActiveStatus::INACTIVE, soloistManager.GetManagerStatus());
    EXPECT_FALSE(soloistManager.GetFrameRateLinker()->IsEnable());

    for (const auto& [id, displaySoloist] : soloistManager.GetIdToSoloistMap()) {
        EXPECT_EQ(nullptr, displaySoloist->frameRateLinker_);
    }

    soloistManager.Stop(soloistId);
    soloistManager.RemoveSoloist(soloistId);
    usleep(INTERVAL_TIME);
}

/**
 * @tc.name: SetFrameRateLinkerEnable002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplaySoloistTest, SetFrameRateLinkerEnable002, TestSize.Level1)
{
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();

    std::shared_ptr<SoloistId> soloistIdObj = OHOS::Rosen::SoloistId::Create();
    int32_t soloistId = soloistIdObj->GetId();
    soloistManager.InsertUseExclusiveThreadFlag(soloistId, true);

    soloistManager.Start(soloistId);
    int64_t timestamp = 0;
    soloistManager.VsyncCallbackInner(timestamp);
    soloistManager.GetIdToSoloistMap()[soloistId]->VsyncCallbackInner(timestamp);

    bool isLinkerEnabled = false;
    soloistManager.SetMainFrameRateLinkerEnable(isLinkerEnabled);
    EXPECT_EQ(ActiveStatus::INACTIVE, soloistManager.GetManagerStatus());
    EXPECT_FALSE(soloistManager.GetFrameRateLinker()->IsEnable());

    for (const auto& [id, displaySoloist] : soloistManager.GetIdToSoloistMap()) {
        EXPECT_EQ(isLinkerEnabled, displaySoloist->frameRateLinker_->IsEnable());
    }

    bool isLinkerEnabled2 = true;
    soloistManager.SetMainFrameRateLinkerEnable(isLinkerEnabled2);
    EXPECT_EQ(ActiveStatus::ACTIVE, soloistManager.GetManagerStatus());
    EXPECT_TRUE(soloistManager.GetFrameRateLinker()->IsEnable());

    for (const auto& [id, displaySoloist] : soloistManager.GetIdToSoloistMap()) {
        EXPECT_EQ(isLinkerEnabled2, displaySoloist->frameRateLinker_->IsEnable());
    }

    soloistManager.Stop(soloistId);
    soloistManager.RemoveSoloist(soloistId);
    soloistManager.GetFrameRateRange();
    usleep(INTERVAL_TIME);
}

/**
 * @tc.name: SetMainFrameRateLinkerEnable
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplaySoloistTest, SetMainFrameRateLinkerEnable, TestSize.Level1)
{
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();

    std::shared_ptr<SoloistId> soloistIdObj = OHOS::Rosen::SoloistId::Create();
    int32_t soloistId = soloistIdObj->GetId();
    soloistManager.InsertUseExclusiveThreadFlag(soloistId, false);

    FrameRateRange frameRateRange1(OLED_MIN_HZ, OLED_120_HZ, OLED_120_HZ);
    EXPECT_TRUE(frameRateRange1.IsValid());
    soloistManager.InsertFrameRateRange(soloistId, frameRateRange1);

    std::shared_ptr<SoloistId> soloistIdObj2 = OHOS::Rosen::SoloistId::Create();
    int32_t soloistId2 = soloistIdObj2->GetId();
    soloistManager.InsertUseExclusiveThreadFlag(soloistId2, false);

    FrameRateRange frameRateRange2(OLED_30_HZ, OLED_120_HZ, OLED_60_HZ);
    EXPECT_TRUE(frameRateRange2.IsValid());
    soloistManager.InsertFrameRateRange(soloistId2, frameRateRange2);

    soloistManager.Start(soloistId);
    soloistManager.Start(soloistId2);
    int32_t rate = 0;
    EXPECT_NE(rate, soloistManager.GetIdToSoloistMap()[soloistId]->GetVSyncRate());
    EXPECT_NE(rate, soloistManager.GetIdToSoloistMap()[soloistId2]->GetVSyncRate());

    soloistManager.Stop(soloistId);
    soloistManager.RemoveSoloist(soloistId);

    soloistManager.Stop(soloistId2);
    soloistManager.RemoveSoloist(soloistId2);
    usleep(INTERVAL_TIME);
}

/**
 * @tc.name: IsCommonDivisor
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplaySoloistTest, IsCommonDivisor, TestSize.Level1)
{
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
    std::shared_ptr<SoloistId> soloistIdObj = OHOS::Rosen::SoloistId::Create();
    int32_t soloistId = soloistIdObj->GetId();
    std::vector<NodeInfo> tests = {
        {90, 60, {90, 120, 144}, 45},
        {90, 60, {30, 60, 60}, 45},
        {90, 60, {30, 120, 60}, 45},
        {60, 60, {90, 120, 144}, 60},
        {60, 0, {90, 120, 144}, 60}
    };

    for (auto& test : tests) {
        RATE_TO_FACTORS.clear();
        RATE_TO_FACTORS.try_emplace(OLED_90_HZ, test.refreshRateList);
        EXPECT_EQ(soloistManager.GetIdToSoloistMap()[soloistId]->FindMatchedRefreshRate(test.vsyncRate,
            test.targetRate), test.result);
    }

    EXPECT_EQ(soloistManager.GetIdToSoloistMap()[soloistId]->IsCommonDivisor(0, 0), false);
    RSDisplaySoloist rsDisplaySoloist;
    rsDisplaySoloist.OnVsyncTimeOut();
    EXPECT_EQ(rsDisplaySoloist.hasRequestedVsync_, false);
}
} // namespace OHOS::Rosen
