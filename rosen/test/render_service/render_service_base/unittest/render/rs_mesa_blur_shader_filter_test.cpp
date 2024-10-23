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

#include "render/rs_mesa_blur_shader_filter.h"
#include "render/rs_pixel_stretch_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSMESABlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMESABlurShaderFilterTest::SetUpTestCase() {}
void RSMESABlurShaderFilterTest::TearDownTestCase() {}
void RSMESABlurShaderFilterTest::SetUp() {}
void RSMESABlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetRadiusTest
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GetRadiusTest, TestSize.Level1)
{
    // 1: blur param
    int radius = 1;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_EQ(mesaShaderFilter->GetRadius(), 1);
}

/**
 * @tc.name: GetDetailedDescriptionTest
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GetDetailedDescriptionTest, TestSize.Level1)
{
    // 1: blur param
    int radius = 1;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 1.f, 1.f, 0, 1.f, 1.f);
    mesaShaderFilter->pixelStretchParam_ = pixelStretchParams;
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
}

/**
 * @tc.name: GenerateGEVisualEffectTest
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GenerateGEVisualEffectTest, TestSize.Level1)
{
    int radius = 0;
    float greyCoef = 1.f; // 1.f: grey coefficients
    auto mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius, greyCoef, greyCoef);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 1.f, 1.f, 0, 1.f, 1.f);
    mesaBlurShaderFilter->pixelStretchParam_ = pixelStretchParams;
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    mesaBlurShaderFilter->SetPixelStretchParams(pixelStretchParams);
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}
} // namespace Rosen
} // namespace OHOS
