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
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::unique_ptr<Typography> typography = nullptr;
    static inline std::vector<std::unique_ptr<OHOS::Rosen::Run>> runs;
    static inline std::unique_ptr<OHOS::Rosen::Run> runNull = std::make_unique<AdapterTxt::RunImpl>(nullptr);
};

void OHDrawingRunTest::SetUpTestCase()
{
    TypographyStyle ts;
    std::shared_ptr<FontCollection> fc = FontCollection::From(std::make_shared<txt::FontCollection>());
    auto tc = OHOS::Rosen::TypographyCreate::Create(ts, fc);

    const std::u16string testStr = u"1234";
    tc->AppendText(testStr);
    typography = tc->CreateTypography();
    if (!typography) {
        std::cout << "OHDrawingRunTest::SetUpTestCase error typography variable acquisition exception" << std::endl;
        return;
    }
    int layoutWidth = 50;
    typography->Layout(layoutWidth);

    std::vector<std::unique_ptr<TextLineBase>> textLines = typography->GetTextLines();
    if (!textLines.size() || !textLines[0]) {
        std::cout << "OHDrawingRunTest::SetUpTestCase error textLines variable acquisition exception" << std::endl;
        return;
    }

    runs = textLines[0]->GetGlyphRuns();
    if (!runs.size() || !runs[0]) {
        std::cout << "OHDrawingRunTest::SetUpTestCase error runs variable acquisition exception" << std::endl;
    }
}

void OHDrawingRunTest::TearDownTestCase() {}

/*
 * @tc.name: InitTest
 * @tc.desc: test for init
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, InitTest, TestSize.Level1)
{
    EXPECT_NE(typography, nullptr);
    EXPECT_NE(runs.size(), 0);
    EXPECT_NE(runs[0], nullptr);
    EXPECT_NE(runNull, nullptr);
}

/*
 * @tc.name: GetFontTest
 * @tc.desc: test for GetFont
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, GetFontTest, TestSize.Level1)
{
    EXPECT_TRUE(runs[0]->GetFont().GetSize() > 0);
    EXPECT_TRUE(runNull->GetFont().GetSize() > 0);
}

/*
 * @tc.name: GetGlyphCountTest
 * @tc.desc: test for GetGlyphCount
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, GetGlyphCountTest, TestSize.Level1)
{
    EXPECT_TRUE(runs[0]->GetGlyphCount() > 0);
    EXPECT_FALSE(runNull->GetGlyphCount() > 0);
}

/*
 * @tc.name: GetGlyphsTest
 * @tc.desc: test for GetGlyphs
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, GetGlyphsTest, TestSize.Level1)
{
    EXPECT_TRUE(runs[0]->GetGlyphs().size() > 0);
    EXPECT_FALSE(runNull->GetGlyphs().size() > 0);
}

/*
 * @tc.name: GetPositionsTest
 * @tc.desc: test for GetPositions
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, GetPositionsTest, TestSize.Level1)
{
    EXPECT_TRUE(runs[0]->GetPositions().size() > 0);
    EXPECT_FALSE(runNull->GetPositions().size() > 0);
}

/*
 * @tc.name: GetOffsetsTest
 * @tc.desc: test for GetOffsets
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, GetOffsetsTest, TestSize.Level1)
{
    EXPECT_TRUE(runs[0]->GetOffsets().size() > 0);
    EXPECT_FALSE(runNull->GetOffsets().size() > 0);
}

/*
 * @tc.name: PaintTest
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingRunTest, PaintTest, TestSize.Level1)
{
    runs[0]->Paint(nullptr, 0, 0);
    runNull->Paint(nullptr, 0, 0);
}
} // namespace Rosen
} // namespace OHOS