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

#include "feature/hyper_graphic_manager/rs_frame_rate_policy.h"
#include "transaction/rs_hgm_config_data.h"
#include "modifier/rs_modifier_type.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RsFrameRatePolicysTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFrameRatePolicysTest::SetUpTestCase() {}
void RsFrameRatePolicysTest::TearDownTestCase() {}
void RsFrameRatePolicysTest::SetUp() {}
void RsFrameRatePolicysTest::TearDown() {}

/**
 * @tc.name: interface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsFrameRatePolicysTest, interface, TestSize.Level1)
{
    auto ins = RSFrameRatePolicy::GetInstance();

    ins->RegisterHgmConfigChangeCallback();

    float speed = 1.0;
    std::string scene = "0";
    EXPECT_EQ(ins->GetPreferredFps(scene, speed), 0);
}

/**
 * @tc.name: interface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsFrameRatePolicysTest, GetRefreshRateMode_Test, TestSize.Level1)
{
    auto ins = RSFrameRatePolicy::GetInstance();

    ins->RegisterHgmConfigChangeCallback();

    EXPECT_NE(ins, nullptr);
}

/**
 * @tc.name: HgmConfigChangeCallback
 * @tc.desc: test results of HgmConfigChangeCallback
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsFrameRatePolicysTest, HgmConfigChangeCallback, TestSize.Level1)
{
    auto ins = RSFrameRatePolicy::GetInstance();
    std::shared_ptr<RSHgmConfigData> configData;
    ins->HgmConfigChangeCallback(configData);
    EXPECT_TRUE(configData == nullptr);

    configData = std::make_shared<RSHgmConfigData>();
    ins->HgmConfigChangeCallback(configData);
    EXPECT_TRUE(configData->configData_.empty() == true);

    AnimDynamicItem item;
    configData->AddAnimDynamicItem(item);
    ins->HgmConfigChangeCallback(configData);
    EXPECT_FALSE(configData->configData_.empty());

    item.animType = "animType";
    item.animName = "animName";
    ins->HgmConfigChangeCallback(configData);
    EXPECT_FALSE(configData->configData_.empty());

    item.animType.clear();
    ins->HgmConfigChangeCallback(configData);
    EXPECT_FALSE(configData->configData_.empty());

    item.animName.clear();
    ins->HgmConfigChangeCallback(configData);
    EXPECT_FALSE(configData->configData_.empty());
}

/**
 * @tc.name: HgmRefreshRateModeChangeCallback
 * @tc.desc: test results of HgmRefreshRateModeChangeCallback
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsFrameRatePolicysTest, HgmRefreshRateModeChangeCallback, TestSize.Level1)
{
    auto ins = RSFrameRatePolicy::GetInstance();
    int32_t refreshRateMode = 0;
    ins->HgmRefreshRateModeChangeCallback(refreshRateMode);
    EXPECT_TRUE(refreshRateMode == 0);
}

/**
 * @tc.name: GetRefreshRateModeName
 * @tc.desc: test results of GetRefreshRateModeName
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsFrameRatePolicysTest, GetRefreshRateModeName, TestSize.Level1)
{
    auto ins = RSFrameRatePolicy::GetInstance();
    int32_t res = ins->GetRefreshRateModeName();
    EXPECT_TRUE(res == -1);
}

/**
 * @tc.name: GetPreferredFps
 * @tc.desc: test results of GetPreferredFps
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsFrameRatePolicysTest, GetPreferredFps, TestSize.Level1)
{
    auto ins = RSFrameRatePolicy::GetInstance();
    std::string scene = "test scene";
    float speed = 1.f;
    int32_t res = ins->GetPreferredFps(scene, speed);
    EXPECT_TRUE(res == 0);

    ins->animAttributes_.insert({ "test scene", { { "test scene", AnimDynamicAttribute() } } });
    res = ins->GetPreferredFps(scene, speed);
    EXPECT_TRUE(res == 0);

    ins->ppi_ = 0;
    res = ins->GetPreferredFps(scene, speed);
    EXPECT_TRUE(res == 0);

    ins->ppi_ = 25.4f;
    speed = 127.f;
    res = ins->GetPreferredFps(scene, speed);
    EXPECT_TRUE(res == 0);
}

/**
 * @tc.name: GetExpectedFrameRate
 * @tc.desc: test results of GetExpectedFrameRate
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsFrameRatePolicysTest, GetExpectedFrameRate, TestSize.Level1)
{
    auto ins = RSFrameRatePolicy::GetInstance();
    ins->animAttributes_.insert({ "translate", { { "translate", AnimDynamicAttribute({0, 26, 25}) } } });
    RSPropertyUnit unit = RSPropertyUnit::PIXEL_POSITION;
    int32_t res = ins->GetExpectedFrameRate(unit, 1.f);
    EXPECT_FALSE(res == 0);

    unit = RSPropertyUnit::PIXEL_SIZE;
    res = ins->GetExpectedFrameRate(unit, 1.f);
    EXPECT_TRUE(res == 0);

    unit = RSPropertyUnit::RATIO_SCALE;
    res = ins->GetExpectedFrameRate(unit, 1.f);
    EXPECT_TRUE(res == 0);

    unit = RSPropertyUnit::ANGLE_ROTATION;
    res = ins->GetExpectedFrameRate(unit, 1.f);
    EXPECT_TRUE(res == 0);

    unit = RSPropertyUnit::UNKNOWN;
    res = ins->GetExpectedFrameRate(unit, 1.f);
    EXPECT_TRUE(res == 0);
}
} // namespace Rosen
} // namespace OHOS