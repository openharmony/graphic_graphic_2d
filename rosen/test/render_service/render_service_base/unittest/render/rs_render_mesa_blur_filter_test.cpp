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
#include "render/rs_render_mesa_blur_filter.h"
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
 * @tc.name: RSMESABlurShaderFilterTest01
 * @tc.desc: Verify function RSMESABlurShaderFilter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, RSMESABlurShaderFilterTest01, TestSize.Level1)
{
    // 1: blur param
    int radius = 1;
    float greyCoefw = 0.1f;
    float greyCoefh = 0.1f;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius, greyCoefw, greyCoefh);
    EXPECT_EQ(mesaShaderFilter->GetRadius(), 1);
}

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
 * @tc.name: GetRadiusTest001
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GetRadiusTest001, TestSize.Level1)
{
    // 10: blur param
    int radius = 10;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_EQ(mesaShaderFilter->GetRadius(), 10);
}

/**
 * @tc.name: GetRadiusTest0012
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GetRadiusTest002, TestSize.Level1)
{
    // 0: blur param
    int radius = 0;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_EQ(mesaShaderFilter->GetRadius(), 0);
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
 * @tc.name: GetDetailedDescriptionTest001
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GetDetailedDescriptionTest001, TestSize.Level1)
{
    // 1000: blur param
    int radius = 1000;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 1.f, 1.f, 0, 1.f, 1.f);
    mesaShaderFilter->pixelStretchParam_ = pixelStretchParams;
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
}

/**
 * @tc.name: GetDetailedDescriptionTest002
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GetDetailedDescriptionTest002, TestSize.Level1)
{
    // 10: blur param
    int radius = 10;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(2.f, 2.f, 1.f, 1.f, 0, 1.f, 1.f);
    mesaShaderFilter->pixelStretchParam_ = pixelStretchParams;
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
}

/**
 * @tc.name: GetDetailedDescriptionTest003
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GetDetailedDescriptionTest003, TestSize.Level1)
{
    // 10: blur param
    int radius = 10;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 2.f, 2.f, 0, 1.f, 1.f);
    mesaShaderFilter->pixelStretchParam_ = pixelStretchParams;
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
}

/**
 * @tc.name: GetDetailedDescriptionTest004
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GetDetailedDescriptionTest004, TestSize.Level1)
{
    // 100: blur param
    int radius = 100;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 1.f, 1.f, 2, 1.f, 1.f);
    mesaShaderFilter->pixelStretchParam_ = pixelStretchParams;
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
}

/**
 * @tc.name: GetDetailedDescriptionTest005
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GetDetailedDescriptionTest005, TestSize.Level1)
{
    // 10: blur param
    int radius = 10;
    auto mesaShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    EXPECT_NE(mesaShaderFilter->GetDetailedDescription(), "");
    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 1.f, 1.f, 0, 2.f, 2.f);
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

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GenerateGEVisualEffectTest001, TestSize.Level1)
{
    int radius = 10;
    float greyCoef = 2.0f; // 1.f: grey coefficients
    auto mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius, greyCoef, greyCoef);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(2.f, 2.f, 1.f, 1.f, 0, 1.f, 1.f);
    mesaBlurShaderFilter->pixelStretchParam_ = pixelStretchParams;
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    mesaBlurShaderFilter->SetPixelStretchParams(pixelStretchParams);
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest002
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GenerateGEVisualEffectTest002, TestSize.Level1)
{
    int radius = 2000;
    float greyCoef = 2.f; // 1.f: grey coefficients
    auto mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius, greyCoef, greyCoef);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 2.f, 2.f, 0, 1.f, 1.f);
    mesaBlurShaderFilter->pixelStretchParam_ = pixelStretchParams;
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    mesaBlurShaderFilter->SetPixelStretchParams(pixelStretchParams);
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest003
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GenerateGEVisualEffectTest003, TestSize.Level1)
{
    int radius = 0;
    float greyCoef = 2.f; // 1.f: grey coefficients
    auto mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius, greyCoef, greyCoef);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 1.f, 1.f, 2, 1.f, 1.f);
    mesaBlurShaderFilter->pixelStretchParam_ = pixelStretchParams;
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    mesaBlurShaderFilter->SetPixelStretchParams(pixelStretchParams);
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest004
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMESABlurShaderFilterTest, GenerateGEVisualEffectTest004, TestSize.Level1)
{
    int radius = 0;
    float greyCoef = 1.1f; // 1.f: grey coefficients
    auto mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(radius, greyCoef, greyCoef);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    // 0, 1.f: valid mesa blur params
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(1.f, 1.f, 1.f, 1.f, 0, 2.f, 2.f);
    mesaBlurShaderFilter->pixelStretchParam_ = pixelStretchParams;
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    mesaBlurShaderFilter->SetPixelStretchParams(pixelStretchParams);
    mesaBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

} // namespace Rosen
} // namespace OHOS
