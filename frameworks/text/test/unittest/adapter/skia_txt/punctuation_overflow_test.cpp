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
    struct LayoutConfig {
        double fontSize = PunctuationOverflowTest::fontSize;
        double layoutWidth = 100.0;
        bool punctuationOverflow = false;
        TextDirection direction = TextDirection::LTR;
        TextAlign textAlign = TextAlign::START;
    };

    static void SetUpTestCase()
    {
        fontCollection_ = FontCollection::From(std::make_shared<txt::FontCollection>());
        ASSERT_NE(fontCollection_, nullptr);
    }

    static std::unique_ptr<TypographyCreate> CreateTypographyBuilder(const LayoutConfig& config)
    {
        TypographyStyle style;
        style.punctuationOverflow = config.punctuationOverflow;
        style.textDirection = config.direction;
        style.textAlign = config.textAlign;

        auto create = TypographyCreate::Create(style, fontCollection_);
        return create;
    }

    static void AddTextStyle(std::unique_ptr<TypographyCreate>& create, double fontSize)
    {
        TextStyle textStyle;
        textStyle.fontSize = fontSize;
        create->PushStyle(textStyle);
    }

    static std::unique_ptr<Typography> BuildAndLayout(const std::u16string& text, const LayoutConfig& config)
    {
        auto create = CreateTypographyBuilder(config);
        if (!create) {
            return nullptr;
        }
        AddTextStyle(create, config.fontSize);
        create->AppendText(text);

        auto typography = create->CreateTypography();
        if (!typography) {
            return nullptr;
        }
        typography->Layout(config.layoutWidth);
        return typography;
    }

    static LayoutConfig CreateConfig(double fontSize, double layoutWidth, bool punctuationOverflow)
    {
        LayoutConfig config;
        config.fontSize = fontSize;
        config.layoutWidth = layoutWidth;
        config.punctuationOverflow = punctuationOverflow;
        return config;
    }

    static LayoutConfig CreateConfigWithDirection(
        double fontSize, double layoutWidth, bool punctuationOverflow, TextDirection direction)
    {
        LayoutConfig config;
        config.fontSize = fontSize;
        config.layoutWidth = layoutWidth;
        config.punctuationOverflow = punctuationOverflow;
        config.direction = direction;
        return config;
    }

    static LayoutConfig CreateConfigWithAlign(
        double fontSize, double layoutWidth, bool punctuationOverflow,
        TextDirection direction, TextAlign textAlign)
    {
        LayoutConfig config;
        config.fontSize = fontSize;
        config.layoutWidth = layoutWidth;
        config.punctuationOverflow = punctuationOverflow;
        config.direction = direction;
        config.textAlign = textAlign;
        return config;
    }

    static double MeasureTextWidth(const std::u16string& text, double fontSize)
    {
        LayoutConfig config = CreateConfig(fontSize, 10000.0, false);
        auto typography = BuildAndLayout(text, config);
        if (!typography) {
            return -1.0;
        }
        return typography->GetLongestLineWithIndent();
    }

    static std::shared_ptr<FontCollection> fontCollection_;
    static constexpr double fontSize = 20.0;
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
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试。", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试。", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest002
 * @tc.desc: Chinese comma (，), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest002, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试，", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试，", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest003
 * @tc.desc: Enumeration comma (、), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest003, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试、", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试、", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest004
 * @tc.desc: Semicolon (；), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest004, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试；", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试；", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest005
 * @tc.desc: Colon (：), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest005, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试：", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试：", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest006
 * @tc.desc: Question mark (？), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest006, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试？", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试？", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest007
 * @tc.desc: Exclamation mark (！), measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest007, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试！", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试！", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
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
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试（", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试（", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 119.99990));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 119.99990));
}

