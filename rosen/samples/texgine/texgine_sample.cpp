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

TexginePaint g_rainbowPaint;
TexginePaint g_actualBorderPaint;
TexginePaint g_borderPaint;

double Draw(TexgineCanvas &texgineCanvas, const std::list<struct TypographyData> &typographies,
    const struct FeatureTestOption &option, double y)
{
    double maxHeight = 0;
    double x = 0;
    for (const auto &data : typographies) {
        const auto &typography = data.typography;
        // 800 is the max width of all test
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
            texgineCanvas.Save();
            texgineCanvas.Translate(x, y);
            Boundary boundary = {data.rainbowStart, data.rainbowEnd};
            auto boxes = typography->GetTextRectsByBoundary(boundary, data.hs, data.ws);
            int32_t rainbowColorIndex = 0;
            for (auto &box : boxes) {
                g_rainbowPaint.SetColor(colors[rainbowColorIndex++]);
                // 255 is the max value of Alpha, 0.2 means the transparency set to 0.2
                g_rainbowPaint.SetAlpha(255 * 0.2);
                texgineCanvas.DrawRect(box.rect, g_rainbowPaint);
                g_rainbowPaint.SetColor(SK_ColorGRAY);
                // 255 is the max value of Alpha, 0.2 means the transparency set to 0.3
                g_rainbowPaint.SetAlpha(255 * 0.3);
                texgineCanvas.DrawRect(box.rect, g_rainbowPaint);
                rainbowColorIndex %= sizeof(colors) / sizeof(SkColor);
            }
            texgineCanvas.Restore();
        }
        if (!data.comment.empty()) {
            SkiaFramework::DrawString(*texgineCanvas.GetCanvas(), data.comment, x, y);
        }
        if (option.needBorder) {
            g_borderPaint.SetColor(option.colorBorder);
            TexgineRect rect1 = TexgineRect::MakeXYWH(x, y, typography->GetMaxWidth(), typography->GetHeight());
            texgineCanvas.DrawRect(rect1, g_borderPaint);

            g_actualBorderPaint.SetColor(option.colorBorder);
            TexgineRect rect2 = TexgineRect::MakeXYWH(x, y, typography->GetActualWidth(), typography->GetHeight());
            texgineCanvas.DrawRect(rect2, g_actualBorderPaint);
        }
        x += typography->GetMaxWidth() + option.marginLeft;
        maxHeight = std::max(maxHeight, typography->GetHeight());
    }
    // The upper and lower intervals of each test content is 50
    y += maxHeight + option.marginTop + 50;

    return y;
}

void OnDraw(RSCanvas &canvas)
{
    g_actualBorderPaint.SetStyle(TexginePaint::STROKE);

    RSPen borderPen = g_actualBorderPaint.GetPen();
    const float intervals[2] = {1.0f, 1.0f};
    // 2 means number of elements in the intervals array
    auto pathEffect = RSPathEffect::CreateDashPathEffect(intervals, 2, 0.0f);
    borderPen.SetPathEffect(pathEffect);

    TexginePaint paint;
    paint.SetPen(borderPen);
    g_borderPaint = paint;

    RSPen testBorderPen = borderPen;
    testBorderPen.SetColor(0xff000000);

    TexginePaint rainbowPaint;
    rainbowPaint.SetStyle(TexginePaint::Style::FILL);

    canvas.Save();
    // move canvas to (50, 50)
    canvas.Translate(50, 50);
    double y = 0;
    TexgineCanvas texgineCanvas;
    texgineCanvas.SetCanvas(&canvas);

    const auto &tests = FeatureTestCollection::GetInstance().GetTests();
    for (const auto &ptest : tests) {
        if (ptest == nullptr) {
            continue;
        }

        double yStart = y;
        canvas.Save();
        canvas.Translate(50, 50);   // move canvas to (50, 50)
        const auto &option = ptest->GetFeatureTestOption();
        const auto &typographies = ptest->GetTypographies();
        y = Draw(texgineCanvas, typographies, option, y);
        canvas.Restore();
        canvas.AttachPen(testBorderPen);
        // 800 is the max width of all test
        canvas.DrawRect(RSRect(0, yStart, 800, y));
        canvas.DetachPen();
        SkiaFramework::DrawString(canvas, ptest->GetTestName(), 0, yStart);
    }
    canvas.Restore();
}
} // namespace

int main()
{
    const auto &tests = FeatureTestCollection::GetInstance().GetTests();
    for (const auto &ptest : tests) {
        if (ptest == nullptr) {
            continue;
        }

        MemoryUsageScope scope(ptest->GetTestName());
        ScopedTrace layoutScope(ptest->GetTestName());
        LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), ptest->GetTestName());
        ptest->Layout();
        for (const auto &typography : ptest->GetTypographies()) {
            ReportMemoryUsage("typography", *typography.typography, true);
        }
    }

    SkiaFramework sf;
    sf.SetWindowWidth(720);     // 720 means the window width
    sf.SetWindowHeight(1280);   // 1280 means the window width
    sf.SetWindowScale(720.0 / 900.0);   // 720 / 900 means tht window's scale
    sf.SetDrawFunc(OnDraw);
    sf.Run();
    return 0;
}
