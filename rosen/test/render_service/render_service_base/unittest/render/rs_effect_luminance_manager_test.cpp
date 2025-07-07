/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "render/rs_effect_luminance_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSEffectLuminanceManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectLuminanceManagerTest::SetUpTestCase() {}
void RSEffectLuminanceManagerTest::TearDownTestCase() {}
void RSEffectLuminanceManagerTest::SetUp() {}
void RSEffectLuminanceManagerTest::TearDown() {}

/**
 * @tc.name: BrightnessSetGetTest
 * @tc.desc: test results of SetDisplayHeadroom & GetDisplayHeadroom
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectLuminanceManagerTest, BrightnessSetGetTest, TestSize.Level1)
{
    auto& manager = RSEffectLuminanceManager::GetInstance();
    manager.SetDisplayHeadroom(0, 4.0f);
    EXPECT_EQ(manager.GetDisplayHeadroom(0), 4.0f);
}

/**
 * @tc.name: GetBrightnessMappingTest
 * @tc.desc: test results of GetBrightnessMapping
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectLuminanceManagerTest, GetBrightnessMappingTest, TestSize.Level1)
{
    EXPECT_EQ(RSEffectLuminanceManager::GetBrightnessMapping(10.0f, 4.0f), 4.0f);
    EXPECT_EQ(RSEffectLuminanceManager::GetBrightnessMapping(1.5f, -1.0f), 0.0f);
    EXPECT_NE(RSEffectLuminanceManager::GetBrightnessMapping(1.5f, 1.25f), 1.25f);
    EXPECT_NE(RSEffectLuminanceManager::GetBrightnessMapping(1.5f, 1.1f), 1.1f);
}

/**
 * @tc.name: CalcBezierResultYTest01
 * @tc.desc: test results of CalcBezierResultYTest
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectLuminanceManagerTest, CalcBezierResultYTest01, TestSize.Level1)
{
    float y = 0.0f;
    EXPECT_TRUE(RSEffectLuminanceManager::CalcBezierResultY({0.0f, 0.0f}, {1.0f, 1.0f}, {0.5f, 0.5f}, 0.5f, y));
    EXPECT_FALSE(RSEffectLuminanceManager::CalcBezierResultY({0.0f, 0.0f}, {1.0f, 1.0f}, {2.0f, 2.0f}, 2.0f, y));
    EXPECT_TRUE(RSEffectLuminanceManager::CalcBezierResultY({0.0f, 0.0f}, {1.0f, 1.0f}, {2.0f, 2.0f}, 1.0f, y));
    EXPECT_TRUE(RSEffectLuminanceManager::CalcBezierResultY({4.0f, 0.0f}, {-1.0f, 1.0f}, {2.0f, 2.0f}, 2.0f, y));
}
} // namespace OHOS::Rosen