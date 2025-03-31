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

#include "gtest/gtest.h"
#include "paragraph_style.h"
#include "text_style.h"

using namespace testing;
using namespace testing::ext;

namespace txt {
class ParagraphStyleTest : public testing::Test {};

/*
 * @tc.name: ParagraphStyleTest001
 * @tc.desc: test for ConvertToTextStyle
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphStyleTest, ParagraphStyleTest001, TestSize.Level1)
{
    auto paragraphStyle = std::make_shared<OHOS::Rosen::SPText::ParagraphStyle>();
    auto result = paragraphStyle->ConvertToTextStyle();
    EXPECT_EQ(result.fontSize, paragraphStyle->fontSize);
}

/*
 * @tc.name: ParagraphStyleTest002
 * @tc.desc: test for GetEquivalentAlign
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphStyleTest, ParagraphStyleTest002, TestSize.Level1)
{
    auto paragraphStyle = std::make_shared<OHOS::Rosen::SPText::ParagraphStyle>();
    // 默认 TextDirection 为 LTR
    paragraphStyle->textAlign = OHOS::Rosen::SPText::TextAlign::START;
    auto result1 = paragraphStyle->GetEquivalentAlign();
    EXPECT_EQ(result1, OHOS::Rosen::SPText::TextAlign::LEFT);

    paragraphStyle->textAlign = OHOS::Rosen::SPText::TextAlign::END;
    auto result2 = paragraphStyle->GetEquivalentAlign();
    EXPECT_EQ(result2, OHOS::Rosen::SPText::TextAlign::RIGHT);

    paragraphStyle->textAlign = OHOS::Rosen::SPText::TextAlign::JUSTIFY;
    auto result3 = paragraphStyle->GetEquivalentAlign();
    EXPECT_EQ(result3, OHOS::Rosen::SPText::TextAlign::JUSTIFY);

    // 此处更改 TextDirection 为 RTL
    paragraphStyle->textDirection = OHOS::Rosen::SPText::TextDirection::RTL;
    paragraphStyle->textAlign = OHOS::Rosen::SPText::TextAlign::START;
    auto result4 = paragraphStyle->GetEquivalentAlign();
    EXPECT_EQ(result4, OHOS::Rosen::SPText::TextAlign::RIGHT);

    paragraphStyle->textAlign = OHOS::Rosen::SPText::TextAlign::END;
    auto result5 = paragraphStyle->GetEquivalentAlign();
    EXPECT_EQ(result5, OHOS::Rosen::SPText::TextAlign::LEFT);

    paragraphStyle->textAlign = OHOS::Rosen::SPText::TextAlign::JUSTIFY;
    auto result6 = paragraphStyle->GetEquivalentAlign();
    EXPECT_EQ(result6, OHOS::Rosen::SPText::TextAlign::JUSTIFY);
}

} // namespace txt