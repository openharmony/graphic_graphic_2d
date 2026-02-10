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

#include <gtest/gtest.h>
#include "typography_create.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class TypographyEllipsisTest : public testing::Test {
};

/*
 * @tc.name: TypographyEllipsisTest001
 * @tc.desc: Test that different ellipsis strings create different typography layouts
 * @tc.type: FUNC
 */
HWTEST_F(TypographyEllipsisTest, TypographyEllipsisTest001, TestSize.Level0)
{
    // Arrange
    std::shared_ptr<FontCollection> fontCollection = FontCollection::Create();
    ASSERT_NE(fontCollection, nullptr);

    TypographyStyle style;
    style.maxLines = 1;
    style.textAlign = TextAlign::LEFT;

    // Create first typography with ellipsis "..."
    std::unique_ptr<TypographyCreate> typographyCreate1 = TypographyCreate::Create(style, fontCollection);
    ASSERT_NE(typographyCreate1, nullptr);

    TextStyle textStyle;
    textStyle.fontSize = 20.0;
    typographyCreate1->PushStyle(textStyle);

    const std::u16string text = u"This is a very long text that should be truncated";
    typographyCreate1->AppendText(text);
    typographyCreate1->PopStyle();

    auto typography1 = typographyCreate1->CreateTypography();
    ASSERT_NE(typography1, nullptr);

    typography1->Layout(100.0);
    auto longestLine1 = typography1->GetLongestLineWithIndent();

    // Create second typography with ellipsis "......" (different ellipsis)
    TypographyStyle style2;
    style2.maxLines = 1;
    style2.textAlign = TextAlign::LEFT;
    style2.ellipsis = u"......";

    std::unique_ptr<TypographyCreate> typographyCreate2 = TypographyCreate::Create(style2, fontCollection);
    ASSERT_NE(typographyCreate2, nullptr);

    typographyCreate2->PushStyle(textStyle);
    typographyCreate2->AppendText(text);
    typographyCreate2->PopStyle();

    auto typography2 = typographyCreate2->CreateTypography();
    ASSERT_NE(typography2, nullptr);

    typography2->Layout(100.0);
    auto longestLine2 = typography2->GetLongestLineWithIndent();

    // The two typographies should have different longest lines due to different ellipsis lengths
    EXPECT_LT(longestLine1, longestLine2);
}

/*
 * @tc.name: TypographyEllipsisTest002
 * @tc.desc: Test that changing ellipsis string updates the layout correctly
 * @tc.type: FUNC
 */
HWTEST_F(TypographyEllipsisTest, TypographyEllipsisTest002, TestSize.Level0)
{
    // Arrange
    std::shared_ptr<FontCollection> fontCollection = FontCollection::Create();
    ASSERT_NE(fontCollection, nullptr);

    TypographyStyle style;
    style.maxLines = 1;
    style.ellipsis = u"...";

    std::unique_ptr<TypographyCreate> typographyCreate = TypographyCreate::Create(style, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);

    TextStyle textStyle;
    textStyle.fontSize = 20.0;
    typographyCreate->PushStyle(textStyle);

    const std::u16string text = u"Not short text, test ellipsis.";
    typographyCreate->AppendText(text);
    typographyCreate->PopStyle();

    // Act - Create first typography
    auto typography1 = typographyCreate->CreateTypography();
    ASSERT_NE(typography1, nullptr);
    typography1->Layout(100.0);
    auto longestLine1 = typography1->GetLongestLineWithIndent();

    // Create second typography with different ellipsis
    TypographyStyle style2;
    style2.maxLines = 1;
    style2.ellipsis = u"......";

    std::unique_ptr<TypographyCreate> typographyCreate2 = TypographyCreate::Create(style2, fontCollection);
    ASSERT_NE(typographyCreate2, nullptr);

    typographyCreate2->PushStyle(textStyle);
    typographyCreate2->AppendText(text);
    typographyCreate2->PopStyle();

    auto typography2 = typographyCreate2->CreateTypography();
    ASSERT_NE(typography2, nullptr);
    typography2->Layout(100.0);
    auto longestLine2 = typography2->GetLongestLineWithIndent();

    // Verify that different ellipsis strings result in different longest lines
    // The cache should treat them as different due to different ellipsis
    EXPECT_LT(longestLine1, longestLine2);
}

/*
 * @tc.name: TypographyEllipsisTest003
 * @tc.desc: Test that same ellipsis string uses cache efficiently
 * @tc.type: FUNC
 */
