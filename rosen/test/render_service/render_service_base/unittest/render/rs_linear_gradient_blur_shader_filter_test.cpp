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

#include "render/rs_linear_gradient_blur_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSLinearGradientBlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLinearGradientBlurShaderFilterTest::SetUpTestCase() {}
void RSLinearGradientBlurShaderFilterTest::TearDownTestCase() {}
void RSLinearGradientBlurShaderFilterTest::SetUp() {}
void RSLinearGradientBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GenerateGEVisualEffectTest
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTest, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>>fractionStops = {{0.f, 1.f}};
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops,
                                                                             GradientDirection::LEFT);
    auto rsLinearGradientBlurShaderFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara,
                                                                                               geoWidth, geoHeight);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTest001, TestSize.Level1)
{
    float blurRadius = FLT_MAX;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTest002, TestSize.Level1)
{
    float blurRadius = FLT_MIN;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTest003, TestSize.Level1)
{
    float blurRadius = FLT_MIN;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTest004, TestSize.Level1)
{
    float blurRadius = FLT_MIN;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTest005, TestSize.Level1)
{
    float blurRadius = FLT_MIN;
    std::vector<std::pair<float, float>> fractionStops = { { FLT_MIN, FLT_MAX } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTest006, TestSize.Level1)
{
    float blurRadius = FLT_MIN;
    std::vector<std::pair<float, float>> fractionStops = { { FLT_MIN, FLT_MAX } };
    float geoWidth = FLT_MIN;
    float geoHeight = FLT_MIN;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTest007, TestSize.Level1)
{
    float blurRadius = FLT_MIN;
    std::vector<std::pair<float, float>> fractionStops = { { FLT_MIN, FLT_MAX } };
    float geoWidth = FLT_MAX;
    float geoHeight = FLT_MAX;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTest008, TestSize.Level1)
{
    float blurRadius = FLT_MIN;
    std::vector<std::pair<float, float>> fractionStops = { { FLT_MAX, FLT_MIN } };
    float geoWidth = FLT_MAX;
    float geoHeight = FLT_MAX;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTest009, TestSize.Level1)
{
    float blurRadius = FLT_MAX;
    std::vector<std::pair<float, float>> fractionStops = { { FLT_MAX, FLT_MIN } };
    float geoWidth = FLT_MAX;
    float geoHeight = FLT_MAX;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestTOP
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTestTOP, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestRIGHT
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTestRIGHT, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::RIGHT);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestBOTTOM
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTestBOTTOM, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::BOTTOM);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestLEFT_TOP
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTestLEFT_TOP, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::LEFT_TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestLEFT_BOTTOM
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTestLEFT_BOTTOM, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::LEFT_BOTTOM);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestRIGHT_TOP
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTestRIGHT_TOP, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::RIGHT_TOP);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestRIGHT_BOTTOM
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTestRIGHT_BOTTOM, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::RIGHT_BOTTOM);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestNONE
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTestNONE, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::NONE);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestSTART_TO_END
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTestSTART_TO_END, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::START_TO_END);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestEND_TO_START
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GenerateGEVisualEffectTestEND_TO_START, TestSize.Level1)
{
    float blurRadius = 0.f;
    std::vector<std::pair<float, float>> fractionStops = { { 0.f, 1.f } };
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::END_TO_START);
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(linearGradientBlurPara, geoWidth, geoHeight);
    ASSERT_NE(rsLinearGradientBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsLinearGradientBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GetLinearGradientBlurRadius003
 * @tc.desc: Verify function GetLinearGradientBlurRadius
 * @tc.type:FUNC
 */
HWTEST_F(RSLinearGradientBlurShaderFilterTest, GetLinearGradientBlurRadius003, TestSize.Level1)
{
    float geoWidth = 0.f;
    float geoHeight = 0.f;
    auto rsLinearGradientBlurShaderFilter =
        std::make_shared<RSLinearGradientBlurShaderFilter>(nullptr, geoWidth, geoHeight);
    EXPECT_TRUE(rsLinearGradientBlurShaderFilter->GetLinearGradientBlurRadius() == 0.);
}
} // namespace Rosen
} // namespace OHOS