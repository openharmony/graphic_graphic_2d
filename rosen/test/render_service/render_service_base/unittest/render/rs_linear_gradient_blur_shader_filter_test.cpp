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