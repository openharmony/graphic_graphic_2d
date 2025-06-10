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
#include "render/rs_render_kawase_blur_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSKawaseBlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSKawaseBlurShaderFilterTest::SetUpTestCase() {}
void RSKawaseBlurShaderFilterTest::TearDownTestCase() {}
void RSKawaseBlurShaderFilterTest::SetUp() {}
void RSKawaseBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetRadiusTest
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSKawaseBlurShaderFilterTest, GetRadiusTest, TestSize.Level1)
{
    int radius = 1;
    auto kawaseShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);
    ASSERT_NE(kawaseShaderFilter, nullptr);
    EXPECT_EQ(kawaseShaderFilter->GetRadius(), 1);
}

/**
 * @tc.name: GetDescriptionTest
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSKawaseBlurShaderFilterTest, GetDescriptionTest, TestSize.Level1)
{
    int radius = 1;
    auto kawaseShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);
    ASSERT_NE(kawaseShaderFilter, nullptr);
    std::string filterString = ", radius: " + std::to_string(radius) + " sigma";
    std::string out = "";
    kawaseShaderFilter->GetDescription(out);
    EXPECT_EQ(out, filterString);

    kawaseShaderFilter->isMesablurAllEnable_ = true;
    filterString += ", replaced by Mesa.";
    out = "";
    kawaseShaderFilter->GetDescription(out);
    EXPECT_EQ(out, filterString);
}

/**
 * @tc.name: GenerateGEVisualEffectTest
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSKawaseBlurShaderFilterTest, GenerateGEVisualEffectTest, TestSize.Level1)
{
    int radius = 0;
    RSKawaseBlurShaderFilter::SetMesablurAllEnabledByCCM(false);
    auto kawaseBlurShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);
    ASSERT_NE(kawaseBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    kawaseBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}


/**
 * @tc.name: GenerateGEVisualEffectTestMaxRadius
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSKawaseBlurShaderFilterTest, GenerateGEVisualEffectTestMaxRadius, TestSize.Level1)
{
    int radius = INT_MAX;
    auto kawaseBlurShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);
    ASSERT_NE(kawaseBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    kawaseBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTestMinRadius
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSKawaseBlurShaderFilterTest, GenerateGEVisualEffectTestMinRadius, TestSize.Level1)
{
    int radius = INT_MIN;
    auto kawaseBlurShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);
    ASSERT_NE(kawaseBlurShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    kawaseBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    kawaseBlurShaderFilter->isMesablurAllEnable_ = true;
    kawaseBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}
} // namespace Rosen
} // namespace OHOS