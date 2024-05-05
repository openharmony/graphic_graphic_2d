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

#include "ui/rs_frame_rate_policy.h"
#include "transaction/rs_hgm_config_data.h"
#include "modifier/rs_modifier_type.h"

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
};

void RSFrameRatePolicyTest::SetUpTestCase() {}
void RSFrameRatePolicyTest::TearDownTestCase() {}
void RSFrameRatePolicyTest::SetUp() {}
void RSFrameRatePolicyTest::TearDown() {}

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
    instance->GetPreferredFps(scene, speed);
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

    instance->GetRefreshRateModeName();
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
    std::shared_ptr<RSHgmConfigData> configData;
    instance->HgmConfigChangeCallback(configData);
    EXPECT_TRUE(configData == nullptr);

    configData = std::make_shared<RSHgmConfigData>();
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
    EXPECT_TRUE(refreshRateMode == 0);
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
    EXPECT_TRUE(res == 0);
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
    RSPropertyUnit unit = RSPropertyUnit::PIXEL_POSITION;
    int32_t res = instance->GetExpectedFrameRate(unit, 1.f);
    EXPECT_TRUE(res == 0);

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
} // namespace Rosen
} // namespace OHOS