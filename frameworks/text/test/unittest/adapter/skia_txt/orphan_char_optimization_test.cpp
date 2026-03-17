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
 * @tc.desc: Test basic orphan char optimization with Chinese text
 * @tc.type: FUNC
 *
 * Scenario: Test orphanCharOptimization with Chinese text "环境不错感觉还结结巴巴呵呵你好"
 *           Font size: 50, Max width: 700
 *           Compare line widths with orphanCharOptimization enabled/disabled
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest001, TestSize.Level0)
{
    // Test with orphanCharOptimization = false (baseline)
    TypographyStyle typographyStyleDisabled;
    typographyStyleDisabled.orphanCharOptimization = false;
    typographyStyleDisabled.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyleDisabled, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    // Get line metrics with disabled
    std::vector<LineMetrics> lineMetricsDisabled = typography_->GetLineMetrics();
    size_t lineCountDisabled = lineMetricsDisabled.size();
    EXPECT_EQ(lineCountDisabled, 2);

    // Get each line's width
    std::vector<double> lineWidthsDisabled;
    std::vector<TextRange> lineTextRangeDisable;
    for (size_t i = 0; i < lineCountDisabled; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        lineWidthsDisabled.push_back(lineMetric.width);
        lineTextRangeDisable.push_back({lineMetric.startIndex, lineMetric.endIndex});
    }

    // Reset for next test
    typography_.reset();
    typographyCreate_.reset();

    // Test with orphanCharOptimization = true
    TypographyStyle typographyStyleEnabled;
    typographyStyleEnabled.orphanCharOptimization = true;
    typographyStyleEnabled.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyleEnabled, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(maxWidth);

    // Get line metrics with enabled
    std::vector<LineMetrics> lineMetricsEnabled = typography_->GetLineMetrics();
    size_t lineCountEnabled = lineMetricsEnabled.size();
    EXPECT_EQ(lineCountEnabled, 2);

    // Get each line's width
    std::vector<double> lineWidthsEnabled;
    std::vector<TextRange> lineTextRangeEnabled;
    for (size_t i = 0; i < lineCountEnabled; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        lineWidthsEnabled.push_back(lineMetric.width);
        lineTextRangeEnabled.push_back({lineMetric.startIndex, lineMetric.endIndex});
    }

    EXPECT_DOUBLE_EQ(lineWidthsEnabled[0], 649.99932861328125);
    EXPECT_DOUBLE_EQ(lineWidthsEnabled[1], 99.9998779296875);
    EXPECT_NE(lineWidthsEnabled[0], lineWidthsDisabled[0]);
    EXPECT_NE(lineWidthsEnabled[1], lineWidthsDisabled[1]);
    EXPECT_EQ(lineTextRangeDisable[0].start, 0);
    EXPECT_EQ(lineTextRangeDisable[0].end, 14);
    EXPECT_EQ(lineTextRangeDisable[1].start, 14);
    EXPECT_EQ(lineTextRangeDisable[1].end, 15);
    EXPECT_EQ(lineTextRangeEnabled[0].start, 0);
    EXPECT_EQ(lineTextRangeEnabled[0].end, 13);
    EXPECT_EQ(lineTextRangeEnabled[1].start, 13);
    EXPECT_EQ(lineTextRangeEnabled[1].end, 15);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest002
 * @tc.desc: Test orphan char line feature - verify "世界abc" does not affect orphan char "好"
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错感觉还结结巴巴呵呵你好，世界abc"
 *           Font size: 50, Max width: 700
 *           Target: orphan char is "好" in "呵呵你", verify "世界abc" does not affect orphan trigger
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest002, TestSize.Level0)
{
    // Test with orphanCharOptimization = false (baseline)
    TypographyStyle typographyStyleDisabled;
    typographyStyleDisabled.orphanCharOptimization = false;
    typographyStyleDisabled.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyleDisabled, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好，世界abc");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    // Get line metrics with disabled
    std::vector<LineMetrics> lineMetricsDisabled = typography_->GetLineMetrics();
    size_t lineCountDisabled = lineMetricsDisabled.size();
    EXPECT_EQ(lineCountDisabled, 2);

    std::vector<double> lineWidthsDisabled;
    std::vector<TextRange> lineTextRangeDisabled;
    for (size_t i = 0; i < lineCountDisabled; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        lineWidthsDisabled.push_back(lineMetric.width);
        lineTextRangeDisabled.push_back({lineMetric.startIndex, lineMetric.endIndex});
        EXPECT_LE(lineMetric.width, maxWidth);
    }

    // Reset for next test
    typography_.reset();
    typographyCreate_.reset();

    // Test with orphanCharOptimization = true
    TypographyStyle typographyStyleEnabled;
    typographyStyleEnabled.orphanCharOptimization = true;
    typographyStyleEnabled.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyleEnabled, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(maxWidth);

    // Get line metrics with enabled
    std::vector<LineMetrics> lineMetricsEnabled = typography_->GetLineMetrics();
    size_t lineCountEnabled = lineMetricsEnabled.size();
    EXPECT_EQ(lineCountEnabled, 2);

    std::vector<double> lineWidthsEnabled;
    std::vector<TextRange> lineTextRangeEnabled;
    for (size_t i = 0; i < lineCountEnabled; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        lineWidthsEnabled.push_back(lineMetric.width);
        lineTextRangeEnabled.push_back({lineMetric.startIndex, lineMetric.endIndex});
        EXPECT_LE(lineMetric.width, maxWidth);
    }

    EXPECT_DOUBLE_EQ(lineWidthsEnabled[0], 649.99932861328125);
    EXPECT_DOUBLE_EQ(lineWidthsEnabled[1], 332.29962158203125);
    EXPECT_NE(lineWidthsEnabled[0], lineWidthsDisabled[0]);
    EXPECT_NE(lineWidthsEnabled[1], lineWidthsDisabled[1]);
    EXPECT_EQ(lineTextRangeDisabled[0].start, 0);
    EXPECT_EQ(lineTextRangeDisabled[0].end, 14);
    EXPECT_EQ(lineTextRangeDisabled[1].start, 14);
    EXPECT_EQ(lineTextRangeDisabled[1].end, 21);
    EXPECT_EQ(lineTextRangeEnabled[0].start, 0);
    EXPECT_EQ(lineTextRangeEnabled[0].end, 13);
    EXPECT_EQ(lineTextRangeEnabled[1].start, 13);
    EXPECT_EQ(lineTextRangeEnabled[1].end, 21);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest003
 * @tc.desc: Test orphan char paragraph feature - verify each paragraph triggers orphan after \n
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错感觉还结结巴巴呵呵你好，\n环境不错感觉还结结巴巴呵呵可能吧。。。。。"
 *           Font size: 50, Max width: 700
 *           Target: After \n split, each paragraph should trigger orphan char
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest003, TestSize.Level0)
{
    // Test with orphanCharOptimization = false (baseline)
    TypographyStyle typographyStyleDisabled;
    typographyStyleDisabled.orphanCharOptimization = false;
    typographyStyleDisabled.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyleDisabled, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好，\n环境不错感觉还结结巴巴呵呵可能吧。。。。。");
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    // Get line metrics with disabled
    std::vector<LineMetrics> lineMetricsDisabled = typography_->GetLineMetrics();
    size_t lineCountDisabled = lineMetricsDisabled.size();
    EXPECT_EQ(lineCountDisabled, 4);

    std::vector<double> lineWidthsDisabled;
    std::vector<TextRange> lineTextRangeDisabled;
    for (size_t i = 0; i < lineCountDisabled; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        lineWidthsDisabled.push_back(lineMetric.width);
        lineTextRangeDisabled.push_back({lineMetric.startIndex, lineMetric.endIndex});
    }

    // Reset for next test
    typography_.reset();
    typographyCreate_.reset();

    // Test with orphanCharOptimization = true
    TypographyStyle typographyStyleEnabled;
    typographyStyleEnabled.orphanCharOptimization = true;
    typographyStyleEnabled.fontSize = 50.0;

    typographyCreate_ = TypographyCreate::Create(typographyStyleEnabled, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(maxWidth);

    // Get line metrics with enabled
    std::vector<LineMetrics> lineMetricsEnabled = typography_->GetLineMetrics();
    size_t lineCountEnabled = lineMetricsEnabled.size();
    EXPECT_EQ(lineCountEnabled, 4);

    std::vector<double> lineWidthsEnabled;
    std::vector<TextRange> lineTextRangeEnabled;
    for (size_t i = 0; i < lineCountEnabled; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        lineWidthsEnabled.push_back(lineMetric.width);
        lineTextRangeEnabled.push_back({lineMetric.startIndex, lineMetric.endIndex});
    }

    std::vector<double> expectWidthsEnabled{649.99932861328125, 149.99981689453125, 649.99932861328125,
        399.99957275390625};
    std::vector<double> expectWidthsDisabled{699.999267578125, 99.9998779296875, 699.999267578125,
        349.9996337890625};
    std::vector<TextRange> expectTextRangesDisabled{{0, 14}, {14, 16}, {17, 31}, {31, 38}};
    std::vector<TextRange> expectTextRangesEnabled{{0, 13}, {13, 16}, {17, 30}, {30, 38}};

    for (size_t i = 0; i < lineCountEnabled; ++i) {
        EXPECT_DOUBLE_EQ(lineWidthsEnabled[i], expectWidthsEnabled[i]);
        EXPECT_DOUBLE_EQ(lineWidthsDisabled[i], expectWidthsDisabled[i]);
        EXPECT_EQ(lineTextRangeDisabled[i].start, expectTextRangesDisabled[i].start);
        EXPECT_EQ(lineTextRangeDisabled[i].end, expectTextRangesDisabled[i].end);
        EXPECT_EQ(lineTextRangeEnabled[i].start, expectTextRangesEnabled[i].start);
        EXPECT_EQ(lineTextRangeEnabled[i].end, expectTextRangesEnabled[i].end);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest004
 * @tc.desc: Test orphan char with different font sizes - verify line height correction after orphan triggers
 * @tc.type: FUNC
 *
 * Scenario: Test with mixed font sizes: "环境不错感觉还结结巴巴呵呵" + "你"(size 200) + "好"(size 50)
 *           Font size: 50 (base), Max width: 897
 *           Target: "你" is large, without orphan trigger it stays in first line and heightens it;
 *                   after orphan trigger moves to second line, first line height should decrease
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest004, TestSize.Level0)
{
    // Test with orphanCharOptimization = false (baseline)
    TypographyStyle typographyStyleDisabled;
    typographyStyleDisabled.orphanCharOptimization = true;

    typographyCreate_ = TypographyCreate::Create(typographyStyleDisabled, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    // Append base text with font size 50
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

    // Get line metrics with disabled
    std::vector<LineMetrics> lineMetricsEnabled = typography_->GetLineMetrics();
    size_t lineCountEnabled = lineMetricsEnabled.size();
    EXPECT_EQ(lineCountEnabled, 2);

    std::vector<double> expectWidthsEnabled{649.99932861328125, 250};
    std::vector<double> expectHeightsEnabled{59, 234};
    std::vector<TextRange> expectRangesEnabled{{0, 13}, {13, 15}};
    for (size_t i = 0; i < lineCountEnabled; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(expectWidthsEnabled[i], lineMetric.width);
        EXPECT_DOUBLE_EQ(expectHeightsEnabled[i], lineMetric.height);
        EXPECT_EQ(expectRangesEnabled[i].start, lineMetric.startIndex);
        EXPECT_EQ(expectRangesEnabled[i].end, lineMetric.endIndex);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest005
 * @tc.desc: Test orphan char locale feature - verify locale affects orphan trigger
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错感觉还结结巴巴呵呵你好" with different locales
 *           Font size: 50, Max width: 700
 *           Target: First paragraph locale=en (should NOT trigger orphan),
 *                   second paragraph locale=zh-Hans (should trigger orphan)
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest005, TestSize.Level0)
{
    TypographyStyle typographyStyleEn;
    typographyStyleEn.orphanCharOptimization = true;
    TextStyle styleHans;
    styleHans.fontSize = 50.0;
    styleHans.locale = "zh-Hans";

    typographyCreate_ = TypographyCreate::Create(typographyStyleEn, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = StrToU16Str("环境不错感觉还结结巴巴呵呵你好");
    typographyCreate_->PushStyle(styleHans);
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    double maxWidth = 700.0;
    typography_->Layout(maxWidth);

    // Get line metrics with disabled
    std::vector<LineMetrics> lineMetricsDisabled = typography_->GetLineMetrics();
    size_t lineCountDisabled = lineMetricsDisabled.size();
    EXPECT_EQ(lineCountDisabled, 2);

    // Get each line's width
    std::vector<double> lineWidthsHans;
    std::vector<TextRange> lineTextRangeHans;
    for (size_t i = 0; i < lineCountDisabled; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        lineWidthsHans.push_back(lineMetric.width);
        lineTextRangeHans.push_back({lineMetric.startIndex, lineMetric.endIndex});
    }

    // Reset for next test
    typography_.reset();
    typographyCreate_.reset();

    // Test with orphanCharOptimization = true and locale = "en"
    TypographyStyle typographyStyleEnabled;
    typographyStyleEnabled.orphanCharOptimization = true;
    TextStyle style;
    style.fontSize = 50.0;
    style.locale = "en";

    typographyCreate_ = TypographyCreate::Create(typographyStyleEnabled, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    typographyCreate_->PushStyle(style);
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(maxWidth);

    // Get line metrics with enabled
    std::vector<LineMetrics> lineMetricsEnabled = typography_->GetLineMetrics();
    size_t lineCountEnabled = lineMetricsEnabled.size();
    EXPECT_EQ(lineCountEnabled, 2);

    // Get each line's width
    std::vector<double> lineWidthsEn;
    std::vector<TextRange> lineTextRangeEn;
    for (size_t i = 0; i < lineCountEnabled; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        lineWidthsEn.push_back(lineMetric.width);
        lineTextRangeEn.push_back({lineMetric.startIndex, lineMetric.endIndex});
    }

    EXPECT_DOUBLE_EQ(lineWidthsHans[0], 649.99932861328125);
    EXPECT_DOUBLE_EQ(lineWidthsHans[1], 99.9998779296875);
    EXPECT_DOUBLE_EQ(lineWidthsEn[0], 699.999267578125);
    EXPECT_DOUBLE_EQ(lineWidthsEn[1], 49.99993896484375);
    EXPECT_EQ(lineTextRangeHans[0].start, 0);
    EXPECT_EQ(lineTextRangeHans[0].end, 13);
    EXPECT_EQ(lineTextRangeHans[1].start, 13);
    EXPECT_EQ(lineTextRangeHans[1].end, 15);
    EXPECT_EQ(lineTextRangeEn[0].start, 0);
    EXPECT_EQ(lineTextRangeEn[0].end, 14);
    EXPECT_EQ(lineTextRangeEn[1].start, 14);
    EXPECT_EQ(lineTextRangeEn[1].end, 15);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest006
 * @tc.desc: Test orphan char optimization with empty text
 * @tc.type: FUNC
 *
 * Scenario: Test with empty string, verify it doesn't crash
 *           Font size: 50, Max width: 700
 *           Target: Empty text should handle gracefully without triggering orphan
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest006, TestSize.Level0)
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
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest007
 * @tc.desc: Test orphan char optimization with single character
 * @tc.type: FUNC
 *
 * Scenario: Test with single character "你"
 *           Font size: 50, Max width: 700
 *           Target: Single character should not trigger orphan (no previous line to adjust)
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest007, TestSize.Level0)
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
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest008
 * @tc.desc: Test orphan char optimization with very wide text
 * @tc.type: FUNC
 *
 * Scenario: Test with "你好" but very wide maxWidth (2000)
 *           Font size: 50, Max width: 2000
 *           Target: All text fits in one line, orphan should not trigger
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest008, TestSize.Level0)
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
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest009
 * @tc.desc: Test orphan char optimization with very narrow width
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错" but very narrow maxWidth (50)
 *           Font size: 50, Max width: 50
 *           Target: Each character becomes separate line, verify behavior
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest009, TestSize.Level0)
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
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest010
 * @tc.desc: Test orphan char optimization with Emoji
 * @tc.type: FUNC
 *
 * Scenario: Test with "平衡行测试好吃的苹果😂😊😗🥲"
 *           Font size: 50, Max width: 700
 *           Target: Emoji should not interfere with orphan char detection
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest012, TestSize.Level0)
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

    LineMetrics lineMetric;
    typography_->GetLineMetricsAt(0, &lineMetric);
    std::vector<double> expectWidthsEnabled{687.17138671875, 62.390625};
    // Emoji takes 2 UTF-16 code units
    std::vector<TextRange> expectRangesEnabled{{0, 16}, {16, 18}};
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        EXPECT_DOUBLE_EQ(lineMetric.width, expectWidthsEnabled[i]);
        EXPECT_EQ(lineMetric.startIndex, expectRangesEnabled[i].start);
        EXPECT_EQ(lineMetric.endIndex, expectRangesEnabled[i].end);
    }
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest011
 * @tc.desc: Test orphan char optimization with justify alignment
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错感觉还结结巴巴呵呵你好"
 *           Font size: 50, textAlign: JUSTIFY, Max width: 700
 *           Target: Verify text alignment doesn't break orphan optimization
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest016, TestSize.Level0)
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

    // Get each line's width
    std::vector<double> lineWidths;
    std::vector<TextRange> lineTextRanges;
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        lineWidths.push_back(lineMetric.width);
        lineTextRanges.push_back({lineMetric.startIndex, lineMetric.endIndex});
    }

    EXPECT_DOUBLE_EQ(lineWidths[0], 700);
    EXPECT_DOUBLE_EQ(lineWidths[1], 99.9998779296875);
    EXPECT_EQ(lineTextRanges[0].start, 0);
    EXPECT_EQ(lineTextRanges[0].end, 13);
    EXPECT_EQ(lineTextRanges[1].start, 13);
    EXPECT_EQ(lineTextRanges[1].end, 15);
}

