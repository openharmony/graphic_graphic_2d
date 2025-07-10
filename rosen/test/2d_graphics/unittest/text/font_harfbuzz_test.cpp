/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "text/font.h"
#include "text/font_harfbuzz.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontHarfbuzzTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FontHarfbuzzTest::SetUpTestCase() {}
void FontHarfbuzzTest::TearDownTestCase() {}
void FontHarfbuzzTest::SetUp() {}
void FontHarfbuzzTest::TearDown() {}

/**
 * @tc.name: FontHarfbuzzSetNominalGlyphTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:ICG6L3
 * @tc.author:
 */
HWTEST_F(FontHarfbuzzTest, FontHarfbuzzSetNominalGlyphTest001, TestSize.Level1)
{
    Font font = Font();
    hb_codepoint_t unicode = 0;
    hb_codepoint_t glyph = 0;
    bool ret = FontHarfbuzz::SetNominalGlyph(nullptr, (void*)&font, unicode, &glyph, nullptr);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: FontHarfbuzzSetGlyphAdvanceTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:ICG6L3
 * @tc.author:
 */
HWTEST_F(FontHarfbuzzTest, FontHarfbuzzSetGlyphAdvanceTest001, TestSize.Level1)
{
    Font font = Font();
    hb_codepoint_t glyph = 0;
    hb_position_t position = FontHarfbuzz::SetGlyphAdvance(nullptr, (void*)&font, glyph, nullptr);
    ASSERT_NE(position, 0);
    font.SetSubpixel(true);
    position = FontHarfbuzz::SetGlyphAdvance(nullptr, (void*)&font, glyph, nullptr);
    ASSERT_NE(position, 0);
}

/**
 * @tc.name: FontHarfbuzzGetGlyphExtentsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:ICG6L3
 * @tc.author:
 */
HWTEST_F(FontHarfbuzzTest, FontHarfbuzzGetGlyphExtentsTest001, TestSize.Level1)
{
    Font font = Font();
    hb_codepoint_t glyph = 0;
    hb_glyph_extents_t extents = { 0 };
    bool ret = FontHarfbuzz::GetGlyphExtents(nullptr, (void*)&font, glyph, &extents, nullptr);
    ASSERT_EQ(ret, true);
    font.SetSubpixel(true);
    ret = FontHarfbuzz::GetGlyphExtents(nullptr, (void*)&font, glyph, &extents, nullptr);
    ASSERT_EQ(ret, true);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
