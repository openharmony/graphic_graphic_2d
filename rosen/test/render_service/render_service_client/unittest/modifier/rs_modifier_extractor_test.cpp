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
#include "modifier/rs_modifier_extractor.h"
#include "ui/rs_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint32_t SIZE_RSCOLOR = 255;
class RSModifierExtractorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierExtractorTest::SetUpTestCase() {}
void RSModifierExtractorTest::TearDownTestCase() {}
void RSModifierExtractorTest::SetUp() {}
void RSModifierExtractorTest::TearDown() {}

/**
 * @tc.name: NodeTest001
 * @tc.desc: Node Test
 * @tc.type: FUNC
 * @tc.require:issueI9N22N
 */
HWTEST_F(RSModifierExtractorTest, NodeTest001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    // Node is nullptr
    ASSERT_EQ(extractor.GetPivotZ(), 0.f);

    RSColor valTest1 { 0, 0, 0, 0 };

    ASSERT_EQ(extractor.GetBloom(), 0.f);

    // Node is not nullptr
    ASSERT_EQ(extractor.GetBackgroundBlurRadius(), 0.f);

    ASSERT_EQ(extractor.GetBackgroundBlurSaturation(), 0.f);

    ASSERT_EQ(extractor.GetDynamicDimDegree(), 0.f);

    ASSERT_EQ(extractor.GetBackgroundBlurBrightness(), 0.f);

    ASSERT_EQ(extractor.GetBackgroundBlurMaskColor(), valTest1);

    ASSERT_EQ(extractor.GetBackgroundBlurColorMode(), BLUR_COLOR_MODE::DEFAULT);

    ASSERT_EQ(extractor.GetBackgroundBlurRadiusX(), 0.f);
    ASSERT_EQ(extractor.GetBackgroundBlurRadiusY(), 0.f);
    ASSERT_EQ(extractor.GetForegroundBlurRadius(), 0.f);
    ASSERT_EQ(extractor.GetForegroundBlurSaturation(), 0.f);
    ASSERT_EQ(extractor.GetForegroundBlurBrightness(), 0.f);

    ASSERT_EQ(extractor.GetForegroundBlurMaskColor(), valTest1);
    ASSERT_EQ(extractor.GetForegroundBlurColorMode(), BLUR_COLOR_MODE::DEFAULT);
    ASSERT_EQ(extractor.GetForegroundBlurRadiusX(), 0.f);
    ASSERT_EQ(extractor.GetForegroundBlurRadiusY(), 0.f);
    RSColor valTest2 { SIZE_RSCOLOR, SIZE_RSCOLOR, SIZE_RSCOLOR };
    ASSERT_EQ(extractor.GetLightColor(), valTest2);
}

/**
 * @tc.name: GetBorderDashWidth001
 * @tc.desc: test results of GetBorderDashWidth
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierExtractorTest, GetBorderDashWidth001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    Vector4f vector = extractor.GetBorderDashWidth();
    EXPECT_EQ(vector, Vector4f(0.f));
}

/**
 * @tc.name: GetBorderDashGap001
 * @tc.desc: test results of GetBorderDashGap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierExtractorTest, GetBorderDashGap001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    Vector4f vector = extractor.GetBorderDashGap();
    EXPECT_EQ(vector, Vector4f(0.f));
}

/**
 * @tc.name: GetOutlineDashWidth001
 * @tc.desc: test results of GetOutlineDashWidth
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierExtractorTest, GetOutlineDashWidth001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    Vector4f vector = extractor.GetOutlineDashWidth();
    EXPECT_EQ(vector, Vector4f(0.f));
}

/**
 * @tc.name: GetOutlineDashGap001
 * @tc.desc: test results of GetOutlineDashWidth
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierExtractorTest, GetOutlineDashGap001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    Vector4f vector = extractor.GetOutlineDashWidth();
    EXPECT_EQ(vector, Vector4f(0.f));
}

/**
 * @tc.name: GetOutlineColor001
 * @tc.desc: test results of GetOutlineColor
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetOutlineColor001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    Vector4<Color> vector = extractor.GetOutlineColor();
    EXPECT_TRUE(vector == Vector4<Color>(RgbPalette::Transparent()));
}

/**
 * @tc.name: GetOutlineWidth001
 * @tc.desc: test results of GetOutlineWidth
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetOutlineWidth001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    Vector4f vector = extractor.GetOutlineWidth();
    EXPECT_TRUE(vector == Vector4f(0.f));
}

/**
 * @tc.name: GetOutlineStyle001
 * @tc.desc: test results of GetOutlineStyle
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetOutlineStyle001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    Vector4<uint32_t> vector = extractor.GetOutlineStyle();
    EXPECT_TRUE(vector == Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE)));
}

/**
 * @tc.name: GetOutlineRadius001
 * @tc.desc: test results of GetOutlineRadius
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetOutlineRadius001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    Vector4f vector = extractor.GetOutlineRadius();
    EXPECT_TRUE(vector == Vector4f(0.f));
}

/**
 * @tc.name: GetHDRBrightnessFactor001
 * @tc.desc: test results of GetHDRBrightnessFactor
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetHDRBrightnessFactor001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    float hdrBrightnessFactor = extractor.GetHDRBrightnessFactor();
    EXPECT_TRUE(hdrBrightnessFactor == 1.0f);
}

/**
 * @tc.name: GetForegroundEffectRadius001
 * @tc.desc: test results of GetForegroundEffectRadius
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetForegroundEffectRadius001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    float radius = extractor.GetForegroundEffectRadius();
    EXPECT_TRUE(radius == 0.f);
}

/**
 * @tc.name: GetShadowMask001
 * @tc.desc: test results of GetShadowMask
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetShadowMask001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    bool res = extractor.GetShadowMask();
    EXPECT_FALSE(res);
}

/**
 * @tc.name: GetShadowIsFilled001
 * @tc.desc: test results of GetShadowIsFilled
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetShadowIsFilled001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    bool res = extractor.GetShadowIsFilled();
    EXPECT_FALSE(res);
}

/**
 * @tc.name: GetShadowColorStrategy001
 * @tc.desc: test results of GetShadowColorStrategy
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetShadowColorStrategy001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    int strategy = extractor.GetShadowColorStrategy();
    EXPECT_TRUE(strategy == 0);
}

/**
 * @tc.name: GetSpherizeDegree001
 * @tc.desc: test results of GetSpherizeDegree
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetSpherizeDegree001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    float degree = extractor.GetSpherizeDegree();
    EXPECT_TRUE(degree == 0.f);
}

/**
 * @tc.name: GetAttractionFraction001
 * @tc.desc: test results of GetAttractionFraction
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetAttractionFraction001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    float fraction = extractor.GetAttractionFractionValue();
    EXPECT_TRUE(fraction == 0.f);
}

/**
 * @tc.name: GetAttractionDstPointValue001
 * @tc.desc: test results of GetAttractionDstPointValue
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetAttractionDstPointValue001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    Vector2f attractionDstPoint = extractor.GetAttractionDstPointValue();
    EXPECT_TRUE(attractionDstPoint.x_ == 0.f);
}

/**
 * @tc.name: GetLightUpEffectDegree001
 * @tc.desc: test results of GetLightUpEffectDegree
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetLightUpEffectDegree001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    float degree = extractor.GetLightUpEffectDegree();
    EXPECT_TRUE(degree == 0.f);
}

/**
 * @tc.name: GetDynamicDimDegree001
 * @tc.desc: test results of GetDynamicDimDegree
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetDynamicDimDegree001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    float degree = extractor.GetDynamicDimDegree();
    EXPECT_TRUE(degree == 0.f);
}

/**
 * @tc.name: GetLightIntensity001
 * @tc.desc: test results of GetLightIntensity
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetLightIntensity001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    float intensity = extractor.GetLightIntensity();
    EXPECT_TRUE(intensity == 0.f);
}

/**
 * @tc.name: GetLightPosition001
 * @tc.desc: test results of GetLightPosition
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetLightPosition001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    Vector4f position = extractor.GetLightPosition();
    EXPECT_TRUE(position == Vector4f(0.f));
}

/**
 * @tc.name: GetIlluminatedBorderWidth001
 * @tc.desc: test results of GetIlluminatedBorderWidth
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetIlluminatedBorderWidth001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    float borderWidth = extractor.GetIlluminatedBorderWidth();
    EXPECT_TRUE(borderWidth == 0.f);
}

/**
 * @tc.name: GetIlluminatedType001
 * @tc.desc: test results of GetIlluminatedType
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierExtractorTest, GetIlluminatedType001, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    int illuminatedType = extractor.GetIlluminatedType();
    EXPECT_TRUE(illuminatedType == 0);
}

/**
 * @tc.name: GetScaleZ
 * @tc.desc: test results of GetScaleZ
 * @tc.type: FUNC
 * @tc.require: issueIAP7XJ
 */
