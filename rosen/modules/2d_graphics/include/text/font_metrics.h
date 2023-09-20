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

#ifndef FONT_METRICS_H
#define FONT_METRICS_H

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct FontMetrics {
    bool operator==(const FontMetrics& that)
    {
        return fFlags == that.fFlags &&
               fTop == that.fTop &&
               fAscent == that.fAscent &&
               fDescent == that.fDescent &&
               fBottom == that.fBottom &&
               fLeading == that.fLeading &&
               fAvgCharWidth == that.fAvgCharWidth &&
               fMaxCharWidth == that.fMaxCharWidth &&
               fXMin == that.fXMin &&
               fXMax == that.fXMax &&
               fXHeight == that.fXHeight &&
               fCapHeight == that.fCapHeight &&
               fUnderlineThickness == that.fUnderlineThickness &&
               fUnderlinePosition == that.fUnderlinePosition &&
               fStrikeoutThickness == that.fStrikeoutThickness &&
               fStrikeoutPosition == that.fStrikeoutPosition;
    }

    enum FontMetricsFlags {
        UNDERLINE_THICKNESS_IS_VALID_FLAG = 1 << 0,
        UNDERLINE_POSITION_IS_VALID_FLAG  = 1 << 1,
        STRIKEOUT_THICKNESS_IS_VALID_FLAG = 1 << 2,
        STRIKEOUT_POSITION_IS_VALID_FLAG  = 1 << 3,
        BOUNDS_INVALID_FLAG               = 1 << 4,
    };

    uint32_t fFlags;
    scalar fTop;
    scalar fAscent;
    scalar fDescent;
    scalar fBottom;
    scalar fLeading;
    scalar fAvgCharWidth;
    scalar fMaxCharWidth;
    scalar fXMin;
    scalar fXMax;
    scalar fXHeight;
    scalar fCapHeight;
    scalar fUnderlineThickness;
    scalar fUnderlinePosition;
    scalar fStrikeoutThickness;
    scalar fStrikeoutPosition;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif