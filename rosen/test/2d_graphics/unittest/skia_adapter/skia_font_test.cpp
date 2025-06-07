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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_font.h"
#include "text/font.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaFontTest::SetUpTestCase() {}
void SkiaFontTest::TearDownTestCase() {}
void SkiaFontTest::SetUp() {}
void SkiaFontTest::TearDown() {}

/**
 * @tc.name: SkiaFont001
 * @tc.desc: Test SkiaFont's constructor
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, SkiaFont001, TestSize.Level1)
{
    std::shared_ptr<SkiaFont> skiaFont = std::make_shared<SkiaFont>(nullptr, 1, 1, 1);
    ASSERT_TRUE(skiaFont != nullptr);
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    ASSERT_TRUE(skiaFont != nullptr);
}

/**
 * @tc.name: SetEdging001
 * @tc.desc: Test SetEdging
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, SetEdging001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    skiaFont->SetEdging(FontEdging::ALIAS);
    ASSERT_TRUE(skiaFont->GetFont().getEdging() == SkFont::Edging::kAlias);
}

/**
 * @tc.name: SetSubpixel001
 * @tc.desc: Test SetSubpixel
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, SetSubpixel001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    skiaFont->SetSubpixel(true);
    ASSERT_TRUE(skiaFont->GetFont().isSubpixel());
}

/**
 * @tc.name: SetHinting001
 * @tc.desc: Test SetHinting
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, SetHinting001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    skiaFont->SetHinting(FontHinting::NONE);
    ASSERT_TRUE(skiaFont->GetFont().getHinting() == SkFontHinting::kNone);
}

/**
 * @tc.name: SetTypeface001
 * @tc.desc: Test SetTypeface
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, SetTypeface001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    skiaFont->SetTypeface(nullptr);
    ASSERT_TRUE(skiaFont->GetTypeface() != nullptr);
}

/**
 * @tc.name: SetScaleX001
 * @tc.desc: Test SetScaleX
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, SetScaleX001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    skiaFont->SetScaleX(1);
    ASSERT_TRUE(skiaFont->GetFont().getScaleX() == 1);
}

/**
 * @tc.name: SetLinearMetrics001
 * @tc.desc: Test SetLinearMetrics
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, SetLinearMetrics001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    skiaFont->SetLinearMetrics(true);
    ASSERT_TRUE(skiaFont->GetFont().isLinearMetrics());
}

/**
 * @tc.name: GetMetrics001
 * @tc.desc: Test GetMetrics
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, GetMetrics001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    auto metrics = skiaFont->GetMetrics(nullptr);
    ASSERT_TRUE(metrics > 0);
}

/**
 * @tc.name: GetWidths001
 * @tc.desc: Test GetWidths
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, GetWidths001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    uint16_t glyphs[] = { 0, 0 };
    scalar widths[] = { 0, 0 };
    skiaFont->GetWidths(glyphs, 2, widths); // 2:count
    skiaFont->GetWidths(glyphs, 2, widths, nullptr); // 2:count
    Rect rect1;
    Rect rect2;
    Rect bounds[] = { rect1, rect2 };
    skiaFont->GetWidths(glyphs, 2, widths, bounds);
    ASSERT_TRUE(rect1.GetWidth() >= 0);
}

/**
 * @tc.name: GetSize001
 * @tc.desc: Test GetSize
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, GetSize001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    auto size = skiaFont->GetSize();
    ASSERT_TRUE(size > 0);
}

/**
 * @tc.name: GetTypeface001
 * @tc.desc: Test GetTypeface
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, GetTypeface001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    auto typeface2 = skiaFont->GetTypeface();
    ASSERT_TRUE(typeface2 != nullptr);
}

/**
 * @tc.name: MeasureText001
 * @tc.desc: Test MeasureText
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(SkiaFontTest, MeasureText001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    auto size = skiaFont->MeasureText("11", 2, TextEncoding::UTF8, nullptr); // 2:byteLength
    ASSERT_TRUE(size > 0);
}

/**
 * @tc.name: MeasureText002
 * @tc.desc: Test MeasureText
 * @tc.type: FUNC
 * @tc.require: AR20250515745872
 */
