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
#include "convert.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OH_Drawing_ConvertTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_ConvertTest001
 * @tc.desc: test for  Convert FontCollection
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_ConvertTest, OH_Drawing_ConvertTest001, TestSize.Level1)
{
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::shared_ptr<OHOS::Rosen::AdapterTxt::FontCollection> adapterFontCollection = AdapterTxt::Convert(
        fontCollection);
    EXPECT_EQ(adapterFontCollection != nullptr, true);
}

/*
 * @tc.name: OH_Drawing_ConvertTest002
 * @tc.desc: test for  Convert Affinity
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_ConvertTest, OH_Drawing_ConvertTest002, TestSize.Level1)
{
    // 0 for unit test
    const SPText::PositionWithAffinity positionWithAffinity = {0, SPText::Affinity::UPSTREAM};
    IndexAndAffinity indexAndAffinity = AdapterTxt::Convert(positionWithAffinity);
    EXPECT_EQ(indexAndAffinity.index == 0, true);
}

/*
 * @tc.name: OH_Drawing_ConvertTest003
 * @tc.desc: test for  Convert Range
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_ConvertTest, OH_Drawing_ConvertTest003, TestSize.Level1)
{
    // 0 for unit test
    SPText::Range<size_t> range(0, 0);
    Boundary boundary = AdapterTxt::Convert(range);
    EXPECT_EQ(boundary.leftIndex == 0, true);
}

/*
 * @tc.name: OH_Drawing_ConvertTest004
 * @tc.desc: test for  Convert TextBox
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_ConvertTest, OH_Drawing_ConvertTest004, TestSize.Level1)
{
    SkRect skRect;
    const SPText::TextBox box(skRect, SPText::TextDirection::RTL);
    TextRect textRect = AdapterTxt::Convert(box);
    EXPECT_EQ(textRect.direction == TextDirection::RTL, true);
}

/*
 * @tc.name: OH_Drawing_ConvertTest005
 * @tc.desc: test for  Convert TextRectHeightStyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_ConvertTest, OH_Drawing_ConvertTest005, TestSize.Level1)
{
    TextRectHeightStyle textRectHeightStyle = TextRectHeightStyle::TIGHT;
    SPText::RectHeightStyle rectHeightStyle = AdapterTxt::Convert(textRectHeightStyle);
    EXPECT_EQ(rectHeightStyle == SPText::RectHeightStyle::TIGHT, true);
}

/*
 * @tc.name: OH_Drawing_ConvertTest006
 * @tc.desc: test for  Convert TextRectWidthStyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_ConvertTest, OH_Drawing_ConvertTest006, TestSize.Level1)
{
    TextRectWidthStyle textRectWidthStyle = TextRectWidthStyle::MAX;
    SPText::RectWidthStyle rectWidthStyle = AdapterTxt::Convert(textRectWidthStyle);
    EXPECT_EQ(rectWidthStyle == SPText::RectWidthStyle::MAX, true);
}

/*
 * @tc.name: OH_Drawing_ConvertTest007
 * @tc.desc: test for  Convert TypographyStyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_ConvertTest, OH_Drawing_ConvertTest007, TestSize.Level1)
{
    TypographyStyle typographyStyle;
    SPText::ParagraphStyle paragraphStyle = AdapterTxt::Convert(typographyStyle);
    EXPECT_EQ(paragraphStyle.fontWeight == SPText::FontWeight::W400, true);
}

/*
 * @tc.name: OH_Drawing_ConvertTest008
 * @tc.desc: test for  Convert PlaceholderSpan
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_ConvertTest, OH_Drawing_ConvertTest008, TestSize.Level1)
{
    PlaceholderSpan placeholderSpan;
    placeholderSpan.alignment = PlaceholderVerticalAlignment::OFFSET_AT_BASELINE;
    SPText::PlaceholderRun placeholderRun = AdapterTxt::Convert(placeholderSpan);
    EXPECT_EQ(placeholderRun.alignment == SPText::PlaceholderAlignment::BASELINE, true);
}

/*
 * @tc.name: OH_Drawing_ConvertTest009
 * @tc.desc: test for  Convert PlaceholderSpan
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_ConvertTest, OH_Drawing_ConvertTest009, TestSize.Level1)
{
    TextStyle textStyle;
    SPText::TextStyle sptextStyle = AdapterTxt::Convert(textStyle);
    //The default fontsize for TextStyle is 14.0
    EXPECT_EQ(sptextStyle.fontSize == 14.0, true);
}
} // namespace Rosen
} // namespace OHOS