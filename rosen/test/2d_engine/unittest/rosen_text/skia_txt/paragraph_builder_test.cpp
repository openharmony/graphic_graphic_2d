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
#include "font_collection.h"
#include "paragraph_builder.h"
#include "paragraph_builder_impl.h"
#include "paragraph_style.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::SPText;

namespace txt {
class ParagraphBuilderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<ParagraphBuilder> paragraphBuilder_ = nullptr;
};

void ParagraphBuilderTest::SetUpTestCase()
{
    ParagraphStyle paragraphStyle;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    if (!fontCollection) {
        std::cout << "ParagraphBuilderTest::SetUpTestCase error fontCollection is nullptr" << std::endl;
        return;
    }
    paragraphBuilder_ = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    if (!paragraphBuilder_) {
        std::cout << "ParagraphBuilderTest::SetUpTestCase error paragraphBuilder_ is nullptr" << std::endl;
    }
}

void ParagraphBuilderTest::TearDownTestCase()
{
}

/*
 * @tc.name: ParagraphBuilderTest001
 * @tc.desc: test for PushStyle
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest001, TestSize.Level1)
{
    EXPECT_EQ(paragraphBuilder_ != nullptr, true);
    TextStyle style;
    paragraphBuilder_->PushStyle(style);
}

/*
 * @tc.name: ParagraphBuilderTest002
 * @tc.desc: test for Pop
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest002, TestSize.Level1)
{
    EXPECT_EQ(paragraphBuilder_ != nullptr, true);
    paragraphBuilder_->Pop();
}

/*
 * @tc.name: ParagraphBuilderTest003
 * @tc.desc: test for PeekStyle
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest003, TestSize.Level1)
{
    EXPECT_EQ(paragraphBuilder_ != nullptr, true);
    paragraphBuilder_->PeekStyle();
}

/*
 * @tc.name: ParagraphBuilderTest004
 * @tc.desc: test for AddText
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest004, TestSize.Level1)
{
    EXPECT_EQ(paragraphBuilder_ != nullptr, true);
    std::u16string text(u"text");
    paragraphBuilder_->AddText(text);
}

/*
 * @tc.name: ParagraphBuilderTest005
 * @tc.desc: test for AddPlaceholder
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest005, TestSize.Level1)
{
    EXPECT_EQ(paragraphBuilder_ != nullptr, true);
    PlaceholderRun placeholderRunDefault;
    // 50 just for test
    PlaceholderRun placeholderRun(50, 50, PlaceholderAlignment::BASELINE,
        TextBaseline::ALPHABETIC, 0.0);
    paragraphBuilder_->AddPlaceholder(placeholderRunDefault);
    paragraphBuilder_->AddPlaceholder(placeholderRun);
}

/*
 * @tc.name: ParagraphBuilderTest006
 * @tc.desc: test for Build
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest006, TestSize.Level1)
{
    EXPECT_EQ(paragraphBuilder_ != nullptr, true);
    EXPECT_EQ(paragraphBuilder_->Build() != nullptr, true);
}

/*
 * @tc.name: ParagraphBuilderTest007
 * @tc.desc: test for locale
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest007, TestSize.Level1)
{
    static const std::string localeZh = "zh-Hans";
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ParagraphStyle paragraphStyle;
    paragraphStyle.locale = "";
    EXPECT_EQ(ParagraphBuilder::Create(paragraphStyle, fontCollection) != nullptr, true);
    paragraphStyle.locale = localeZh;
    EXPECT_EQ(ParagraphBuilder::Create(paragraphStyle, fontCollection) != nullptr, true);

    auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    TextStyle textStyle;
    textStyle.locale = "";
    builder->PushStyle(textStyle);
    EXPECT_EQ(builder->Build() != nullptr, true);
    textStyle.locale = localeZh;
    builder->PushStyle(textStyle);
    EXPECT_EQ(builder->Build() != nullptr, true);
}
} // namespace txt