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
public:
    static void SetUpTestCase();
    static inline std::shared_ptr<OHOS::Rosen::Typography> typography_ = nullptr;
    static inline std::vector<std::unique_ptr<TextLineBase>> vectorTextLineBase_;
};

void OHDrawingTextLineBaseTest::SetUpTestCase()
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());

    if (!fontCollection) {
        std::cout << "OHDrawingTextLineBaseTest::SetUpTestCase error fontCollection is nullptr" << std::endl;
        return;
    }
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    if (!typographyCreate) {
        std::cout << "OHDrawingTextLineBaseTest::SetUpTestCase error typographyCreate is nullptr" << std::endl;
        return;
    }
    std::u16string wideText(u"OHDrawingTextLineBaseTest");
    typographyCreate->AppendText(wideText);
    typography_ = typographyCreate->CreateTypography();
    if (!typography_) {
        std::cout << "OHDrawingTextLineBaseTest::SetUpTestCase error typography_ is nullptr" << std::endl;
        return;
    }
    Drawing::Canvas canvas;
    // 200 for unit test
    typography_->Layout(200);
    // 100 for unit test
    typography_->Paint(&canvas, 100, 100);
    vectorTextLineBase_ = typography_->GetTextLines();
    if (!vectorTextLineBase_.size() || !vectorTextLineBase_.at(0)) {
        std::cout << "OHDrawingTextLineBaseTest::SetUpTestCase error vectorTextLineBase_ variable acquisition exception"
                  << std::endl;
        return;
    }
}

/*
 * @tc.name: OHDrawingTextLineBaseTest001
 * @tc.desc: test for constuctor of GetGlyphCount
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingTextLineBaseTest, OHDrawingTextLineBaseTest001, TestSize.Level1)
{
    EXPECT_EQ(typography_ != nullptr, true);
    EXPECT_EQ(vectorTextLineBase_.size() != 0, true);
    EXPECT_EQ(vectorTextLineBase_.at(0) != nullptr, true);
    EXPECT_EQ(vectorTextLineBase_.at(0)->GetGlyphCount() > 0, true);
}

/*
 * @tc.name: OHDrawingTextLineBaseTest002
 * @tc.desc: test for constuctor of GetGlyphRuns
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingTextLineBaseTest, OHDrawingTextLineBaseTest002, TestSize.Level1)
{
    EXPECT_EQ(typography_ != nullptr, true);
    EXPECT_EQ(vectorTextLineBase_.size() != 0, true);
    EXPECT_EQ(vectorTextLineBase_.at(0) != nullptr, true);
    EXPECT_EQ(vectorTextLineBase_.at(0)->GetGlyphRuns().size() > 0, true);
}

/*
 * @tc.name: OHDrawingTextLineBaseTest003
 * @tc.desc: test for constuctor of GetTextRange
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingTextLineBaseTest, OHDrawingTextLineBaseTest003, TestSize.Level1)
{
    EXPECT_EQ(typography_ != nullptr, true);
    EXPECT_EQ(vectorTextLineBase_.size() != 0, true);
    EXPECT_EQ(vectorTextLineBase_.at(0) != nullptr, true);
    SPText::Range<size_t> rangeDefault;
    SPText::Range<size_t> range(0, 1);
    EXPECT_EQ(rangeDefault == range, false);

    EXPECT_EQ(vectorTextLineBase_.at(0)->GetTextRange().leftIndex, 0);
}

/*
 * @tc.name: OHDrawingTextLineBaseTest004
 * @tc.desc: test for constuctor of Paint
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingTextLineBaseTest, OHDrawingTextLineBaseTest004, TestSize.Level1)
{
    EXPECT_EQ(typography_ != nullptr, true);
    EXPECT_EQ(vectorTextLineBase_.size() != 0, true);
    EXPECT_EQ(vectorTextLineBase_.at(0) != nullptr, true);
    Drawing::Canvas canvas;
    vectorTextLineBase_.at(0)->Paint(&canvas, 0.0, 0.0);
}
} // namespace Rosen
} // namespace OHOS