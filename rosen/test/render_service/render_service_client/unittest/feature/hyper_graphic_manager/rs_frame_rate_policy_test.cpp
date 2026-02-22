/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "feature/hyper_graphic_manager/rs_frame_rate_policy.h"
#include "transaction/rs_hgm_config_data.h"
#include "modifier/rs_render_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSFrameRatePolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void InitializeRSFrameRatePolicy();
};

void RSFrameRatePolicyTest::SetUpTestCase() {}
void RSFrameRatePolicyTest::TearDownTestCase() {}
void RSFrameRatePolicyTest::SetUp() {}
void RSFrameRatePolicyTest::TearDown() {}
void RSFrameRatePolicyTest::InitializeRSFrameRatePolicy()
{
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    instance->ppi_ = 1.0f;
    instance->xDpi_ = 1.0f;
    instance->yDpi_ = 1.0f;
    instance->pageNameList_.clear();
    instance->currentRefreshRateModeName_ = -1;
    instance->animAttributes_.clear();
    instance->energyInfo_ = EnergyInfo();
}

/**
 * @tc.name: interface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSFrameRatePolicyTest, interface, TestSize.Level1)
{
    auto instance = RSFrameRatePolicy::GetInstance();

    instance->RegisterHgmConfigChangeCallback();

    std::string scene = "0";
    float speed = 1.0;
    EXPECT_EQ(instance->GetPreferredFps(scene, speed), 0);
}

/**
 * @tc.name: interface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSFrameRatePolicyTest, GetRefreshRateMode_Test, TestSize.Level1)
{
    auto instance = RSFrameRatePolicy::GetInstance();

    instance->RegisterHgmConfigChangeCallback();

    EXPECT_EQ(instance->GetRefreshRateModeName(), -1);
}

/**
 * @tc.name: HgmConfigChangeCallback
 * @tc.desc: test results of HgmConfigChangeCallback
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, HgmConfigChangeCallback, TestSize.Level1)
{
    auto instance = RSFrameRatePolicy::GetInstance();
    std::shared_ptr<RSHgmConfigData> configData = std::make_shared<RSHgmConfigData>();
    instance->HgmConfigChangeCallback(configData);
    EXPECT_TRUE(configData->configData_.empty() == true);

    AnimDynamicItem item;
    configData->AddAnimDynamicItem(item);
    instance->HgmConfigChangeCallback(configData);
    EXPECT_TRUE(configData->configData_.empty() != true);

    item.animType = "animType";
    item.animName = "animName";
    instance->HgmConfigChangeCallback(configData);
    EXPECT_TRUE(configData->configData_.empty() != true);

    item.animType.clear();
    instance->HgmConfigChangeCallback(configData);
    EXPECT_TRUE(configData->configData_.empty() != true);

    item.animName.clear();
    instance->HgmConfigChangeCallback(configData);
    EXPECT_TRUE(configData->configData_.empty() != true);
}

/**
 * @tc.name: HgmRefreshRateModeChangeCallback
 * @tc.desc: test results of HgmRefreshRateModeChangeCallback
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, HgmRefreshRateModeChangeCallback, TestSize.Level1)
{
    auto instance = RSFrameRatePolicy::GetInstance();
    int32_t refreshRateMode = 0;
    instance->HgmRefreshRateModeChangeCallback(refreshRateMode);
    EXPECT_TRUE(instance->GetRefreshRateModeName() >= -1);
}

/**
 * @tc.name: GetRefreshRateModeName
 * @tc.desc: test results of GetRefreshRateModeName
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, GetRefreshRateModeName, TestSize.Level1)
{
    auto instance = RSFrameRatePolicy::GetInstance();
    int32_t res = instance->GetRefreshRateModeName();
    EXPECT_TRUE(res == -1);
}

/**
 * @tc.name: GetPreferredFps
 * @tc.desc: test results of GetPreferredFps
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, GetPreferredFps, TestSize.Level1)
{
    auto instance = RSFrameRatePolicy::GetInstance();
    std::string scene = "test scene";
    float speed = 1.f;
    int32_t res = instance->GetPreferredFps(scene, speed);
    EXPECT_TRUE(res == 0);

    instance->animAttributes_.insert({ "test scene", { { "test scene", AnimDynamicAttribute() } } });
    res = instance->GetPreferredFps(scene, speed);
    EXPECT_TRUE(res == 0);

    instance->ppi_ = 0;
    res = instance->GetPreferredFps(scene, speed);
    EXPECT_TRUE(res == 0);

    instance->ppi_ = 25.4f;
    speed = 127.f;
    res = instance->GetPreferredFps(scene, speed);
    EXPECT_TRUE(res == 0);
}

/**
 * @tc.name: GetExpectedFrameRate
 * @tc.desc: test results of GetExpectedFrameRate
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, GetExpectedFrameRate, TestSize.Level1)
{
    auto instance = RSFrameRatePolicy::GetInstance();
    instance->animAttributes_.insert({ "translate", { { "translate", AnimDynamicAttribute({0, 26, 25}) } } });
    RSPropertyUnit unit = RSPropertyUnit::PIXEL_POSITION;
    int32_t res = instance->GetExpectedFrameRate(unit, 1.f);
    EXPECT_FALSE(res == 0);

    unit = RSPropertyUnit::PIXEL_SIZE;
    res = instance->GetExpectedFrameRate(unit, 1.f);
    EXPECT_TRUE(res == 0);

    unit = RSPropertyUnit::RATIO_SCALE;
    res = instance->GetExpectedFrameRate(unit, 1.f);
    EXPECT_TRUE(res == 0);

    unit = RSPropertyUnit::ANGLE_ROTATION;
    res = instance->GetExpectedFrameRate(unit, 1.f);
    EXPECT_TRUE(res == 0);

    unit = RSPropertyUnit::UNKNOWN;
    res = instance->GetExpectedFrameRate(unit, 1.f);
    EXPECT_TRUE(res == 0);
}

/**
 * @tc.name: GetTouchOrPointerAction
 * @tc.desc: test results of GetTouchOrPointerAction
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, GetTouchOrPointerAction, TestSize.Level1)
{
    auto instance = RSFrameRatePolicy::GetInstance();
    int32_t undefinedPointerAction = -1;
    int32_t sendMoveDuration = 1100000;

    EXPECT_FALSE(instance->GetTouchOrPointerAction(undefinedPointerAction));

    EXPECT_TRUE(instance->GetTouchOrPointerAction(TOUCH_CANCEL));

    EXPECT_TRUE(instance->GetTouchOrPointerAction(TOUCH_DOWN));

    usleep(sendMoveDuration);
    EXPECT_TRUE(instance->GetTouchOrPointerAction(TOUCH_MOVE));

    EXPECT_FALSE(instance->GetTouchOrPointerAction(TOUCH_MOVE));

    usleep(sendMoveDuration);
    EXPECT_TRUE(instance->GetTouchOrPointerAction(TOUCH_MOVE));

    EXPECT_TRUE(instance->GetTouchOrPointerAction(TOUCH_UP));

    EXPECT_TRUE(instance->GetTouchOrPointerAction(TOUCH_BUTTON_DOWN));

    EXPECT_TRUE(instance->GetTouchOrPointerAction(TOUCH_BUTTON_UP));

    EXPECT_TRUE(instance->GetTouchOrPointerAction(TOUCH_PULL_DOWN));

    EXPECT_TRUE(instance->GetTouchOrPointerAction(TOUCH_PULL_UP));

    usleep(sendMoveDuration);
    EXPECT_TRUE(instance->GetTouchOrPointerAction(TOUCH_PULL_MOVE));

    EXPECT_TRUE(instance->GetTouchOrPointerAction(AXIS_BEGIN));

    EXPECT_TRUE(instance->GetTouchOrPointerAction(AXIS_END));
    
    EXPECT_TRUE(instance->GetTouchOrPointerAction(AXIS_UPDATE));

    EXPECT_FALSE(instance->GetTouchOrPointerAction(AXIS_UPDATE));

    usleep(sendMoveDuration);
    EXPECT_TRUE(instance->GetTouchOrPointerAction(AXIS_UPDATE));
}

/**
 * @tc.name: TestDispatchUpdateBranch_Case1
 * @tc.desc: Test DispatchUpdateBranch when configData is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, TestDispatchUpdateBranch_Case1, TestSize.Level1)
{
    InitializeRSFrameRatePolicy();

    auto instance = RSFrameRatePolicy::GetInstance();
    instance->DispatchUpdateBranch(nullptr);
    EXPECT_EQ(instance->GetPageNameList().size(), 0);
    EXPECT_EQ(instance->ppi_, 1.0f);
    EXPECT_EQ(instance->xDpi_, 1.0f);
    EXPECT_EQ(instance->yDpi_, 1.0f);
    EXPECT_EQ(instance->animAttributes_.size(), 0);
}

/**
 * @tc.name: TestDispatchUpdateBranch_Case2
 * @tc.desc: Test DispatchUpdateBranch when IsSyncConfig and !IsSyncEnergyData
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, TestDispatchUpdateBranch_Case2, TestSize.Level1)
{
    InitializeRSFrameRatePolicy();

    auto instance = RSFrameRatePolicy::GetInstance();
    auto configData = std::make_shared<RSHgmConfigData>();
    configData->SetIsSyncConfig(true);
    configData->SetPpi(25.4f);
    configData->SetXDpi(25.4f);
    configData->SetYDpi(25.4f);
    std::string pageName = "TestPage";
    configData->AddPageName(pageName);
    AnimDynamicItem item;
    item.animType = "testType";
    item.animName = "testName";
    item.minSpeed = 1;
    item.maxSpeed = 10;
    item.preferredFps = 60;
    configData->AddAnimDynamicItem(item);

    instance->DispatchUpdateBranch(configData);
    EXPECT_EQ(instance->GetPageNameList().size(), 1);
    EXPECT_EQ(instance->ppi_, 25.4f);
    EXPECT_EQ(instance->xDpi_, 25.4f);
    EXPECT_EQ(instance->yDpi_, 25.4f);
    EXPECT_EQ(instance->animAttributes_.size(), 1);
}

/**
 * @tc.name: TestDispatchUpdateBranch_Case3
 * @tc.desc: Test DispatchUpdateBranch when !IsSyncConfig and IsSyncEnergyData
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, TestDispatchUpdateBranch_Case3, TestSize.Level1)
{
    InitializeRSFrameRatePolicy();

    auto instance = RSFrameRatePolicy::GetInstance();
    auto configData = std::make_shared<RSHgmConfigData>();
    configData->SetIsSyncConfig(false);
    // Set energy info
    EnergyInfo energyInfo;
    energyInfo.componentName = "TestComponent";
    energyInfo.componentPid = 1234;
    energyInfo.componentDefaultFps = 60;
    configData->SetEnergyInfo(energyInfo);

    instance->DispatchUpdateBranch(configData);
    EXPECT_EQ(instance->energyInfo_.componentName, "TestComponent");
    EXPECT_EQ(instance->energyInfo_.componentPid, 1234);
    EXPECT_EQ(instance->energyInfo_.componentDefaultFps, 60);
}

/**
 * @tc.name: TestGetComponentDefaultFps_Case1
 * @tc.desc: Test when energyInfo_.componentName != scene
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, TestGetComponentDefaultFps_Case1, TestSize.Level1)
{
    InitializeRSFrameRatePolicy();

    auto instance = RSFrameRatePolicy::GetInstance();
    std::string scene = "testScene";
    int32_t frameRate = 30;
    instance->energyInfo_.componentName = "otherComponent";
    instance->energyInfo_.componentDefaultFps = 60;

    int32_t result = instance->GetComponentDefaultFps(scene, frameRate);
    EXPECT_EQ(result, frameRate);
}

/**
 * @tc.name: TestGetComponentDefaultFps_Case2
 * @tc.desc: Test when energyInfo_.componentName == scene and energyInfo_.componentDefaultFps == 0
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, TestGetComponentDefaultFps_Case2, TestSize.Level1)
{
    InitializeRSFrameRatePolicy();

    auto instance = RSFrameRatePolicy::GetInstance();
    std::string scene = "testScene";
    int32_t frameRate = 0;
    instance->energyInfo_.componentName = scene;
    instance->energyInfo_.componentDefaultFps = 0;

    int32_t result = instance->GetComponentDefaultFps(scene, frameRate);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: TestGetComponentDefaultFps_Case3
 * @tc.desc: Test when energyInfo_.componentName == scene and energyInfo_.componentDefaultFps != 0, frameRate = 0
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, TestGetComponentDefaultFps_Case3, TestSize.Level1)
{
    InitializeRSFrameRatePolicy();

    auto instance = RSFrameRatePolicy::GetInstance();
    std::string scene = "testScene";
    int32_t frameRate = 0;
    instance->energyInfo_.componentName = scene;
    instance->energyInfo_.componentDefaultFps = 60;

    int32_t result = instance->GetComponentDefaultFps(scene, frameRate);
    EXPECT_EQ(result, 60);
}

/**
 * @tc.name: TestGetComponentDefaultFps_Case4
 * @tc.desc: Test when energyInfo_.componentDefaultFps != 0, frameRate > energyInfo_.componentDefaultFps
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, TestGetComponentDefaultFps_Case4, TestSize.Level1)
{
    InitializeRSFrameRatePolicy();

    auto instance = RSFrameRatePolicy::GetInstance();
    std::string scene = "testScene";
    int32_t frameRate = 90;
    instance->energyInfo_.componentName = scene;
    instance->energyInfo_.componentDefaultFps = 60;

    int32_t result = instance->GetComponentDefaultFps(scene, frameRate);
    EXPECT_EQ(result, 60);
}

/**
 * @tc.name: TestGetComponentDefaultFps_Case5
 * @tc.desc: Test when energyInfo_.componentDefaultFps != 0 and frameRate < energyInfo_.componentDefaultFps
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRatePolicyTest, TestGetComponentDefaultFps_Case5, TestSize.Level1)
{
    InitializeRSFrameRatePolicy();

    auto instance = RSFrameRatePolicy::GetInstance();
    std::string scene = "testScene";
    int32_t frameRate = 30;
    instance->energyInfo_.componentName = scene;
    instance->energyInfo_.componentDefaultFps = 60;

    int32_t result = instance->GetComponentDefaultFps(scene, frameRate);
    EXPECT_EQ(result, 30);
}
} // namespace Rosen
} // namespace OHOS