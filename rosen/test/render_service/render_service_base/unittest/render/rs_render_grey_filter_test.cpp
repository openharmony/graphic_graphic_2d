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
#include "render/rs_render_grey_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSGreyShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSGreyShaderFilterTest::SetUpTestCase() {}
void RSGreyShaderFilterTest::TearDownTestCase() {}
void RSGreyShaderFilterTest::SetUp() {}
void RSGreyShaderFilterTest::TearDown() {}

/**
 * @tc.name: GenerateGEVisualEffectTest
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSGreyShaderFilterTest, GenerateGEVisualEffectTest, TestSize.Level1)
{
    float greyCoefLow = 0.f;
    float greyCoefHigh = 0.f;
    auto greyShaderFilter = std::make_shared<RSGreyShaderFilter>(greyCoefLow, greyCoefHigh);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    greyShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: greyCoefLow is invalid
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSGreyShaderFilterTest, GenerateGEVisualEffectTest001, TestSize.Level1)
{
    float greyCoefLow = -1.f;
    float greyCoefHigh = 0.f;
    auto greyShaderFilter = std::make_shared<RSGreyShaderFilter>(greyCoefLow, greyCoefHigh);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    greyShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest002
 * @tc.desc: greyCoefLow is max
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSGreyShaderFilterTest, GenerateGEVisualEffectTest002, TestSize.Level1)
{
    float greyCoefLow = 10000000.f;
    float greyCoefHigh = 0.f;
    auto greyShaderFilter = std::make_shared<RSGreyShaderFilter>(greyCoefLow, greyCoefHigh);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    greyShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest003
 * @tc.desc: greyCoefHigh is invalid
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSGreyShaderFilterTest, GenerateGEVisualEffectTest003, TestSize.Level1)
{
    float greyCoefLow = 0.f;
    float greyCoefHigh = -1.f;
    auto greyShaderFilter = std::make_shared<RSGreyShaderFilter>(greyCoefLow, greyCoefHigh);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    greyShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest004
 * @tc.desc: greyCoefHigh is max
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSGreyShaderFilterTest, GenerateGEVisualEffectTest004, TestSize.Level1)
{
    float greyCoefLow = 0.f;
    float greyCoefHigh = 100000.f;
    auto greyShaderFilter = std::make_shared<RSGreyShaderFilter>(greyCoefLow, greyCoefHigh);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    greyShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest004
 * @tc.desc: greyCoefHigh and greyCoefLow is invalid
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSGreyShaderFilterTest, GenerateGEVisualEffectTest005, TestSize.Level1)
{
    float greyCoefLow = -1.f;
    float greyCoefHigh = -1.f;
    auto greyShaderFilter = std::make_shared<RSGreyShaderFilter>(greyCoefLow, greyCoefHigh);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    greyShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest004
 * @tc.desc: greyCoefHigh and greyCoefLow is max
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSGreyShaderFilterTest, GenerateGEVisualEffectTest006, TestSize.Level1)
{
    float greyCoefLow = 10000000.f;
    float greyCoefHigh = 10000000.f;
    auto greyShaderFilter = std::make_shared<RSGreyShaderFilter>(greyCoefLow, greyCoefHigh);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    greyShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}
} // namespace Rosen
} // namespace OHOS