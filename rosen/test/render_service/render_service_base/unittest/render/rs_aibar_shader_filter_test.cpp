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

#include "render/rs_aibar_shader_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSAIBarShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAIBarShaderFilterTest::SetUpTestCase() {}
void RSAIBarShaderFilterTest::TearDownTestCase() {}
void RSAIBarShaderFilterTest::SetUp() {}
void RSAIBarShaderFilterTest::TearDown() {}

/**
 * @tc.name: DrawImageRectTest
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAIBarShaderFilterTest, DrawImageRectTest, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    EXPECT_FALSE(rsAIBarShaderFilter->GetAiInvertCoef().empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSAIBarShaderFilterTest, GenerateGEVisualEffectTest, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsAIBarShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: IsAiInvertCoefValidTest001
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest001, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_TRUE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest002
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest002, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 1.0, 0.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest003
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest003, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { -1.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest004
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest004, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 2.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest005
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest005, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, -1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest006
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest006, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 2.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest007
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest007, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, -0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest008
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest008, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 1.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest009
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest009, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, -0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest010
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest010, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 1.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest011
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest011, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, -1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest012
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest012, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 2.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest013
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest013, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, -45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

} // namespace Rosen
} // namespace OHOS