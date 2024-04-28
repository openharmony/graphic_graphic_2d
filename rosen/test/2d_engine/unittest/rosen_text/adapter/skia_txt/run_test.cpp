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
#include "run.h"
#include "text_line_base.h"
#include "typography.h"
#include "typography_create.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OH_Drawing_RunTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_RunTest001
 * @tc.desc: test for constuctor of Run
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_RunTest, OH_Drawing_RunTest001, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    const char* text = "test";
    const std::u16string wideText =
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(text);
    typographyCreate->AppendText(wideText);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    Drawing::Canvas* canvas = new Drawing::Canvas();
    typography->Layout(200);
    typography->Paint(canvas, 100, 100);
    std::vector<std::unique_ptr<TextLineBase>> vectorTextLineBase = typography->GetTextLines();
    EXPECT_EQ(vectorTextLineBase.size() > 0, true);
    std::vector<std::unique_ptr<OHOS::Rosen::Run>> vectorRun = vectorTextLineBase[0]->GetGlyphRuns();
    EXPECT_EQ(vectorRun.size() > 0, true);
    vectorRun[0]->GetFont();
    EXPECT_EQ(vectorRun[0]->GetGlyphCount() > 0, true);
    vectorRun[0]->GetGlyphs();
    vectorRun[0]->GetPositions();
    vectorRun[0]->GetOffsets();
    vectorRun[0]->Paint(canvas, 10.0, 10.0);
    delete canvas;
    canvas = nullptr;
}
}// namespace Rosen
}// namespace OHOS