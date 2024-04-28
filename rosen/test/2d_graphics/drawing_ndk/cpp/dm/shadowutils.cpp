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
#include "shadowutils.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include "test_common.h"
#include "common/log_common.h"
#include <vector>
#include <array>

enum {
    kW = 800,
    kH = 960,
    N = 4
};

// DrawRect mapRect( OH_Drawing_Matrix* m, DrawRect& rect)  {
//     OH_Drawing_Point2D src[] = {{rect.fLeft, rect.fTop},
//                                 {rect.fRight, rect.fTop},
//                                 {rect.fLeft, rect.fBottom},
//                                 {rect.fRight, rect.fBottom}};
//     OH_Drawing_Point2D dst[N];
//
//     //mapRect函数未能实现 设定src 等于 dst
//     for (int i = 0; i < N;i++)
//         dst[i] = src[i];
//
////    OH_Drawing_MatrixSetPolyToPoly(m,src,dst,4);
////    OH_Drawing_Rect* srcRc = OH_Drawing_RectCreate(rect.fLeft, rect.fTop, rect.fRight, rect.fBottom);
////    OH_Drawing_Rect* dstRc = OH_Drawing_RectCreate(0, 0, 0, 0);
////    OH_Drawing_MatrixSetRectToRect(m, srcRc, dstRc, OH_Drawing_ScaleToFit::SCALE_TO_FIT_CENTER);
//
//    // 计算新矩形的边界
//    float left = std::min({dst[0].x,dst[1].x,dst[2].x,dst[3].x});
//    float top = std::min({dst[0].y,dst[1].y,dst[2].y,dst[3].y});
//    float right = std::max({dst[0].x,dst[1].x,dst[2].x,dst[3].x});
//    float bottom = std::max({dst[0].y,dst[1].y,dst[2].y,dst[3].y});
//
//    DRAWING_LOGI("mapxy src  {(%{public}2f, %{public}2f,%{public}2f, %{public}2f)}",rect.fLeft,rect.fTop,rect.fRight,rect.fBottom);
//    DRAWING_LOGI("mapxy dst  {(%{public}2f, %{public}2f,%{public}2f, %{public}2f)}",left,top,right,bottom);
//    return {left,top,right,bottom};
//}

void draw_shadow(OH_Drawing_Canvas *canvas, OH_Drawing_Path *path, float height, uint32_t color,
                 OH_Drawing_Point3D lightPos, float lightR, bool isAmbient, OH_Drawing_CanvasShadowFlags flags) {
    float ambientAlpha = isAmbient ? .5f : 0.f;
    float spotAlpha = isAmbient ? 0.f : .5f;
    uint8_t a = (color >> 24) & 0xFF;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    uint32_t ambientColor = OH_Drawing_ColorSetArgb(a * ambientAlpha, r, g, b);
    uint32_t spotColor = OH_Drawing_ColorSetArgb(a * spotAlpha, r, g, b);
    OH_Drawing_Point3D planeParams = {0, 0, height};

    OH_Drawing_CanvasDrawShadow(canvas, path, planeParams, lightPos, lightR, ambientColor, spotColor, flags);
}

ShadowUtils::ShadowUtils(ShadowMode m) : sMode(m) {
    // skia dm file gm/fontregen.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "shadowutils";
}

void ShadowUtils::OnTestFunction(OH_Drawing_Canvas *canvas) {
    draw_paths(canvas, sMode);
}