/*
 * @tc.name: PunctuationOverflowTest009
 * @tc.desc: Left book title mark (《) at line end - should not hang, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest009, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试《", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试《", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 119.99990));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 119.99990));
}

/*
 * @tc.name: PunctuationOverflowTest010
 * @tc.desc: Left bracket (【) at line end - should not hang, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest010, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试【", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试【", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 119.99990));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 119.99990));
}

/*
 * @tc.name: PunctuationOverflowTest011
 * @tc.desc: Left double quotation mark (\u201C) at line end - should hang.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest011, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试\u201C", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试\u201C", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 129.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest012
 * @tc.desc: Middle dot (·) at line end - should not hang.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest012, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试·", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试·", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 119.99990));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 119.99990));
}

/*
 * @tc.name: PunctuationOverflowTest013
 * @tc.desc: Dash (——) at line end - should not hang.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest013, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试——", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试——", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 119.99990));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 119.99990));
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
    double textWidth = MeasureTextWidth(u"テスト文章テスト", fontSize);
    auto typEnabled = BuildAndLayout(u"テスト文章テスト。", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"テスト文章テスト。", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 179.99986));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 139.99989));
}

/*
 * @tc.name: PunctuationOverflowTest015
 * @tc.desc: Korean text with period, measure text width then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest015, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"테스트 문장 테스트", fontSize);
    auto typEnabled = BuildAndLayout(u"테스트 문장 테스트.", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"테스트 문장 테스트.", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 162.61981));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 139.59983));
}


/*
 * @tc.name: PunctuationOverflowTest016
 * @tc.desc: Mixed Chinese/English text with Chinese period, measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest016, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"Hello测试Test", fontSize);
    auto typEnabled = BuildAndLayout(u"Hello测试Test。", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"Hello测试Test。", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 144.01988));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 87.21992));
}

/*
 * @tc.name: PunctuationOverflowTest017
 * @tc.desc: Chinese text with right quotation mark (\u201D), measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest017, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试\u201D", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试\u201D", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 129.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

// ============================================================================
// Group 4: LTR and RTL text direction
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest018
 * @tc.desc: LTR direction with Chinese period, measure text width then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest018, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithDirection(fontSize, textWidth, true, TextDirection::LTR));
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithDirection(fontSize, textWidth, false, TextDirection::LTR));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest019
 * @tc.desc: RTL Arabic text with Arabic comma (\u060C), measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest019, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"العربية", fontSize);
    auto typEnabled = BuildAndLayout(
        u"العربية،", CreateConfigWithDirection(fontSize, textWidth, true, TextDirection::RTL));
    auto typDisabled = BuildAndLayout(
        u"العربية،", CreateConfigWithDirection(fontSize, textWidth, false, TextDirection::RTL));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 54.79995));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 49.07995));
}

/*
 * @tc.name: PunctuationOverflowTest020
 * @tc.desc: RTL Arabic text with Arabic question mark (\u061F), measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest020, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"العربية", fontSize);
    auto typEnabled = BuildAndLayout(
        u"العربية؟", CreateConfigWithDirection(fontSize, textWidth, true, TextDirection::RTL));
    auto typDisabled = BuildAndLayout(
        u"العربية؟", CreateConfigWithDirection(fontSize, textWidth, false, TextDirection::RTL));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 49.07995));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 49.07995));
}

/*
 * @tc.name: PunctuationOverflowTest021
 * @tc.desc: RTL Hebrew text with period, measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest021, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"עברית", fontSize);
    auto typEnabled = BuildAndLayout(
        u"עברית.", CreateConfigWithDirection(fontSize, textWidth, true, TextDirection::RTL));
    auto typDisabled = BuildAndLayout(
        u"עברית.", CreateConfigWithDirection(fontSize, textWidth, false, TextDirection::RTL));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 56.77993));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 52.15994));
}

/*
 * @tc.name: PunctuationOverflowTest022
 * @tc.desc: LTR English with Chinese period at end, measure then compare.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest022, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"Hello World Test", fontSize);
    auto typEnabled = BuildAndLayout(
        u"Hello World Test。", CreateConfigWithDirection(fontSize, textWidth, true, TextDirection::LTR));
    auto typDisabled = BuildAndLayout(
        u"Hello World Test。", CreateConfigWithDirection(fontSize, textWidth, false, TextDirection::LTR));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 168.35984));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 111.55989));
}

// ============================================================================
// Group 5: Edge cases
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest023
 * @tc.desc: Two trailing punctuations (。。), measure then compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest023, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试。。", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试。。", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 99.99992));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));

    auto metricsEnabled = typEnabled->GetLineMetrics();
    auto metricsDisabled = typDisabled->GetLineMetrics();
    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 99.99992));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest024
 * @tc.desc: No trailing punctuation - both enabled and disabled should produce same result.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest024, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本测试", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 1);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 119.99990));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 119.99990));
}

/*
 * @tc.name: PunctuationOverflowTest025
 * @tc.desc: Punctuation at start of text - not a trailing hanging scenario.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest025, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(u"。测试文本测试", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"。测试文本测试", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 119.99990));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 119.99990));
}

/*
 * @tc.name: PunctuationOverflowTest026
 * @tc.desc: Very wide layout - text fits on one line, no difference.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest026, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本。", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本。", CreateConfig(fontSize, textWidth + 100.0, true));
    auto typDisabled = BuildAndLayout(u"测试文本。", CreateConfig(fontSize, textWidth + 100.0, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 1);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 99.99992));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest027
 * @tc.desc: Very narrow layout - text wraps heavily.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest027, TestSize.Level0)
{
    auto typEnabled = BuildAndLayout(u"测试文本测试文本。", CreateConfig(fontSize, 30.0, true));
    auto typDisabled = BuildAndLayout(u"测试文本测试文本。", CreateConfig(fontSize, 30.0, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 8);
    EXPECT_EQ(typDisabled->GetLineCount(), 9);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 39.99996));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 19.99998));
}

/*
 * @tc.name: PunctuationOverflowTest028
 * @tc.desc: Empty text - should not crash.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest028, TestSize.Level0)
{
    auto typEnabled = BuildAndLayout(u"", CreateConfig(fontSize, 100.0, true));
    auto typDisabled = BuildAndLayout(u"", CreateConfig(fontSize, 100.0, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 1);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 0.0));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 0.0));
}

/*
 * @tc.name: PunctuationOverflowTest029
 * @tc.desc: Punctuation-only text, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest029, TestSize.Level0)
{
    auto typEnabled = BuildAndLayout(u"。", CreateConfig(fontSize, 100.0, true));
    auto typDisabled = BuildAndLayout(u"。", CreateConfig(fontSize, 100.0, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 1);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 19.99998));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 19.99998));
}

/*
 * @tc.name: PunctuationOverflowTest030
 * @tc.desc: Hard newline with punctuation before \n, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest030, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本。\n第二行", fontSize);
    auto typEnabled = BuildAndLayout(u"测试文本。\n第二行", CreateConfig(fontSize, textWidth, true));
    auto typDisabled = BuildAndLayout(u"测试文本。\n第二行", CreateConfig(fontSize, textWidth, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 99.99992));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

// ============================================================================
// Group 6: Text alignment scenarios
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest031
 * @tc.desc: LEFT alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest031, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, true, TextDirection::LTR, TextAlign::LEFT));
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, false, TextDirection::LTR, TextAlign::LEFT));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest032
 * @tc.desc: RIGHT alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest032, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, true, TextDirection::LTR, TextAlign::RIGHT));
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, false, TextDirection::LTR, TextAlign::RIGHT));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest033
 * @tc.desc: CENTER alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest033, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, true, TextDirection::LTR, TextAlign::CENTER));
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, false, TextDirection::LTR, TextAlign::CENTER));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest034
 * @tc.desc: JUSTIFY alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest034, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, true, TextDirection::LTR, TextAlign::JUSTIFY));
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, false, TextDirection::LTR, TextAlign::JUSTIFY));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest035
 * @tc.desc: START alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest035, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, true, TextDirection::LTR, TextAlign::START));
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, false, TextDirection::LTR, TextAlign::START));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest036
 * @tc.desc: END alignment with trailing period, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest036, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, true, TextDirection::LTR, TextAlign::END));
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。", CreateConfigWithAlign(fontSize, textWidth, false, TextDirection::LTR, TextAlign::END));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest037
 * @tc.desc: JUSTIFY alignment with RTL Arabic text and Arabic comma, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest037, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"العربية للنصوص", fontSize);
    auto typEnabled = BuildAndLayout(
        u"العربية للنصوص،", CreateConfigWithAlign(fontSize, textWidth, true, TextDirection::RTL, TextAlign::JUSTIFY));
    auto typDisabled = BuildAndLayout(
        u"العربية للنصوص،", CreateConfigWithAlign(fontSize, textWidth, false, TextDirection::RTL, TextAlign::JUSTIFY));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
}

/*
 * @tc.name: PunctuationOverflowTest038
 * @tc.desc: CENTER alignment with RTL Arabic text and Arabic comma, compare enabled vs disabled.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest038, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"العربية للنصوص", fontSize);
    auto typEnabled = BuildAndLayout(
        u"العربية للنصوص،", CreateConfigWithAlign(fontSize, textWidth, true, TextDirection::RTL, TextAlign::CENTER));
    auto typDisabled = BuildAndLayout(
        u"العربية للنصوص،", CreateConfigWithAlign(fontSize, textWidth, false, TextDirection::RTL, TextAlign::CENTER));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 2);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 128.07989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 73.59994));
}

/*
 * @tc.name: PunctuationOverflowTest039
 * @tc.desc: JUSTIFY alignment with multi-line text and multiple trailing punctuations.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest039, TestSize.Level0)
{
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    auto typEnabled = BuildAndLayout(
        u"测试文本测试。测试文本。", CreateConfigWithAlign(fontSize, textWidth, true, TextDirection::LTR, TextAlign::JUSTIFY));
    auto typDisabled = BuildAndLayout(
        u"测试文本测试。测试文本。", CreateConfigWithAlign(fontSize, textWidth, false, TextDirection::LTR, TextAlign::JUSTIFY));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 2);
    EXPECT_EQ(typDisabled->GetLineCount(), 3);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 139.99989));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 99.99992));
}

/*
 * @tc.name: PunctuationOverflowTest040
 * @tc.desc: Punctuation-only text with short layout width.
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest040, TestSize.Level0)
{
    auto typEnabled = BuildAndLayout(u"。", CreateConfig(fontSize, 1.0, true));
    auto typDisabled = BuildAndLayout(u"。", CreateConfig(fontSize, 1.0, false));
    ASSERT_NE(typEnabled, nullptr);
    ASSERT_NE(typDisabled, nullptr);

    EXPECT_EQ(typEnabled->GetLineCount(), 1);
    EXPECT_EQ(typDisabled->GetLineCount(), 1);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typEnabled->GetLongestLineWithIndent(), 19.99998));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typDisabled->GetLongestLineWithIndent(), 19.99998));
}

// ============================================================================
// Group 7: Punctuation hanging takes priority over ellipsis
// ============================================================================

/*
 * @tc.name: PunctuationOverflowTest041
 * @tc.desc: When punctuation hanging is triggered, ellipsis should not be applied.
 *           Layout with punctuation overflow, measure width, then set ellipsis and
 *           re-layout - width should remain the same (ellipsis not triggered).
 * @tc.type: FUNC
 */
