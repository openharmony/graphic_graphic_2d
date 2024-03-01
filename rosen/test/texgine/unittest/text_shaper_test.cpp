/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "font_collection.h"
#include "measurer.h"
#include "mock/mock_any_span.h"
#include "mock/mock_measurer.h"
#include "param_test_macros.h"
#include "texgine_exception.h"
#include "texgine/font_providers.h"
#include "texgine_text_blob.h"
#include "texgine_text_blob_builder.h"
#include "texgine/typography_types.h"
#include "text_shaper.h"
#include "typeface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct MockVars {
    std::vector<uint16_t> catchedBufferGlyphs;
    std::vector<float> catchedBufferPos;
    std::vector<std::string> catchedGenerateFontCollectionFamilies;

    std::shared_ptr<TexgineTextBlob> retvalTextBlobBuilderMake = nullptr;
    std::shared_ptr<FontCollection> retvalGenerateFontCollection =
        std::make_shared<FontCollection>(std::vector<std::shared_ptr<VariantFontStyleSet>>{});
    std::unique_ptr<MockMeasurer> retvalMeasurerCreate = std::make_unique<MockMeasurer>();
} g_tsMockvars;

void InitTsMockVars(struct MockVars &&vars)
{
    g_tsMockvars = std::move(vars);
}


std::shared_ptr<FontCollection> FontProviders::GenerateFontCollection(
    const std::vector<std::string> &families) const noexcept(true)
{
    g_tsMockvars.catchedGenerateFontCollectionFamilies = families;
    return g_tsMockvars.retvalGenerateFontCollection;
}

std::unique_ptr<Measurer> Measurer::Create(const std::vector<uint16_t> &text,
    const FontCollection &fontCollection)
{
    return std::move(g_tsMockvars.retvalMeasurerCreate);
}

struct TextSpanInfo {
    CharGroups cgs_ = CharGroups::CreateEmpty();
};

class ControllerForTest {
public:
    static std::shared_ptr<TextSpan> GenerateTextSpan(TextSpanInfo info)
    {
        auto ts = std::make_shared<TextSpan>();
        ts->cgs_ = info.cgs_;
        return ts;
    }
};

auto GenerateTextSpan(TextSpanInfo info)
{
    return ControllerForTest::GenerateTextSpan(info);
}

class TextShaperTest : public testing::TestWithParam<std::shared_ptr<TextSpan>> {
public:
    static void SetUpTestCase()
    {
        // {1, 1, 0, 0.1, 0.1} is {code point, advanceX, advanceY, offsetX, offsetY}
        cgs1_.PushBack({ .glyphs = { {1, 1, 0, 0.1, 0.1}, {2, 1, 0, 0.2, 0.2} }, .visibleWidth = 2 });
        cgs2_.PushBack({ .glyphs = { {1, 1, 0, 0.1, 0.1} }, .visibleWidth = 1 });
        cgs2_.PushBack({ .glyphs = { {2, 1, 0, 0.2, 0.2} }, .visibleWidth = 1 });
    }

    static inline CharGroups cgs1_ = CharGroups::CreateEmpty();
    static inline CharGroups cgs2_ = CharGroups::CreateEmpty();
    static inline std::shared_ptr<TypographyStyle> ysNormal_ = std::make_shared<TypographyStyle>();
    static inline std::shared_ptr<TypographyStyle> ysNoProvider_ = std::make_shared<TypographyStyle>();
};