void ShadowUtils::draw_paths(OH_Drawing_Canvas *canvas, ShadowMode mode) {
    std::vector<OH_Drawing_Path *> paths;
    std::vector<DrawRect> pathsBounds;
    // add rrect
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, 50, 50);
    OH_Drawing_RoundRect *roundRect = OH_Drawing_RoundRectCreate(rect, 10.0, 10.00002f);
    OH_Drawing_PathAddRoundRect(path, roundRect, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    paths.push_back(OH_Drawing_PathCopy(path));
    pathsBounds.push_back({0, 0, 50, 50});
    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_PathDestroy(path);

    // add rrect 这里应该是不规则的圆角矩形，四个圆角不同，oh暂不支持，先用普通圆角代替
    OH_Drawing_RoundRect *addRRect = OH_Drawing_RoundRectCreate(rect, 9.0, 13.0); // skia oddRRect.setNinePatch(SkRect::MakeWH(50, 50), 9, 13, 6, 16);
    path = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRoundRect(path, addRRect, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    paths.push_back(OH_Drawing_PathCopy(path));
    pathsBounds.push_back({0, 0, 50, 50});
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RoundRectDestroy(addRRect);

    // add rect
    path = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRect(path, 0, 0, 50, 50, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    paths.push_back(OH_Drawing_PathCopy(path));
    pathsBounds.push_back({0, 0, 50, 50});
    OH_Drawing_PathDestroy(path);

    // add circle
    path = OH_Drawing_PathCreate();
    OH_Drawing_PathAddArc(path, rect, 0, 360);
    paths.push_back(OH_Drawing_PathCopy(path));
    pathsBounds.push_back({0, 0, 50, 50});
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);

    path = OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(path, 100, 50, 20, 100, 0, 0);
    paths.push_back(OH_Drawing_PathCopy(path));
    pathsBounds.push_back({0, 0, 100, 100});
    OH_Drawing_PathDestroy(path);

    // add oval
    path = OH_Drawing_PathCreate();
    rect = OH_Drawing_RectCreate(0, 0, 20, 60);
    OH_Drawing_PathAddArc(path, rect, 0, 360);
    paths.push_back(OH_Drawing_PathCopy(path));
    pathsBounds.push_back({0, 0, 20, 60});
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);

    // star
    std::vector<OH_Drawing_Path *> concavePaths;
    std::vector<DrawRect> concavePathsBounds;
    path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0.0, -33.3333);
    OH_Drawing_PathLineTo(path, 9.62, -16.6667);
    OH_Drawing_PathLineTo(path, 28.867f, -16.6667f);
    OH_Drawing_PathLineTo(path, 19.24f, 0.0f);
    OH_Drawing_PathLineTo(path, 28.867f, 16.6667f);
    OH_Drawing_PathLineTo(path, 9.62f, 16.6667f);
    OH_Drawing_PathLineTo(path, 0.0f, 33.3333f);
    OH_Drawing_PathLineTo(path, -9.62f, 16.6667f);
    OH_Drawing_PathLineTo(path, -28.867f, 16.6667f);
    OH_Drawing_PathLineTo(path, -19.24f, 0.0f);
    OH_Drawing_PathLineTo(path, -28.867f, -16.6667f);
    OH_Drawing_PathLineTo(path, -9.62f, -16.6667f);
    OH_Drawing_PathClose(path);
    concavePaths.push_back(OH_Drawing_PathCopy(path));
    concavePathsBounds.push_back({-28.867f, -33.3333, 28.867f, 33.3333f});
    OH_Drawing_PathDestroy(path);

    // dumbbell
    path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 50.0, 0);
    OH_Drawing_PathCubicTo(path, 100, 25, 60, 50, 50, 0);
    OH_Drawing_PathCubicTo(path, 0, -25, 40, -50, 50, 0);
    concavePaths.push_back(OH_Drawing_PathCopy(path));
    concavePathsBounds.push_back({0, -50, 100, 50});
    OH_Drawing_PathDestroy(path);

    static constexpr float kPad = 15.f;
    static constexpr float kLightR = 100.f;
    static constexpr float kHeight = 50.f;

    // transform light position relative to canvas to handle tiling
    OH_Drawing_Point2D lightXY = {250, 400};
    OH_Drawing_Point3D lightPos = {lightXY.x, lightXY.y, 500};

    OH_Drawing_CanvasTranslate(canvas, 3 * kPad, 3 * kPad);
    OH_Drawing_CanvasSave(canvas);
    float x = 0;
    float dy = 0;
    std::vector<OH_Drawing_Matrix *> matrices;
    OH_Drawing_Matrix *mI = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixReset(mI);
    matrices.push_back(mI);

    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreateRotation(33.0, 25.0, 25.0);
    //    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    //    OH_Drawing_MatrixReset(matrix);
    //    OH_Drawing_MatrixRotate(matrix, 33.0, 25.0, 25.0);
    OH_Drawing_MatrixPostScale(matrix, 1.2, 0.8, 25.0, 25.0);
    matrices.push_back(matrix);

    float N = 80;
    for (auto m : matrices) {
        for (OH_Drawing_CanvasShadowFlags flags : {SHADOW_FLAGS_NONE, SHADOW_FLAGS_TRANSPARENT_OCCLUDER}) {
            int pathCounter = 0;
            for (const auto path : paths) {
                // 计算各个path的bound，避免碰撞，并对其进行移动
                DrawRect postMBounds = pathsBounds[pathCounter];
                float boundWidth = postMBounds.width();
                float boundHeight = postMBounds.height();
                //                if(!OH_Drawing_MatrixIsIdentity(m)){
                ////                    postMBounds = mapRect(m, pathsBounds[pathCounter]);
                //                    OH_Drawing_Rect* srcRect = DrawCreateRect(postMBounds);
                //                    OH_Drawing_Rect* dstRect = OH_Drawing_RectCreate(0, 0, 0, 0);
                //                    OH_Drawing_MatrixSetRectToRect(matrixCopy, srcRect, dstRect, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
                //                    boundWidth = OH_Drawing_RectGetWidth(dstRect);
                //                    boundHeight = OH_Drawing_RectGetHeight(dstRect);
                //                    OH_Drawing_RectDestroy(srcRect);
                //                    OH_Drawing_RectDestroy(dstRect);
                //
                //                }
                float w = boundWidth + kHeight;
                float dx = w + kPad;
                if (x + dx > kW - 3 * kPad) {
                    OH_Drawing_CanvasRestore(canvas);
                    OH_Drawing_CanvasTranslate(canvas, 0, dy);
                    OH_Drawing_CanvasSave(canvas);
                    x = 0;
                    dy = 0;
                }
                /////test
                OH_Drawing_CanvasSave(canvas);
                OH_Drawing_CanvasConcatMatrix(canvas, m);
                if (SHADOW_FLAGS_TRANSPARENT_OCCLUDER == flags && 0 == pathCounter % 3) {
                    OH_Drawing_CanvasSave(canvas);
                    OH_Drawing_CanvasRotate(canvas, 180, 25, 25);
                }
                if (kDebugColorNoOccluders == mode || kDebugColorOccluders == mode) {
                    draw_shadow(canvas, path, kHeight, 0xFFFF0000, lightPos, kLightR, true, flags);
                    draw_shadow(canvas, path, kHeight, 0xFF0000FF, lightPos, kLightR, false, flags);
                } else if (kGrayscale == mode) {
                    uint32_t ambientColor = OH_Drawing_ColorSetArgb(0.1 * 255, 0, 0, 0);
                    uint32_t spotColor = OH_Drawing_ColorSetArgb(0.25 * 255, 0, 0, 0);
                    OH_Drawing_CanvasDrawShadow(canvas, path, {0, 0, kHeight}, lightPos, kLightR, ambientColor, spotColor, flags);
                }

                OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
                OH_Drawing_BrushSetAntiAlias(brush, true);
                OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
                OH_Drawing_PenSetAntiAlias(pen, true);
                if (kDebugColorNoOccluders == mode) {
                    if (flags & SHADOW_FLAGS_TRANSPARENT_OCCLUDER) {
                        OH_Drawing_PenSetColor(pen, 0xFF00FFFF); // SK_ColorCYAN
                    } else {
                        OH_Drawing_PenSetColor(pen, 0xFF00FF00); // SK_ColorGREEN
                    }
                    OH_Drawing_PenSetWidth(pen, 0);
                    OH_Drawing_CanvasAttachPen(canvas, pen);
                } else {
                    OH_Drawing_BrushSetColor(brush, kDebugColorOccluders == mode ? 0xFFCCCCCC : 0xFFFFFFFF); // SK_ColorLTGRAY : SK_ColorWHITE
                    if (flags & SHADOW_FLAGS_TRANSPARENT_OCCLUDER)
                        OH_Drawing_BrushSetAlpha(brush, 0x80); // 0.5 alpha
                    OH_Drawing_CanvasAttachBrush(canvas, brush);
                }
                OH_Drawing_CanvasDrawPath(canvas, path);
                OH_Drawing_CanvasDetachPen(canvas);
                OH_Drawing_CanvasDetachBrush(canvas);
                OH_Drawing_PenDestroy(pen);
                OH_Drawing_BrushDestroy(brush);
                if (SHADOW_FLAGS_TRANSPARENT_OCCLUDER == flags && 0 == pathCounter % 3) {
                    OH_Drawing_CanvasRestore(canvas);
                }
                OH_Drawing_CanvasRestore(canvas);
                // 同样需要移动
                OH_Drawing_CanvasTranslate(canvas, dx, 0);
                x += dx;

                dy = std::max(dy, boundHeight + kPad + kHeight);
                //                dy = std::max(dy, postMBounds.height() + kPad + kHeight);
                ++pathCounter;
            }
        }
    }
    dy = 3 * N;
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasTranslate(canvas, 0, dy);
    OH_Drawing_CanvasSave(canvas);
    x = kPad;
    dy = 0;
    for (auto m : matrices) {
        int pathCounter = 0;
        for (const auto path : concavePaths) {
            /// 防止碰撞

            DrawRect postMBounds = concavePathsBounds[pathCounter];
            float boundWidth = postMBounds.width();
            float boundHeight = postMBounds.height();
            //            if(!OH_Drawing_MatrixIsIdentity(m)){
            ////                postMBounds = mapRect(m, concavePathsBounds[pathCounter]);
            //                OH_Drawing_Rect* srcRect = DrawCreateRect(postMBounds);
            //                OH_Drawing_Rect* dstRect = OH_Drawing_RectCreate(0, 0, 0, 0);
            //                OH_Drawing_MatrixSetRectToRect(matrixCopy, srcRect, dstRect, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
            //                boundWidth = OH_Drawing_RectGetWidth(dstRect);
            //                boundHeight = OH_Drawing_RectGetHeight(dstRect);
            //                OH_Drawing_RectDestroy(srcRect);
            //                OH_Drawing_RectDestroy(dstRect);
            //            }
            float w = boundWidth + kHeight;
            float dx = w + kPad;

            OH_Drawing_CanvasSave(canvas);
            OH_Drawing_CanvasConcatMatrix(canvas, m);
            if (kDebugColorNoOccluders == mode || kDebugColorOccluders == mode) {
                draw_shadow(canvas, path, kHeight, 0xFFFF0000, lightPos, kLightR,
                            true, SHADOW_FLAGS_NONE);
                draw_shadow(canvas, path, kHeight, 0xFF0000FF, lightPos, kLightR,
                            false, SHADOW_FLAGS_NONE);
            } else if (kGrayscale == mode) {
                uint32_t ambientColor = OH_Drawing_ColorSetArgb(0.1 * 255, 0, 0, 0);
                uint32_t spotColor = OH_Drawing_ColorSetArgb(0.25 * 255, 0, 0, 0);
                OH_Drawing_CanvasDrawShadow(canvas, path, {0, 0, kHeight}, lightPos, kLightR, ambientColor, spotColor, SHADOW_FLAGS_NONE);
            }
            OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
            OH_Drawing_BrushSetAntiAlias(brush, true);
            OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
            OH_Drawing_PenSetAntiAlias(pen, true);
            if (kDebugColorNoOccluders == mode) {
                OH_Drawing_PenSetColor(pen, 0xFF00FF00);
                OH_Drawing_PenSetWidth(pen, 0);
                OH_Drawing_CanvasAttachPen(canvas, pen);
            } else {
                OH_Drawing_BrushSetColor(brush, kDebugColorOccluders == mode ? 0xFFCCCCCC : 0xFFFFFFFF); // SK_ColorLTGRAY : SK_ColorWHITE
                OH_Drawing_CanvasAttachBrush(canvas, brush);
            }
            OH_Drawing_CanvasDrawPath(canvas, path);
            OH_Drawing_CanvasDetachPen(canvas);
            OH_Drawing_CanvasDetachBrush(canvas);
            OH_Drawing_PenDestroy(pen);
            OH_Drawing_BrushDestroy(brush);
            OH_Drawing_CanvasRestore(canvas);
            OH_Drawing_CanvasTranslate(canvas, dx, 0);
            x += dx;
            dy = std::max(dy, boundHeight + kPad + kHeight);
            pathCounter++;
        }
    }

    OH_Drawing_Matrix *invCanvasM = OH_Drawing_MatrixCreate();
    OH_Drawing_CanvasGetTotalMatrix(canvas, invCanvasM);
    OH_Drawing_Matrix *invert = OH_Drawing_MatrixCreate();
    ;

    bool ret1 = OH_Drawing_MatrixIsIdentity(invCanvasM);
    bool ret2 = OH_Drawing_MatrixInvert(invCanvasM, invert);
    if (ret1 || ret2) {
        if (ret1)
            OH_Drawing_MatrixSetMatrix(invCanvasM, 1, 0, 0, 0, 1, 0, 0, 0, 1);
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasConcatMatrix(canvas, invert);
        OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
        OH_Drawing_BrushSetAntiAlias(brush, true);
        OH_Drawing_BrushSetColor(brush, 0xFF000000);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_Point *center = OH_Drawing_PointCreate(lightPos.x, lightPos.y);
        OH_Drawing_CanvasDrawCircle(canvas, center, kLightR / 10.0);
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_BrushDestroy(brush);
        OH_Drawing_CanvasRestore(canvas);
    }
}