HWTEST_F(PunctuationOverflowTest, PunctuationOverflowTest041, TestSize.Level0)
{
    // Step 1: Layout with punctuation overflow enabled, no ellipsis
    double textWidth = MeasureTextWidth(u"测试文本测试", fontSize);
    TypographyStyle style;
    style.punctuationOverflow = true;
    auto create = TypographyCreate::Create(style, fontCollection_);
    ASSERT_NE(create, nullptr);
    AddTextStyle(create, fontSize);
    create->AppendText(u"测试文本测试。");
    auto typography = create->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(textWidth);

    // Punctuation should hang (1 line), measure the width
    EXPECT_EQ(typography->GetLineCount(), 1);
    double widthWithoutEllipsis = typography->GetLongestLineWithIndent();

    // Step 2: Now set ellipsis and re-layout with same width
    TypographyStyle styleWithEllipsis;
    styleWithEllipsis.punctuationOverflow = true;
    styleWithEllipsis.ellipsis = u"\u2026";
    auto createWithEllipsis = TypographyCreate::Create(styleWithEllipsis, fontCollection_);
    ASSERT_NE(createWithEllipsis, nullptr);
    AddTextStyle(createWithEllipsis, fontSize);
    createWithEllipsis->AppendText(u"测试文本测试。");
    auto typographyWithEllipsis = createWithEllipsis->CreateTypography();
    ASSERT_NE(typographyWithEllipsis, nullptr);
    typographyWithEllipsis->Layout(textWidth);

    // Step 3: Verify - punctuation hanging should take priority over ellipsis
    // Width should not change because punctuation already hangs beyond the line
    double widthWithEllipsis = typographyWithEllipsis->GetLongestLineWithIndent();
    EXPECT_EQ(typographyWithEllipsis->GetLineCount(), 1);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(widthWithEllipsis, widthWithoutEllipsis));
}
} // namespace Rosen
} // namespace OHOS
