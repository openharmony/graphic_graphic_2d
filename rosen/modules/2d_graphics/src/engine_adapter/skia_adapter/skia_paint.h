/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef SKIA_PAINT_H
#define SKIA_PAINT_H

#include "include/core/SkPaint.h"

#include <memory>
#include <vector>

#include "draw/brush.h"
#include "draw/pen.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct PaintData
{
    SkPaint paint;
    bool isEnabled = false;
};

class SkiaPaint {
public:
    SkiaPaint() noexcept;
    ~SkiaPaint() {};

    void ApplyBrushToFill(const Brush& brush);
    void ApplyPenToStroke(const Pen& pen);

    void BrushToSkPaint(const Brush& brush, SkPaint &paint);
    void PenToSkPaint(const Pen& pen, SkPaint &paint);

    void DisableStroke();
    void DisableFill();

    std::vector<std::shared_ptr<PaintData>> GetSortedPaints();
    void SetStrokeFirst(bool isStrokeFirst);
    bool IsStrokeFirst();
private:
    void ApplyFilter(SkPaint &paint, const Filter& filter);

    std::shared_ptr<PaintData> stroke_;
    std::shared_ptr<PaintData> fill_;
    bool isStrokeFirst_;
};
}
}
}
#endif