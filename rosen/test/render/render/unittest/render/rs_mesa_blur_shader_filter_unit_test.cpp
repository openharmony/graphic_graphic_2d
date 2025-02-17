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
class RSMESABlurShaderFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMESABlurShaderFilterUnitTest::SetUpTestCase() {}
void RSMESABlurShaderFilterUnitTest::TearDownTestCase() {}
void RSMESABlurShaderFilterUnitTest::SetUp() {}
void RSMESABlurShaderFilterUnitTest::TearDown() {}

/**
 * @tc.name: GetRadiusTest
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterUnitTest, GetRadiusTest, TestSize.Level1)
{
    // 1: blur param
    int radius = 1;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_EQ(mesaShaderFilter->GetRadius(), 1);
}

/**
 * @tc.name: TestGetDetailedDescription01
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterUnitTest, TestGetDetailedDescription01, TestSize.Level1)
{
    // 1: blur param
    int radius = 1;
    auto testFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_NE(testFilter->GetDetailedDescription(), "");
    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 1.f, 1.f, 0, 1.f, 1.f);
    testFilter->pixelStretchParam_ = pixelStretchParams;
    EXPECT_NE(testFilter->GetDetailedDescription(), "");
}

/**
 * @tc.name: GenerateGEVisualEffectTest
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterUnitTest, TestGenerateGEVisualEffect01, TestSize.Level1)
{
    int radius = 0;
    float greyCoef = 1.f; // 1.f: grey coefficients
    auto testFilter = std::make_shared<RSMESABlurShaderFilter>(radius, greyCoef, greyCoef);
    auto effectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    testFilter->GenerateGEVisualEffect(effectContainer);
    EXPECT_FALSE(effectContainer->filterVec_.empty());

    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 1.f, 1.f, 0, 1.f, 1.f);
    testFilter->pixelStretchParam_ = pixelStretchParams;
    testFilter->GenerateGEVisualEffect(effectContainer);
    EXPECT_FALSE(effectContainer->filterVec_.empty());

    testFilter->SetPixelStretchParams(pixelStretchParams);
    testFilter->GenerateGEVisualEffect(effectContainer);
    EXPECT_FALSE(effectContainer->filterVec_.empty());
}
} // namespace Rosen
} // namespace OHOS
