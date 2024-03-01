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

#ifndef ROSEN_MODULES_SPTEXT_DRAWING_PAINTER_IMPL_H
#define ROSEN_MODULES_SPTEXT_DRAWING_PAINTER_IMPL_H

#include "modules/skparagraph/include/ParagraphPainter.h"
#include "txt/paint_record.h"

#include "draw/canvas.h"
#include "rosen_text/symbol_animation_config.h"
#include "symbol_engine/hm_symbol_run.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
using ParagraphPainter = skia::textlayout::ParagraphPainter;

class RSCanvasParagraphPainter : public ParagraphPainter {
public:
    RSCanvasParagraphPainter(Drawing::Canvas* canvas, const std::vector<PaintRecord>& paints);

    void drawTextBlob(const std::shared_ptr<RSTextBlob>& blob, SkScalar x, SkScalar y,
        const SkPaintOrID& paint) override;
    void drawTextShadow(const std::shared_ptr<RSTextBlob>& blob, SkScalar x, SkScalar y, SkColor color,
        SkScalar blurSigma) override;
    void drawRect(const SkRect& rect, const SkPaintOrID& paint) override;
    void drawRRect(const SkRRect& rrect, const SkColor color) override;
    void drawFilledRect(const SkRect& rect, const DecorationStyle& decorStyle) override;
    void drawPath(const RSPath& path, const DecorationStyle& decorStyle) override;
    void drawLine(SkScalar x0, SkScalar y0, SkScalar x1, SkScalar y1, const DecorationStyle& decorStyle) override;
    void SymbolAnimation(const PaintRecord &pr);
    void DrawSymbolSkiaTxt(RSTextBlob* blob, const RSPoint& offset, const PaintRecord &pr);

    void clipRect(const SkRect& rect) override;
    void translate(SkScalar dx, SkScalar dy) override;

    void save() override;
    void restore() override;

    void SetAnimation(
        std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>& animationFunc
    )
    {
        if (animationFunc != nullptr) {
            animationFunc_ = animationFunc;
        }
    }

    void SetParagraphId(uint32_t id)
    {
        paragraphId_ = id;
    }

private:
    Drawing::Canvas* canvas_;
    const std::vector<PaintRecord>& paints_;
    std::function<bool(
        const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)> animationFunc_ = nullptr;
    uint64_t symbolCount_ = 0;
    uint32_t paragraphId_ = 0;
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_DRAWING_PAINTER_IMPL_H
