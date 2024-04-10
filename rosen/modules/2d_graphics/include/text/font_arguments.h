/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FONT_ARGUMENTS_H
#define FONT_ARGUMENTS_H

#include <cstdint>

namespace OHOS {
namespace Rosen {
namespace Drawing {
// Represents a set of actual arguments for a font.
struct FontArguments {
struct VariationPosition {
    struct Coordinate {
        uint32_t axis;
        float value;
    };
    const Coordinate* coordinates;
    int coordinateCount;
};

/*
 * Specify a palette to use and overrides for palette entries.
 *  `overrides` is a list of pairs of palette entry index and color.
 *  The overriden palette entries will use the associated color.
 *  Override pairs with palette entry indices out of range will not be applied.
 *  Later override entries override earlier ones.
 */
struct Palette {
    struct Override {
        int index;
        uint32_t color;
    };
    int index;
    const Override* overrides;
    int overrideCount;
};

FontArguments(): fontCollectionIndex_(0), variationDesignPosition_{nullptr, 0}, palette_{0, nullptr, 0}
{
}

/* 
 * Specify the index of the desired font.
 *  Font formats like ttc, dfont, cff, cid, pfr, t42, t1, and fon may actually be indexed
 *  collections of fonts.
 */
FontArguments& SetCollectionIndex(int fontCollectionIndex)
{
    fontCollectionIndex_ = fontCollectionIndex;
    return *this;
}

/*
 * Specify a position in the variation design space.
 *  Any axis not specified will use the default value.
 *  Any specified axis not actually present in the font will be ignored.
 */
FontArguments& SetVariationDesignPosition(VariationPosition position)
{
    variationDesignPosition_.coordinates = position.coordinates;
    variationDesignPosition_.coordinateCount = position.coordinateCount;
    return *this;
}

int GetCollectionIndex() const
{
    return fontCollectionIndex_;
}

VariationPosition GetVariationDesignPosition() const
{
    return variationDesignPosition_;
}

FontArguments& SetPalette(Palette palette)
{
    palette_.index = palette.index;
    palette_.overrides = palette.overrides;
    palette_.overrideCount = palette.overrideCount;
    return *this;
}

Palette GetPalette() const
{
    return palette_;
}

private:
    int fontCollectionIndex_;
    VariationPosition variationDesignPosition_;
    Palette palette_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // FONT_ARGUMENTS_H
