/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {

RSPaintFilterCanvas::RSPaintFilterCanvas(SkCanvas* canvas, float alpha)
    : SkPaintFilterCanvas(canvas), alphaStack_({ std::clamp(alpha, 0.f, 1.f) }) // construct stack with given alpha
{}

RSPaintFilterCanvas::RSPaintFilterCanvas(SkSurface* skSurface, float alpha)
    : SkPaintFilterCanvas(skSurface ? skSurface->getCanvas() : nullptr), skSurface_(skSurface),
      alphaStack_({ std::clamp(alpha, 0.f, 1.f) }) // construct stack with given alpha
{}

SkSurface* RSPaintFilterCanvas::GetSurface() const
{
    return skSurface_;
}

bool RSPaintFilterCanvas::onFilter(SkPaint& paint) const
{
    if (alphaStack_.top() >= 1.f) {
        return true;
    } else if (alphaStack_.top() <= 0.f) {
        return false;
    }
    // use alphaStack_.top() to multiply alpha
    paint.setAlphaf(paint.getAlphaf() * alphaStack_.top());
    return true;
}

void RSPaintFilterCanvas::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint)
{
    SkPaint filteredPaint(paint ? *paint : SkPaint());
    if (this->onFilter(filteredPaint)) {
        this->SkCanvas::onDrawPicture(picture, matrix, &filteredPaint);
    }
}

void RSPaintFilterCanvas::MultiplyAlpha(float alpha)
{
    // multiply alpha to top of stack
    alphaStack_.top() *= std::clamp(alpha, 0.f, 1.f);
}

int RSPaintFilterCanvas::SaveAlpha()
{
    // make a copy of top of stack
    alphaStack_.push(alphaStack_.top());
    // return prev stack height
    return alphaStack_.size() - 1;
}

float RSPaintFilterCanvas::GetAlpha() const
{
    // return top of stack
    return alphaStack_.top();
}

void RSPaintFilterCanvas::RestoreAlpha()
{
    // sanity check, stack should not be empty
    if (alphaStack_.size() <= 1u) {
        return;
    }
    alphaStack_.pop();
}

void RSPaintFilterCanvas::RestoreAlphaToCount(int count)
{
    // sanity check, stack should not be empty
    if (count < 1) {
        count = 1;
    }
    // poo stack until stack height equals count
    int n = static_cast<int>(alphaStack_.size()) - count;
    for (int i = 0; i < n; ++i) {
        alphaStack_.pop();
    }
}

std::pair<int, int> RSPaintFilterCanvas::SaveCanvasAndAlpha()
{
    // simultaneously save canvas and alpha
    return { save(), SaveAlpha() };
}

void RSPaintFilterCanvas::RestoreCanvasAndAlpha(std::pair<int, int>& count)
{
    // simultaneously restore canvas and alpha
    restoreToCount(count.first);
    RestoreAlphaToCount(count.second);
}
} // namespace Rosen
} // namespace OHOS
