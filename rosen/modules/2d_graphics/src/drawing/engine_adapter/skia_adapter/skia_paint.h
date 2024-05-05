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

#include <memory>
#include <vector>

#include "include/core/SkPaint.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkShader.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkImageFilter.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPathEffect.h"
#include "src/core/SkPaintDefaults.h"

#include "draw/paint.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
const int MAX_PAINTS_NUMBER = 2;
struct SortedPaints {
    SkPaint* paints_[MAX_PAINTS_NUMBER] = { 0 };
    int count_ = 0;
};

class SkiaPaint {
public:
    SkiaPaint() noexcept;
    ~SkiaPaint();

    static void BrushToSkPaint(const Brush& brush, SkPaint& paint);
    static void PenToSkPaint(const Pen& pen, SkPaint& paint);
    static void PaintToSkPaint(const Paint& paint, SkPaint& skPaint);

    void ApplyPaint(const Paint& paint);
    SortedPaints& GetSortedPaints();
    void Reset();

    static bool CanComputeFastBounds(const Brush& brush);
    static const Rect& ComputeFastBounds(const Brush& brush, const Rect& orig, Rect* storage);

    static bool AsBlendMode(const Brush& brush);

private:
    static void ApplyFilter(SkPaint& paint, const Filter& filter);
    static void ApplyStrokeParam(const Paint& paint, SkPaint& skPaint);

    int paintInUse_ = 0;
    SkPaint paints_[MAX_PAINTS_NUMBER];
    SkPaint defaultPaint_;

    SortedPaints sortedPaints_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif