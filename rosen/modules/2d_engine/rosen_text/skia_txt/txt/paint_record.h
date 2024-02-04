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

#ifndef ROSEN_MODULES_SPTEXT_PAINT_RECORD_H
#define ROSEN_MODULES_SPTEXT_PAINT_RECORD_H

#include <optional>

#include "include/core/SkColor.h"

#include "draw/brush.h"
#include "draw/pen.h"
#include "symbol_engine/hm_symbol_txt.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
struct PaintRecord {
    using RSBrush = OHOS::Rosen::Drawing::Brush;
    using RSPen = OHOS::Rosen::Drawing::Pen;
    using RSColor = OHOS::Rosen::Drawing::Color;

    RSColor color;
    std::optional<RSBrush> brush;
    std::optional<RSPen> pen;
    // hm symbol feature
    bool isSymbolGlyph = false;
    HMSymbolTxt symbol;

    PaintRecord() {}
    PaintRecord(RSBrush brush, RSPen pen) : brush(brush), pen(pen) {}
    PaintRecord(std::optional<RSBrush> brush, std::optional<RSPen> pen) : brush(brush), pen(pen) {}

    void SetColor(const RSColor& c)
    {
        color = c;
    }

    void SetColor(SkColor c)
    {
        SetColor(ToRSColor(c));
    }

    bool operator==(const PaintRecord& rhs) const
    {
        return brush == rhs.brush && pen == rhs.pen;
    }

    bool operator!=(const PaintRecord& rhs) const
    {
        return !(*this == rhs);
    }

    static RSColor ToRSColor(SkColor c)
    {
        return RSColor(SkColorGetR(c), SkColorGetG(c), SkColorGetB(c), SkColorGetA(c));
    }
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_PAINT_RECORD_H
