/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "draw/canvas.h"
#include "text_flip_effect.h"
#include "typography_mock.h"
#include "typography.h"
#include "typography_create.h"
#include "font_collection.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

class MockCanvas : public Drawing::Canvas {
public:
    MOCK_METHOD(void, DrawTextBlob, (const Drawing::TextBlob* blob, const float x, const float y), (override));
};

class TextFlipEffectTest : public testing::Test {
public:
    void SetUp() override
    {
        auto textEffect = TextEffectFactoryCreator::GetInstance().CreateTextEffect(TextEffectStrategy::FLIP);
        effect_ = std::static_pointer_cast<TextFlipEffect>(textEffect);
        ASSERT_NE(effect_, nullptr);
        EXPECT_EQ(effect_->strategy_, TextEffectStrategy::FLIP);
        mockTypography_ = std::make_shared<MockTypography>();
        mockCanvas_ = std::make_unique<MockCanvas>();
    }

    void TearDown() override
    {
        effect_.reset();
        mockTypography_.reset();
        mockCanvas_.reset();
    }

protected:
    std::shared_ptr<TextFlipEffect> effect_{nullptr};
    std::shared_ptr<MockTypography> mockTypography_{nullptr};
    std::unique_ptr<MockCanvas> mockCanvas_{nullptr};

    TypographyConfig CreateConfig()
    {
        return { mockTypography_, {0, 10} };
    }
};

