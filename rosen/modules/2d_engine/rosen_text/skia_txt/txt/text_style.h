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

#ifndef ROSEN_MODULES_SPTEXT_TEXT_STYLE_H
#define ROSEN_MODULES_SPTEXT_TEXT_STYLE_H

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "include/core/SkColor.h"
#include "include/core/SkPoint.h"
#include "paint_record.h"
#include "text_types.h"
#include "symbol_engine/hm_symbol_txt.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
// FontFeatures is the container that stores the
// feature used to control how a font selects glyphs.
class FontFeatures {
public:
    void SetFeature(std::string tag, int value);

    std::string GetFeatureSettings() const;

    const std::map<std::string, int>& GetFontFeatures() const;

private:
    std::map<std::string, int> features_;
};

// Axis tags and values that can be applied in a text style to control the
// attributes of variable fonts.
class FontVariations {
public:
    void SetAxisValue(std::string tag, float value);

    const std::map<std::string, float>& GetAxisValues() const;

private:
    std::map<std::string, float> axis_;
};

// TextShadow contains parameters that control
// how the text shadow is displayed.
class TextShadow {
public:
    TextShadow();

    TextShadow(SkColor color, SkPoint offset, double blurSigma);

    bool operator==(const TextShadow& other) const;

    bool operator!=(const TextShadow& other) const;

    bool HasShadow() const;

    SkColor color = SK_ColorBLACK;
    SkPoint offset{0, 0};
    double blurSigma = 0.0;
};

struct RectStyle {
    SkColor color = 0;
    SkScalar leftTopRadius = 0.0f;
    SkScalar rightTopRadius = 0.0f;
    SkScalar rightBottomRadius = 0.0f;
    SkScalar leftBottomRadius = 0.0f;

    bool operator ==(const RectStyle& rhs) const
    {
        return color == rhs.color &&
            leftTopRadius == rhs.leftTopRadius &&
            rightTopRadius == rhs.rightTopRadius &&
            rightBottomRadius == rhs.rightBottomRadius &&
            leftBottomRadius == rhs.leftBottomRadius;
    }

    bool operator !=(const RectStyle& rhs) const
    {
        return color != rhs.color ||
            leftTopRadius != rhs.leftTopRadius ||
            rightTopRadius != rhs.rightTopRadius ||
            rightBottomRadius != rhs.rightBottomRadius ||
            leftBottomRadius != rhs.leftBottomRadius;
    }
};

// TextStyle is a collection of parameters that control how text is displayed,
// including parameters for fonts, decorations, and text.
class TextStyle {
public:
    TextStyle();

    bool operator==(TextStyle const& other) const;

    SkColor color = SK_ColorWHITE;

    TextDecoration decoration = TextDecoration::NONE;
    // transparent for no decoration color.
    SkColor decorationColor = SK_ColorTRANSPARENT;
    TextDecorationStyle decorationStyle = TextDecorationStyle::SOLID;
    double decorationThicknessMultiplier = 1.0;

    FontWeight fontWeight = FontWeight::W400;
    FontWidth fontWidth = FontWidth::NORMAL;
    FontStyle fontStyle = FontStyle::NORMAL;
    TextBaseline baseline = TextBaseline::ALPHABETIC;
    bool halfLeading = false;
    std::vector<std::string> fontFamilies;
    double fontSize = 14.0;
    double letterSpacing = 0.0;
    double wordSpacing = 0.0;
    double height = 1.0;
    bool heightOverride = false;
    std::string locale;
    RectStyle backgroundRect = {0, 0.0f, 0.0f, 0.0f, 0.0f};
    int styleId = 0;

    std::optional<PaintRecord> background;
    std::optional<PaintRecord> foreground;

    std::vector<TextShadow> textShadows;
    FontFeatures fontFeatures;
    FontVariations fontVariations;

    // symbol glyph
    bool isSymbolGlyph = false;
    HMSymbolTxt symbol;
    double baseLineShift = 0.0;
};

} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_TEXT_STYLE_H