/*
 * @tc.name: OHDrawingOrphanCharOptimizationSceneTest012
 * @tc.desc: Test orphan char optimization with letterSpacing
 * @tc.type: FUNC
 *
 * Scenario: Test with "环境不错感觉还结结巴巴呵呵你好"
 *           Font size: 50, letterSpacing: 3.0, Max width: 700
 *           Target: Verify letterSpacing doesn't break orphan optimization
 */
HWTEST_F(OHDrawingOrphanCharOptimizationTest, OHDrawingOrphanCharOptimizationSceneTest018, TestSize.Level0)
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

    // Get each line's width
    std::vector<double> lineWidths;
    std::vector<TextRange> lineTextRanges;
    for (size_t i = 0; i < lineCount; ++i) {
        LineMetrics lineMetric;
        EXPECT_TRUE(typography_->GetLineMetricsAt(i, &lineMetric));
        lineWidths.push_back(lineMetric.width);
        lineTextRanges.push_back({lineMetric.startIndex, lineMetric.endIndex});
    }

    EXPECT_DOUBLE_EQ(lineWidths[0], 688.99932861328125);
    EXPECT_DOUBLE_EQ(lineWidths[1], 105.9998779296875);
    EXPECT_EQ(lineTextRanges[0].start, 0);
    EXPECT_EQ(lineTextRanges[0].end, 13);
    EXPECT_EQ(lineTextRanges[1].start, 13);
    EXPECT_EQ(lineTextRanges[1].end, 15);
}
}// namespace Rosen
} // namespace OHOS