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

#include "feature/hyper_graphic_manager/hgm_rp_energy.h"
#include "hgm_energy_consumption_policy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class HgmRPEnergyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    std::shared_ptr<HgmRPEnergy> hgmRPEnergy_;
};

void HgmRPEnergyTest::SetUpTestCase() {}
void HgmRPEnergyTest::TearDownTestCase() {}

void HgmRPEnergyTest::SetUp()
{
    hgmRPEnergy_ = std::make_shared<HgmRPEnergy>();
}

void HgmRPEnergyTest::TearDown()
{
    hgmRPEnergy_ = nullptr;
}

/**
 * @tc.name: TestSyncEnergyInfoToRP
 * @tc.desc: Test HgmRPEnergy.SyncEnergyInfoToRP
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestSyncEnergyInfoToRP, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    EnergyInfo energyInfo;
    energyInfo.componentName = "TestComponent";
    energyInfo.componentPid = 1234;
    energyInfo.componentDefaultFps = 60;

    hgmRPEnergy_->SyncEnergyInfoToRP(energyInfo);

    EXPECT_FALSE(hgmRPEnergy_->energyInfo_.componentName.empty());
    EXPECT_NE(hgmRPEnergy_->energyInfo_.componentPid, 0);
    EXPECT_NE(hgmRPEnergy_->energyInfo_.componentDefaultFps, 0);
}

/**
 * @tc.name: TestSetComponentDefaultFps_Case1
 * @tc.desc: Test SetComponentDefaultFps when energyInfo_.componentName != "swiper_drag_scene"
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestSetComponentDefaultFps_Case1, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    EnergyInfo energyInfo;
    energyInfo.componentName = "TestComponent";
    energyInfo.componentPid = 1234;
    energyInfo.componentDefaultFps = 60;
    hgmRPEnergy_->SyncEnergyInfoToRP(energyInfo);

    FrameRateRange rsRange;
    rsRange.componentScene_ = ComponentScene::UNKNOWN_SCENE;
    rsRange.type_ = 0;

    hgmRPEnergy_->SetComponentDefaultFps(rsRange, 1234);

    EXPECT_EQ(rsRange.min_, 0);
    EXPECT_EQ(rsRange.max_, 0);
    EXPECT_EQ(rsRange.preferred_, 0);
}

/**
 * @tc.name: TestSetComponentDefaultFps_Case2
 * @tc.desc: Test TestSetComponentDefaultFps_Case2
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestSetComponentDefaultFps_Case2, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    EnergyInfo energyInfo;
    energyInfo.componentName = "swiper_drag_scene";
    energyInfo.componentPid = 1234;
    energyInfo.componentDefaultFps = 60;
    hgmRPEnergy_->SyncEnergyInfoToRP(energyInfo);

    FrameRateRange rsRange;
    rsRange.componentScene_ = ComponentScene::UNKNOWN_SCENE;
    rsRange.type_ = 0;

    hgmRPEnergy_->SetComponentDefaultFps(rsRange, 5678);

    EXPECT_EQ(rsRange.min_, 0);
    EXPECT_EQ(rsRange.max_, 0);
    EXPECT_EQ(rsRange.preferred_, 0);
}

/**
 * @tc.name: TestSetComponentDefaultFps_Case3
 * @tc.desc: Test TestSetComponentDefaultFps_Case3
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestSetComponentDefaultFps_Case3, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    EnergyInfo energyInfo;
    energyInfo.componentName = "swiper_drag_scene";
    energyInfo.componentPid = 1234;
    energyInfo.componentDefaultFps = 60;
    hgmRPEnergy_->SyncEnergyInfoToRP(energyInfo);

    FrameRateRange rsRange;
    rsRange.min_ = 10;
    rsRange.max_ = 20;
    rsRange.preferred_ = 15;
    rsRange.componentScene_ = ComponentScene::UNKNOWN_SCENE;
    rsRange.type_ = 0;

    hgmRPEnergy_->SetComponentDefaultFps(rsRange, 1234);

    EXPECT_EQ(rsRange.min_, 10);
    EXPECT_EQ(rsRange.max_, 20);
    EXPECT_EQ(rsRange.preferred_, 15);
}

/**
 * @tc.name: TestSetComponentDefaultFps_Case4
 * @tc.desc: Test TestSetComponentDefaultFps_Case4
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestSetComponentDefaultFps_Case4, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    EnergyInfo energyInfo;
    energyInfo.componentName = "swiper_drag_scene";
    energyInfo.componentPid = 1234;
    energyInfo.componentDefaultFps = 60;
    hgmRPEnergy_->SyncEnergyInfoToRP(energyInfo);

    FrameRateRange rsRange;
    rsRange.componentScene_ = ComponentScene::UNKNOWN_SCENE;
    rsRange.type_ = 0;

    hgmRPEnergy_->SetComponentDefaultFps(rsRange, 1234);

    EXPECT_EQ(rsRange.min_, 0);
    EXPECT_EQ(rsRange.max_, 0);
    EXPECT_EQ(rsRange.preferred_, 0);
}

/**
 * @tc.name: TestSetComponentDefaultFps_Case5
 * @tc.desc: Test TestSetComponentDefaultFps_Case5
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestSetComponentDefaultFps_Case5, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    EnergyInfo energyInfo;
    energyInfo.componentName = "swiper_drag_scene";
    energyInfo.componentPid = 1234;
    energyInfo.componentDefaultFps = 60;
    hgmRPEnergy_->SyncEnergyInfoToRP(energyInfo);

    FrameRateRange rsRange;
    rsRange.componentScene_ = ComponentScene::SWIPER_FLING;
    rsRange.type_ = 0;

    hgmRPEnergy_->SetComponentDefaultFps(rsRange, 1234);

    EXPECT_EQ(rsRange.min_, 0);
    EXPECT_EQ(rsRange.max_, 60);
    EXPECT_EQ(rsRange.preferred_, 60);
}

/**
 * @tc.name: TestSetComponentDefaultFps_Case6
 * @tc.desc: Test TestSetComponentDefaultFps_Case6
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestSetComponentDefaultFps_Case6, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    EnergyInfo energyInfo;
    energyInfo.componentName = "swiper_drag_scene";
    energyInfo.componentPid = 1234;
    energyInfo.componentDefaultFps = 60;
    hgmRPEnergy_->SyncEnergyInfoToRP(energyInfo);

    FrameRateRange rsRange;
    rsRange.componentScene_ = ComponentScene::UNKNOWN_SCENE;
    rsRange.type_ = SWIPER_DRAG_FRAME_RATE_TYPE;

    hgmRPEnergy_->SetComponentDefaultFps(rsRange, 1234);

    EXPECT_EQ(rsRange.min_, 0);
    EXPECT_EQ(rsRange.max_, 60);
    EXPECT_EQ(rsRange.preferred_, 60);
}
} // namespace OHOS::Rosen