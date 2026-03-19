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

    static std::shared_ptr<FontCollection> fontCollection_;
};

std::shared_ptr<FontCollection> PunctuationOverflowTest::fontCollection_ = nullptr;

// ============================================================================
// Group 1: Basic functionality verification
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest001
 * @tc.desc: Test single punctuation hanging - line count comparison
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest001, TestSize.Level0)
{
    const std::u16string text = u"这是一行很长很长很长很长很长的文本。";
    const double layoutWidth = 200.0;
    const double fontSize = 20.0;

    // Create typography with punctuationOverflow disabled
    TypographyStyle styleDisabled;
    styleDisabled.punctuationOverflow = false;
    TextStyle textStyleDisabled;
    textStyleDisabled.fontSize = fontSize;

    auto createDisabled = TypographyCreate::Create(styleDisabled, fontCollection_);
    ASSERT_NE(createDisabled, nullptr);
    createDisabled->PushStyle(textStyleDisabled);
    createDisabled->AppendText(text);
    auto typographyDisabled = createDisabled->CreateTypography();
    ASSERT_NE(typographyDisabled, nullptr);
    typographyDisabled->Layout(layoutWidth);
    int lineCountDisabled = typographyDisabled->GetLineCount();

    // Create typography with punctuationOverflow enabled
    TypographyStyle styleEnabled;
    styleEnabled.punctuationOverflow = true;
    TextStyle textStyleEnabled;
    textStyleEnabled.fontSize = fontSize;

    auto createEnabled = TypographyCreate::Create(styleEnabled, fontCollection_);
    ASSERT_NE(createEnabled, nullptr);
    createEnabled->PushStyle(textStyleEnabled);
    createEnabled->AppendText(text);
    auto typographyEnabled = createEnabled->CreateTypography();
    ASSERT_NE(typographyEnabled, nullptr);
    typographyEnabled->Layout(layoutWidth);
    int lineCountEnabled = typographyEnabled->GetLineCount();

    // When punctuationOverflow is enabled, there should be fewer or equal lines
    bool isValid = (lineCountEnabled <= lineCountDisabled);
    EXPECT_EQ(isValid, false);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest002
 * @tc.desc: Test single punctuation hanging - actual width comparison
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest002, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本。";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    double maxWidth = typography->GetMaxWidth();
    double actualWidth = typography->GetActualWidth();

    EXPECT_EQ(maxWidth, -1.0);  // TODO: fix expected value
    bool actualWidthValid = (actualWidth > 0.0);
    EXPECT_EQ(actualWidthValid, false);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest003
 * @tc.desc: Test two consecutive punctuations - forced wrap
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest003, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本。。";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    int lineCount = typography->GetLineCount();
    EXPECT_EQ(lineCount, -1);  // TODO: fix expected value
}

// ============================================================================
// Group 2: Punctuation coverage
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest004
 * @tc.desc: Test period punctuation (。)
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest004, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本。";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest005
 * @tc.desc: Test comma punctuation (，)
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest005, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本，";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest006
 * @tc.desc: Test enumeration comma (、)
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest006, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本、";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest007
 * @tc.desc: Test semicolon (；)
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest007, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本；";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest008
 * @tc.desc: Test colon (：)
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest008, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本：";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest009
 * @tc.desc: Test question mark (？)
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest009, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本？";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest010
 * @tc.desc: Test exclamation mark (！)
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest010, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本！";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest011
 * @tc.desc: Test paired quotation marks (""'')
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest011, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本测试\"";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest012
 * @tc.desc: Test parentheses (（))
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest012, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本（测试）";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest013
 * @tc.desc: Test book title marks (《》)
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest013, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本《测试》";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