HWTEST_F(SkiaFontTest, MeasureText002, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    auto size = skiaFont->MeasureText(nullptr, 2, TextEncoding::UTF8, nullptr, nullptr, nullptr); // 2:byteLength
    EXPECT_EQ(static_cast<int>(size), 0);

    size = skiaFont->MeasureText("11", 2, TextEncoding::UTF8, nullptr, nullptr, nullptr); // 2:byteLength
    EXPECT_EQ((size - 1.1399231) < 1e-6, true);

    Brush brush;
    size = skiaFont->MeasureText("11", 2, TextEncoding::UTF8, nullptr, &brush, nullptr); // 2:byteLength
    EXPECT_EQ((size - 1.1399231) < 1e-6, true);

    Pen pen;
    size = skiaFont->MeasureText("11", 2, TextEncoding::UTF8, nullptr, nullptr, &pen); // 2:byteLength
    EXPECT_EQ((size - 1.13999796) < 1e-6, true);
}

/**
 * @tc.name: GetWidthsBoundsTest001
 * @tc.desc: Test GetWidthsBounds
 * @tc.type: FUNC
 * @tc.require: AR20250515745872
 */
HWTEST_F(SkiaFontTest, GetWidthsBoundsTest001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    uint16_t glyphs[] = { 0, 0 };
    scalar widths[] = { 0, 0 };
    Rect rect1;
    Rect rect2;
    Rect bounds[] = { rect1, rect2 };
    skiaFont->GetWidthsBounds(glyphs, 0, widths, nullptr, nullptr, nullptr);
    EXPECT_EQ(static_cast<int>(widths[0]), 0);

    skiaFont->GetWidthsBounds(glyphs, 2, widths, nullptr, nullptr, nullptr); // 2:count
    EXPECT_EQ(static_cast<int>(widths[0]), 0);

    skiaFont->GetWidthsBounds(glyphs, 2, widths, bounds, nullptr, nullptr); // 2:count
    EXPECT_EQ(rect1.GetWidth(), 0);

    Brush brush;
    skiaFont->GetWidthsBounds(glyphs, 2, widths, bounds, &brush, nullptr); // 2:count
    EXPECT_EQ(rect1.GetWidth(), 0);

    Pen pen;
    skiaFont->GetWidthsBounds(glyphs, 2, widths, bounds, nullptr, &pen); // 2:count
    EXPECT_EQ(rect1.GetWidth(), 0);
}

/**
 * @tc.name: GetPosTest001
 * @tc.desc: Test GetPos
 * @tc.type: FUNC
 * @tc.require: AR20250515745872
 */
HWTEST_F(SkiaFontTest, GetPosTest001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    uint16_t glyphs[] = { 0, 0 };
    Point points[] = { {0, 0}, {0, 0} };
    skiaFont->GetPos(glyphs, 0, points);
    EXPECT_EQ(points[0].GetX(), 0);
    EXPECT_EQ(points[0].GetY(), 0);
    Point origin(10, 10);
    skiaFont->GetPos(glyphs, 2, points, origin); // 2:count
    EXPECT_EQ(points[0].GetX(), 10);
    EXPECT_EQ(points[0].GetY(), 10);
}

/**
 * @tc.name: GetSpacingTest001
 * @tc.desc: Test GetSpacing
 * @tc.type: FUNC
 * @tc.require: AR20250515745872
 */
HWTEST_F(SkiaFontTest, GetSpacingTest001, TestSize.Level1)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    auto skiaFont = std::make_shared<SkiaFont>(typeface, 1, 1, 1);
    auto space = skiaFont->GetSpacing();
    EXPECT_EQ(static_cast<int>(space), 1);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS