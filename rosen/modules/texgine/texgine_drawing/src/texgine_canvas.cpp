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

#include "texgine_canvas.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
void TexgineCanvas::DrawLine(double x0, double y0, double x1, double y1, const TexginePaint &paint)
{
    if (canvas_ == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->drawLine(x0, y0, x1, y1, paint.GetPaint());
#else
    RSPoint pointL(x0, y0);
    RSPoint pointR(x1, y1);
    if (paint.GetStyle() == TexginePaint::Style::FILL) {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->DrawLine(pointL, pointR);
        canvas_->DetachBrush();
    } else if (paint.GetStyle() == TexginePaint::Style::STROKE) {
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawLine(pointL, pointR);
        canvas_->DetachPen();
    } else {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawLine(pointL, pointR);
        canvas_->DetachBrush();
        canvas_->DetachPen();
    }
#endif
}

void TexgineCanvas::DrawRect(const TexgineRect &rect, const TexginePaint &paint) const
{
    if (canvas_ == nullptr || rect.GetRect() == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->drawRect(*rect.GetRect(), paint.GetPaint());
#else
    if (paint.GetStyle() == TexginePaint::Style::FILL) {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->DrawRect(*rect.GetRect());
        canvas_->DetachBrush();
    } else if (paint.GetStyle() == TexginePaint::Style::STROKE) {
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawRect(*rect.GetRect());
        canvas_->DetachPen();
    } else {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawRect(*rect.GetRect());
        canvas_->DetachBrush();
        canvas_->DetachPen();
    }
#endif
}

void TexgineCanvas::DrawRRect(const TexgineRect &rect, const TexginePaint &paint) const
{
    if (canvas_ == nullptr || rect.GetRRect() == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->drawRRect(*rect.GetRRect(), paint.GetPaint());
#else
    if (paint.GetStyle() == TexginePaint::Style::FILL) {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->DrawRoundRect(*rect.GetRRect());
        canvas_->DetachBrush();
    } else if (paint.GetStyle() == TexginePaint::Style::STROKE) {
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawRoundRect(*rect.GetRRect());
        canvas_->DetachPen();
    } else {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawRoundRect(*rect.GetRRect());
        canvas_->DetachBrush();
        canvas_->DetachPen();
    }
#endif
}

void TexgineCanvas::DrawTextBlob(
    const std::shared_ptr<TexgineTextBlob> &blob, float x, float y, const TexginePaint &paint)
{
    if (canvas_ == nullptr || blob == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->drawTextBlob(blob->GetTextBlob(), x, y, paint.GetPaint());
#else
    if (paint.GetStyle() == TexginePaint::Style::FILL) {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->DrawTextBlob(blob->GetTextBlob().get(), x, y);
        canvas_->DetachBrush();
    } else if (paint.GetStyle() == TexginePaint::Style::STROKE) {
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawTextBlob(blob->GetTextBlob().get(), x, y);
        canvas_->DetachPen();
    } else {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawTextBlob(blob->GetTextBlob().get(), x, y);
        canvas_->DetachBrush();
        canvas_->DetachPen();
    }
#endif
}

#ifndef USE_ROSEN_DRAWING
void TexgineCanvas::DrawSymbol(const HMSymbolData &symbol, SkPoint locate, const TexginePaint &paint)
#else
void TexgineCanvas::DrawSymbol(const RSHMSymbolData &symbol, RSPoint locate, const TexginePaint &paint)
#endif
{
    if (canvas_ == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->drawSymbol(symbol, locate, paint.GetPaint());
#else
    if (paint.GetStyle() == TexginePaint::Style::FILL) {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->DrawSymbol(symbol, locate);
        canvas_->DetachBrush();
    } else if (paint.GetStyle() == TexginePaint::Style::STROKE) {
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawSymbol(symbol, locate);
        canvas_->DetachPen();
    } else {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawSymbol(symbol, locate);
        canvas_->DetachBrush();
        canvas_->DetachPen();
    }
#endif
}

#ifndef USE_ROSEN_DRAWING
void TexgineCanvas::DrawPath(const SkPath &path, const TexginePaint &paint)
#else
void TexgineCanvas::DrawPath(const RSPath &path, const TexginePaint &paint)
#endif
{
    if (canvas_ == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->drawPath(path, paint.GetPaint());
#else
    if (paint.GetStyle() == TexginePaint::Style::FILL) {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->DrawPath(path);
        canvas_->DetachBrush();
    } else if (paint.GetStyle() == TexginePaint::Style::STROKE) {
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawPath(path);
        canvas_->DetachPen();
    } else {
        canvas_->AttachBrush(paint.GetBrush());
        canvas_->AttachPen(paint.GetPen());
        canvas_->DrawPath(path);
        canvas_->DetachBrush();
        canvas_->DetachPen();
    }
#endif
}

void TexgineCanvas::Clear(uint32_t color) const
{
    if (canvas_ == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->clear(color);
#else
    canvas_->Clear(color);
#endif
}

int TexgineCanvas::Save() const
{
    if (canvas_ == nullptr) {
        // 1 is failed
        return 1;
    }
#ifndef USE_ROSEN_DRAWING
    return canvas_->save();
#else
    return canvas_->Save();
#endif
}

void TexgineCanvas::Translate(float dx, float dy) const
{
    if (canvas_ == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->translate(dx, dy);
#else
    canvas_->Translate(dx, dy);
#endif
}

#ifndef USE_ROSEN_DRAWING
SkCanvas *TexgineCanvas::GetCanvas() const
#else
RSCanvas *TexgineCanvas::GetCanvas() const
#endif
{
    return canvas_;
}

void TexgineCanvas::Restore() const
{
    if (canvas_ == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->restore();
#else
    canvas_->Restore();
#endif
}

#ifndef USE_ROSEN_DRAWING
void TexgineCanvas::SetCanvas(SkCanvas *canvas)
#else
void TexgineCanvas::SetCanvas(RSCanvas *canvas)
#endif
{
    canvas_ = canvas;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
