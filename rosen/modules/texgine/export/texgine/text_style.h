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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TEXT_STYLE_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TEXT_STYLE_H

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "texgine_paint.h"
#include "texgine/typography_types.h"
#include "symbol_engine/hm_symbol_txt.h"

#include "draw/pen.h"
namespace OHOS {
namespace Rosen {
namespace TextEngine {
/*
 * @brief FontFeatures is the container that stores the
 *        feature used to control how a font selects glyphs.
 */
class FontFeatures {
public:
    /*
     * @brief Set a feature by integer value.
     * @param ftag   The tag of feature.
     * @param fvalue The integer value of feature.
     */
    void SetFeature(const std::string& ftag, int fvalue);

    /*
     * @brief Returns the feature map that user set.
     */
    const std::map<std::string, int>& GetFeatures() const;

    /*
     * @brief Implements the equality operator.
     */
    bool operator ==(const FontFeatures& rhs) const;

private:
    friend void ReportMemoryUsage(const std::string& member, const FontFeatures& that, const bool needThis);

    std::map<std::string, int> features_;
};

/*
 * @brief TextShadow contains parameters that control
 *        how the text shadow is displayed.
 */
struct TextShadow {
    // The offset between the shaded text and the main text
    double offsetX = 0.0;
    double offsetY = 0.0;
    uint32_t color = 0xffcccccc;
    double blurLeave = 0;

    bool HasShadow() const;
    // Implements the equality operator.
    bool operator ==(TextShadow const& rhs) const;
};

/*
 * @brief RectStyle contains parameters that control
 *        how the text background rect is displayed.
 */
struct RectStyle {
    uint32_t color = 0;
    double leftTopRadius = 0.0;
    double rightTopRadius = 0.0;
    double rightBottomRadius = 0.0;
    double leftBottomRadius = 0.0;

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

enum class RoundRectType {
    NONE,
    LEFT_ONLY,
    RIGHT_ONLY,
    ALL,
};

/*
 * @brief TextStyle is a collection of parameters that control how text is displayed,
 *        including parameters for fonts, decorations, and text.
 */
struct TextStyle {
    // font style
    FontWeight fontWeight = FontWeight::W400;
    FontStyle fontStyle = FontStyle::NORMAL;
    std::vector<std::string> fontFamilies = {};
    double fontSize = 14.0;
    FontFeatures fontFeature;

    // Decoration style
    TextDecoration decoration = TextDecoration::NONE;
    std::optional<uint32_t> decorationColor = std::nullopt;
    TextDecorationStyle decorationStyle = TextDecorationStyle::SOLID;
    double decorationThicknessScale = 1.0;

    // text style
    uint32_t color = 0xff000000; // black
    TextBaseline baseline = TextBaseline::ALPHABETIC;
    std::string locale = "";
    bool halfLeading = false;
    bool heightOnly = false; // true means text height is heightScale_ * fontSize_
    double heightScale = 1.0;
    double letterSpacing = 0.0;
    double wordSpacing = 0.0;
    std::optional<TexginePaint> foreground = std::nullopt;
    std::optional<TexginePaint> background = std::nullopt;
    std::vector<TextShadow> shadows;
    RectStyle backgroundRect = {0, 0.0, 0.0, 0.0, 0.0};
    int styleId = 0;
    // Implements the equality operator.
    bool operator ==(TextStyle const& rhs) const;

    // symbol glyph
    bool isSymbolGlyph = false;
    HMSymbolTxt symbol;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TEXT_STYLE_H