/**
 * @tc.name: DoShape1
 * @tc.desc: Verify the DoShape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, DoShape1, TestSize.Level1)
{
    TextShaper shaper;
    auto fp = FontProviders::Create();
    std::shared_ptr<TextSpan> tsNullptr;
    auto tsNormal = TextSpan::MakeFromText("normal");
    ASSERT_EXCEPTION(ExceptionType::INVALID_ARGUMENT, shaper.DoShape(tsNullptr, {}, {}, fp));
    ASSERT_EXCEPTION(ExceptionType::INVALID_ARGUMENT, shaper.DoShape(tsNormal, {}, {}, nullptr));
}

/**
 * @tc.name: DoShape2
 * @tc.desc: Verify the DoShape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, DoShape2, TestSize.Level1)
{
    InitTsMockVars({});
    EXPECT_CALL(*g_tsMockvars.retvalMeasurerCreate, Measure).Times(1).WillOnce(testing::Return(0));
    TypographyStyle ys;
    ys.fontFamilies = {"Roboto"};
    auto span = GenerateTextSpan({.cgs_ = cgs1_});

    EXPECT_NO_THROW({
        TextShaper shaper;
        shaper.DoShape(span, {}, ys, FontProviders::Create());
        ASSERT_EQ(ys.fontFamilies, g_tsMockvars.catchedGenerateFontCollectionFamilies);
    });
}

/**
 * @tc.name: DoShape3
 * @tc.desc: Verify the DoShape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, DoShape3, TestSize.Level1)
{
    InitTsMockVars({});
    EXPECT_CALL(*g_tsMockvars.retvalMeasurerCreate, Measure).Times(1).WillOnce(testing::Return(0));
    TextStyle style = {.fontFamilies = {"Sans"}};
    auto span = GenerateTextSpan({.cgs_ = cgs1_});

    EXPECT_NO_THROW({
        TextShaper shaper;
        shaper.DoShape(span, style, {}, FontProviders::Create());
        ASSERT_EQ(style.fontFamilies , g_tsMockvars.catchedGenerateFontCollectionFamilies);
    });
}

/**
 * @tc.name: DoShape4
 * @tc.desc: Verify the DoShape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, DoShape4, TestSize.Level1)
{
    InitTsMockVars({.retvalGenerateFontCollection = nullptr});
    auto span = GenerateTextSpan({});

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.DoShape(span, {}, {}, FontProviders::Create());
        ASSERT_EQ(ret, 1);
    });
}

/**
 * @tc.name: DoShape5
 * @tc.desc: Verify the DoShape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, DoShape5, TestSize.Level1)
{
    InitTsMockVars({});
    EXPECT_CALL(*g_tsMockvars.retvalMeasurerCreate, Measure).Times(1).WillOnce(testing::Return(1));
    auto span = GenerateTextSpan({.cgs_ = cgs1_});

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.DoShape(span, {}, {}, FontProviders::Create());
        ASSERT_EQ(ret, 1);
    });
}

/**
 * @tc.name: DoShape6
 * @tc.desc: Verify the DoShape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, DoShape6, TestSize.Level1)
{
    InitTsMockVars({});
    EXPECT_CALL(*g_tsMockvars.retvalMeasurerCreate, Measure).Times(1).WillOnce(testing::Return(0));
    auto span = GenerateTextSpan({.cgs_ = cgs1_});

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.DoShape(span, {}, {}, FontProviders::Create());
        ASSERT_EQ(ret, 0);
    });
}

/**
 * @tc.name: GenerateTextBlob1
 * @tc.desc: Verify the GenerateTextBlob
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, GenerateTextBlob1, TestSize.Level1)
{
    double spanWidth = 0.0;
    std::vector<double> glyphWidths;
    TextShaper shaper;
    TexgineFont font;
    ASSERT_EXCEPTION(ExceptionType::API_FAILED, shaper.GenerateTextBlob(font,
        CharGroups::CreateEmpty(), spanWidth, glyphWidths));
}

/**
 * @tc.name: GenerateTextBlob2
 * @tc.desc: Verify the GenerateTextBlob
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, GenerateTextBlob2, TestSize.Level1)
{
    EXPECT_NO_THROW({
        double spanWidth = 0.0;
        std::vector<double> glyphWidths;
        TextShaper shaper;
        TexgineFont font;
        shaper.GenerateTextBlob(font, cgs1_, spanWidth, glyphWidths);
        ASSERT_EQ(spanWidth, 2);
        // 2.0 is glyph width
        ASSERT_EQ(glyphWidths, (std::vector<double>{2.0}));
        // {0.1, -0.1, 1.2, -0.2} is the position of code point in text blob
        ASSERT_EQ(g_tsMockvars.catchedBufferPos, (std::vector<float>{0.1, -0.1, 1.2, -0.2}));
        // {1, 2} is the glyph in text blob
        ASSERT_EQ(g_tsMockvars.catchedBufferGlyphs, (std::vector<uint16_t>{1, 2}));
    });
}

/**
 * @tc.name: GenerateTextBlob3
 * @tc.desc: Verify the GenerateTextBlob
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, GenerateTextBlob3, TestSize.Level1)
{
    EXPECT_NO_THROW({
        double spanWidth = 0.0;
        std::vector<double> glyphWidths;
        TextShaper shaper;
        TexgineFont font;
        shaper.GenerateTextBlob(font, cgs2_, spanWidth, glyphWidths);
        ASSERT_EQ(spanWidth, 2);
        ASSERT_EQ(glyphWidths, (std::vector<double>{1.0, 1.0}));
        ASSERT_EQ(g_tsMockvars.catchedBufferPos, (std::vector<float>{0.1, -0.1, 1.2, -0.2}));
        ASSERT_EQ(g_tsMockvars.catchedBufferGlyphs, (std::vector<uint16_t>{1, 2}));
    });
}

/**
 * @tc.name: Shape1
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, Shape1, TestSize.Level1)
{
    std::shared_ptr<TextSpan> tsNullptr = nullptr;
    std::shared_ptr<AnySpan> asNullptr = nullptr;
    TextShaper shaper;
    ASSERT_EXCEPTION(ExceptionType::INVALID_ARGUMENT, shaper.Shape(tsNullptr, {}, FontProviders::Create()));
    ASSERT_EXCEPTION(ExceptionType::INVALID_ARGUMENT, shaper.Shape(asNullptr, {}, FontProviders::Create()));
}

/**
 * @tc.name: Shape2
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, Shape2, TestSize.Level1)
{
    InitTsMockVars({.retvalGenerateFontCollection = nullptr});

    EXPECT_NO_THROW({
        std::shared_ptr<MockAnySpan> mas = std::make_shared<MockAnySpan>();
        TextShaper shaper;
        auto ret = shaper.Shape(mas, {}, FontProviders::Create());
        ASSERT_EQ(ret, 0);
    });
}

/**
 * @tc.name: Shape3
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, Shape3, TestSize.Level1)
{
    InitTsMockVars({.retvalGenerateFontCollection = nullptr});

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.Shape(GenerateTextSpan({}), {}, FontProviders::Create());
        ASSERT_EQ(ret, 1);
    });
}

/**
 * @tc.name: Shape4
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, Shape4, TestSize.Level1)
{
    InitTsMockVars({});
    TextShaper shaper;
    ASSERT_EXCEPTION(ExceptionType::INVALID_CHAR_GROUPS,
        shaper.Shape(GenerateTextSpan({.cgs_ = {}}), {}, FontProviders::Create()));

    InitTsMockVars({});
    EXPECT_CALL(*g_tsMockvars.retvalMeasurerCreate, Measure).Times(1).WillOnce(testing::Return(0));
    cgs1_.Get(0).typeface = nullptr;

    ASSERT_EXCEPTION(ExceptionType::INVALID_ARGUMENT,
        shaper.Shape(GenerateTextSpan({.cgs_ = cgs1_}), {}, FontProviders::Create()));
}

/**
 * @tc.name: Shape5
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, Shape5, TestSize.Level1)
{
    InitTsMockVars({});
    EXPECT_CALL(*g_tsMockvars.retvalMeasurerCreate, Measure).Times(1).WillOnce(testing::Return(0));
    cgs1_.Get(0).typeface = std::make_unique<Typeface>(nullptr);

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.Shape(GenerateTextSpan({.cgs_ = cgs1_}), {}, FontProviders::Create());
        ASSERT_EQ(ret, 1);
    });
}

/**
 * @tc.name: Shape6
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(TextShaperTest, Shape6, TestSize.Level1)
{
    InitTsMockVars({});
    EXPECT_CALL(*g_tsMockvars.retvalMeasurerCreate, Measure).Times(1).WillOnce(testing::Return(0));
    g_tsMockvars.retvalTextBlobBuilderMake = std::make_shared<TexgineTextBlob>();

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.Shape(GenerateTextSpan({.cgs_ = cgs1_}), {}, FontProviders::Create());
        ASSERT_EQ(ret, 0);
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
