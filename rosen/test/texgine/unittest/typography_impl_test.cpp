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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "shaper.h"
#include "typography_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct MockVars {
    std::shared_ptr<TypographyStyle> typographyStyle_ = std::make_shared<TypographyStyle>();
    std::vector<VariantSpan> variantSpan_;
    std::shared_ptr<FontProviders> providers_ = FontProviders::Create();
    std::vector<LineMetrics> lineMetrics_;
    std::shared_ptr<Boundary> boundary_;
} g_tiMockvars;

void InitTiMockVars(struct MockVars &&vars)
{
    g_tiMockvars = std::move(vars);
}

std::vector<LineMetrics> Shaper::DoShape(std::vector<VariantSpan> spans, const TypographyStyle &ys,
    const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit)
{
    return g_tiMockvars.lineMetrics_;
}

void VariantSpan::PaintShadow(TexgineCanvas &canvas, double offsetx, double offsety) noexcept(false)
{
}

void VariantSpan::Paint(TexgineCanvas &canvas, double offsetx, double offsety) noexcept(false)
{
}

class TypographyImplTest : public testing::Test {
};

/**
 * @tc.name:Layout
 * @tc.desc: Verify the Layout
 * @tc.type:FUNC
 */
HWTEST_F(TypographyImplTest, Layout, TestSize.Level1)
{
    InitTiMockVars({});
    std::shared_ptr<TypographyImpl> ti = std::make_shared<TypographyImpl>(*g_tiMockvars.typographyStyle_,
        g_tiMockvars.variantSpan_, g_tiMockvars.providers_);
    EXPECT_NO_THROW({
        InitTiMockVars({});
        ti->Layout(0.0);

        LineMetrics lm;
        std::vector<LineMetrics> lineMetrics;
        lineMetrics.emplace_back(lm);
        InitTiMockVars({.lineMetrics_ = lineMetrics});
        ti->Layout(0.0);
    });
}

/**
 * @tc.name:Paint
 * @tc.desc: Verify the Paint
 * @tc.type:FUNC
 */
HWTEST_F(TypographyImplTest, Paint, TestSize.Level1)
{
    InitTiMockVars({});
    std::shared_ptr<TypographyImpl> ti = std::make_shared<TypographyImpl>(*g_tiMockvars.typographyStyle_,
        g_tiMockvars.variantSpan_, g_tiMockvars.providers_);
    EXPECT_NO_THROW({
        std::shared_ptr<VariantSpan> vs = std::make_shared<VariantSpan>(TextSpan::MakeEmpty());
        std::vector<VariantSpan> variantSpan;
        variantSpan.emplace_back(*vs);
        LineMetrics lm = {.lineSpans = variantSpan};
        std::vector<LineMetrics> lineMetrics;
        lineMetrics.emplace_back(lm);
        InitTiMockVars({.lineMetrics_ = lineMetrics});
        std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
        ti->Layout(0.0);
        ti->Paint(*tc, 0.0, 0.0);
    });
}

/**
 * @tc.name:GetTextRectsByBoundary
 * @tc.desc: Verify the GetTextRectsByBoundary
 * @tc.type:FUNC
 */
HWTEST_F(TypographyImplTest, GetTextRectsByBoundary, TestSize.Level1)
{
    InitTiMockVars({});
    std::shared_ptr<TypographyImpl> ti = std::make_shared<TypographyImpl>(*g_tiMockvars.typographyStyle_,
        g_tiMockvars.variantSpan_, g_tiMockvars.providers_);
    EXPECT_NO_THROW({
        std::shared_ptr<VariantSpan> vs = std::make_shared<VariantSpan>(TextSpan::MakeEmpty());
        std::vector<VariantSpan> variantSpan;
        variantSpan.emplace_back(*vs);
        LineMetrics lm = {.lineSpans = variantSpan};
        std::vector<LineMetrics> lineMetrics;
        lineMetrics.emplace_back(lm);
        std::shared_ptr<Boundary> boundary1 = std::make_shared<Boundary>(1, 0);
        std::shared_ptr<Boundary> boundary2 = std::make_shared<Boundary>(-1, 0);
        InitTiMockVars({.lineMetrics_ = lineMetrics, .boundary_ = boundary1});
        ti->Layout(0.0);
        ti->GetTextRectsByBoundary(*g_tiMockvars.boundary_, TextRectHeightStyle::TIGHT,
            TextRectWidthStyle::MAX_WIDTH);
        InitTiMockVars({.lineMetrics_ = lineMetrics, .boundary_ = boundary2});
        ti->Layout(0.0);
        ti->GetTextRectsByBoundary(*g_tiMockvars.boundary_, TextRectHeightStyle::TIGHT,
            TextRectWidthStyle::MAX_WIDTH);
    });
}

/**
 * @tc.name:GetGlyphIndexByCoordinate
 * @tc.desc: Verify the GetGlyphIndexByCoordinate
 * @tc.type:FUNC
 */
HWTEST_F(TypographyImplTest, GetGlyphIndexByCoordinate, TestSize.Level1)
{
    InitTiMockVars({});
    std::shared_ptr<TypographyImpl> ti = std::make_shared<TypographyImpl>(*g_tiMockvars.typographyStyle_,
        g_tiMockvars.variantSpan_, g_tiMockvars.providers_);
    EXPECT_NO_THROW({
        ti->Layout(0.0);
        ti->GetGlyphIndexByCoordinate(0.0, 0.0);
        ti->GetGlyphIndexByCoordinate(1.0, 0.0);
        ti->GetGlyphIndexByCoordinate(0.0, 1.0);
        ti->GetGlyphIndexByCoordinate(1.0, 1.0);
    });
}

/**
 * @tc.name:GetWordBoundaryByIndex
 * @tc.desc: Verify the GetWordBoundaryByIndex
 * @tc.type:FUNC
 */
HWTEST_F(TypographyImplTest, GetWordBoundaryByIndex, TestSize.Level1)
{
    InitTiMockVars({});
    std::shared_ptr<TypographyImpl> ti = std::make_shared<TypographyImpl>(*g_tiMockvars.typographyStyle_,
        g_tiMockvars.variantSpan_, g_tiMockvars.providers_);
    EXPECT_NO_THROW({
        ti->GetWordBoundaryByIndex(0);
        ti->GetWordBoundaryByIndex(1);
    });
}

/**
 * @tc.name:GetFunctions
 * @tc.desc: Verify the GetFunction
 * @tc.type:FUNC
 */
HWTEST_F(TypographyImplTest, GetFunctions, TestSize.Level1)
{
    InitTiMockVars({});
    std::shared_ptr<TypographyImpl> ti = std::make_shared<TypographyImpl>(*g_tiMockvars.typographyStyle_,
        g_tiMockvars.variantSpan_, g_tiMockvars.providers_);
    EXPECT_NO_THROW({
        ti->GetMaxWidth();
        ti->GetHeight();
        ti->GetActualWidth();
        ti->GetMinIntrinsicWidth();
        ti->GetMaxIntrinsicWidth();
        ti->GetAlphabeticBaseline();
        ti->GetIdeographicBaseline();
        ti->GetLineCount();
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
