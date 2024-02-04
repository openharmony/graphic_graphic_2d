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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS