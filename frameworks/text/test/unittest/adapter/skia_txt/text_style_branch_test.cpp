/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "paragraph_style.h"
#include "platform.h"
#include "text_style.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace SPText {

class TextStyleBranchTest : public testing::Test {};

/**
 * @tc.name: TextShadowHasShadow001
 * @tc.desc: Test HasShadow with various offset and blurSigma combinations
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleBranchTest, TextShadowHasShadow001, TestSize.Level0)
{
    // Case 1: offset is zero and blurSigma <= 0.5 -> HasShadow returns false
    TextShadow shadow1;
    shadow1.offset = SkPoint::Make(0, 0);
    shadow1.blurSigma = 0.0;
    EXPECT_FALSE(shadow1.HasShadow());

    // Case 2: offset is zero and blurSigma == 0.5 -> HasShadow returns false
    TextShadow shadow2;
    shadow2.offset = SkPoint::Make(0, 0);
    shadow2.blurSigma = 0.5;
    EXPECT_FALSE(shadow2.HasShadow());

    // Case 3: offset is non-zero -> HasShadow returns true
    TextShadow shadow3;
    shadow3.offset = SkPoint::Make(1, 0);
    shadow3.blurSigma = 0.0;
    EXPECT_TRUE(shadow3.HasShadow());

    // Case 4: blurSigma > 0.5 -> HasShadow returns true
    TextShadow shadow4;
    shadow4.offset = SkPoint::Make(0, 0);
    shadow4.blurSigma = 1.0;
    EXPECT_TRUE(shadow4.HasShadow());
}

/**
 * @tc.name: FontFeaturesGetFeatureSettings001
 * @tc.desc: Test GetFeatureSettings with various feature combinations
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleBranchTest, FontFeaturesGetFeatureSettings001, TestSize.Level0)
{
    // Case 1: no features -> empty string
    FontFeatures features1;
    EXPECT_EQ(features1.GetFeatureSettings(), "");

    // Case 2: single feature -> "tag=value"
    FontFeatures features2;
    features2.SetFeature("liga", 1);
    EXPECT_EQ(features2.GetFeatureSettings(), "liga=1");

    // Case 3: multiple features -> "tag1=value1,tag2=value2"
    FontFeatures features3;
    features3.SetFeature("liga", 1);
    features3.SetFeature("kern", 0);
    std::string result = features3.GetFeatureSettings();
    EXPECT_TRUE(result.find("liga=1") != std::string::npos);
    EXPECT_TRUE(result.find("kern=0") != std::string::npos);
}

class ParagraphStyleBranchTest : public testing::Test {};

/**
 * @tc.name: ParagraphStyleGetEquivalentAlign001
 * @tc.desc: Test GetEquivalentAlign with various alignment and direction combinations
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphStyleBranchTest, ParagraphStyleGetEquivalentAlign001, TestSize.Level0)
{
    // Case 1: START + LTR -> LEFT
    ParagraphStyle style1;
    style1.textAlign = TextAlign::START;
    style1.textDirection = TextDirection::LTR;
    EXPECT_EQ(style1.GetEquivalentAlign(), TextAlign::LEFT);

    // Case 2: START + RTL -> RIGHT
    ParagraphStyle style2;
    style2.textAlign = TextAlign::START;
    style2.textDirection = TextDirection::RTL;
    EXPECT_EQ(style2.GetEquivalentAlign(), TextAlign::RIGHT);

    // Case 3: END + LTR -> RIGHT
    ParagraphStyle style3;
    style3.textAlign = TextAlign::END;
    style3.textDirection = TextDirection::LTR;
    EXPECT_EQ(style3.GetEquivalentAlign(), TextAlign::RIGHT);

    // Case 4: END + RTL -> LEFT
    ParagraphStyle style4;
    style4.textAlign = TextAlign::END;
    style4.textDirection = TextDirection::RTL;
    EXPECT_EQ(style4.GetEquivalentAlign(), TextAlign::LEFT);

    // Case 5: CENTER -> CENTER
    ParagraphStyle style5;
    style5.textAlign = TextAlign::CENTER;
    EXPECT_EQ(style5.GetEquivalentAlign(), TextAlign::CENTER);

    // Case 6: LEFT -> LEFT
    ParagraphStyle style6;
    style6.textAlign = TextAlign::LEFT;
    EXPECT_EQ(style6.GetEquivalentAlign(), TextAlign::LEFT);

    // Case 7: RIGHT -> RIGHT
    ParagraphStyle style7;
    style7.textAlign = TextAlign::RIGHT;
    EXPECT_EQ(style7.GetEquivalentAlign(), TextAlign::RIGHT);
}

/**
 * @tc.name: ParagraphStyleConvertToTextStyle001
 * @tc.desc: Test ConvertToTextStyle with positive and negative fontSize
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphStyleBranchTest, ParagraphStyleConvertToTextStyle001, TestSize.Level0)
{
    // Case 1: fontSize >= 0 -> uses provided value
    ParagraphStyle style1;
    style1.fontSize = 16.0;
    TextStyle result1 = style1.ConvertToTextStyle();
    EXPECT_EQ(result1.fontSize, 16.0);

    // Case 2: fontSize < 0 -> uses default (14.0)
    ParagraphStyle style2;
    style2.fontSize = -1.0;
    TextStyle result2 = style2.ConvertToTextStyle();
    EXPECT_EQ(result2.fontSize, 14.0);
}

class PlatformBranchTest : public testing::Test {};

/**
 * @tc.name: DefaultFamilyNameMgr001
 * @tc.desc: Test DefaultFamilyNameMgr singleton, theme families, and font family generation
 * @tc.type: FUNC
 */
