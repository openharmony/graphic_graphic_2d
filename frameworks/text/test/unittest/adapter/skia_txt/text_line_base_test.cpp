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
#include "text_line_base.h"
#include "typography.h"
#include "typography_create.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHDrawingTextLineBaseTest : public testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

private:
    // 50 is the width of the layout, just for test
    int layoutWidth_ = 50;
    // 100 is the max lines of the paragraph, just for test
    int maxLines_ = 100;

    std::unique_ptr<Typography> typography_;
    std::vector<std::unique_ptr<TextLineBase>> textLine_;
};

void OHDrawingTextLineBaseTest::SetUp()
{
    TypographyStyle typographyStyle;
    typographyStyle.maxLines = maxLines_;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    ASSERT_NE(fontCollection, nullptr);
    std::shared_ptr<TypographyCreate> typographyCreate = TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    typographyCreate->AppendText(u"Hello World!");
    typography_ = typographyCreate->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    typography_->Layout(layoutWidth_);
    textLine_ = typography_->GetTextLines();
}

void OHDrawingTextLineBaseTest::TearDown()
{
    typography_.reset();
    textLine_.clear();
}

/*
 * @tc.name: OHDrawingTextLineBaseTest003
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingTextLineBaseTest, OHDrawingTextLineBaseTest003, TestSize.Level1)
{
    EXPECT_EQ(textLine_.size(), 2);
    ASSERT_NE(textLine_.at(0), nullptr);
    Drawing::Canvas canvas;
    textLine_.at(0)->Paint(&canvas, 0.0, 0.0);
}

/*
 * @tc.name: OHDrawingTextLineBaseTest004
 * @tc.desc: test for nullptr, only for the branch coverage
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingTextLineBaseTest, OHDrawingTextLineBaseTest004, TestSize.Level1)
{
    auto textLineImpl = std::make_unique<AdapterTxt::TextLineBaseImpl>(nullptr);

    textLineImpl->Paint(nullptr, 0.0, 0.0);
    EXPECT_EQ(textLineImpl->GetGlyphCount(), 0);
    EXPECT_EQ(textLineImpl->GetGlyphRuns().size(), 0);
    EXPECT_EQ(textLineImpl->GetTextRange().rightIndex, 0);
    EXPECT_EQ(textLineImpl->GetTextRange().leftIndex, 0);
}

/*
 * @tc.name: TextLineBaseTest006
 * @tc.desc: branch coverage
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingTextLineBaseTest, OHDrawingTextLineBaseTest006, TestSize.Level1)
{
    std::unique_ptr<TextLineBase> textLineBase = std::make_unique<AdapterTxt::TextLineBaseImpl>(nullptr);

    std::string ellipsisStr;
    std::unique_ptr<TextLineBase> line
        = textLineBase->CreateTruncatedLine(10, OHOS::Rosen::EllipsisModal::HEAD, ellipsisStr);
    EXPECT_EQ(line, nullptr);

    double ascent = 0;
    double descent = 0;
    double leading = 0;
    EXPECT_FLOAT_EQ(textLineBase->GetTypographicBounds(&ascent, &descent, &leading), 0.0);
    EXPECT_FLOAT_EQ(ascent, 0.0);
    EXPECT_FLOAT_EQ(descent, 0.0);
    EXPECT_FLOAT_EQ(leading, 0.0);

    EXPECT_EQ(textLineBase->GetImageBounds().GetLeft(), 0);
    EXPECT_FLOAT_EQ(textLineBase->GetTrailingSpaceWidth(), 0.0);
}
} // namespace Rosen
} // namespace OHOS