// ============================================================================
// Group 3: Text direction tests
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest014
 * @tc.desc: Test LTR text with Chinese punctuation
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest014, TestSize.Level0)
{
    const std::u16string text = u"Hello World Hello Test。";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    style.textDirection = TextDirection::LTR;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest015
 * @tc.desc: Test RTL text with punctuation
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest015, TestSize.Level0)
{
    const std::u16string text = u"\u0627\u0644\u0639\u0631\u0628\u064A\u0629\u3002"; // العربية。
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    style.textDirection = TextDirection::RTL;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

// ============================================================================
// Group 4: Boundary condition tests
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest016
 * @tc.desc: Test critical width boundary
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest016, TestSize.Level0)
{
    const std::u16string text = u"测试文本。";
    const double layoutWidth = 100.0;
    const double fontSize = 20.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest017
 * @tc.desc: Test insufficient width - punctuation must wrap
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest017, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本。";
    const double layoutWidth = 50.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest018
 * @tc.desc: Test multi-line text with punctuation at end of each line
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest018, TestSize.Level0)
{
    const std::u16string text = u"第一行文本，第二行文本，第三行文本，";
    const double layoutWidth = 120.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value

    auto lineMetrics = typography->GetLineMetrics();
    EXPECT_EQ(lineMetrics.size(), static_cast<size_t>(-1));  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest019
 * @tc.desc: Test adequate width - single line display
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest019, TestSize.Level0)
{
    const std::u16string text = u"测试。";
    const double layoutWidth = 500.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

// ============================================================================
// Group 5: Composite scenario tests
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest020
 * @tc.desc: Test mixed Chinese and English text
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest020, TestSize.Level0)
{
    const std::u16string text = u"Hello测试World。";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

// ============================================================================
// Group 6: Exception scenario tests
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest021
 * @tc.desc: Test empty text - should not crash
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest021, TestSize.Level0)
{
    const std::u16string text = u"";
    const double layoutWidth = 100.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest022
 * @tc.desc: Test punctuation only text
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest022, TestSize.Level0)
{
    const std::u16string text = u"。";
    const double layoutWidth = 50.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest023
 * @tc.desc: Test punctuation at line start - should not trigger hanging
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest023, TestSize.Level0)
{
    const std::u16string text = u"。测试文本";
    const double layoutWidth = 100.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest024
 * @tc.desc: Test toggle between enabled and disabled
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest024, TestSize.Level0)
{
    const std::u16string text = u"测试文本测试文本测试文本。";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    // Test with punctuationOverflow disabled
    TypographyStyle styleDisabled;
    styleDisabled.punctuationOverflow = false;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto createDisabled = TypographyCreate::Create(styleDisabled, fontCollection_);
    ASSERT_NE(createDisabled, nullptr);
    createDisabled->PushStyle(textStyle);
    createDisabled->AppendText(text);
    auto typographyDisabled = createDisabled->CreateTypography();
    ASSERT_NE(typographyDisabled, nullptr);
    typographyDisabled->Layout(layoutWidth);

    // Test with punctuationOverflow enabled
    TypographyStyle styleEnabled;
    styleEnabled.punctuationOverflow = true;

    auto createEnabled = TypographyCreate::Create(styleEnabled, fontCollection_);
    ASSERT_NE(createEnabled, nullptr);
    createEnabled->PushStyle(textStyle);
    createEnabled->AppendText(text);
    auto typographyEnabled = createEnabled->CreateTypography();
    ASSERT_NE(typographyEnabled, nullptr);
    typographyEnabled->Layout(layoutWidth);

    EXPECT_EQ(typographyDisabled->GetLineCount(), -1);  // TODO: fix expected value
    EXPECT_EQ(typographyEnabled->GetLineCount(), -1);  // TODO: fix expected value
}

