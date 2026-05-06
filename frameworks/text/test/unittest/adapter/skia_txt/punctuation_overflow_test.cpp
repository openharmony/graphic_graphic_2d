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

#include "gtest/gtest.h"

#include "font_collection.h"
#include "typography.h"
#include "typography_create.h"
#include "txt/text_bundle_config_parser.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class PunctuationOverflowTest : public testing::Test {
protected:
    static void SetUpTestCase()
    {
        fontCollection_ = FontCollection::From(std::make_shared<txt::FontCollection>());
        ASSERT_NE(fontCollection_, nullptr);
    }

    static std::unique_ptr<Typography> BuildAndLayout(
        const std::u16string& text, double fontSize, double layoutWidth,
        bool punctuationOverflow, TextDirection direction = TextDirection::LTR,
        TextAlign textAlign = TextAlign::START)
    {
        TypographyStyle style;
        style.punctuationOverflow = punctuationOverflow;
        style.textDirection = direction;
        style.textAlign = textAlign;
        TextStyle textStyle;
        textStyle.fontSize = fontSize;

        auto create = TypographyCreate::Create(style, fontCollection_);
        if (!create) {
            return nullptr;
        }
        create->PushStyle(textStyle);
        create->AppendText(text);
        auto typography = create->CreateTypography();
        if (!typography) {
            return nullptr;
        }
        typography->Layout(layoutWidth);
        return typography;
    }

    static double MeasureTextWidth(const std::u16string& text, double fontSize)
    {
        auto typography = BuildAndLayout(text, fontSize, 10000.0, false);
        if (!typography) {
            return -1.0;
        }
        return typography->GetLongestLineWithIndent();
    }

    static std::shared_ptr<FontCollection> fontCollection_;
    static constexpr double FONT_SIZE = 20.0;
};

std::shared_ptr<FontCollection> PunctuationOverflowTest::fontCollection_ = nullptr;

// ============================================================================
// Group 1: Chinese punctuation types
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest001
 * @tc.desc: Chinese period (。), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest001, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试。", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试。", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), textWidth);
}

