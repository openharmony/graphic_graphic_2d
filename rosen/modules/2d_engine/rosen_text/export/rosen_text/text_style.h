/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_TEXT_STYLE_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_TEXT_STYLE_H

#include <map>
#ifdef USE_GRAPHIC_TEXT_GINE
#include <optional>
#endif
#include <string>
#include <vector>

#include "draw/color.h"
#include "draw/pen.h"
#include "include/core/SkPaint.h" // SKIA
#include "utils/point.h"

#include "typography_types.h"

namespace OHOS {
namespace Rosen {
class FontFeatures {
public:
    void SetFeature(std::string tag, int value);
    std::string GetFeatureSettings() const;
    const std::map<std::string, int> &GetFontFeatures() const;
    bool operator ==(const FontFeatures& rhs) const;

private:
    std::map<std::string, int> featureMap_;
};

struct TextShadow {
#ifndef USE_GRAPHIC_TEXT_GINE
    Drawing::Color color_ = Drawing::Color::COLOR_BLACK;
    Drawing::PointF offset_;
    double blurRadius_ = 0.0;
#else
    Drawing::Color color = Drawing::Color::COLOR_BLACK;
    Drawing::PointF offset;
    double blurRadius = 0.0;
#endif

    TextShadow();
#ifndef USE_GRAPHIC_TEXT_GINE
    TextShadow(Drawing::Color color, Drawing::PointF offset, double blurRadius);
#else
    TextShadow(Drawing::Color shadowColor, Drawing::Point shadowOffset, double shadowBlurRadius);
#endif
    bool operator ==(const TextShadow& rhs) const;
    bool operator !=(const TextShadow& rhs) const;
    bool HasShadow() const;
};

struct TextStyle {
#ifndef USE_GRAPHIC_TEXT_GINE
    Drawing::Color color_ = Drawing::Color::COLOR_WHITE;
    TextDecoration decoration_ = TextDecoration::NONE;
    Drawing::Color decorationColor_ = Drawing::Color::COLOR_TRANSPARENT;
    TextDecorationStyle decorationStyle_ = TextDecorationStyle::SOLID;
    double decorationThicknessScale_ = 1.0;
    FontWeight fontWeight_ = FontWeight::W400;
    FontStyle fontStyle_ = FontStyle::NORMAL;
    TextBaseline baseline_ = TextBaseline::ALPHABETIC;
    std::vector<std::string> fontFamilies_;
    double fontSize_ = 14.0; // default is libtxt text style font size
    double letterSpacing_ = 0.0;
    double wordSpacing_ = 0.0;
    double heightScale_ = 1.0;
    bool heightOnly_ = false;
    std::u16string ellipsis_;
    std::string locale_;
    std::optional<SkPaint> background_; // SKIA
    std::optional<SkPaint> foreground_; // SKIA
#else
    Drawing::Color color = Drawing::Color::COLOR_WHITE;
    TextDecoration decoration = TextDecoration::NONE;
    Drawing::Color decorationColor = Drawing::Color::COLOR_TRANSPARENT;
    TextDecorationStyle decorationStyle = TextDecorationStyle::SOLID;
    double decorationThicknessScale = 1.0;
    FontWeight fontWeight = FontWeight::W400;
    FontStyle fontStyle = FontStyle::NORMAL;
    TextBaseline baseline = TextBaseline::ALPHABETIC;
    std::vector<std::string> fontFamilies;
    double fontSize = 14.0; // default is libtxt text style font size
    double letterSpacing = 0.0;
    double wordSpacing = 0.0;
    double heightScale = 1.0;
    bool heightOnly = false;
    std::u16string ellipsis;
    std::string locale;
    std::optional<SkPaint> background; // SKIA
    std::optional<SkPaint> foreground; // SKIA
#endif
    // if Pen and SkPaint are setting, use pen first
#ifndef USE_GRAPHIC_TEXT_GINE
    std::optional<Drawing::Pen> backgroundPen_;
    std::optional<Drawing::Pen> foregroundPen_;
    std::vector<TextShadow> shadows_;
    FontFeatures fontFeatures_;
#else
    std::optional<Drawing::Pen> backgroundPen;
    std::optional<Drawing::Pen> foregroundPen;
    std::vector<TextShadow> shadows;
    FontFeatures fontFeatures;
#endif

    bool operator ==(const TextStyle &rhs) const;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TEXT_STYLE_H
