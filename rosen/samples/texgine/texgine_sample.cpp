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

#include <include/core/SkCanvas.h>
#include <include/effects/SkDashPathEffect.h>
#include <skia_framework.h>
#include <texgine/typography.h>
#include <texgine/utils/exlog.h>
#include <texgine/utils/memory_usage_scope.h>
#include <texgine/utils/trace.h>

#include "feature_test/feature_test_framework.h"
#include "texgine_canvas.h"
#include "texgine_paint.h"
#include "texgine_rect.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
SkColor colors[] = {
    SK_ColorRED,
    SK_ColorYELLOW,
    SK_ColorGREEN,
    SK_ColorCYAN,
    SK_ColorBLUE,
    SK_ColorMAGENTA
};
} // namespace

void OnDraw(SkCanvas &canvas)
{
    SkPaint actualBorderPaint;
    actualBorderPaint.setStyle(SkPaint::kStroke_Style);

    SkPaint borderPaint = actualBorderPaint;
    const SkScalar intervals[2] = {1.0f, 1.0f};
    borderPaint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0.0f));

    SkPaint testBorderPaint = borderPaint;
    testBorderPaint.setColor(0xff000000);

    SkPaint rainbowPaint;
    rainbowPaint.setStyle(SkPaint::kFill_Style);

    canvas.save();
    canvas.translate(50, 50);
    double y = 0;
    TexgineCanvas texgineCanvas;
    texgineCanvas.SetCanvas(&canvas);

    const auto &tests = FeatureTestCollection::GetInstance().GetTests();
    for (const auto &ptest : tests) {
        if (ptest == nullptr) {
            continue;
        }

        double yStart = y;
        canvas.save();
        canvas.translate(50, 50);
        const auto &option = ptest->GetFeatureTestOption();
        const auto &typographies = ptest->GetTypographies();

        double maxHeight = 0;
        double x = 0;
        for (const auto &data : typographies) {
            const auto &typography = data.typography;
            if ((x + typography->GetMaxWidth() >= 800) || (x != 0 && data.atNewline)) {
                x = 0;
                y += maxHeight + option.marginTop;
                maxHeight = 0;
            }

            if (data.onPaint) {
                data.onPaint(data, texgineCanvas, x, y);
            } else {
                typography->Paint(texgineCanvas, x, y);
            }

            if (data.needRainbowChar.value_or(option.needRainbowChar)) {
                canvas.save();
                canvas.translate(x, y);
                Boundary boundary = {data.rainbowStart, data.rainbowEnd};
                auto boxes = typography->GetTextRectsByBoundary(boundary, data.hs, data.ws);
                int32_t rainbowColorIndex = 0;
                for (auto &box : boxes) {
                    rainbowPaint.setColor(colors[rainbowColorIndex++]);
                    rainbowPaint.setAlpha(255 * 0.2);
                    canvas.drawRect(*(box.rect_.GetRect().get()), rainbowPaint);
                    rainbowPaint.setColor(SK_ColorGRAY);
                    rainbowPaint.setAlpha(255 * 0.3);
                    canvas.drawRect(*(box.rect_.GetRect().get()), rainbowPaint);
                    rainbowColorIndex %= sizeof(colors) / sizeof(*colors);
                }
                canvas.restore();
            }

            if (!data.comment.empty()) {
                SkiaFramework::DrawString(canvas, data.comment, x, y);
            }

            if (option.needBorder) {
                borderPaint.setColor(option.colorBorder);
                canvas.drawRect(SkRect::MakeXYWH(x, y, typography->GetMaxWidth(),
                    typography->GetHeight()), borderPaint);

                actualBorderPaint.setColor(option.colorBorder);
                canvas.drawRect(SkRect::MakeXYWH(x, y, typography->GetActualWidth(),
                    typography->GetHeight()), actualBorderPaint);
            }

            x += typography->GetMaxWidth() + option.marginLeft;
            maxHeight = std::max(maxHeight, typography->GetHeight());
        }
        y += maxHeight + option.marginTop + 50;

        canvas.restore();
        canvas.drawRect(SkRect::MakeXYWH(0, yStart, 800, y - yStart), testBorderPaint);
        SkiaFramework::DrawString(canvas, ptest->GetTestName(), 0, yStart);
    }
    canvas.restore();
}

int main()
{
    const auto &tests = FeatureTestCollection::GetInstance().GetTests();
    for (const auto &ptest : tests) {
        if (ptest == nullptr) {
            continue;
        }

        MemoryUsageScope scope(ptest->GetTestName());
        ScopedTrace layoutScope(ptest->GetTestName());
        LOGSCOPED(sl, LOG2EX_DEBUG(), ptest->GetTestName());
        ptest->Layout();
        for (const auto &typography : ptest->GetTypographies()) {
            ReportMemoryUsage("typography", *typography.typography, true);
        }
    }

    SkiaFramework sf;
    sf.SetWindowWidth(720);
    sf.SetWindowHeight(1280);
    sf.SetWindowScale(720.0 / 900.0);
    sf.SetDrawFunc(OnDraw);
    sf.Run();
    return 0;
}