// ============================================================================
// Group 7: Multi-line and hard line break tests
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest025
 * @tc.desc: Test multi-line text with punctuation at end of each line
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest025, TestSize.Level0)
{
    // Multiple lines, each ending with punctuation
    const std::u16string text = u"第一行很长很长的文本内容。第二行很长很长的文本内容。第三行很长很长的文本内容。";
    const double layoutWidth = 150.0;
    const double fontSize = 16.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest026
 * @tc.desc: Test hard newline (\n) with punctuation at line end
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest026, TestSize.Level0)
{
    // Text with explicit newline character and punctuation at end
    const std::u16string text = u"测试文本内容。\n第二行测试内容。";
    const double layoutWidth = 200.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest027
 * @tc.desc: Test hard newline (\n) with punctuation before newline
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest027, TestSize.Level0)
{
    // Punctuation followed by hard newline - should not hang because of newline
    const std::u16string text = u"测试文本内容。\n第二行测试内容";
    const double layoutWidth = 200.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest028
 * @tc.desc: Test multiple hard newlines with punctuation
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest028, TestSize.Level0)
{
    // Multiple hard newlines with punctuation
    const std::u16string text = u"第一行。\n第二行，\n第三行；";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest029
 * @tc.desc: Test hard newline in middle of long text
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest029, TestSize.Level0)
{
    // Long text with hard newline in the middle
    const std::u16string text = u"这是一段很长很长的文本内容需要换行显示。\n这是第二段很长很长的文本内容也需要换行显示。";
    const double layoutWidth = 200.0;
    const double fontSize = 16.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

// ============================================================================
// Group 8: Ellipsis scenario tests
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest030
 * @tc.desc: Test Chinese ellipsis (six dots) at line end
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest030, TestSize.Level0)
{
    // Chinese ellipsis (six dots: ……)
    const std::u16string text = u"测试文本测试文本测试文本……";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest031
 * @tc.desc: Test single ellipsis character (……) hanging behavior
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest031, TestSize.Level0)
{
    // Single ellipsis as punctuation
    const std::u16string text = u"测试文本……";
    const double layoutWidth = 100.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest032
 * @tc.desc: Test text ending with period followed by ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest032, TestSize.Level0)
{
    // Period followed by ellipsis
    const std::u16string text = u"测试文本测试文本。……";
    const double layoutWidth = 120.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest033
 * @tc.desc: Test ellipsis with maxLines constraint
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest033, TestSize.Level0)
{
    // Ellipsis with maxLines constraint
    const std::u16string text = u"测试文本测试文本测试文本测试文本……";
    const double layoutWidth = 100.0;
    const double fontSize = 18.0;
    const size_t maxLines = 2;

    TypographyStyle style;
    style.punctuationOverflow = true;
    style.maxLines = maxLines;
    style.ellipsis = u"\u2026";  // Standard ellipsis character
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
    EXPECT_EQ(typography->DidExceedMaxLines(), false);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest034
 * @tc.desc: Test ellipsis in middle of text line
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest034, TestSize.Level0)
{
    // Ellipsis in middle, not at line end
    const std::u16string text = u"测试文本……继续内容";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

// ============================================================================
// Group 9: Composite multi-line scenario tests
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest035
 * @tc.desc: Test multi-line paragraph with mixed punctuation types
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest035, TestSize.Level0)
{
    // Multiple lines with different punctuation types at end
    const std::u16string text = u"第一行内容很长，需要换行。第二行内容也很长，继续测试。第三行内容再次展示！";
    const double layoutWidth = 120.0;
    const double fontSize = 16.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    int lineCount = typography->GetLineCount();
    EXPECT_EQ(lineCount, -1);  // TODO: fix expected value

    // Verify we can get line metrics for each line
    auto lineMetrics = typography->GetLineMetrics();
    EXPECT_EQ(lineMetrics.size(), static_cast<size_t>(-1));  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest036
 * @tc.desc: Test very long text spanning many lines with punctuation
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest036, TestSize.Level0)
{
    // Very long text that spans multiple lines
    const std::u16string text = u"这是一段非常非常长的文本内容，它包含了很多字和标点符号。这段文本会跨越很多行，每一行都可能以不同的标点符号结尾。我们测试第一行，测试第二行，测试第三行，测试第四行，测试第五行，测试第六行。";
    const double layoutWidth = 100.0;
    const double fontSize = 16.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value

    // Verify each line has valid metrics
    auto lineMetrics = typography->GetLineMetrics();
    for (size_t i = 0; i < lineMetrics.size(); ++i) {
        bool hasValidWidth = (lineMetrics[i].width > 0.0);
        EXPECT_EQ(hasValidWidth, true);  // TODO: fix expected value
    }
}

/*
 * @tc.name: PunctuationOverflowTest037
 * @tc.desc: Test hard newline mixed with soft wrapping
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest037, TestSize.Level0)
{
    // Mix of hard newlines and soft wrapping
    const std::u16string text = u"第一段内容很长需要软换行。\n第二段内容很短。\n第三段内容又很长需要软换行继续显示。";
    const double layoutWidth = 120.0;
    const double fontSize = 16.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest038
 * @tc.desc: Test punctuation hanging with ellipsis and newline combination
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest038, TestSize.Level0)
{
    // Ellipsis followed by hard newline
    const std::u16string text = u"测试文本内容……\n第二行继续内容。";
    const double layoutWidth = 150.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest039
 * @tc.desc: Test multiple consecutive ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest039, TestSize.Level0)
{
    // Multiple consecutive ellipsis (should wrap)
    const std::u16string text = u"测试文本…………";
    const double layoutWidth = 100.0;
    const double fontSize = 18.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value
}

/*
 * @tc.name: PunctuationOverflowTest040
 * @tc.desc: Test paragraph with varying line lengths and punctuation
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest040, TestSize.Level0)
{
    // Lines with different lengths, some ending with punctuation
    const std::u16string text = u"短。中等长度的文本内容。很长的文本内容需要换行显示，测试标点悬挂功能。极短。";
    const double layoutWidth = 120.0;
    const double fontSize = 16.0;

    TypographyStyle style;
    style.punctuationOverflow = true;
    TextStyle textStyle;
    textStyle.fontSize = fontSize;

    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    create->PushStyle(textStyle);
    create->AppendText(text);
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(layoutWidth);

    EXPECT_EQ(typography->GetLineCount(), -1);  // TODO: fix expected value

    // Verify width variation across lines
    auto lineMetrics = typography->GetLineMetrics();
    bool hasMetrics = (lineMetrics.size() > 0);
    EXPECT_EQ(hasMetrics, true);  // TODO: fix expected value
}

} // namespace Rosen
} // namespace OHOS
