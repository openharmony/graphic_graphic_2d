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

#ifndef ROSEN_TEXT_PROPERTIES_TEXT_STYLE_H
#define ROSEN_TEXT_PROPERTIES_TEXT_STYLE_H

#include <map>
#include <string>
#include <vector>

#include "draw/color.h"
#include "draw/pen.h"

namespace rosen {
enum class TextDecoration {
    NONE = 0x0,
    UNDERLINE = 0x1,
    OVERLINE = 0x2,
    LINETHROUGH = 0x4,
};

enum class TextDecorationStyle {
    SOLID,
    DOUBLE,
    DOTTED,
    DASHED,
    WAVY,
};

enum class FontWeight {
    W100, // thin
    W200,
    W300,
    W400,
    W500,
    W600,
    W700,
    W800,
    W900,
};

enum class FontStyle {
    NORMAL,
    ITALIC,
};

enum class TextBaseline {
    ALPHABETIC,
    IDEOGRAPHIC,
};

enum class EllipsisModal {
    HEAD = 0,
    MIDDLE = 1,
    TAIL = 2,
};

class FontFeatures {
public:
    void SetFeature(std::string tag, int value);
    std::string GetFeatureSettings() const;
    // @not need all in txt
    std::map<std::string, int> getGetFontFeatures() const
    {
        return featureMap_;
    }
private:
    std::map<std::string, int> featureMap_;
};

class TextShadow {
public:
    OHOS::Rosen::Drawing::Color color_ = OHOS::Rosen::Drawing::Color::COLOR_BLACK;
    OHOS::Rosen::Drawing::Point offset_;
    double blurRadius_ = 0.0;
    TextShadow() {};
    TextShadow(OHOS::Rosen::Drawing::Color color, OHOS::Rosen::Drawing::Point offset, double blurRadius);
    bool operator == (const TextShadow& rhs) const;
    bool operator != (const TextShadow& rhs) const;
    bool hasShadow() const;
};

class DRAWING_API TextStyle {
public:
    OHOS::Rosen::Drawing::Color color_ = OHOS::Rosen::Drawing::Color::COLOR_WHITE;
    TextDecoration decoration_ = TextDecoration::NONE;
    OHOS::Rosen::Drawing::Color decorationColor_ = OHOS::Rosen::Drawing::Color::COLOR_TRANSPARENT;
    TextDecorationStyle decorationStyle_ = TextDecorationStyle::SOLID;
    double decorationThicknessMultiplier_ = 1.0;
    FontWeight fontWeight_ = FontWeight::W400;
    FontStyle fontStyle_ = FontStyle::NORMAL;
    TextBaseline textBaseline_ = TextBaseline::ALPHABETIC;
    std::vector<std::string> fontFamilies_;
    double fontSize_ = 14.0;
    double letterSpacing_ = 0.0;
    double wordSpacing_ = 0.0;
    double height_ = 1.0;
    bool hasHeightOverride_ = false;
    std::u16string ellipsis_;
    EllipsisModal ellipsisModal_ = EllipsisModal::TAIL;
    std::string locale_;
    bool hasBackground_ = false;
    OHOS::Rosen::Drawing::Pen background_;
    bool hasForeground_ = false;
    OHOS::Rosen::Drawing::Pen foreground_;
    std::vector<TextShadow> textShadows_;
    FontFeatures fontFeatures_;
    TextStyle();
    bool equals(const TextStyle& rhs) const;
};
} // namespace rosen
#endif // ROSEN_TEXT_PROPERTIES_TEXT_STYLE_H
