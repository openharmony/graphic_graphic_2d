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

    canvas_->drawLine(x0, y0, x1, y1, paint.GetPaint());
}

void TexgineCanvas::DrawRect(TexgineRect &rect, const TexginePaint &paint) const
{
    if (canvas_ == nullptr || rect.GetRect() == nullptr) {
        return;
    }
    canvas_->drawRect(*rect.GetRect(), paint.GetPaint());
}

void TexgineCanvas::DrawTextBlob(std::shared_ptr<TexgineTextBlob> &blob, float x, float y, const TexginePaint &paint)
{
    if (canvas_ == nullptr || blob == nullptr) {
        return;
    }

    canvas_->drawTextBlob(blob->GetTextBlob(), x, y, paint.GetPaint());
}

void TexgineCanvas::Clear(uint32_t color) const
{
    if (canvas_ == nullptr) {
        return;
    }

    canvas_->clear(color);
}

int TexgineCanvas::Save()
{
    if (canvas_ == nullptr) {
        // 1 is failed
        return 1;
    }

    return canvas_->save();
}

void TexgineCanvas::Translate(float dx, float dy) const
{
    if (canvas_ == nullptr) {
        return;
    }

    canvas_->translate(dx, dy);
}

std::shared_ptr<SkCanvas> TexgineCanvas::GetCanvas() const
{
    return canvas_;
}

void TexgineCanvas::Restore() const
{
    if (canvas_ == nullptr) {
        return;
    }

    canvas_->restore();
}

void TexgineCanvas::SetCanvas(const SkCanvas &canvas)
{
    if (canvas_ == nullptr) {
        return;
    }

    *canvas_ = canvas;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
