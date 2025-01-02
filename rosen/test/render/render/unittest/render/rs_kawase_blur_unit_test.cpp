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

#include "render/rs_kawase_blur.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class KawaseBlurFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void KawaseBlurFilterUnitTest::SetUpTestCase() {}
void KawaseBlurFilterUnitTest::TearDownTestCase() {}
void KawaseBlurFilterUnitTest::SetUp() {}
void KawaseBlurFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestGetKawaseBlurFilter
 * @tc.desc: Verify function GetKawaseBlurFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterUnitTest, TestGetKawaseBlurFilter, TestSize.Level1)
{
    KawaseBlurFilter* testFilter = KawaseBlurFilter::GetKawaseBlurFilter();

    Drawing::Canvas testCanvas;
    Drawing::Rect rectSrc;
    Drawing::Rect rectDst;
    KawaseParameter testParam = KawaseParameter(rectSrc, rectDst, 1);
    std::shared_ptr<Drawing::Image> testImage;

    testFilter->ApplyKawaseBlur(testCanvas, testImage, testParam);
    EXPECT_NE(testFilter, nullptr);
}

/**
 * @tc.name: TestSetupBlurEffectAdvancedFilter01
 * @tc.desc: Verify function setupBlurEffectAdvancedFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterUnitTest, TestSetupBlurEffectAdvancedFilter01, TestSize.Level1)
{
    auto testFilter = std::make_shared<KawaseBlurFilter>();
    testFilter->setupBlurEffectAdvancedFilter();
    EXPECT_NE(testFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: TestGetShaderTransform01
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterUnitTest, TestGetShaderTransform01, TestSize.Level1)
{
    auto testFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas testCanvas(1, 1);
    Drawing::Rect testRect;
    testFilter->GetShaderTransform(&testCanvas, testRect, 1.0f, 1.0f);
    EXPECT_NE(testFilter, nullptr);
}

/**
 * @tc.name: TestOutputOriginalImage01
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterUnitTest, TestOutputOriginalImage01, TestSize.Level1)
{
    auto testFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas testCanvas(1, 1);
    auto testImage = std::make_shared<Drawing::Image>();
    Drawing::Rect rectSrc;
    Drawing::Rect rectDst;
    auto testColorFilter = std::make_shared<Drawing::ColorFilter>();
    KawaseParameter testParam(std, rectDst, 1, testColorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(testCanvas, testImage, testParam);
    EXPECT_NE(testFilter, nullptr);
}

/**
 * @tc.name: TestApplyBlur01
 * @tc.desc: Verify function ApplyBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterUnitTest, TestApplyBlur01, TestSize.Level1)
{
    auto testFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas testCanvas(1, 1);
    auto testImage = std::make_shared<Drawing::Image>();
    auto testBlurImage = std::make_shared<Drawing::Image>();
    Drawing::Rect rectSrc;
    Drawing::Rect rectDst;
    auto testColorFilter = std::make_shared<Drawing::ColorFilter>();
    KawaseParameter testParam(rectSrc, rectDst, 1, testColorFilter, 0.f);
    EXPECT_TRUE(testFilter->ApplyBlur(testCanvas, testImage, testBlurImage, testParam));
}

/**
 * @tc.name: TestApplyBlur02
 * @tc.desc: Verify function ApplyBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterUnitTest, TestApplyBlur02, TestSize.Level1)
{
    auto testFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas testCanvas(1, 1);
    auto testBlurImage = std::make_shared<Drawing::Image>();
    Drawing::Rect rectSrc;
    Drawing::Rect rectDst;
    auto testColorFilter = std::make_shared<Drawing::ColorFilter>();
    KawaseParameter testParam(rectSrc, rectDst, 1, testColorFilter, 0.f);
    EXPECT_TRUE(!testFilter->ApplyBlur(testCanvas, nullptr, testBlurImage, testParam));
}

/**
 * @tc.name: TestAdjustRadiusAndScale01
 * @tc.desc: Verify function AdjustRadiusAndScale
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterUnitTest, TestAdjustRadiusAndScale01, TestSize.Level1)
{
    auto testFilter = std::make_shared<KawaseBlurFilter>();
    testFilter->ComputeRadiusAndScale(101);
    EXPECT_EQ(testFilter->blurScale_, 0.0625f);
    testFilter->ComputeRadiusAndScale(40);
    EXPECT_EQ(testFilter->blurScale_, 0.125f);
    testFilter->ComputeRadiusAndScale(20);
    EXPECT_EQ(testFilter->blurScale_, 0.25f);
    testFilter->ComputeRadiusAndScale(1);
    EXPECT_EQ(testFilter->blurScale_, 0.5F);
}

/**
 * @tc.name: TestGetDescription01
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterUnitTest, TestGetDescription01, TestSize.Level1)
{
    auto testFilter = std::make_shared<KawaseBlurFilter>();
    EXPECT_EQ(testFilter->GetDescription(), "blur radius is 0.000000");
}

/**
 * @tc.name: TestCheckInputImage
 * @tc.desc: Verify function CheckInputImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterUnitTest, TestCheckInputImage, TestSize.Level1)
{
    auto testFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas testCanvas(1, 1);
    auto testImage = std::make_shared<Drawing::Image>();
    auto checkImage = std::make_shared<Drawing::Image>();
    Drawing::Rect rectSrc;
    Drawing::Rect rectDst;
    auto testColorFilter = std::make_shared<Drawing::ColorFilter>();
    KawaseParameter testParam(rectSrc, rectDst, 1, testColorFilter, 0.f);
    testFilter->CheckInputImage(testCanvas, testImage, testParam, checkImage);
    EXPECT_EQ(testParam.src.GetBottom(), 0);
}

/**
 * @tc.name: TestExecutePingPongBlur
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterUnitTest, TestExecutePingPongBlur, TestSize.Level1)
{
    auto testFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas testCanvas;
    auto inputImage = std::make_shared<Drawing::Image>();
    Drawing::Rect rectSrc;
    Drawing::Rect rectDst;
    auto testColorFilter = std::make_shared<Drawing::ColorFilter>();
    KawaseParameter testParam(rectSrc, rectDst, 1, testColorFilter, 0.f);
    KawaseBlurFilter::BlurParams blurParams;
    testFilter->ExecutePingPongBlur(testCanvas, inputImage, testParam, blurParams);
    testFilter->SetupSimpleFilter();
    testFilter->ExecutePingPongBlur(testCanvas, inputImage, testParam, blurParams);
    EXPECT_EQ(testFilter->blurEffectAF_, nullptr);
}

} // namespace Rosen
} // namespace OHOS