/*
 * @tc.name: PunctuationOverflowTest002
 * @tc.desc: Chinese comma (，), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest002, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试，", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试，", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), textWidth);
}

/*
 * @tc.name: PunctuationOverflowTest003
 * @tc.desc: Enumeration comma (、), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest003, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试、", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试、", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), textWidth);
}

/*
 * @tc.name: PunctuationOverflowTest004
 * @tc.desc: Semicolon (；), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest004, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试；", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试；", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), textWidth);
}

/*
 * @tc.name: PunctuationOverflowTest005
 * @tc.desc: Colon (：), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest005, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试：", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试：", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), textWidth);
}

/*
 * @tc.name: PunctuationOverflowTest006
 * @tc.desc: Question mark (？), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest006, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试？", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试？", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), textWidth);
}

/*
 * @tc.name: PunctuationOverflowTest007
 * @tc.desc: Exclamation mark (！), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest007, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试！", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试！", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), textWidth);
}

// ============================================================================
// Group 2: Non-hanging punctuation - should NOT trigger hanging
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest008
 * @tc.desc: Left parenthesis (（) at line end - should not hang, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest008, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试（", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试（", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest009
 * @tc.desc: Left book title mark (《) at line end - should not hang, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest009, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试《", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试《", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest010
 * @tc.desc: Left bracket (【) at line end - should not hang, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest010, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试【", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试【", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest011
 * @tc.desc: Left double quotation mark (\u201C) at line end - should not hang.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest011, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试\u201C", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试\u201C", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest012
 * @tc.desc: Middle dot (·) at line end - should not hang.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest012, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试·", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试·", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest013
 * @tc.desc: Dash (——) at line end - should not hang.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest013, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试——", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试——", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

// ============================================================================
// Group 3: Different languages
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest014
 * @tc.desc: Japanese text with period (。), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest014, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"テスト文章テスト", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"テスト文章テスト。", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"テスト文章テスト。", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), textWidth);
}

/*
 * @tc.name: PunctuationOverflowTest015
 * @tc.desc: Korean text with period, measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest015, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"테스트 문장 테스트", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"테스트 문장 테스트.", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"테스트 문장 테스트.", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest016
 * @tc.desc: English text with period, measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest016, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"Hello World test", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"Hello World test.", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"Hello World test.", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest017
 * @tc.desc: Mixed Chinese/English text with Chinese period, measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest017, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"Hello测试Test", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"Hello测试Test。", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"Hello测试Test。", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest018
 * @tc.desc: Chinese text with right quotation mark (\u201D), measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest018, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试\u201D", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试\u201D", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

// ============================================================================
// Group 4: LTR and RTL text direction
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest019
 * @tc.desc: LTR direction with Chinese period, measure text width then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest019, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, true, TextDirection::LTR);
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, false, TextDirection::LTR);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest020
 * @tc.desc: RTL Arabic text with Arabic comma (\u060C), measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest020, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"العربية", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"العربية،", FONT_SIZE, textWidth, true, TextDirection::RTL);
    auto typDisabled = BuildAndLayout(
        u"العربية،", FONT_SIZE, textWidth, false, TextDirection::RTL);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest021
 * @tc.desc: RTL Arabic text with Arabic question mark (\u061F), measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest021, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"العربية", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"العربية؟", FONT_SIZE, textWidth, true, TextDirection::RTL);
    auto typDisabled = BuildAndLayout(
        u"العربية؟", FONT_SIZE, textWidth, false, TextDirection::RTL);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest022
 * @tc.desc: RTL Hebrew text with period, measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest022, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"עברית", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"עברית.", FONT_SIZE, textWidth, true, TextDirection::RTL);
    auto typDisabled = BuildAndLayout(
        u"עברית.", FONT_SIZE, textWidth, false, TextDirection::RTL);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest023
 * @tc.desc: LTR English with Chinese period at end, measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest023, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"Hello World Test", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"Hello World Test。", FONT_SIZE, textWidth, true, TextDirection::LTR);
    auto typDisabled = BuildAndLayout(
        u"Hello World Test。", FONT_SIZE, textWidth, false, TextDirection::LTR);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

// ============================================================================
// Group 5: Edge cases
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest024
 * @tc.desc: Two trailing punctuations (。。), measure then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest024, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试。。", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试。。", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);

    auto metricsEnabled = typEnabled->GetLineMetrics();
    auto metricsDisabled = typDisabled->GetLineMetrics();
    EXPECT_EQ(metricsEnabled.size(), 0u);
    EXPECT_EQ(metricsDisabled.size(), 0u);
    EXPECT_DOUBLE_EQ(metricsEnabled[0].width, 0.0);
    EXPECT_DOUBLE_EQ(metricsDisabled[0].width, 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest025
 * @tc.desc: No trailing punctuation - both enabled and disabled should produce same result.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest025, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本测试", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 1);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest026
 * @tc.desc: Punctuation at start of text - not a trailing hanging scenario.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest026, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"。测试文本测试", FONT_SIZE, textWidth, true);
    auto typDisabled = BuildAndLayout(u"。测试文本测试", FONT_SIZE, textWidth, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest027
 * @tc.desc: Very wide layout - text fits on one line, no difference.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest027, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本。", FONT_SIZE);
    auto typEnabled = BuildAndLayout(u"测试文本。", FONT_SIZE, textWidth + 100.0, true);
    auto typDisabled = BuildAndLayout(u"测试文本。", FONT_SIZE, textWidth + 100.0, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 1);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest028
 * @tc.desc: Very narrow layout - text wraps heavily.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest028, TestSize.Level0)
{
    auto typEnabled = BuildAndLayout(u"测试文本测试文本。", FONT_SIZE, 30.0, true);
    auto typDisabled = BuildAndLayout(u"测试文本测试文本。", FONT_SIZE, 30.0, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest029
 * @tc.desc: Empty text - should not crash.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest029, TestSize.Level0)
{
    auto typEnabled = BuildAndLayout(u"", FONT_SIZE, 100.0, true);
    auto typDisabled = BuildAndLayout(u"", FONT_SIZE, 100.0, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest030
 * @tc.desc: Punctuation-only text, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest030, TestSize.Level0)
{
    auto typEnabled = BuildAndLayout(u"。", FONT_SIZE, 100.0, true);
    auto typDisabled = BuildAndLayout(u"。", FONT_SIZE, 100.0, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 1);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest031
 * @tc.desc: Hard newline with punctuation before \n, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest031, TestSize.Level0)
{
    auto typEnabled = BuildAndLayout(u"测试文本。\n第二行测试", FONT_SIZE, 500.0, true);
    auto typDisabled = BuildAndLayout(u"测试文本。\n第二行测试", FONT_SIZE, 500.0, false);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

// ============================================================================
// Group 6: Text alignment scenarios
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest032
 * @tc.desc: LEFT alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest032, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, true, TextDirection::LTR, TextAlign::LEFT);
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, false, TextDirection::LTR, TextAlign::LEFT);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest033
 * @tc.desc: RIGHT alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest033, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, true, TextDirection::LTR, TextAlign::RIGHT);
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, false, TextDirection::LTR, TextAlign::RIGHT);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest034
 * @tc.desc: CENTER alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest034, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, true, TextDirection::LTR, TextAlign::CENTER);
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, false, TextDirection::LTR, TextAlign::CENTER);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest035
 * @tc.desc: JUSTIFY alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest035, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, true, TextDirection::LTR, TextAlign::JUSTIFY);
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, false, TextDirection::LTR, TextAlign::JUSTIFY);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest036
 * @tc.desc: START alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest036, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, true, TextDirection::LTR, TextAlign::START);
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, false, TextDirection::LTR, TextAlign::START);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest037
 * @tc.desc: END alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest037, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, true, TextDirection::LTR, TextAlign::END);
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, false, TextDirection::LTR, TextAlign::END);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest038
 * @tc.desc: JUSTIFY alignment with RTL direction and trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest038, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, true, TextDirection::RTL, TextAlign::JUSTIFY);
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, false, TextDirection::RTL, TextAlign::JUSTIFY);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest039
 * @tc.desc: CENTER alignment with RTL direction and trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest039, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, true, TextDirection::RTL, TextAlign::CENTER);
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", FONT_SIZE, textWidth, false, TextDirection::RTL, TextAlign::CENTER);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

/*
 * @tc.name: PunctuationOverflowTest040
 * @tc.desc: JUSTIFY alignment with multi-line text and multiple trailing punctuations.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest040, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", FONT_SIZE);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。测试文本。", FONT_SIZE, textWidth, true, TextDirection::LTR, TextAlign::JUSTIFY);
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。测试文本。", FONT_SIZE, textWidth, false, TextDirection::LTR, TextAlign::JUSTIFY);
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 0);
    EXPECT_EQ(typDisabled->GetLineCount(), 0);
    EXPECT_DOUBLE_EQ(typEnabled->GetLongestLineWithIndent(), 0.0);
    EXPECT_DOUBLE_EQ(typDisabled->GetLongestLineWithIndent(), 0.0);
}

} // namespace Rosen
} // namespace OHOS
