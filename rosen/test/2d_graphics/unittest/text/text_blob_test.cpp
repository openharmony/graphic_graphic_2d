/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "text/text_blob.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TextBlobTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TextBlobTest::SetUpTestCase() {}
void TextBlobTest::TearDownTestCase() {}
void TextBlobTest::SetUp() {}
void TextBlobTest::TearDown() {}

/**
 * @tc.name: TextContrastTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(TextBlobTest, TextContrastTest001, TestSize.Level1)
{
    Font font;
    auto textBlob1 = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    ASSERT_NE(textBlob1, nullptr);
    EXPECT_EQ(textBlob1->GetTextContrast(), TextContrast::FOLLOW_SYSTEM);
    ProcessTextConstrast::Instance().SetTextContrast(TextContrast::ENABLE_CONTRAST);
    auto textBlob2 = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    ASSERT_NE(textBlob2, nullptr);
    EXPECT_EQ(textBlob2->GetTextContrast(), ProcessTextConstrast::Instance().GetTextContrast());
    textBlob2->SetTextContrast(TextContrast::DISABLE_CONTRAST);
    EXPECT_EQ(textBlob2->GetTextContrast(), TextContrast::DISABLE_CONTRAST);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

