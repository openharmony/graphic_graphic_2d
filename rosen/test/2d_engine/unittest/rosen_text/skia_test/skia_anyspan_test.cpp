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
#include "texgine/any_span.h"
#include "texgine/font_providers.h"
#include "texgine/text_style.h"
#include "texgine/typography_style.h"
#include "texgine/typography_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class OHSkiaAnySpanTest : public testing::Test {};

void AnySpan::ReportMemoryUsage(const std::string& member, const bool needThis) const {}

void FontFeatures::SetFeature(const std::string &ftag, int fvalue)
{
    features_[ftag] = fvalue;
}

const std::map<std::string, int> &FontFeatures::GetFeatures() const
{
    return features_;
}

std::shared_ptr<FontProviders> FontProviders::Create() noexcept(true)
{
    return std::make_shared<FontProviders>();
}

TextAlign TypographyStyle::GetEquivalentAlign() const
{
    return align;
}

TextStyle TypographyStyle::ConvertToTextStyle() const
{
    TextStyle style;
    style.fontSize = fontSize;
    style.fontFamilies = fontFamilies;
    style.heightScale = heightScale;
    style.halfLeading = halfLeading;
    style.heightOnly = heightOnly;
    style.fontWeight = fontWeight;
    style.fontStyle = fontStyle;

    return style;
}

class MySpanForTest : public AnySpan {
public:
    explicit MySpanForTest()
    {
        // 10 is used for initialization
        width = 10;
        // 10 is used for initialization
        height = 10;
        alignment = AnySpanAlignment::OFFSET_AT_BASELINE;
        baseline = TextBaseline::ALPHABETIC;
        // 10 is used for initialization
        baselineOffset = 10;
    }

    double GetWidth() const override
    {
        return width;
    }

    double GetHeight() const override
    {
        return height;
    }

    AnySpanAlignment GetAlignment() const override
    {
        return alignment;
    }

    TextBaseline GetBaseline() const override
    {
        return baseline;
    }

    double GetLineOffset() const override
    {
        return baselineOffset;
    }

    void Paint(TexgineCanvas &canvas, double offsetX, double offsetY) override
    {
    }

private:
    double width;
    double height;
    AnySpanAlignment alignment;
    TextBaseline baseline;
    double baselineOffset;
};

/*
 * @tc.name: OHSkiaAnySpanTest001
 * @tc.desc: test for anyspan
 * @tc.type: FUNC
 */
HWTEST_F(OHSkiaAnySpanTest, OHSkiaAnySpanTest001, TestSize.Level1)
{
    MySpanForTest mySpan;
    TextStyle textStyle;
    mySpan.SetTextStyle(textStyle);
    mySpan.SetRoundRectType(RoundRectType::NONE);
    // 10 is used for initialization
    mySpan.SetTopInGroup(10);
    mySpan.SetBottomInGroup(10);
    mySpan.SetMaxRoundRectRadius(10);
    EXPECT_EQ(mySpan.GetTopInGroup(), 10);
    EXPECT_EQ(mySpan.GetBottomInGroup(), 10);
    EXPECT_EQ(mySpan.GetMaxRoundRectRadius(), 10);

    TextAlign textAlign1 = TextAlign::DEFAULT | TextAlign::START;
    textAlign1 = TextAlign::DEFAULT & TextAlign::START;
    textAlign1 = ~TextAlign::DEFAULT;
    textAlign1 = TextAlign::DEFAULT ^ TextAlign::START;
    TextAlign textAlign2 = TextAlign::DEFAULT;
    textAlign2 &= textAlign1;
    textAlign2 |= textAlign1;
    textAlign2 ^= textAlign1;
    TextDecoration textDecoration1 = TextDecoration::NONE & TextDecoration::UNDERLINE;
    textDecoration1 = TextDecoration::NONE | TextDecoration::UNDERLINE;
    textDecoration1 = TextDecoration::NONE ^ TextDecoration::UNDERLINE;
    textDecoration1 = TextDecoration::NONE + TextDecoration::UNDERLINE;
    TextDecoration textDecoration2 = TextDecoration::NONE;
    textDecoration2 &= textDecoration1;
    textDecoration2 |= textDecoration1;
    textDecoration2 ^= textDecoration1;
    textDecoration2 += textDecoration1;
}

/*
 * @tc.name: OHSkiaAnySpanTest002
 * @tc.desc: test for anyspan
 * @tc.type: FUNC
 */
HWTEST_F(OHSkiaAnySpanTest, OHSkiaAnySpanTest002, TestSize.Level1)
{
    FontFeatures fontFeatures;
    // 1 is used for test
    fontFeatures.SetFeature("123", 1);
    std::map<std::string, int> features = fontFeatures.GetFeatures();
    FontProviders fontProviders;
    std::shared_ptr<FontProviders> fontProviders_ptr = fontProviders.Create();
    TypographyStyle typographyStyle;
    TextAlign textAlign = typographyStyle.GetEquivalentAlign();
    EXPECT_EQ(textAlign, typographyStyle.align);
    TextStyle textStyle = typographyStyle.ConvertToTextStyle();
    EXPECT_EQ(textStyle.fontSize, typographyStyle.fontSize);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
