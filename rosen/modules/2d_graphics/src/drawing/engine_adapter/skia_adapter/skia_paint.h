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

#include "draw/brush.h"
#include "draw/pen.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct SkStyleFillCore {
    bool antiAlias_ = false;
    uint8_t alpha_ = 0;
    SkColor color_ = 0xFF000000;
    SkBlendMode blendMode_ = SkBlendMode::kSrcOver;
    sk_sp<SkColorSpace> colorSpace_ = nullptr;
    sk_sp<SkShader> shader_ = nullptr;
    sk_sp<SkColorFilter> colorFilter_ = nullptr;
    sk_sp<SkImageFilter> imageFilter_ = nullptr;
    sk_sp<SkMaskFilter> maskFilter_ = nullptr;

    void Reset()
    {
        antiAlias_ = false;
        alpha_ = 0xFF;
        color_ = 0xFF000000;
        blendMode_ = SkBlendMode::kSrcOver;
        colorSpace_ = nullptr;
        shader_ = nullptr;
        colorFilter_ = nullptr;
        imageFilter_ = nullptr;
        maskFilter_ = nullptr;
    }

    bool operator==(const SkStyleFillCore& other) const
    {
        return antiAlias_ == other.antiAlias_ && alpha_ == other.alpha_ && color_ == other.color_ &&
               blendMode_ == other.blendMode_ && colorSpace_ == other.colorSpace_ && shader_ == other.shader_ &&
               colorFilter_ == other.colorFilter_ && imageFilter_ == other.imageFilter_ &&
               maskFilter_ == other.maskFilter_;
    }
};

struct SkStyleStrokeCore {
    SkScalar miter_ = SkPaintDefaults_MiterLimit;
    SkScalar width_ = 0;
    SkPaint::Join join_ = SkPaint::kDefault_Join;
    SkPaint::Cap cap_ = SkPaint::kDefault_Cap;
    sk_sp<SkPathEffect> pathEffect_ = nullptr;
    SkStyleFillCore extend_;

    void Reset()
    {
        miter_ = SkPaintDefaults_MiterLimit;
        width_ = 0;
        join_ = SkPaint::kDefault_Join;
        cap_ = SkPaint::kDefault_Cap;
        pathEffect_ = nullptr;
        extend_.Reset();
    }
};

struct PaintData {
    union {
        SkStyleFillCore* fillCore_;
        SkStyleStrokeCore* strokeCore_;
    } paintData_;
    bool isEnabled_ = false;
};

const int MAX_PAINTS_NUMBER = 2;
struct SortedPaints {
    SkPaint* paints_[MAX_PAINTS_NUMBER] = { 0 };
    int count_ = 0;
};

class SkiaPaint {
public:
    SkiaPaint() noexcept;
    ~SkiaPaint();

    void ApplyBrushToFill(const Brush& brush);
    void ApplyPenToStroke(const Pen& pen);

    static void BrushToSkPaint(const Brush& brush, SkPaint& paint);
    static void PenToSkPaint(const Pen& pen, SkPaint& paint);

    void DisableStroke();
    void DisableFill();

    SortedPaints& GetSortedPaints();
    void SetStrokeFirst(bool isStrokeFirst);
    bool IsStrokeFirst() const;

    static bool CanComputeFastBounds(const Brush& brush);
    static const Rect& ComputeFastBounds(const Brush& brush, const Rect& orig, Rect* storage);

    static bool AsBlendMode(const Brush& brush);

private:
    template <class T>
    void ApplyBrushOrPenToFillCore(const T& brushOrPen, SkStyleFillCore& fillData);
    void ApplyPenToStrokeCore(const Pen& pen, SkStyleStrokeCore& strokeData);
    void GenerateFillPaint();
    void GenerateStrokePaint();
    void GenerateFillAndStrokePaint();
    void ApplyFillCoreToSkPaint(const SkStyleFillCore& fillCore, SkPaint& paint);
    void ApplyStrokeCoreToSkPaint(const SkStyleStrokeCore& strokeCore, SkPaint& paint);
    static void ApplyFilter(SkPaint& paint, const Filter& filter);

    bool isStrokeFirst_;
    PaintData stroke_;
    PaintData fill_;

    SkPaint fillPaint_;
    SkPaint strokePaint_;
    SkPaint fillAndStrokePaint_;
    SkPaint defaultPaint_;

    SortedPaints sortedPaints_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif