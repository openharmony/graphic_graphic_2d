/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <variant>

#include <gtest/gtest.h>

#include "font_collection.h"
#include "font_styles.h"
#include "mock/mock_any_span.h"
#include "mock/mock_measurer.h"
#include "param_test_macros.h"
#include "texgine_exception.h"
#include "text_breaker.h"
#include "text_converter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::vector<std::string> families_;
std::shared_ptr<FontCollection> FontProviders::GenerateFontCollection(
    const std::vector<std::string> &families) const noexcept(true)
{
    families_ = families;
    std::vector<std::shared_ptr<VariantFontStyleSet>> sets;
    return std::make_shared<FontCollection>(std::move(sets));
}

std::unique_ptr<Measurer> measurer = nullptr;
std::unique_ptr<Measurer> Measurer::Create(const std::vector<uint16_t> &text, const FontCollection &fontCollection)
{
    return std::move(measurer);
}

std::vector<Boundary> boundaries;

class MeasurerForTest : public Measurer {
public:
    virtual ~MeasurerForTest() = default;
    MeasurerForTest() : Measurer({}, {{}}) {}
    void SetMeasurerArgs(int ret, const CharGroups &cgs)
    {
        ret_ = ret;
        cgs_ = cgs;
    }

    int Measure(CharGroups &cgs) override
    {
        cgs = cgs_;
        return ret_;
    }

    const std::vector<Boundary> &GetWordBoundary() const override
    {
        return boundaries;
    }

private:
    CharGroups cgs_;
    int ret_ = 0;
};

class TextBreakerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        tpstyle_.fontFamilies_ = {"seguiemj"};
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("m"), .glyphs_ = {{0x013B, 13.664}}, .visibleWidth_ = 14,});
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("o"), .glyphs_ = {{0x0145, 9.456}}, .visibleWidth_ = 10,});
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("s"), .glyphs_ = {{0x0166, 7.28}}, .visibleWidth_ = 8,});
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("t"), .glyphs_ = {{0x016E, 5.88}}, .visibleWidth_ = 6,});

        cgs2_ = cgs1_;
        cgs2_.PushBack({.chars_ = TextConverter::ToUTF16(" "), .glyphs_ = {{0x0002, 4.32}}, .invisibleWidth_ = 5,});
    }

    void PrepareWordBreak(int ret, CharGroups &cgs, WordBreakType type = WordBreakType::BREAKWORD)
    {
        tpstyle_.wordBreakType_ = type;
        auto m = std::make_unique<MeasurerForTest>();
        m->SetMeasurerArgs(ret, cgs);
        measurer = std::move(m);
    }

    static inline TypographyStyle tpstyle_;
    static inline TextStyle textStyle_;
    std::shared_ptr<TextSpan> tsNullptr_ = nullptr;
    std::shared_ptr<TextSpan> tsNormal_ = TextSpan::MakeFromText("most");
    CharGroups emptyCgs_ = CharGroups::CreateEmpty();
    static inline CharGroups cgs1_ = CharGroups::CreateEmpty();
    static inline CharGroups cgs2_ = CharGroups::CreateEmpty();
    FontCollection fontCollection_ = std::vector<std::shared_ptr<VariantFontStyleSet>>{};
    std::vector<VariantSpan> spans_;
    TextBreaker breaker;
    std::vector<Boundary> boundaries_;
};

/**
 * @tc.name: GenerateFontCollection1
 * @tc.desc: Verify the GenerateFontCollection
 * @tc.type:FUNC
 */
HWTEST_F(TextBreakerTest, GenerateFontCollection1, TestSize.Level1)
{
    textStyle_.fontFamilies_ = {};

    EXPECT_NO_THROW({
        auto ret = breaker.GenerateFontCollection(tpstyle_, textStyle_, FontProviders::Create());
        ASSERT_NE(ret, nullptr);
        ASSERT_EQ(tpstyle_.fontFamilies_, families_);
    });
}

/**
 * @tc.name: GenerateFontCollection2
 * @tc.desc: Verify the GenerateFontCollection
 * @tc.type:FUNC
 */
HWTEST_F(TextBreakerTest, GenerateFontCollection2, TestSize.Level1)
{
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument, breaker.GenerateFontCollection(tpstyle_, textStyle_, nullptr));
}

/**
 * @tc.name: GenerateFontCollection3
 * @tc.desc: Verify the GenerateFontCollection
 * @tc.type:FUNC
 */
HWTEST_F(TextBreakerTest, GenerateFontCollection3, TestSize.Level1)
{
    tpstyle_.fontFamilies_ = {"seguiemj"};
    textStyle_.fontFamilies_ = {"robot"};

    EXPECT_NO_THROW({
        auto ret = breaker.GenerateFontCollection(tpstyle_, textStyle_, FontProviders::Create());
        ASSERT_NE(ret, nullptr);
        ASSERT_EQ(textStyle_.fontFamilies_, families_);
    });
}

/**
 * @tc.name: Measure1
 * @tc.desc: Verify the Measure
 * @tc.type:FUNC
 */
HWTEST_F(TextBreakerTest, Measure1, TestSize.Level1)
{
    size_t leftIndex = 0;
    size_t rightIndex = 5;
    boundaries = {{ leftIndex, rightIndex }};
    PrepareWordBreak(1, emptyCgs_);

    EXPECT_NO_THROW({
        auto ret = breaker.Measure({}, tsNormal_->u16vect_, fontCollection_, emptyCgs_, boundaries_);
        ASSERT_NE(ret, 0);
    });
}

/**
 * @tc.name: Measure2
 * @tc.desc: Verify the Measure
 * @tc.type: FUNC
 */
HWTEST_F(TextBreakerTest, Measure2, TestSize.Level1)
{
    boundaries = {};
    PrepareWordBreak(0, emptyCgs_);

    EXPECT_NO_THROW({
        auto ret = breaker.Measure({}, tsNormal_->u16vect_, fontCollection_, emptyCgs_, boundaries_);
        ASSERT_EQ(ret, 1);
    });
}

/**
 * @tc.name: Measure3
 * @tc.desc: Verify the Measure
 * @tc.type:FUNC
 */
HWTEST_F(TextBreakerTest, Measure3, TestSize.Level1)
{
    size_t leftIndex = 0;
    size_t rightIndex = 5;
    boundaries = {{ leftIndex, rightIndex }};
    PrepareWordBreak(0, emptyCgs_);

    EXPECT_NO_THROW({
        auto ret = breaker.Measure({}, tsNormal_->u16vect_, fontCollection_, emptyCgs_, boundaries_);
        ASSERT_EQ(ret, 0);
    });
}

/**
 * @tc.name: BreakWord1
 * @tc.desc: Verify the BreakWord
 * @tc.type:FUNC
 */
HWTEST_F(TextBreakerTest, BreakWord1, TestSize.Level1)
{
    EXPECT_NO_THROW({
        breaker.BreakWord(emptyCgs_, tpstyle_, textStyle_, spans_);
        ASSERT_EQ(spans_.size(), 0);
    });
}

/**
 * @tc.name: BreakWord2
 * @tc.desc: Verify the BreakWord
 * @tc.type:FUNC
 */
HWTEST_F(TextBreakerTest, BreakWord2, TestSize.Level1)
{
    EXPECT_NO_THROW({
        breaker.BreakWord(cgs1_, tpstyle_, textStyle_, spans_);
        ASSERT_EQ(spans_.size(), 1);
        auto span = spans_[0].TryToTextSpan();
        ASSERT_EQ(span->GetPreBreak(), span->GetPostBreak());
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
