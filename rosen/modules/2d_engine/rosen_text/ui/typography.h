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

#ifndef ROSEN_TEXT_UI_TYPOGRAPHY_H_
#define ROSEN_TEXT_UI_TYPOGRAPHY_H_

#include <vector>
#include "rosen_text/properties/typography_base.h"
#include "rosen_text/properties/typography_create_base.h"
#include "rosen_text/properties/text_style.h"
#include "rosen_text/properties/typography_style.h"
#include "draw/canvas.h"

namespace rosen {
using namespace OHOS::Rosen::Drawing;
class DRAWING_API Typography {
public:
    Typography();
    ~Typography() = default;
    void Init(std::shared_ptr<TypographyCreateBase> typographyCreateBase);
    double GetMaxWidth();
    double GetHeight();
    double GetLongestLine();
    double GetMinIntrinsicWidth();
    double GetMaxIntrinsicWidth();
    double GetAlphabeticBaseline();
    double GetIdeographicBaseline();
    bool DidExceedMaxLines();
    void Layout(double width);
    void Paint(Canvas* canvas, double x, double y);

    std::vector<TypographyProperties::TextBox> GetRectsForRange(
        size_t start,
        size_t end,
        TypographyProperties::RectHeightStyle heightStyle,
        TypographyProperties::RectWidthStyle widthStyle);
    std::vector<TypographyProperties::TextBox> GetRectsForPlaceholders();
    TypographyProperties::PositionAndAffinity GetGlyphPositionAtCoordinate(double dx,
        double dy);

    TypographyProperties::PositionAndAffinity GetGlyphPositionAtCoordinateWithCluster(double dx,
        double dy);
    TypographyProperties::Range<size_t> GetWordBoundary(size_t offset);
    TypographyProperties::Range<size_t> GetActualTextRange(int lineNumber, bool includeSpaces);
    size_t GetLineCount();
    double GetLineHeight(int lineNumber);
    double GetLineWidth(int lineNumber);

private:
    std::unique_ptr<TypographyBase> typography_;
};
} // namespace rosen
#endif // ROSEN_TEXT_UI_TYPOGRAPHY_H_
