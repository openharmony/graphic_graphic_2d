/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_PROPERITES_TYPOGRAPHY_TXT_H_
#define ROSEN_TEXT_PROPERITES_TYPOGRAPHY_TXT_H_

#include <vector>
#include "rosen_text/properties/text_style.h"
#include "rosen_text/properties/typography_style.h"
#include "rosen_text/properties/typography_base.h"
#include "rosen_text/properties/typography_properties.h"
#include "txt/paragraph_txt.h"
#include "draw/canvas.h"

namespace rosen {
using namespace OHOS::Rosen::Drawing;
class TypographyTxt : public TypographyBase {
public:
    TypographyTxt();
    ~TypographyTxt();
    void Init(std::shared_ptr<TypographyCreateBase> typographyCreateBase) override;
    double GetMaxWidth() override;
    double GetHeight() override;
    double GetLongestLine() override;
    double GetMinIntrinsicWidth() override;
    double GetMaxIntrinsicWidth() override;
    double GetAlphabeticBaseline() override;
    double GetIdeographicBaseline() override;
    bool DidExceedMaxLines() override;
    void Layout(double width) override;
    void Paint(Canvas* canvas, double x, double y) override;
    void Paint(Canvas* canvas, Path* path, double hOffset, double vOffset) override;

    std::vector<TypographyProperties::TextBox> GetRectsForRange(
        size_t start,
        size_t end,
        TypographyProperties::RectHeightStyle heightStyle,
        TypographyProperties::RectWidthStyle widthStyle) override;
    std::vector<TypographyProperties::TextBox> GetRectsForPlaceholders() override;
    TypographyProperties::PositionAndAffinity GetGlyphPositionAtCoordinate(double dx,
        double dy) override;

    TypographyProperties::PositionAndAffinity GetGlyphPositionAtCoordinateWithCluster(double dx,
        double dy) override;
    TypographyProperties::Range<size_t> GetWordBoundary(size_t offset) override;
    TypographyProperties::Range<size_t> GetActualTextRange(int lineNumber, bool includeSpaces) override;
    size_t GetLineCount() override;
    double GetLineHeight(int lineNumber) override;
    double GetLineWidth(int lineNumber) override;

private:
    std::unique_ptr<txt::Paragraph> paragraphTxt_;
};
} // namespace rosen
#endif // ROSEN_TEXT_PROPERITES_TYPOGRAPHY_TXT_H_