/*
 * @tc.name: TextFlipEffectTest001
 * @tc.desc: Test for UpdateEffectConfig action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest001, TestSize.Level0)
{
    std::unordered_map<TextEffectAttribute, std::string> config1 = {
        {TextEffectAttribute::FLIP_DIRECTION, "up"},
        {TextEffectAttribute::BLUR_ENABLE, "false"},
    };

    std::unordered_map<TextEffectAttribute, std::string> config2 = {
        {TextEffectAttribute::FLIP_DIRECTION, "down"},
        {TextEffectAttribute::BLUR_ENABLE, "true"},
    };

    EXPECT_EQ(effect_->UpdateEffectConfig(config1), TEXT_EFFECT_SUCCESS);
    EXPECT_EQ(effect_->UpdateEffectConfig(config2), TEXT_EFFECT_SUCCESS);
    EXPECT_EQ(effect_->direction_, TextEffectFlipDirection::DOWN);
    EXPECT_TRUE(effect_->blurEnable_);

    std::unordered_map<TextEffectAttribute, std::string> invalidConfig = {
        {TextEffectAttribute::FLIP_DIRECTION, "invalid"},
        {TextEffectAttribute::BLUR_ENABLE, "invalid"}
    };
    EXPECT_EQ(effect_->UpdateEffectConfig(invalidConfig), TEXT_EFFECT_INVALID_INPUT);
    EXPECT_EQ(effect_->direction_, TextEffectFlipDirection::DOWN);
    EXPECT_TRUE(effect_->blurEnable_);

    std::unordered_map<TextEffectAttribute, std::string> configEmpty;
    EXPECT_EQ(effect_->UpdateEffectConfig(configEmpty), TEXT_EFFECT_SUCCESS);
}

/*
 * @tc.name: TextFlipEffectTest002
 * @tc.desc: Test for AppendTypography action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest002, TestSize.Level0)
{
    TypographyConfig config = CreateConfig();
    EXPECT_CALL(*mockTypography_, SetTextEffectAssociation(true)).Times(1);
    EXPECT_EQ(effect_->AppendTypography({config}), TEXT_EFFECT_SUCCESS);
    EXPECT_EQ(effect_->typographyConfig_.typography, mockTypography_);

    auto anotherTypography = std::make_shared<MockTypography>();
    TypographyConfig anotherConfig = { anotherTypography, {5, 15} };
    EXPECT_EQ(effect_->AppendTypography({anotherConfig}), TEXT_EFFECT_UNKNOWN);

    EXPECT_EQ(effect_->AppendTypography({}), TEXT_EFFECT_INVALID_INPUT);
    TypographyConfig nullptrConfig = { nullptr, {0, 0} };
    EXPECT_EQ(effect_->AppendTypography({nullptrConfig}), TEXT_EFFECT_INVALID_INPUT);
}

/*
 * @tc.name: TextFlipEffectTest003
 * @tc.desc: Test for UpdateTypography action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest003, TestSize.Level0)
{
    EXPECT_CALL(*mockTypography_, SetTextEffectAssociation(true)).Times(2);
    EXPECT_CALL(*mockTypography_, SetTextEffectAssociation(false)).Times(1);
    effect_->AppendTypography({CreateConfig()});
    TypographyConfig newConfig = { mockTypography_, {5, 15} };
    std::vector<std::pair<TypographyConfig, TypographyConfig>> update = {
        {CreateConfig(), newConfig}
    };
    
    EXPECT_EQ(effect_->UpdateTypography(update), TEXT_EFFECT_SUCCESS);
    EXPECT_EQ(effect_->typographyConfig_.rawTextRange.first, 5);
    EXPECT_EQ(effect_->typographyConfig_.rawTextRange.second, 15);

    TypographyConfig configNull = { nullptr, {5, 15} };
    std::vector<std::pair<TypographyConfig, TypographyConfig>> updateNull = {
        {CreateConfig(), configNull}
    };
    EXPECT_EQ(effect_->UpdateTypography(updateNull), TEXT_EFFECT_INVALID_INPUT);

    std::vector<std::pair<TypographyConfig, TypographyConfig>> emptyUpdate;
    EXPECT_EQ(effect_->UpdateTypography(emptyUpdate), TEXT_EFFECT_INVALID_INPUT);
    
    auto anotherTypography = std::make_shared<MockTypography>();
    TypographyConfig anotherConfig = { anotherTypography, {5, 15} };
    std::vector<std::pair<TypographyConfig, TypographyConfig>> mismatchedUpdate = {
        {anotherConfig, anotherConfig}
    };
    EXPECT_EQ(effect_->UpdateTypography(mismatchedUpdate), TEXT_EFFECT_INVALID_INPUT);
    effect_->typographyConfig_.typography = nullptr;
    TypographyConfig invalidConfig = { nullptr, {5, 15} };
    std::vector<std::pair<TypographyConfig, TypographyConfig>> invalidUpdate = {
        {invalidConfig, anotherConfig}
    };
    EXPECT_EQ(effect_->UpdateTypography(invalidUpdate), TEXT_EFFECT_UNKNOWN);
}

/*
 * @tc.name: TextFlipEffectTest004
 * @tc.desc: Test for RemoveTypography action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest004, TestSize.Level0)
{
    EXPECT_CALL(*mockTypography_, SetTextEffectAssociation(false)).Times(2);
    EXPECT_CALL(*mockTypography_, SetTextEffectAssociation(true)).Times(2);
    TypographyConfig config = CreateConfig();
    effect_->AppendTypography({config});
    effect_->RemoveTypography({config});
    
    EXPECT_EQ(effect_->typographyConfig_.typography, nullptr);
    EXPECT_EQ(effect_->typographyConfig_.rawTextRange.first, 0);
    EXPECT_EQ(effect_->typographyConfig_.rawTextRange.second, 0);

    effect_->AppendTypography({CreateConfig()});
    auto anotherTypography = std::make_shared<MockTypography>();
    TypographyConfig anotherConfig = { anotherTypography, {5, 15} };
    effect_->RemoveTypography({anotherConfig});
    EXPECT_NE(effect_->typographyConfig_.typography, nullptr);
    effect_->RemoveTypography({});
    EXPECT_EQ(effect_->typographyConfig_.typography, nullptr);
}

/*
 * @tc.name: TextFlipEffectTest005
 * @tc.desc: Test for StartEffect and StopEffect action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest005, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.enableAutoSpace = false;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    std::u16string text = u"88";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate->CreateTypography();
    double maxWidth = 1500;
    typography0->Layout(maxWidth);

    std::shared_ptr<Typography> typography(typography0.release());
    TypographyConfig config = { typography, {0, text.length()} };
    effect_->AppendTypography({config});
    effect_->StartEffect(mockCanvas_.get(), 100.0, 200.0);
    effect_->StopEffect();
    TypographyConfig config1 = { typography, {0, text.length()} };
    effect_->AppendTypography({config1});
    typography->SetSkipTextBlobDrawing(true);
    typography->Paint(mockCanvas_.get(), 100.0, 200.0);
    effect_->StartEffect(mockCanvas_.get(), 100.0, 200.0);
    EXPECT_FALSE(effect_->lastAllBlobGlyphIds_.empty());

    typography->SetSkipTextBlobDrawing(false);
    effect_->StopEffect();
    EXPECT_TRUE(effect_->lastAllBlobGlyphIds_.empty());
}

/*
 * @tc.name: TextFlipEffectTest006
 * @tc.desc: Test for DrawTextFlip action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest006, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.enableAutoSpace = false;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    std::u16string text = u"88";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate->CreateTypography();
    double maxWidth = 1500;
    typography0->Layout(maxWidth);
    std::shared_ptr<Typography> typography(typography0.release());
    TypographyConfig config = { typography, {0, text.length()} };
    effect_->AppendTypography({config});
    typography->SetSkipTextBlobDrawing(true);
    typography->Paint(mockCanvas_.get(), 100.0, 200.0);

    std::vector<TextBlobRecordInfo> records = typography->GetTextBlobRecordInfo();
    double x = 0.0;
    double y = 0.0;
    effect_->DrawTextFlip(records, mockCanvas_.get(), x, y);
    EXPECT_TRUE(effect_->lastAllBlobGlyphIds_.empty());
}

/*
 * @tc.name: TextFlipEffectTest007
 * @tc.desc: Test for DrawResidualText action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest007, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.enableAutoSpace = false;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    std::u16string text = u"88";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate->CreateTypography();
    double maxWidth = 1500;
    typography0->Layout(maxWidth);
    std::shared_ptr<Typography> typography(typography0.release());
    TypographyConfig config = { typography, {0, text.length()} };
    effect_->AppendTypography({config});
    typography->SetSkipTextBlobDrawing(true);
    typography->Paint(mockCanvas_.get(), 100.0, 200.0);
    std::vector<TextBlobRecordInfo> records = typography->GetTextBlobRecordInfo();
    EXPECT_FALSE(records.empty());
    double height = records[0].blob->Bounds()->GetHeight();
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        typography->GetAnimation();
    effect_->currentGlyphIndex_ = 0;
    effect_->lastAllBlobGlyphIds_.clear();
    effect_->lastAllBlobGlyphIds_.emplace_back(10);
    effect_->lastAllBlobGlyphIds_.emplace_back(20);
    effect_->DrawResidualText(mockCanvas_.get(), height, animationFunc);
    EXPECT_FALSE(effect_->lastAllBlobGlyphIds_.empty());
    effect_->currentGlyphIndex_ = 2;
    effect_->lastAllBlobGlyphIds_.clear();
    effect_->DrawResidualText(mockCanvas_.get(), height, animationFunc);
    EXPECT_NE(effect_->currentGlyphIndex_, 0);
}


/*
 * @tc.name: TextFlipEffectTest008
 * @tc.desc: Test for DrawTextFlipElements action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest008, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.enableAutoSpace = false;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    std::u16string text = u"88";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate->CreateTypography();
    double maxWidth = 1500;
    typography0->Layout(maxWidth);
    std::shared_ptr<Typography> typography(typography0.release());
    TypographyConfig config = { typography, {0, text.length()} };
    effect_->AppendTypography({config});
    typography->SetSkipTextBlobDrawing(true);
    typography->Paint(mockCanvas_.get(), 100.0, 200.0);
    std::vector<TextBlobRecordInfo> records = typography->GetTextBlobRecordInfo();
    EXPECT_FALSE(records.empty());
    double height = records[0].blob->Bounds()->GetHeight();
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        typography->GetAnimation();
    std::vector<std::vector<TextEngine::TextEffectElement>> effectElements;
    std::vector<TextEngine::TextEffectElement> unEffectElements;
    std::vector<TextEngine::TextEffectElement> inEffectElements;
    effectElements.emplace_back(unEffectElements);
    effectElements.emplace_back(inEffectElements);
    effect_->DrawTextFlipElements(mockCanvas_.get(), height, Drawing::Color::COLOR_BLACK,
        animationFunc, effectElements);
    EXPECT_EQ(effect_->currentGlyphIndex_, 0);
}

/*
 * @tc.name: TextFlipEffectTest009
 * @tc.desc: Test for ClearTypography action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest009, TestSize.Level1)
{
    TypographyConfig config = CreateConfig();
    effect_->AppendTypography({config});
    effect_->ClearTypography();
    EXPECT_EQ(effect_->typographyConfig_.typography, nullptr);
    effect_->AppendTypography({config});
    effect_->typographyConfig_.typography = nullptr;
    effect_->ClearTypography();
    EXPECT_EQ(effect_->typographyConfig_.rawTextRange.second, 10);
}

/*
 * @tc.name: TextFlipEffectTest010
 * @tc.desc: Test for GenerateChangeElements action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest010, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.enableAutoSpace = false;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    std::u16string text = u"88";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate->CreateTypography();
    double maxWidth = 1500;
    typography0->Layout(maxWidth);
    std::shared_ptr<Typography> typography(typography0.release());
    TypographyConfig config = { typography, {0, text.length()} };
    effect_->AppendTypography({config});
    typography->SetSkipTextBlobDrawing(true);
    typography->Paint(mockCanvas_.get(), 100.0, 200.0);
    std::vector<TextBlobRecordInfo> records = typography->GetTextBlobRecordInfo();
    EXPECT_FALSE(records.empty());
    effect_->GenerateChangeElements(records[0].blob, 100.0, 100.0);
    EXPECT_TRUE(effect_->lastAllBlobGlyphIds_.empty());
    effect_->currentGlyphIndex_ = 0;
    effect_->GenerateChangeElements(records[0].blob, 100.0, 100.0);
    EXPECT_TRUE(effect_->lastAllBlobGlyphIds_.empty());
}

/*
 * @tc.name: TextFlipEffectTest011
 * @tc.desc: Test for GenerateFlipConfig action
 * @tc.type: FUNC
 */
HWTEST_F(TextFlipEffectTest, TextFlipEffectTest011, TestSize.Level1)
{
    effect_->blurEnable_ = true;
    EXPECT_EQ(effect_->GenerateFlipConfig(15.0).size(), 2);
}