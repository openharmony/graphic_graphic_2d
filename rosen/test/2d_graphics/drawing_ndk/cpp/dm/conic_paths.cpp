/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "conic_paths.h"
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_brush.h>

void ConicPaths::Makepath()
{
    OH_Drawing_Path *conicCircle = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(conicCircle, 0, 0);
    OH_Drawing_PathConicTo(conicCircle, 0, 50, 50, 50, w);     // 50曲线
    OH_Drawing_PathRConicTo(conicCircle, 50, 0, 50, -50, w);   // 50,-50曲线
    OH_Drawing_PathRConicTo(conicCircle, 0, -50, -50, -50, w); // -50曲线
    OH_Drawing_PathRConicTo(conicCircle, -50, 0, -50, 50, w);  // 50,-50曲线
    pathsBounds.push_back({ 0, -50, 50, 50 });
    fPaths.push_back(conicCircle);

    OH_Drawing_Path *hyperbola = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(hyperbola, 0, 0);
    OH_Drawing_PathConicTo(hyperbola, 0, 100, 100, 100, 2); // 100,2曲线
    pathsBounds.push_back({ 0, 0, 100, 100 });
    fPaths.push_back(hyperbola);

    OH_Drawing_Path *thinHyperbola = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(thinHyperbola, 0, 0);
    OH_Drawing_PathConicTo(thinHyperbola, 100, 100, 5, 0, 2); // 100,5,2曲线
    pathsBounds.push_back({ 0, 0, 100, 100 });
    fPaths.push_back(thinHyperbola);

    OH_Drawing_Path *veryThinHyperbola = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(veryThinHyperbola, 0, 0);
    OH_Drawing_PathConicTo(veryThinHyperbola, 100, 100, 1, 0, 2); // 100,1,2曲线
    pathsBounds.push_back({ 0, 0, 100, 100 });
    fPaths.push_back(veryThinHyperbola);

    OH_Drawing_Path *closedHyperbola = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(closedHyperbola, 0, 0);
    OH_Drawing_PathConicTo(closedHyperbola, 100, 100, 0, 0, 2); // 100,2曲线
    pathsBounds.push_back({ 0, 0, 100, 100 });
    fPaths.push_back(closedHyperbola);

    OH_Drawing_Path *nearParabola = OH_Drawing_PathCreate();
    // using 1 as weight defaults to using quadTo
    OH_Drawing_PathMoveTo(nearParabola, 0, 0);
    OH_Drawing_PathConicTo(nearParabola, 0, 100, 100, 100, 0.999f); // 0, 100, 100, 100, 0.999f曲线
    pathsBounds.push_back({ 0, 0, 100, 100 });
    fPaths.push_back(nearParabola);

    OH_Drawing_Path *thinEllipse = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(thinEllipse, 0, 0);
    OH_Drawing_PathConicTo(thinEllipse, 100, 100, 5, 0, 0.5f); // 100, 100, 5, 0, 0.5f曲线
    pathsBounds.push_back({ 0, 0, 100, 100 });
    fPaths.push_back(thinEllipse);

    OH_Drawing_Path *veryThinEllipse = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(veryThinEllipse, 0, 0);
    OH_Drawing_PathConicTo(veryThinEllipse, 100, 100, 1, 0, 0.5f); // 100, 100, 1, 0, 0.5f曲线
    pathsBounds.push_back({ 0, 0, 100, 100 });
    fPaths.push_back(veryThinEllipse);

    OH_Drawing_Path *closedEllipse = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(closedEllipse, 0, 0);
    OH_Drawing_PathConicTo(closedEllipse, 100, 100, 0, 0, 0.5f); // 100, 100, 0, 0, 0.5f曲线
    pathsBounds.push_back({ 0, 0, 100, 100 });
    fPaths.push_back(closedEllipse);
}

void ConicPaths::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    constexpr uint8_t kAlphaValue[] = {0xFF, 0x40};
    constexpr float margin = 15;
    OH_Drawing_CanvasTranslate(canvas, margin, margin);

    Makepath();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();

    for (int p = 0; p < fPaths.size(); ++p) {
        OH_Drawing_CanvasSave(canvas);
        for (unsigned int a = 0; a < sizeof(kAlphaValue); ++a) {
            for (int aa = 0; aa < 2; ++aa) {     // 2 max
                for (int fh = 0; fh < 2; ++fh) { // 2 max
                    OH_Drawing_PenSetWidth(pen, fh != 0);
                    DrawRect bounds = pathsBounds[p]; // const SkRect& bounds = fPaths[p].getBounds();
                    OH_Drawing_CanvasSave(canvas);
                    OH_Drawing_CanvasTranslate(canvas, -bounds.left, -bounds.top);
                    if (fh != 0) {
                        OH_Drawing_CanvasDetachBrush(canvas);
                        OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(kAlphaValue[a], 0, 0, 0));
                        OH_Drawing_CanvasAttachPen(canvas, pen);
                        OH_Drawing_PenSetAntiAlias(pen, (bool)aa);
                        OH_Drawing_CanvasAttachPen(canvas, pen);
                        OH_Drawing_CanvasDrawPath(canvas, fPaths[p]);
                    } else {
                        OH_Drawing_CanvasDetachPen(canvas);
                        OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(kAlphaValue[a], 0, 0, 0));
                        OH_Drawing_BrushSetAntiAlias(brush, (bool)aa);
                        OH_Drawing_CanvasAttachBrush(canvas, brush);
                        OH_Drawing_CanvasDrawPath(canvas, fPaths[p]);
                        OH_Drawing_CanvasDetachBrush(canvas);
                    }
                    OH_Drawing_CanvasRestore(canvas);

                    OH_Drawing_CanvasTranslate(canvas, 110, 0); // 110距离
                }
            }
        }
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, 0, 110); // 110距离
    }
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);

    // draw fGiantCircle path
    OH_Drawing_Path *fGiantCircle = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(fGiantCircle, 2.1e+11f, -1.05e+11f);
    OH_Drawing_PathConicTo(fGiantCircle, 2.1e+11f, 0, 1.05e+11f, 0, w);
    OH_Drawing_PathConicTo(fGiantCircle, 0, 0, 0, -1.05e+11f, w);
    OH_Drawing_PathConicTo(fGiantCircle, 0, -2.1e+11f, 1.05e+11f, -2.1e+11f, w);
    OH_Drawing_PathConicTo(fGiantCircle, 2.1e+11f, -2.1e+11f, 2.1e+11f, -1.05e+11f, w);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawPath(canvas, fGiantCircle);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);

    OH_Drawing_PathDestroy(fGiantCircle);
    for (int f = 0; f < fPaths.size(); ++f) {
        OH_Drawing_PathDestroy(fPaths[f]);
    }
}
