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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_font_style_set.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontStyleSetTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaFontStyleSetTest::SetUpTestCase() {}
void SkiaFontStyleSetTest::TearDownTestCase() {}
void SkiaFontStyleSetTest::SetUp() {}
void SkiaFontStyleSetTest::TearDown() {}

/**
 * @tc.name: CreateTypeface001
 * @tc.desc: Test CreateTypeface
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaFontStyleSetTest, CreateTypeface001, TestSize.Level1)
{
    auto skiaFontStyleSet = SkiaFontStyleSet::CreateEmpty();
    ASSERT_TRUE(skiaFontStyleSet->CreateTypeface(0) == nullptr);
}

/**
 * @tc.name: GetStyle001
 * @tc.desc: Test GetStyle
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaFontStyleSetTest, GetStyle001, TestSize.Level1)
{
    std::shared_ptr<FontStyleSetImpl> fontStyleSetImpl = nullptr;
    auto skiaFontStyleSet1 = std::make_shared<FontStyleSet>(fontStyleSetImpl);
    FontStyle fontStyle;
    std::string styleName = "style";
    skiaFontStyleSet1->GetStyle(0, &fontStyle, &styleName);
    auto skiaFontStyleSet2 = SkiaFontStyleSet::CreateEmpty();
    skiaFontStyleSet2->GetStyle(0, &fontStyle, &styleName);
}

/**
 * @tc.name: MatchStyle001
 * @tc.desc: Test MatchStyle
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaFontStyleSetTest, MatchStyle001, TestSize.Level1)
{
    std::shared_ptr<FontStyleSetImpl> fontStyleSetImpl = nullptr;
    auto skiaFontStyleSet1 = std::make_shared<FontStyleSet>(fontStyleSetImpl);
    FontStyle fontStyle;
    auto typeface1 = skiaFontStyleSet1->MatchStyle(fontStyle);
    auto skiaFontStyleSet2 = SkiaFontStyleSet::CreateEmpty();
    auto typeface2 = skiaFontStyleSet2->MatchStyle(fontStyle);
    ASSERT_TRUE(typeface1 == nullptr);
    ASSERT_TRUE(typeface2 == nullptr);
}

/**
 * @tc.name: Count001
 * @tc.desc: Test Count
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaFontStyleSetTest, Count001, TestSize.Level1)
{
    auto skiaFontStyleSet = SkiaFontStyleSet::CreateEmpty();
    ASSERT_TRUE(skiaFontStyleSet->Count() >= 0);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS