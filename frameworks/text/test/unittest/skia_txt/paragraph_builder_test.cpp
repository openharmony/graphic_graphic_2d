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
#include "modules/skparagraph/src/ParagraphBuilderImpl.h"
#include "paragraph_builder.h"
#include "paragraph_builder_impl.h"
#include "paragraph_style.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::SPText;

namespace txt {
class ParagraphBuilderTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

private:
    std::shared_ptr<ParagraphBuilderImpl> paragraphBuilder_;
};


void ParagraphBuilderTest::SetUp()
{
    ParagraphStyle paragraphStyle;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    paragraphBuilder_ = std::make_shared<ParagraphBuilderImpl>(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder_, nullptr);
    ASSERT_NE(paragraphBuilder_->builder_, nullptr);
}


void ParagraphBuilderTest::TearDown()
{
    paragraphBuilder_.reset();
}

/*
 * @tc.name: ParagraphBuilderTest001
 * @tc.desc: test for PushStyle
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest001, TestSize.Level1)
{
    TextStyle style;
    paragraphBuilder_->PushStyle(style);
    auto peekStyle = paragraphBuilder_->builder_->peekStyle();
    EXPECT_EQ(peekStyle.fFontSize, style.fontSize);
    EXPECT_EQ(peekStyle.fHeight, style.height);
    EXPECT_EQ(peekStyle.fHeightOverride, style.heightOverride);
    EXPECT_EQ(peekStyle.fBaselineShift, style.baseLineShift);
    EXPECT_EQ(peekStyle.fHalfLeading, style.halfLeading);
    EXPECT_EQ(peekStyle.fBackgroundRect.color, style.backgroundRect.color);
    EXPECT_EQ(peekStyle.fStyleId, style.styleId);
    EXPECT_EQ(peekStyle.fLetterSpacing, style.letterSpacing);
    EXPECT_EQ(peekStyle.fWordSpacing, style.wordSpacing);
    EXPECT_EQ(static_cast<int>(peekStyle.fTextBaseline), static_cast<int>(style.baseline));
    EXPECT_EQ(peekStyle.fColor, style.color);
    EXPECT_EQ(peekStyle.fHasBackground, style.background.has_value());
    EXPECT_EQ(peekStyle.fIsPlaceholder, style.isPlaceholder);
}

/*
 * @tc.name: ParagraphBuilderTest002
 * @tc.desc: test for Pop
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest002, TestSize.Level1)
{
    TextStyle style;
    // 10 is just for test
    style.styleId = 10;
    paragraphBuilder_->PushStyle(style);
    TextStyle style1;
    paragraphBuilder_->PushStyle(style1);
    paragraphBuilder_->Pop();
    EXPECT_EQ(paragraphBuilder_->builder_->peekStyle().fStyleId, style.styleId);
}

/*
 * @tc.name: ParagraphBuilderTest004
 * @tc.desc: test for AddText
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest004, TestSize.Level1)
{
    std::u16string text = u"text";
    paragraphBuilder_->AddText(text);
    auto skText = paragraphBuilder_->builder_->getText();
    EXPECT_EQ(std::string(skText.data()), std::string(text.begin(), text.end()));
}

/*
 * @tc.name: ParagraphBuilderTest005
 * @tc.desc: test for AddPlaceholder
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest005, TestSize.Level1)
{
    PlaceholderRun placeholderRunDefault;
    // 50 just for test
    PlaceholderRun placeholderRun(50, 50, PlaceholderAlignment::BASELINE, TextBaseline::ALPHABETIC, 0.0);
    paragraphBuilder_->AddPlaceholder(placeholderRunDefault);
    paragraphBuilder_->AddPlaceholder(placeholderRun);
    paragraphBuilder_->AddText(u"text");
    auto builder = static_cast<skia::textlayout::ParagraphBuilderImpl*>(paragraphBuilder_->builder_.get());
    // only two placeholders, because we add two placeholders before
    EXPECT_EQ(builder->fPlaceholders.size(), 2);
    EXPECT_EQ(builder->fPlaceholders[0].fStyle.fHeight, placeholderRunDefault.height);
    EXPECT_EQ(builder->fPlaceholders[0].fStyle.fWidth, placeholderRunDefault.width);
    EXPECT_EQ(builder->fPlaceholders[1].fStyle.fHeight, placeholderRun.height);
    EXPECT_EQ(builder->fPlaceholders[1].fStyle.fWidth, placeholderRun.width);
}

/*
 * @tc.name: ParagraphBuilderTest007
 * @tc.desc: test for locale
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest007, TestSize.Level1)
{
    std::string localeEN = "en-US";
    TextStyle textStyle;
    textStyle.locale = localeEN;

    paragraphBuilder_->PushStyle(textStyle);
    paragraphBuilder_->AddText(u"text");
    EXPECT_EQ(paragraphBuilder_->builder_->peekStyle().fLocale, SkString(localeEN));
    EXPECT_EQ(paragraphBuilder_->builder_->peekStyle().getLocale(), SkString(localeEN));
}

/*
 * @tc.name: ParagraphBuilderTest008
 * @tc.desc: test for italic font style
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest008, TestSize.Level1)
{
    TextStyle textStyle;
    textStyle.fontStyle = FontStyle::ITALIC;
    paragraphBuilder_->PushStyle(textStyle);
    paragraphBuilder_->AddText(u"text");
    EXPECT_EQ(paragraphBuilder_->builder_->peekStyle().fFontStyle.GetSlant(), RSFontStyle::Slant::ITALIC_SLANT);
}

/*
 * @tc.name: ParagraphBuilderTest009
 * @tc.desc: test for oblique font style
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest009, TestSize.Level1)
{
    TextStyle textStyle;
    textStyle.fontStyle = FontStyle::OBLIQUE;
    paragraphBuilder_->PushStyle(textStyle);
    paragraphBuilder_->AddText(u"text");
    EXPECT_EQ(paragraphBuilder_->builder_->peekStyle().fFontStyle.GetSlant(), RSFontStyle::Slant::OBLIQUE_SLANT);
}

/*
 * @tc.name: ParagraphBuilderTest010
 * @tc.desc: test for default font style
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest010, TestSize.Level1)
{
    TextStyle textStyle;
    // 999 is not a valid value, so the default value should be used
    textStyle.fontStyle = static_cast<FontStyle>(999);
    paragraphBuilder_->PushStyle(textStyle);
    paragraphBuilder_->AddText(u"text");
    EXPECT_EQ(paragraphBuilder_->builder_->peekStyle().fFontStyle.GetSlant(), RSFontStyle::Slant::UPRIGHT_SLANT);
}

/*
 * @tc.name: ParagraphBuilderTest011
 * @tc.desc: test for MakeTextShadow
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest011, TestSize.Level1)
{
    TextStyle textStyle;
    textStyle.textShadows.emplace_back();
    EXPECT_EQ(textStyle.textShadows.size(), 1);
    paragraphBuilder_->PushStyle(textStyle);
    paragraphBuilder_->AddText(u"text");
    // builder should have one shadow, because we only push one shadow
    ASSERT_EQ(textStyle.textShadows.size(), 1);
    ASSERT_EQ(textStyle.textShadows.size(), paragraphBuilder_->builder_->peekStyle().fTextShadows.size());
    EXPECT_EQ(paragraphBuilder_->builder_->peekStyle().fTextShadows.at(0).fColor, textStyle.textShadows.at(0).color);
    EXPECT_EQ(paragraphBuilder_->builder_->peekStyle().fTextShadows.at(0).fOffset, textStyle.textShadows.at(0).offset);
}

/*
 * @tc.name: ParagraphBuilderTest012
 * @tc.desc: test for isPlaceholder is true
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest012, TestSize.Level1)
{
    TextStyle textStyle;
    textStyle.isPlaceholder = true;
    paragraphBuilder_->PushStyle(textStyle);
    paragraphBuilder_->AddText(u"text");
    EXPECT_EQ(paragraphBuilder_->builder_->peekStyle().isPlaceholder(), textStyle.isPlaceholder);
}

/*
 * @tc.name: ParagraphBuilderTest013
 * @tc.desc: test for BuildLineFetcher
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest013, TestSize.Level1)
{
    TextStyle textStyle;
    textStyle.textShadows.emplace_back();
    EXPECT_EQ(textStyle.textShadows.size(), 1);
    paragraphBuilder_->PushStyle(textStyle);
    paragraphBuilder_->AddText(u"text");
    EXPECT_EQ(paragraphBuilder_->BuildLineFetcher()->GetUnicodeSize(), 4);
}

/*
 * @tc.name: ParagraphBuilderTest014
 * @tc.desc: test for BuildLineFetcher
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest014, TestSize.Level1)
{
    EXPECT_EQ(paragraphBuilder_->BuildLineFetcher(), nullptr);
}

/*
 * @tc.name: ParagraphBuilderTest015
 * @tc.desc: test for TextStyleToSkStyle ParagraphSpacing
 * @tc.type: FUNC
 */
 HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest015, TestSize.Level1)
 {
     skia::textlayout::ParagraphStyle skStyle;
     ParagraphStyle txt;
     txt.paragraphSpacing = 100.0f;
     txt.isEndAddParagraphSpacing = true;
     skStyle.setParagraphSpacing(txt.paragraphSpacing);
     skStyle.setIsEndAddParagraphSpacing(txt.isEndAddParagraphSpacing);
     EXPECT_EQ(skStyle.getParagraphSpacing(), txt.paragraphSpacing);
     EXPECT_EQ(skStyle.getIsEndAddParagraphSpacing(), txt.isEndAddParagraphSpacing);

     txt.paragraphSpacing = 0.0f;
     txt.isEndAddParagraphSpacing = true;
     skStyle.setParagraphSpacing(txt.paragraphSpacing);
     skStyle.setIsEndAddParagraphSpacing(txt.isEndAddParagraphSpacing);
     EXPECT_EQ(skStyle.getParagraphSpacing(), txt.paragraphSpacing);
     EXPECT_EQ(skStyle.getIsEndAddParagraphSpacing(), txt.isEndAddParagraphSpacing);
 }

/*
 * @tc.name: ParagraphBuilderTest016
 * @tc.desc: test for TextStyleToSkStyle AutoSpace
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphBuilderTest, ParagraphBuilderTest016, TestSize.Level1)
{
     skia::textlayout::ParagraphStyle skStyle;
     ParagraphStyle txt;
     txt.enableAutoSpace = true;
     skStyle.setEnableAutoSpace(txt.enableAutoSpace);
     EXPECT_EQ(skStyle.getEnableAutoSpace(), txt.enableAutoSpace);
}
} // namespace txt