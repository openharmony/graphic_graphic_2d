/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef OHOS_ROSEN_TEXT_TYPOGRAPHY_MOCK_H
#define OHOS_ROSEN_TEXT_TYPOGRAPHY_MOCK_H

#include <gmock/gmock.h>

#include "rosen_text/typography.h"

namespace OHOS::Rosen {
class MockTypography : public Typography {
public:
    MOCK_METHOD(double, GetMaxWidth, (), (const, override));
    MOCK_METHOD(double, GetHeight, (), (const, override));
    MOCK_METHOD(double, GetActualWidth, (), (const, override));
    MOCK_METHOD(double, GetMinIntrinsicWidth, (), (override));
    MOCK_METHOD(double, GetMaxIntrinsicWidth, (), (override));
    MOCK_METHOD(double, GetAlphabeticBaseline, (), (override));
    MOCK_METHOD(double, GetIdeographicBaseline, (), (override));
    MOCK_METHOD(double, GetGlyphsBoundsTop, (), (override));
    MOCK_METHOD(double, GetGlyphsBoundsBottom, (), (override));
    MOCK_METHOD(double, GetGlyphsBoundsLeft, (), (override));
    MOCK_METHOD(double, GetGlyphsBoundsRight, (), (override));
    MOCK_METHOD(bool, DidExceedMaxLines, (), (const, override));
    MOCK_METHOD(int, GetLineCount, (), (const, override));
    MOCK_METHOD(void, MarkDirty, (), (override));
    MOCK_METHOD(int32_t, GetUnresolvedGlyphsCount, (), (override));
    MOCK_METHOD(void, UpdateFontSize, (size_t from, size_t to, float fontSize), (override));
    MOCK_METHOD(void, SetIndents, (const std::vector<float>& indents), (override));
    MOCK_METHOD(float, DetectIndents, (size_t index), (override));
    MOCK_METHOD(void, Layout, (double width), (override));
    MOCK_METHOD(void, Paint, (SkCanvas* canvas, double x, double y), (override));
    MOCK_METHOD(void, Paint, (Drawing::Canvas* canvas, double x, double y), (override));
    MOCK_METHOD(void, Paint,
        (Drawing::Canvas* canvas, Drawing::Path* path, double hOffset, double vOffset), (override));
    MOCK_METHOD(std::vector<TextRect>, GetTextRectsByBoundary,
        (size_t left, size_t right, TextRectHeightStyle heightStyle, TextRectWidthStyle widthStyle), (override));
    MOCK_METHOD(std::vector<TextRect>, GetTextRectsOfPlaceholders, (), (override));
    MOCK_METHOD(IndexAndAffinity, GetGlyphIndexByCoordinate, (double x, double y), (override));
    MOCK_METHOD(Boundary, GetWordBoundaryByIndex, (size_t index), (override));
    MOCK_METHOD(Boundary, GetActualTextRange, (int lineNumber, bool includeSpaces), (override));
    MOCK_METHOD(Boundary, GetEllipsisTextRange, (), (override));
    MOCK_METHOD(double, GetLineHeight, (int lineNumber), (override));
    MOCK_METHOD(double, GetLineWidth, (int lineNumber), (override));
    MOCK_METHOD(void, SetAnimation,
        (std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& animationFunc), (override));
    MOCK_METHOD(void, SetParagraghId, (uint32_t id), (override));
    MOCK_METHOD(Drawing::FontMetrics, MeasureText, (), (override));
    MOCK_METHOD(bool, GetLineInfo,
        (int lineNumber, bool oneLine, bool includeWhitespace, LineMetrics* lineMetrics), (override));
    MOCK_METHOD(std::vector<LineMetrics>, GetLineMetrics, (), (override));
    MOCK_METHOD(bool, GetLineMetricsAt, (int lineNumber, LineMetrics* lineMetrics), (override));
    MOCK_METHOD(Drawing::FontMetrics, GetFontMetrics, (const OHOS::Rosen::TextStyle& textStyle), (override));
    MOCK_METHOD(bool, GetLineFontMetrics,
        (size_t lineNumber, size_t& charNumber, std::vector<Drawing::FontMetrics>& fontMetrics), (override));
    MOCK_METHOD(std::vector<std::unique_ptr<TextLineBase>>, GetTextLines, (), (const, override));
    MOCK_METHOD(std::unique_ptr<Typography>, CloneSelf, (), (override));
    MOCK_METHOD(double, GetLongestLineWithIndent, (), (const, override));
    MOCK_METHOD(void, UpdateColor, (size_t from, size_t to, const Drawing::Color& color), (override));
    MOCK_METHOD(void*, GetParagraph, (), (override));
    MOCK_METHOD(Drawing::RectI, GeneratePaintRegion, (double x, double y), (const, override));
    MOCK_METHOD(void, Relayout,
        (double width, const TypographyStyle& typograhyStyle, const std::vector<TextStyle>& textStyles), (override));
    MOCK_METHOD(void, UpdateAllTextStyles, (const TextStyle& textStyleTemplate), (override));
    MOCK_METHOD(void, SetTextEffectState, (bool state), (override));
    MOCK_METHOD(bool, HasEnabledTextEffect, (), (const, override));
    MOCK_METHOD(void, SetTextEffectAssociation, (bool association), (override));
    MOCK_METHOD(bool, GetTextEffectAssociation, (), (const, override));
    MOCK_METHOD(std::vector<TextBlobRecordInfo>, GetTextBlobRecordInfo, (), (const, override));
};
} // namespace OHOS::Rosen
#endif