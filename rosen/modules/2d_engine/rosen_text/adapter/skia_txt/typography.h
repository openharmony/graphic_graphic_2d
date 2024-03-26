/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_ADAPTER_TXT_TYPOGRAPHY_H
#define ROSEN_TEXT_ADAPTER_TXT_TYPOGRAPHY_H

#include "rosen_text/typography.h"
#include "txt/paragraph.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
class Typography : public ::OHOS::Rosen::Typography {
public:
    explicit Typography(std::unique_ptr<SPText::Paragraph> paragraph);

    double GetMaxWidth() const override;
    double GetHeight() const override;
    double GetActualWidth() const override;
    double GetMinIntrinsicWidth() override;
    double GetMaxIntrinsicWidth() override;
    double GetAlphabeticBaseline() override;
    double GetIdeographicBaseline() override;
    double GetGlyphsBoundsTop() override;
    double GetGlyphsBoundsBottom() override;
    double GetGlyphsBoundsLeft() override;
    double GetGlyphsBoundsRight() override;
    bool DidExceedMaxLines() const override;
    int GetLineCount() const override;
    void MarkDirty() override;
    int32_t GetUnresolvedGlyphsCount() override;
    void UpdateFontSize(size_t from, size_t to, float fontSize) override;

    void SetIndents(const std::vector<float>& indents) override;
    float DetectIndents(size_t index) override;
    void Layout(double width) override;
    void Paint(SkCanvas *canvas, double x, double y) override;
    void Paint(Drawing::Canvas *drawCanvas, double x, double y) override;

    std::vector<TextRect> GetTextRectsByBoundary(size_t left, size_t right,
        TextRectHeightStyle heightStyle, TextRectWidthStyle widthStyle) override;
    std::vector<TextRect> GetTextRectsOfPlaceholders() override;
    IndexAndAffinity GetGlyphIndexByCoordinate(double x, double y) override;
    Boundary GetWordBoundaryByIndex(size_t index) override;
    Boundary GetActualTextRange(int lineNumber, bool includeSpaces) override;
    double GetLineHeight(int lineNumber) override;
    double GetLineWidth(int lineNumber) override;
    void SetAnimation(
        std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>& animationFunc
    ) override;
    void SetParagraghId(uint32_t id) override;
    Drawing::FontMetrics MeasureText() override;
    bool GetLineInfo(int lineNumber, bool oneLine, bool includeWhitespace, LineMetrics* lineMetrics) override;
    std::vector<LineMetrics> GetLineMetrics() override;
    bool GetLineMetricsAt(int lineNumber, LineMetrics* lineMetrics) override;
    Drawing::FontMetrics GetFontMetrics(const OHOS::Rosen::TextStyle& textStyle) override;
    bool GetLineFontMetrics(const size_t lineNumber, size_t& charNumber,
        std::vector<Drawing::FontMetrics>& fontMetrics) override;
private:
    std::unique_ptr<SPText::Paragraph> paragraph_ = nullptr;
};
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TXT_TYPOGRAPHY_H