HWTEST_F(PlatformBranchTest, DefaultFamilyNameMgr001, TestSize.Level0)
{
    auto& mgr = DefaultFamilyNameMgr::GetInstance();

    // GetInstance returns valid singleton
    auto families = mgr.GetDefaultFontFamilies();
    EXPECT_GT(families.size(), 0u);

    // GetThemeFontFamilies returns empty initially
    auto themeFamilies = mgr.GetThemeFontFamilies();
    EXPECT_EQ(themeFamilies.size(), 0u);

    // ModifyThemeFontFamilies adds theme families
    mgr.ModifyThemeFontFamilies(3);
    auto modifiedFamilies = mgr.GetThemeFontFamilies();
    EXPECT_EQ(modifiedFamilies.size(), 3u);

    // GenerateThemeFamilyName with index 0
    std::string name0 = DefaultFamilyNameMgr::GenerateThemeFamilyName(0);
    EXPECT_EQ(name0, OHOS_THEME_FONT);

    // GenerateThemeFamilyName with index > 0
    std::string name1 = DefaultFamilyNameMgr::GenerateThemeFamilyName(1);
    EXPECT_EQ(name1, std::string(OHOS_THEME_FONT) + "1");

    // IsThemeFontFamily with matching name
    EXPECT_TRUE(DefaultFamilyNameMgr::IsThemeFontFamily("ohosthemefont"));

    // IsThemeFontFamily with non-matching name
    EXPECT_FALSE(DefaultFamilyNameMgr::IsThemeFontFamily("sans-serif"));

    // IsThemeFontFamily with uppercase prefix
    EXPECT_TRUE(DefaultFamilyNameMgr::IsThemeFontFamily("OhosThemeFont2"));

    // GetDefaultFontFamilies returns combined families after modification
    auto combinedFamilies = mgr.GetDefaultFontFamilies();
    EXPECT_GE(combinedFamilies.size(), 2u);
    DefaultFamilyNameMgr::GetInstance().ModifyThemeFontFamilies(0);
}

} // namespace SPText
} // namespace Rosen
} // namespace OHOS
