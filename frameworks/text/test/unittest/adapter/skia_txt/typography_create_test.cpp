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

#include <codecvt>

#include "gtest/gtest.h"
#include "typography_create.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OH_Drawing_TypographyCreateTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_TypographyCreateTest001
 * @tc.desc: test for constuctor of TypographyCreate
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyCreateTest, OH_Drawing_TypographyCreateTest001, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    EXPECT_NE(fontCollection, nullptr);
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    EXPECT_NE(typographyCreate, nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyCreateTest002
 * @tc.desc: test for constuctor of TypographyCreate
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyCreateTest, OH_Drawing_TypographyCreateTest002, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    OHOS::Rosen::TextStyle txtStyle;
    typographyCreate->PushStyle(txtStyle);
    typographyCreate->PopStyle();
    const char* text = "test";
    const std::u16string wideText =
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(text);
    typographyCreate->AppendText(wideText);
    // 0.0 for unit test
    const OHOS::Rosen::PlaceholderSpan placeholderSpan = {0.0, 0.0,
        PlaceholderVerticalAlignment::OFFSET_AT_BASELINE, TextBaseline::ALPHABETIC, 0.0};
    typographyCreate->AppendPlaceholder(placeholderSpan);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography != nullptr, true);
}

/*
 * @tc.name: OH_Drawing_TypographyCreateTest003
 * @tc.desc: test for constuctor of TypographyCreate
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyCreateTest, OH_Drawing_TypographyCreateTest003, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    EXPECT_NE(fontCollection, nullptr);
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    const char* text = "test";
    const std::u16string wideText =
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(text);
    typographyCreate->AppendText(wideText);

    std::unique_ptr<OHOS::Rosen::LineTypography> graphy = typographyCreate->CreateLineTypography();
    EXPECT_NE(graphy, nullptr);
    auto paragraph = graphy->GetTempTypography();
    EXPECT_NE(paragraph, nullptr);
    EXPECT_EQ(graphy->GetUnicodeSize(), 4);
}

/*
 * @tc.name: OH_Drawing_TypographyCreateTest004
 * @tc.desc: test for constuctor of TypographyCreate
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyCreateTest, OH_Drawing_TypographyCreateTest004, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    EXPECT_NE(fontCollection, nullptr);
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);

    std::unique_ptr<OHOS::Rosen::LineTypography> graphy = typographyCreate->CreateLineTypography();
    EXPECT_EQ(graphy, nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyCreateTest005
 * @tc.desc: test for invalid symbolId
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyCreateTest, OH_Drawing_TypographyCreateTest005, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    ASSERT_NE(fontCollection, nullptr);
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle txtStyle;
    typographyCreate->PushStyle(txtStyle);
    typographyCreate->AppendSymbol(0);

    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(100); // 100.0 for unit test
    EXPECT_LT(typography->GetActualWidth(), 0);
}
} // namespace Rosen
} // namespace OHOS