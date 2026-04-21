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

#include "adapter/skia/txt/paragraph_style.h"
#include "gtest/gtest.h"
#include "rosen_text/typography_style.h"
#include "service/skia_txt/convert.h"
#include "service/skia_txt/font_collection.h"
#include "service/skia_txt/typography_create.h"
#include "service/skia_txt/typography.h"

#include <codecvt>
#include <locale>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class OHDrawingOrphanCharOptimizationTest : public testing::Test {
public:
    std::shared_ptr<FontCollection> fontCollection_;
    std::unique_ptr<TypographyCreate> typographyCreate_;
    std::unique_ptr<Typography> typography_;

    void SetUp() override
    {
        fontCollection_ = FontCollection::Create();
        ASSERT_NE(fontCollection_, nullptr);
    }

    void TearDown() override
    {
        typography_.reset();
        typographyCreate_.reset();
        fontCollection_.reset();
    }

    std::u16string StrToU16Str(const std::string& str)
    {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(str);
    }
};

/*
 * @tc.name: OHDrawingOrphanCharOptimizationTest001
 * @tc.desc: Test TypographyStyle copy assignment with orphanCharOptimization = true and default value
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationTest002, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    EXPECT_EQ(typographyStyle.orphanCharOptimization, false);
    typographyStyle.orphanCharOptimization = true;

    TypographyStyle typographyStyleCopy = typographyStyle;
    EXPECT_EQ(typographyStyleCopy.orphanCharOptimization, true);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationTest002
 * @tc.desc: Test TypographyStyle copy constructor with orphanCharOptimization
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationTest003, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;

    TypographyStyle typographyStyleCopy(typographyStyle);
    EXPECT_EQ(typographyStyleCopy.orphanCharOptimization, true);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationTest003
 * @tc.desc: Test Convert TypographyStyle to ParagraphStyle with orphanCharOptimization = true
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationTest004, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;

    SPText::ParagraphStyle paragraphStyle = AdapterTxt::Convert(typographyStyle);
    EXPECT_EQ(paragraphStyle.orphanCharOptimization, true);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationTest004
 * @tc.desc: Test Convert TypographyStyle to ParagraphStyle with orphanCharOptimization = false
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationTest005, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = false;

    SPText::ParagraphStyle paragraphStyle = AdapterTxt::Convert(typographyStyle);
    EXPECT_EQ(paragraphStyle.orphanCharOptimization, false);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest001
 * @tc.desc: Test basic orphan char optimization disabled with Chinese text
 * @tc.type: FUNC
 *
 * Scenario: Test orphanCharOptimization = false with Chinese text "环境不错感觉还结结巴巴呵呵你好"
 *           Font size: 50, Max width: 700
 *           Target: Verify layout behavior when orphan optimization is disabled
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest001, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = false;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 2);

    std::vector<double> expectWidths{699.999267578125, 49.99993896484375};
    std::vector<TextRange> expectRanges{{0, 14}, {14, 15}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(expectWidths[i], lineMetric.width);
        EXPECT_EQ(expectRanges[i].start, lineMetric.startIndex);
        EXPECT_EQ(expectRanges[i].end, lineMetric.endIndex);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest002
 * @tc.desc: Test basic orphan char optimization enabled with Chinese text
 * @tc.type: FUNC
 *
 * Scenario: Test orphanCharOptimization = true with Chinese text "环境不错感觉还结结巴巴呵呵你好"
 *           Font size: 50, Max width: 700
 *           Target: Verify orphan char "好" triggers optimization, moving to second line with "你好"
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest002, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 2);

    std::vector<double> expectWidths{649.99932861328125, 99.9998779296875};
    std::vector<TextRange> expectRanges{{0, 13}, {13, 15}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(expectWidths[i], lineMetric.width);
        EXPECT_EQ(expectRanges[i].start, lineMetric.startIndex);
        EXPECT_EQ(expectRanges[i].end, lineMetric.endIndex);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest003
 * @tc.desc: Test orphan char line feature disabled - verify "世界abc" effect
 * @tc.type: FUNC
 *
 * Scenario: Test orphanCharOptimization = false with "环境不错感觉还结结巴巴呵呵你好，世界abc"
 *           Font size: 50, Max width: 700
 *           Target: orphan char is "好" in "呵呵你好", verify behavior when disabled
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest003, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = false;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好，世界abc");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 2);

    std::vector<double> expectWidths{699.999267578125, 282.2996826171875};
    std::vector<TextRange> expectRanges{{0, 14}, {14, 21}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(expectWidths[i], lineMetric.width);
        EXPECT_EQ(expectRanges[i].start, lineMetric.startIndex);
        EXPECT_EQ(expectRanges[i].end, lineMetric.endIndex);
        EXPECT_LE(lineMetric.width, maxWidth);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest004
 * @tc.desc: Test orphan char line feature enabled - verify "世界abc" does not affect orphan char "好"
 * @tc.type: FUNC
 *
 * Scenario: Test orphanCharOptimization = true with "环境不错感觉还结结巴巴呵呵你好，世界abc"
 *           Font size: 50, Max width: 700
 *           Target: orphan char is "好" in "呵呵你", verify "世界abc" does not affect orphan trigger
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest004, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好，世界abc");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 2);

    std::vector<double> expectWidths{649.99932861328125, 332.29962158203125};
    std::vector<TextRange> expectRanges{{0, 13}, {13, 21}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(expectWidths[i], lineMetric.width);
        EXPECT_EQ(expectRanges[i].start, lineMetric.startIndex);
        EXPECT_EQ(expectRanges[i].end, lineMetric.endIndex);
        EXPECT_LE(lineMetric.width, maxWidth);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest005
 * @tc.desc: Test orphan char paragraph feature disabled - verify each paragraph behavior
 * @tc.type: FUNC
 *
 * Scenario: Test orphanCharOptimization = false with "环境不错感觉还结结巴巴呵呵你好，\n环境不错感觉还结结巴巴呵呵可能吧。。。。。"
 *           Font size: 50, Max width: 700
 *           Target: Verify each paragraph's behavior when orphan optimization is disabled
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest005, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = false;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好，\n环境不错感觉还结结巴巴呵呵可能吧。。。。。");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 4);

    std::vector<double> expectWidths{699.999267578125, 99.9998779296875, 699.999267578125, 349.9996337890625};
    std::vector<TextRange> expectRanges{{0, 14}, {14, 16}, {17, 31}, {31, 38}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(expectWidths[i], lineMetric.width);
        EXPECT_EQ(expectRanges[i].start, lineMetric.startIndex);
        EXPECT_EQ(expectRanges[i].end, lineMetric.endIndex);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest006
 * @tc.desc: Test orphan char paragraph feature enabled - verify each paragraph triggers orphan
 * @tc.type: FUNC
 *
 * Scenario: Test orphanCharOptimization = true with "环境不错感觉还结结巴巴呵呵你好，\n环境不错感觉还结结巴巴呵呵可能吧。。。。。"
 *           Font size: 50, Max width: 700
 *           Target: After \n split, each paragraph should trigger orphan char optimization
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest006, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好，\n环境不错感觉还结结巴巴呵呵可能吧。。。。。");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 4);

    std::vector<double> expectWidths{649.99932861328125, 149.99981689453125, 649.99932861328125, 399.99957275390625};
    std::vector<TextRange> expectRanges{{0, 13}, {13, 16}, {17, 30}, {30, 38}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(expectWidths[i], lineMetric.width);
        EXPECT_EQ(expectRanges[i].start, lineMetric.startIndex);
        EXPECT_EQ(expectRanges[i].end, lineMetric.endIndex);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest007
 * @tc.desc: Test orphan char with different font sizes - verify line height correction
 * @tc.type: FUNC
 *
 * Scenario: Test with mixed font sizes: "环境不错感觉还结结巴巴呵呵" + "你"(size 200) + "好"(size 50)
 *           Font size: 50 (base), Max width: 897
 *           Target: "你" is large, after orphan trigger moves to second line, first line height should decrease
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest007, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle styleFontSizeFifty;
    styleFontSizeFifty.fontSize = 50;
    TextStyle styleFontSizeTwoHundred;
    styleFontSizeTwoHundred.fontSize = 200;
    std::u16string baseText = StrToU16Str("环境不错感觉还结结巴巴呵呵");
    typographyCreate_->PushStyle(styleFontSizeFifty);
    typographyCreate_->AppendText(baseText);
    typographyCreate_->PushStyle(styleFontSizeTwoHundred);
    typographyCreate_->AppendText(StrToU16Str("你"));
    typographyCreate_->PushStyle(styleFontSizeFifty);
    typographyCreate_->AppendText(StrToU16Str("好"));
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 897.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 2);

    std::vector<double> expectWidths{649.99932861328125, 250};
    std::vector<double> expectHeights{59, 234};
    std::vector<TextRange> expectRanges{{0, 13}, {13, 15}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(expectWidths[i], lineMetric.width);
        EXPECT_DOUBLE_EQ(expectHeights[i], lineMetric.height);
        EXPECT_EQ(expectRanges[i].start, lineMetric.startIndex);
        EXPECT_EQ(expectRanges[i].end, lineMetric.endIndex);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest008
 * @tc.desc: Test orphan char with locale zh-Hans - verify orphan triggers
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错感觉还结结巴巴呵呵你好" with locale zh-Hans
 *           Font size: 50, Max width: 700
 *           Target: Chinese locale should trigger orphan optimization
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest008, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    TextStyle style;
    style.fontSize = 50.0;
    style.locale = "zh-Hans";

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好");
    typographyCreate_->PushStyle(style);
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 2);

    std::vector<double> expectWidths{649.99932861328125, 99.9998779296875};
    std::vector<TextRange> expectRanges{{0, 13}, {13, 15}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(expectWidths[i], lineMetric.width);
        EXPECT_EQ(expectRanges[i].start, lineMetric.startIndex);
        EXPECT_EQ(expectRanges[i].end, lineMetric.endIndex);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest009
 * @tc.desc: Test orphan char with locale en - verify orphan does not trigger
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错感觉还结结巴巴呵呵你好" with locale en
 *           Font size: 50, Max width: 700
 *           Target: English locale should NOT trigger orphan optimization for Chinese text
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest009, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    TextStyle style;
    style.fontSize = 50.0;
    style.locale = "en";

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好");
    typographyCreate_->PushStyle(style);
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 2);

    std::vector<double> expectWidths{699.999267578125, 49.99993896484375};
    std::vector<TextRange> expectRanges{{0, 14}, {14, 15}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(expectWidths[i], lineMetric.width);
        EXPECT_EQ(expectRanges[i].start, lineMetric.startIndex);
        EXPECT_EQ(expectRanges[i].end, lineMetric.endIndex);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest010
 * @tc.desc: Test orphan char optimization with empty text
 * @tc.type: FUNC
 *
 * Scenario: Test with empty string, verify it doesn't crash
 *           Font size: 50, Max width: 700
 *           Target: Empty text should handle gracefully without triggering orphan
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest010, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 0);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest011
 * @tc.desc: Test orphan char optimization with single character
 * @tc.type: FUNC
 *
 * Scenario: Test with single character "你"
 *           Font size: 50, Max width: 700
 *           Target: Single character should not trigger orphan (no previous line to adjust)
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest011, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("你");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 1);

    LineMetrics lineMetric;
    typography_->GetLineMetricsAt(0, &lineMetric);
    EXPECT_EQ(lineMetric.startIndex, 0);
    EXPECT_EQ(lineMetric.endIndex, 1);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest012
 * @tc.desc: Test orphan char optimization with very wide text
 * @tc.type: FUNC
 *
 * Scenario: Test with "你好" but very wide maxWidth (2000)
 *           Font size: 50, Max width: 2000
 *           Target: All text fits in one line, orphan should not trigger
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest012, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("你好");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 2000.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 1);

    LineMetrics lineMetric;
    typography_->GetLineMetricsAt(0, &lineMetric);
    EXPECT_EQ(lineMetric.startIndex, 0);
    EXPECT_EQ(lineMetric.endIndex, 2);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest013
 * @tc.desc: Test orphan char optimization with very narrow width
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错" but very narrow maxWidth (50)
 *           Font size: 50, Max width: 50
 *           Target: Each character becomes separate line, verify behavior
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest013, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 50.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    // Each character on separate line
    EXPECT_EQ(lineCount, 4);

    size_t expectLineTextRangeSize = 1;
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_LE(lineMetric.width, maxWidth);
        EXPECT_EQ(lineMetric.endIndex - lineMetric.startIndex, expectLineTextRangeSize);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest014
 * @tc.desc: Test orphan char optimization with Emoji
 * @tc.type: FUNC
 *
 * Scenario: Test with "平衡行测试好吃的苹果😂😊😗🥲"
 *           Font size: 50, Max width: 700
 *           Target: Emoji should not interfere with orphan char detection
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest014, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("平衡行测试好吃的苹果😂😊😗🥲");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 2);

    std::vector<double> expectWidths{687.17138671875, 62.390625};
    // Emoji takes 2 UTF-16 code units
    std::vector<TextRange> expectRanges{{0, 16}, {16, 18}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(lineMetric.width, expectWidths[i]);
        EXPECT_EQ(lineMetric.startIndex, expectRanges[i].start);
        EXPECT_EQ(lineMetric.endIndex, expectRanges[i].end);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest015
 * @tc.desc: Test orphan char optimization with justify alignment
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错感觉还结结巴巴呵呵你好"
 *           Font size: 50, textAlign: JUSTIFY, Max width: 700
 *           Target: Verify text alignment doesn't break orphan optimization
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest015, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;
    typographyStyle.textAlign = TextAlign::JUSTIFY;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 2);

    std::vector<double> expectWidths{700, 99.9998779296875};
    std::vector<TextRange> expectRanges{{0, 13}, {13, 15}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(lineMetric.width, expectWidths[i]);
        EXPECT_EQ(lineMetric.startIndex, expectRanges[i].start);
        EXPECT_EQ(lineMetric.endIndex, expectRanges[i].end);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest016
 * @tc.desc: Test orphan char optimization with letterSpacing
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错感觉还结结巴巴呵呵你好"
 *           Font size: 50, letterSpacing: 3.0, Max width: 700
 *           Target: Verify letterSpacing doesn't break orphan optimization
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest016, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle style;
    style.fontSize = 50.0;
    style.letterSpacing = 3.0;

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好");
    typographyCreate_->PushStyle(style);
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    std::vector<LineMetrics> lineMetrics = typography_->GetLineMetrics();
    size_t lineCount = lineMetrics.size();
    EXPECT_EQ(lineCount, 2);

    std::vector<double> expectWidths{688.99932861328125, 105.9998779296875};
    std::vector<TextRange> expectRanges{{0, 13}, {13, 15}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(lineMetric.width, expectWidths[i]);
        EXPECT_EQ(lineMetric.startIndex, expectRanges[i].start);
        EXPECT_EQ(lineMetric.endIndex, expectRanges[i].end);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest017
 * @tc.desc: Test orphan char optimization with -- line
 * @tc.type: FUNC
 *
 * Scenario: Test with "人工智能技术深圳——香港"
 *           Font size: 50, Max width: 585.0
 *           Target: Verify -- scene
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest017, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.orphanCharOptimization = true;
    typographyStyle.fontSize = 50.0;
    typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    TextStyle style;
    style.fontSize = 50.0;
    std::u16string text = StrToU16Str("人工智能技术深圳——香港");
    typographyCreate_->PushStyle(style);
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 585.0;
    typography_->Layout(maxWidth);

    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 2);
    std::vector<double> expectWidths{499.99957275390625, 99.999908447265625};
    std::vector<TextRange> expectRanges{{0, 10}, {10, 12}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(lineMetric.width, expectWidths[i]);
        EXPECT_EQ(lineMetric.startIndex, expectRanges[i].start);
        EXPECT_EQ(lineMetric.endIndex, expectRanges[i].end);
    }
}
} // namespace Rosen
} // namespace OHOS