HWTEST_F(RSModifierExtractorTest, GetScaleZ, TestSize.Level1)
{
    NodeId id = 1;
    auto extractor = std::make_shared<RSModifierExtractor>(id);
    EXPECT_EQ(extractor->GetScaleZ(), 1.0f);
}

/**
 * @tc.name: GetSkew
 * @tc.desc: test results of GetSkew
 * @tc.type: FUNC
 * @tc.require: issueIAP7XJ
 */
HWTEST_F(RSModifierExtractorTest, GetSkew, TestSize.Level1)
{
    NodeId id = 1;
    auto extractor = std::make_shared<RSModifierExtractor>(id);
    EXPECT_EQ(extractor->GetSkew(), Vector3f(0.f, 0.f, 0.f));
}

/**
 * @tc.name: GetPersp
 * @tc.desc: test results of GetPersp
 * @tc.type: FUNC
 * @tc.require: issueIAP7XJ
 */
HWTEST_F(RSModifierExtractorTest, GetPersp, TestSize.Level1)
{
    NodeId id = 1;
    auto extractor = std::make_shared<RSModifierExtractor>(id);
    EXPECT_EQ(extractor->GetPersp(), Vector4f(0.f, 0.f, 0.f, 1.f));
}

/**
 * @tc.name: GetHDRUIBrightnessTest
 * @tc.desc: test results of GetHDRUIBrightness
 * @tc.type: FUNC
 * @tc.require: issueIAP7XJ
 */
HWTEST_F(RSModifierExtractorTest, GetHDRUIBrightnessTest, TestSize.Level1)
{
    NodeId id = 1;
    RSModifierExtractor extractor(id);
    float hdrUIBrightness = extractor.GetHDRUIBrightness();
    EXPECT_TRUE(hdrUIBrightness == 1.0f);
}

/**
 * @tc.name: DumpTest
 * @tc.desc: test results of DumpTest
 * @tc.type: FUNC
 * @tc.require: issueIAP7XJ
 */
HWTEST_F(RSModifierExtractorTest, DumpTest, TestSize.Level1)
{
    NodeId id = 1;
    auto extractor = std::make_shared<RSModifierExtractor>(id);
    std::string dumpInfo = extractor->Dump();
    std::string expected = "Bounds[0.0 0.0 0.0 0.0] Frame[0.0 0.0 0.0 0.0]";
    EXPECT_EQ(dumpInfo, expected);
}

/**
 * @tc.name: GetQuaternionTest
 * @tc.desc: test results of GetQuaternion
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierExtractorTest, GetQuaternionTest, TestSize.Level1)
{
    NodeId id = 1;
    auto extractor = std::make_shared<RSModifierExtractor>(id);
    Quaternion quaternion = extractor->GetQuaternion();
    EXPECT_TRUE(quaternion.IsIdentity());
}

/**
 * @tc.name: GetBackgroundShaderTest
 * @tc.desc: test results of GetBackgroundShader
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierExtractorTest, GetBackgroundShaderTest, TestSize.Level1)
{
    NodeId id = 1;
    auto extractor = std::make_shared<RSModifierExtractor>(id);
    EXPECT_EQ(extractor->GetBackgroundShader(), nullptr);
}

/**
 * @tc.name: GetBgImageTest
 * @tc.desc: test results of GetBgImage
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierExtractorTest, GetBgImageTest, TestSize.Level1)
{
    NodeId id = 1;
    auto extractor = std::make_shared<RSModifierExtractor>(id);
    EXPECT_EQ(extractor->GetBgImage(), nullptr);
}

/**
 * @tc.name: GetShadowPathTest
 * @tc.desc: test results of GetShadowPath
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierExtractorTest, GetShadowPathTest, TestSize.Level1)
{
    NodeId id = 1;
    auto extractor = std::make_shared<RSModifierExtractor>(id);
    EXPECT_EQ(extractor->GetShadowPath(), nullptr);
}

/**
 * @tc.name: GetMaskTest
 * @tc.desc: test results of GetMask
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierExtractorTest, GetMaskTest, TestSize.Level1)
{
    NodeId id = 1;
    auto extractor = std::make_shared<RSModifierExtractor>(id);
    EXPECT_EQ(extractor->GetMask(), nullptr);
}
} // namespace OHOS::Rosen