HWTEST_F(TypographyEllipsisTest, TypographyEllipsisTest003, TestSize.Level0)
{
    // Arrange
    std::shared_ptr<FontCollection> fontCollection = FontCollection::Create();
    ASSERT_NE(fontCollection, nullptr);

    TypographyStyle style;
    style.maxLines = 1;
    style.ellipsis = u"...";

    TextStyle textStyle;
    textStyle.fontSize = 20.0;

    const std::u16string text = u"Test text for caching";

    // Create first typography with ellipsis "..."
    std::unique_ptr<TypographyCreate> typographyCreate1 = TypographyCreate::Create(style, fontCollection);
    ASSERT_NE(typographyCreate1, nullptr);

    typographyCreate1->PushStyle(textStyle);
    typographyCreate1->AppendText(text);
    typographyCreate1->PopStyle();

    auto typography1 = typographyCreate1->CreateTypography();
    ASSERT_NE(typography1, nullptr);
    typography1->Layout(100.0);
    auto longestLine1 = typography1->GetLongestLineWithIndent();

    // Create second typography with same ellipsis "..."
    std::unique_ptr<TypographyCreate> typographyCreate2 = TypographyCreate::Create(style, fontCollection);
    ASSERT_NE(typographyCreate2, nullptr);

    typographyCreate2->PushStyle(textStyle);
    typographyCreate2->AppendText(text);
    typographyCreate2->PopStyle();

    auto typography2 = typographyCreate2->CreateTypography();
    ASSERT_NE(typography2, nullptr);
    typography2->Layout(100.0);
    auto longestLine2 = typography2->GetLongestLineWithIndent();

    // When ellipsis is the same, cache should work and produce identical results
    EXPECT_FLOAT_EQ(longestLine1, longestLine2);
}

/*
 * @tc.name: TypographyEllipsisTest004
 * @tc.desc: Test that different ellipsis strings result in different longest line widths after cache invalidation
 * @tc.type: FUNC
 */
HWTEST_F(TypographyEllipsisTest, TypographyEllipsisTest004, TestSize.Level0)
{
    // Arrange
    std::shared_ptr<FontCollection> fontCollection = FontCollection::Create();
    ASSERT_NE(fontCollection, nullptr);

    TypographyStyle style;
    style.maxLines = 1;
    style.textAlign = TextAlign::LEFT;

    TextStyle textStyle;
    textStyle.fontSize = 20.0f;
    const std::u16string text = u"This is a very long text that will definitely be truncated with ellipsis";

    // Create typography with short ellipsis "..."
    style.ellipsis = u"...";
    std::unique_ptr<TypographyCreate> typographyCreate1 = TypographyCreate::Create(style, fontCollection);
    ASSERT_NE(typographyCreate1, nullptr);
    typographyCreate1->PushStyle(textStyle);
    typographyCreate1->AppendText(text);
    typographyCreate1->PopStyle();

    auto typography1 = typographyCreate1->CreateTypography();
    ASSERT_NE(typography1, nullptr);
    typography1->Layout(200.0);
    auto longestLine1 = typography1->GetLongestLineWithIndent();

    // Create typography with longer ellipsis "------"
    style.ellipsis = u"------";
    std::unique_ptr<TypographyCreate> typographyCreate2 = TypographyCreate::Create(style, fontCollection);
    ASSERT_NE(typographyCreate2, nullptr);
    typographyCreate2->PushStyle(textStyle);
    typographyCreate2->AppendText(text);
    typographyCreate2->PopStyle();

    auto typography2 = typographyCreate2->CreateTypography();
    ASSERT_NE(typography2, nullptr);
    typography2->Layout(200.0);
    auto longestLine2 = typography2->GetLongestLineWithIndent();

    // Create typography with no ellipsis
    style.ellipsis = u"";
    std::unique_ptr<TypographyCreate> typographyCreate3 = TypographyCreate::Create(style, fontCollection);
    ASSERT_NE(typographyCreate3, nullptr);
    typographyCreate3->PushStyle(textStyle);
    typographyCreate3->AppendText(text);
    typographyCreate3->PopStyle();

    auto typography3 = typographyCreate3->CreateTypography();
    ASSERT_NE(typography3, nullptr);
    typography3->Layout(200.0);
    auto longestLine3 = typography3->GetLongestLineWithIndent();

    // Different ellipsis should result in different longest line widths
    EXPECT_GT(longestLine1, longestLine2);
    EXPECT_GT(longestLine2, longestLine3);
    EXPECT_GT(longestLine1, 0.0);
    EXPECT_GT(longestLine2, 0.0);
    EXPECT_GT(longestLine3, 0.0);
}
} // namespace Rosen
} // namespace OHOS
