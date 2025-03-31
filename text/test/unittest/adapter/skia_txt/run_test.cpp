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
#include "run_impl.h"
#include "text_line_base.h"
#include "typography.h"
#include "typography_create.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHDrawingRunTest : public testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

private:
    // 50 is the width of the layout, just for test
    int layoutWidth_ = 50;
    // this is the default font family name, just for test
    std::string familyName_ = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x2d, 0x53, 0x61, 0x6e, 0x73 };

    std::vector<std::unique_ptr<OHOS::Rosen::Run>> runs_;
    std::unique_ptr<Typography> typography_;
};

void OHDrawingRunTest::SetUp()
{
    TypographyStyle ts;
    std::shared_ptr<FontCollection> fc = FontCollection::From(std::make_shared<txt::FontCollection>());
    ASSERT_NE(fc, nullptr);
    auto tc = OHOS::Rosen::TypographyCreate::Create(ts, fc);
    ASSERT_NE(tc, nullptr);
    tc->AppendText(u"12345");
    typography_ = tc->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    typography_->Layout(layoutWidth_);
    std::vector<std::unique_ptr<TextLineBase>> textLines = typography_->GetTextLines();
    ASSERT_EQ(textLines.size(), 1);
    runs_ = textLines.at(0)->GetGlyphRuns();
}

void OHDrawingRunTest::TearDown()
{
    typography_.reset();
    runs_.clear();
}

/*
 * @tc.name: RunTest001
 * @tc.desc: test for GetFont
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, RunTest001, TestSize.Level1)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_.at(0), nullptr);
    EXPECT_EQ(runs_[0]->GetFont().GetTypeface()->GetFamilyName(), familyName_);
}

/*
 * @tc.name: RunTest002
 * @tc.desc: test for GetGlyphCount
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, RunTest002, TestSize.Level1)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_.at(0), nullptr);
    EXPECT_EQ(runs_[0]->GetGlyphCount(), 5);
}

/*
 * @tc.name: RunTest003
 * @tc.desc: test for GetGlyphs
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, RunTest003, TestSize.Level1)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_.at(0), nullptr);
    auto glyphs = runs_[0]->GetGlyphs();
    EXPECT_NE(glyphs[0], 0);
}

/*
 * @tc.name: RunTest004
 * @tc.desc: test for GetPositions
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, RunTest004, TestSize.Level1)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_[0], nullptr);
    EXPECT_EQ(runs_[0]->GetPositions().size(), 5);
    EXPECT_EQ(runs_[0]->GetPositions()[0].GetX(), 0);
}

/*
 * @tc.name: RunTest005
 * @tc.desc: test for GetOffsets
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, RunTest005, TestSize.Level1)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_[0], nullptr);
    EXPECT_EQ(runs_[0]->GetOffsets().size(), 5);
    EXPECT_EQ(runs_[0]->GetOffsets()[0].GetX(), 0);
}

/*
 * @tc.name: RunTest006
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, RunTest006, TestSize.Level1)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_[0], nullptr);
    runs_[0]->Paint(nullptr, 0, 0);
    Drawing::Canvas canvas;
    runs_[0]->Paint(&canvas, 0, 0);
}

/*
 * @tc.name: RunTest007
 * @tc.desc: test for nullptr, only for the branch coverage
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, RunTest007, TestSize.Level1)
{
    std::unique_ptr<AdapterTxt::RunImpl> runNull = std::make_unique<AdapterTxt::RunImpl>(nullptr);
    EXPECT_EQ(runNull->GetFont().GetSize(), Drawing::Font().GetSize());
    EXPECT_EQ(runNull->GetGlyphCount(), 0);
    EXPECT_EQ(runNull->GetGlyphs(), std::vector<uint16_t>());
    EXPECT_EQ(runNull->GetOffsets(), std::vector<Drawing::Point>());
    EXPECT_EQ(runNull->GetPositions(), std::vector<Drawing::Point>());
}

/*
 * @tc.name: RunTest007
 * @tc.desc: branch coverage
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, RunTest008, TestSize.Level1)
{
    EXPECT_EQ(runs_[0]->GetGlyphs(0, 12).size(), 5);
    EXPECT_EQ(runs_[0]->GetPositions(0, 10).size(), 5);

    uint64_t location = 0;
    uint64_t length = 0;
    runs_[0]->GetStringRange(&location, &length);
    EXPECT_EQ(location, 0);
    EXPECT_EQ(length, 5);

    EXPECT_EQ(runs_[0]->GetStringIndices(0, 10).size(), 5);
}

/*
 * @tc.name: RunTest009
 * @tc.desc: branch coverage
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, RunTest009, TestSize.Level1)
{
    EXPECT_EQ(runs_[0]->GetGlyphs(-1, 12), std::vector<uint16_t>());
    EXPECT_EQ(runs_[0]->GetGlyphs(0, -1), std::vector<uint16_t>());
    EXPECT_EQ(runs_[0]->GetPositions(-1, 10), std::vector<Drawing::Point>());
    EXPECT_EQ(runs_[0]->GetPositions(0, -1), std::vector<Drawing::Point>());

    uint64_t location = 0;
    uint64_t length = 0;
    runs_[0]->GetStringRange(nullptr, &length);
    EXPECT_EQ(length, 0);

    runs_[0]->GetStringRange(&location, nullptr);
    EXPECT_EQ(location, 0);

    std::unique_ptr<OHOS::Rosen::Run> runNull = std::make_unique<AdapterTxt::RunImpl>(nullptr);
    location = 10;
    length = 10;
    runNull->GetStringRange(&location, &length);
    EXPECT_EQ(location, 0);
    EXPECT_EQ(length, 0);

    EXPECT_EQ(runs_[0]->GetStringIndices(-1, 10), std::vector<uint64_t>());
    EXPECT_EQ(runs_[0]->GetStringIndices(0, -1), std::vector<uint64_t>());
}
} // namespace Rosen
} // namespace OHOS