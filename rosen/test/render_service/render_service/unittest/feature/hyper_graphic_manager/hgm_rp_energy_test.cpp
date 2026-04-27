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
    std::shared_ptr<HgmRPEnergy> HgmRPEnergy_;
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
 * @tc.name: TestSetTouchState
 * @tc.desc: Test the SetTouchState function of HgmRPEnergy
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestSetTouchState, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->SetTouchState(true);
    EXPECT_TRUE(hgmRPEnergy_->isTouchIdle_);

    hgmRPEnergy_->SetTouchState(false);
    EXPECT_FALSE(hgmRPEnergy_->isTouchIdle_);
}

/**
 * @tc.name: TestHgmConfigUpdateCallbackNull
 * @tc.desc: Test the HgmConfigUpdateCallback function of HgmRPEnergy when configData is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestHgmConfigUpdateCallbackNull, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->HgmConfigUpdateCallback(nullptr);
    EXPECT_TRUE(hgmRPEnergy_->componentPowerConfig_.empty());
}

/**
 * @tc.name: TestHgmConfigUpdateCallbackNotNull
 * @tc.desc: Test the HgmConfigUpdateCallback function of HgmRPEnergy when configData is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestHgmConfigUpdateCallbackNotNull, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    auto configData = std::make_shared<RPHgmConfigData>();
    std::unordered_map<std::string, int32_t> componentPowerConfig = { { "Component1", 60 }, { "Component2", 30 } };
    configData->SetComponentPowerConfig(componentPowerConfig);

    hgmRPEnergy_->HgmConfigUpdateCallback(configData);

    EXPECT_EQ(hgmRPEnergy_->componentPowerConfig_.size(), componentPowerConfig.size());
}

/**
 * @tc.name: TestGetComponentFpsNotIdle
 * @tc.desc: Test the GetComponentFps function when isTouchIdle_ is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestGetComponentFpsNotIdle, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->SetTouchState(false);
    FrameRateRange range;

    hgmRPEnergy_->GetComponentFps(range);

    EXPECT_EQ(range.isEnergyAssurance_, false);
    EXPECT_EQ(range.max_, 0);
    EXPECT_EQ(range.min_, 0);
    EXPECT_EQ(range.preferred_, 0);
}

/**
 * @tc.name: TestGetComponentFpsIdleEmptyConfig
 * @tc.desc: Test the GetComponentFps function when isTouchIdle_ is true and componentPowerConfig_ is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestGetComponentFpsIdleEmptyConfig, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->SetTouchState(true);
    FrameRateRange range;

    hgmRPEnergy_->GetComponentFps(range);

    EXPECT_EQ(range.isEnergyAssurance_, false);
    EXPECT_EQ(range.max_, 0);
    EXPECT_EQ(range.min_, 0);
    EXPECT_EQ(range.preferred_, 0);
}

/**
 * @tc.name: TestGetComponentFpsConfigNoComponent
 * @tc.desc: Test the GetComponentFps function when componentPowerConfig_ is not empty but does not contain the
 * range.GetComponentName() element
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestGetComponentFpsConfigNoComponent, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->SetTouchState(true);
    std::unordered_map<std::string, int32_t> componentPowerConfig = { { "Component2", 60 } };
    hgmRPEnergy_->componentPowerConfig_ = componentPowerConfig;
    FrameRateRange range;

    hgmRPEnergy_->GetComponentFps(range);

    EXPECT_EQ(range.isEnergyAssurance_, false);
    EXPECT_EQ(range.max_, 0);
    EXPECT_EQ(range.min_, 0);
    EXPECT_EQ(range.preferred_, 0);
}

/**
 * @tc.name: TestGetComponentFpsConfigHasComponent
 * @tc.desc: Test the GetComponentFps function when componentPowerConfig_ is not empty and contains the
 * range.GetComponentName() element
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestGetComponentFpsConfigHasComponent, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->SetTouchState(true);
    std::unordered_map<std::string, int32_t> componentPowerConfig = { { "SWIPER_FLING", 60 } };
    hgmRPEnergy_->componentPowerConfig_ = componentPowerConfig;
    FrameRateRange range;
    range.componentScene_ = ComponentScene::SWIPER_FLING;
    range.preferred_ = 30;

    hgmRPEnergy_->GetComponentFps(range);

    EXPECT_EQ(range.isEnergyAssurance_, false);
    EXPECT_EQ(range.preferred_, 30);
}

/**
 * @tc.name: TestGetComponentFpsPreferredGreaterThanIdle
 * @tc.desc: Test the GetComponentFps function when range.preferred_ > idleFps
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestGetComponentFpsPreferredGreaterThanIdle, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->SetTouchState(true);
    std::unordered_map<std::string, int32_t> componentPowerConfig = { { "SWIPER_FLING", 60 } };
    hgmRPEnergy_->componentPowerConfig_ = componentPowerConfig;
    FrameRateRange range;
    range.componentScene_ = ComponentScene::SWIPER_FLING;
    range.preferred_ = 70;

    hgmRPEnergy_->GetComponentFps(range);

    EXPECT_EQ(range.isEnergyAssurance_, true);
    EXPECT_EQ(range.preferred_, 60);
}

/**
 * @tc.name: TestGetComponentFpsPreferredLessThanIdle
 * @tc.desc: Test the GetComponentFps function when range.preferred_ < idleFps
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestGetComponentFpsPreferredLessThanIdle, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->SetTouchState(true);
    std::unordered_map<std::string, int32_t> componentPowerConfig = { { "SWIPER_FLING", 60 } };
    hgmRPEnergy_->componentPowerConfig_ = componentPowerConfig;
    FrameRateRange range;
    range.componentScene_ = ComponentScene::SWIPER_FLING;
    range.preferred_ = 30;

    hgmRPEnergy_->GetComponentFps(range);

    EXPECT_EQ(range.isEnergyAssurance_, false);
    EXPECT_EQ(range.preferred_, 30);
}

/**
 * @tc.name: TestMoveEnergyCommonDataTo
 * @tc.desc: Test the MoveEnergyCommonDataTo function, just call it and assert the input parameter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestMoveEnergyCommonDataTo, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    EnergyCommonDataMap commonData;
    hgmRPEnergy_->MoveEnergyCommonDataTo(commonData);

    EXPECT_EQ(hgmRPEnergy_->energyCommonData_.size(), 0);
}
/**
 * @tc.name: TestAddEnergyCommonDataEmpty
 * @tc.desc: Test the AddEnergyCommonData function when energyCommonData_ is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestAddEnergyCommonDataEmpty, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->AddEnergyCommonData(EnergyEvent::START_NEW_ANIMATION, "key", "value");

    EXPECT_EQ(hgmRPEnergy_->energyCommonData_.size(), 1);
    EXPECT_EQ(hgmRPEnergy_->energyCommonData_[EnergyEvent::START_NEW_ANIMATION].size(), 1);
}

/**
 * @tc.name: TestAddEnergyCommonDataNoEvent
 * @tc.desc: Test the AddEnergyCommonData function when energyCommonData_ is not empty but cannot find the event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestAddEnergyCommonDataNoEvent, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->AddEnergyCommonData(EnergyEvent::START_NEW_ANIMATION, "key1", "value1");
    hgmRPEnergy_->AddEnergyCommonData(EnergyEvent::VOTER_VIDEO_RATE, "key2", "value2");

    EXPECT_EQ(hgmRPEnergy_->energyCommonData_.size(), 2);
    EXPECT_EQ(hgmRPEnergy_->energyCommonData_[EnergyEvent::START_NEW_ANIMATION].size(), 1);
    EXPECT_EQ(hgmRPEnergy_->energyCommonData_[EnergyEvent::VOTER_VIDEO_RATE].size(), 1);
}

/**
 * @tc.name: TestAddEnergyCommonDataHasEvent
 * @tc.desc: Test the AddEnergyCommonData function when energyCommonData_ is not empty and can find the event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestAddEnergyCommonDataHasEvent, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    hgmRPEnergy_->AddEnergyCommonData(EnergyEvent::START_NEW_ANIMATION, "key1", "value1");
    hgmRPEnergy_->AddEnergyCommonData(EnergyEvent::START_NEW_ANIMATION, "key2", "value2");

    EXPECT_EQ(hgmRPEnergy_->energyCommonData_.size(), 1);
    EXPECT_EQ(hgmRPEnergy_->energyCommonData_[EnergyEvent::START_NEW_ANIMATION].size(), 2);
}

/**
 * @tc.name: TestStatisticAnimationTime
 * @tc.desc: Test the StatisticAnimationTime function, call it and assert energyCommonData_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPEnergyTest, TestStatisticAnimationTime, TestSize.Level1)
{
    ASSERT_NE(hgmRPEnergy_, nullptr);

    uint64_t timestamp = 123456;

    hgmRPEnergy_->StatisticAnimationTime(timestamp);

    EXPECT_EQ(hgmRPEnergy_->energyCommonData_.size(), 1);
    EXPECT_EQ(hgmRPEnergy_->energyCommonData_[EnergyEvent::ANIMATION_EXEC_TIME].size(), 1);
    EXPECT_EQ(hgmRPEnergy_->energyCommonData_[EnergyEvent::ANIMATION_EXEC_TIME]["STATIC_ANIMATION_TIME"], "123456");
}

} // namespace OHOS::Rosen