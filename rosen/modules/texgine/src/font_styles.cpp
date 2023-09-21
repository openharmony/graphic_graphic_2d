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

#include <map>

#include "font_styles.h"
#include "texgine_exception.h"
#include "texgine/utils/exlog.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define MULTIPLE 100
#define DIFFERENCE 1

namespace {
FontStyles::Slant ToSlant(const FontStyle style)
{
    std::map<FontStyle, FontStyles::Slant> switchMap = {
        {FontStyle::NORMAL, FontStyles::Slant::UPRIGHT},
        {FontStyle::ITALIC, FontStyles::Slant::ITALIC}
    };
    return switchMap[style];
}

TexgineFontStyle::Slant ToTexgineSlant(const FontStyles::Slant slant)
{
    std::map<FontStyles::Slant, TexgineFontStyle::Slant> switchMap = {
        {FontStyles::Slant::UPRIGHT, TexgineFontStyle::Slant::K_UPRIGHT_SLANT},
        {FontStyles::Slant::ITALIC, TexgineFontStyle::Slant::K_ITALIC_SLANT},
        {FontStyles::Slant::OBLIQUE, TexgineFontStyle::Slant::K_OBLIQUE_SLANT},
    };
    return switchMap[slant];
}
} // namespace

FontStyles::FontStyles(FontWeight weight, FontStyle style)
{
    if (!(0 <= static_cast<int>(weight) && weight < FontWeight::MAX)) {
        LOGEX_FUNC_LINE(ERROR) << "FontWeight Error!";
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    if (!(0 <= static_cast<int>(style) && style < FontStyle::MAX)) {
        LOGEX_FUNC_LINE(ERROR) << "FontStyle Error!";
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    slant_ = ToSlant(style);
    weight_ = static_cast<Weight>(static_cast<int>(weight) + DIFFERENCE);
}

FontStyles::FontStyles(FontStyles::Weight weight, FontStyles::Width width, FontStyles::Slant slant)
{
    if (!(0 <= static_cast<int>(weight) && weight < Weight::MAX)) {
        LOGEX_FUNC_LINE(ERROR) << "Weight Error!";
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    if (!(0 <= static_cast<int>(width) && width < Width::MAX)) {
        LOGEX_FUNC_LINE(ERROR) << "Width Error!";
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    if (!(0 <= static_cast<int>(slant) && slant < FontStyles::Slant::MAX)) {
        LOGEX_FUNC_LINE(ERROR) << "Slant Error!";
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    weight_ = weight;
    width_ = width;
    slant_ = slant;
}

TexgineFontStyle FontStyles::ToTexgineFontStyle() const
{
    return TexgineFontStyle(static_cast<int>(weight_) * MULTIPLE,
        static_cast<int>(width_) + DIFFERENCE, ToTexgineSlant(slant_));
}

int FontStyles::GetWeight() const
{
    return static_cast<int>(weight_);
}

int FontStyles::GetFontStyle() const
{
    return static_cast<int>(slant_);
}

void FontStyles::SetWeight(const int weight)
{
    weight_ = static_cast<Weight>(weight);
}

bool FontStyles::operator==(const FontStyles &rhs) const
{
    return weight_ == rhs.weight_ && width_ == rhs.width_ && slant_ == rhs.slant_;
}

bool FontStyles::operator!=(const FontStyles &rhs) const
{
    return !(*this == rhs);
}

bool FontStyles::operator<(const FontStyles& rhs) const
{
    if (weight_ != rhs.weight_) {
        return weight_ < rhs.weight_;
    }

    if (width_ != rhs.width_) {
        return width_ < rhs.width_;
    }
    return slant_ < rhs.slant_;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
