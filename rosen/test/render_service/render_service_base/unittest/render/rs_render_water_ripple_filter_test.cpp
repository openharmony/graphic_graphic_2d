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

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "render/rs_render_water_ripple_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSWaterRippleShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSWaterRippleShaderFilterTest::SetUpTestCase() {}
void RSWaterRippleShaderFilterTest::TearDownTestCase() {}
void RSWaterRippleShaderFilterTest::SetUp() {}
void RSWaterRippleShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetProgressTest
 * @tc.desc: Verify function GetProgress
 * @tc.type:FUNC
 */
HWTEST_F(RSWaterRippleShaderFilterTest, GetProgressTest, TestSize.Level1)
{
    float progress = 0.1f;
    uint32_t waveCount = 2;
    float rippleCenterX = 0.3f;
    float rippleCenterY = 0.5f;
    uint32_t rippleMode = 1;

    auto rsWaterRippleShaderFilter = std::make_shared<RSWaterRippleShaderFilter>(progress, waveCount, rippleCenterX,
                                                                                rippleCenterY, rippleMode);
    EXPECT_EQ(rsWaterRippleShaderFilter->GetProgress(), 0.1f);
}

/**
 * @tc.name: GetWaveCountTest
 * @tc.desc: Verify function GetWaveCount
 * @tc.type:FUNC
 */
HWTEST_F(RSWaterRippleShaderFilterTest, GetWaveCountTest, TestSize.Level1)
{
    float progress = 0.1f;
    uint32_t waveCount = 2;
    float rippleCenterX = 0.3f;
    float rippleCenterY = 0.5f;
    uint32_t rippleMode = 1;

    auto rsWaterRippleShaderFilter = std::make_shared<RSWaterRippleShaderFilter>(progress, waveCount, rippleCenterX,
                                                                                rippleCenterY, rippleMode);
    EXPECT_EQ(rsWaterRippleShaderFilter->GetWaveCount(), 2);
}

/**
 * @tc.name: GetRippleCenterXTest
 * @tc.desc: Verify function GetRippleCenterX
 * @tc.type:FUNC
 */
HWTEST_F(RSWaterRippleShaderFilterTest, GetRippleCenterXTest, TestSize.Level1)
{
    float progress = 0.1f;
    uint32_t waveCount = 2;
    float rippleCenterX = 0.3f;
    float rippleCenterY = 0.5f;
    uint32_t rippleMode = 1;

    auto rsWaterRippleShaderFilter = std::make_shared<RSWaterRippleShaderFilter>(progress, waveCount, rippleCenterX,
                                                                                rippleCenterY, rippleMode);
    EXPECT_EQ(rsWaterRippleShaderFilter->GetRippleCenterX(), 0.3f);
}

/**
 * @tc.name: GetRippleCenterYTest
 * @tc.desc: Verify function GetRippleCenterY
 * @tc.type:FUNC
 */
HWTEST_F(RSWaterRippleShaderFilterTest, GetRippleCenterYTest, TestSize.Level1)
{
    float progress = 0.1f;
    uint32_t waveCount = 2;
    float rippleCenterX = 0.3f;
    float rippleCenterY = 0.5f;
    uint32_t rippleMode = 1;

    auto rsWaterRippleShaderFilter = std::make_shared<RSWaterRippleShaderFilter>(progress, waveCount, rippleCenterX,
                                                                                rippleCenterY, rippleMode);
    EXPECT_EQ(rsWaterRippleShaderFilter->GetRippleCenterY(), 0.5f);
}

/**
 * @tc.name: GetRippleModeTest
 * @tc.desc: Verify function GetRippleMode
 * @tc.type:FUNC
 */
HWTEST_F(RSWaterRippleShaderFilterTest, GetRippleModeTest, TestSize.Level1)
{
    float progress = 0.1f;
    uint32_t waveCount = 2;
    float rippleCenterX = 0.3f;
    float rippleCenterY = 0.5f;
    uint32_t rippleMode = 1;

    auto rsWaterRippleShaderFilter = std::make_shared<RSWaterRippleShaderFilter>(progress, waveCount, rippleCenterX,
                                                                                rippleCenterY, rippleMode);
    EXPECT_EQ(rsWaterRippleShaderFilter->GetRippleMode(), 1);
}

/**
 * @tc.name: GenerateGEVisualEffectTest
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSWaterRippleShaderFilterTest, GenerateGEVisualEffectTest, TestSize.Level1)
{
    float progress = 0.1f;
    uint32_t waveCount = 2;
    float rippleCenterX = 0.3f;
    float rippleCenterY = 0.5f;
    uint32_t rippleMode = 1;

    auto rsWaterRippleShaderFilter = std::make_shared<RSWaterRippleShaderFilter>(progress, waveCount, rippleCenterX,
                                                                                rippleCenterY, rippleMode);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsWaterRippleShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}
} // namespace Rosen
} // namespace OHOS