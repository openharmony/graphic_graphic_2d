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
class OH_Drawing_TextLineBaseTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_TextLineBaseTest001
 * @tc.desc: test for constuctor of TypographyCreate
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TextLineBaseTest, OH_Drawing_TextLineBaseTest001, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    const char* text = "test";
    const std::u16string wideText =
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(text);
    typographyCreate->AppendText(wideText);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    Drawing::Canvas* canvas = new Drawing::Canvas();
    // 200 for unit test
    typography->Layout(200);
    // 100 for unit test
    typography->Paint(canvas, 100, 100);
    std::vector<std::unique_ptr<TextLineBase>> vectorTextLineBase = typography->GetTextLines();
    if (vectorTextLineBase.size() > 0) {
        vectorTextLineBase[0]->GetGlyphCount();
        std::vector<std::unique_ptr<OHOS::Rosen::Run>> vectorRun = vectorTextLineBase[0]->GetGlyphRuns();
        EXPECT_EQ(vectorRun.size() > 0, true);
        vectorTextLineBase[0]->GetTextRange();
        // 100.0 for unit test
        vectorTextLineBase[0]->Paint(canvas, 100.0, 100.0);
    }
}
} // namespace Rosen
} // namespace OHOS