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

#include <array>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <vector>

#include "test_common.h"

#include "common/log_common.h"

enum { K_W = 800, K_H = 960 };
typedef struct {
    float height;
    uint32_t color;
    OH_Drawing_Point3D lightPos;
    float lightR;
    bool isAmbient;
    OH_Drawing_CanvasShadowFlags flags;
} DRAW_PARAM;

typedef struct {
    std::vector<OH_Drawing_Path*>& paths;
    std::vector<DrawRect>& pathsBounds;
    std::vector<OH_Drawing_Matrix*>& matrices;
    ShadowUtils::ShadowMode mode;
    OH_Drawing_Point3D lightPos;
    float kHeight;
    float kLightR;
    float kPad;
} DRAW_RECT_PARAM;

void draw_shadow(OH_Drawing_Canvas* canvas, OH_Drawing_Path* path, DRAW_PARAM param)
{
    float height = param.height;
    uint32_t color = param.color;
    OH_Drawing_Point3D lightPos = param.lightPos;
    float lightR = param.lightR;
    bool isAmbient = param.isAmbient;
    OH_Drawing_CanvasShadowFlags flags = param.flags;

    float ambientAlpha = isAmbient ? 0.5f : 0.f; // 0.5f ambient alpha
    float spotAlpha = isAmbient ? 0.f : 0.5f;    // 0.5f spot alpha
    uint8_t a = (color >> 24) & 0xFF;            // 24 颜色值 偏移量
    uint8_t r = (color >> 16) & 0xFF;            // 16 颜色值 偏移量
    uint8_t g = (color >> 8) & 0xFF;             // 8 颜色值 偏移量
    uint8_t b = color & 0xFF;

    uint32_t ambientColor = OH_Drawing_ColorSetArgb(a * ambientAlpha, r, g, b);
    uint32_t spotColor = OH_Drawing_ColorSetArgb(a * spotAlpha, r, g, b);
    OH_Drawing_Point3D planeParams = { 0, 0, height };

    OH_Drawing_CanvasDrawShadow(canvas, path, planeParams, lightPos, lightR, ambientColor, spotColor, flags);
}

void make_path_rect(std::vector<OH_Drawing_Path*>& paths, std::vector<DrawRect>& pathsBounds)
{
    float rSize = 50.f;
    // add rrect
    OH_Drawing_Path* path1 = OH_Drawing_PathCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, rSize, rSize);
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 10.f, 10.00002f); // 10.f, 10.00002f 圆角矩形rad
    OH_Drawing_PathAddRoundRect(path1, roundRect, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    paths.push_back(path1);
    pathsBounds.push_back({ 0, 0, rSize, rSize });

    // add rrect 这里应该是不规则的圆角矩形，四个圆角不同，oh暂不支持，先用普通圆角代替
    OH_Drawing_RoundRect* addRRect = OH_Drawing_RoundRectCreate(rect, 9.f, 13.f); // 9.f, 13.f 圆角矩形rad
    OH_Drawing_Path* path2 = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRoundRect(path2, addRRect, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    paths.push_back(path2);
    pathsBounds.push_back({ 0, 0, rSize, rSize });
    OH_Drawing_RoundRectDestroy(addRRect);

    // add rect
    OH_Drawing_Path* path3 = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRect(path3, 0, 0, rSize, rSize, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    paths.push_back(path3);
    pathsBounds.push_back({ 0, 0, rSize, rSize });

    // add circle
    OH_Drawing_Path* path4 = OH_Drawing_PathCreate();
    OH_Drawing_PathAddArc(path4, rect, 0, 360.f); // 360.f 旋转角度
    paths.push_back(path4);
    pathsBounds.push_back({ 0, 0, rSize, rSize });
    OH_Drawing_RectDestroy(rect);

    OH_Drawing_Path* path5 = OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(path5, 100, 50, 20, 100, 0, 0); // 100, 50, 20, 100, 0, 0 gm 要求的图形坐标
    paths.push_back(path5);
    pathsBounds.push_back({ 0, 0, 100.f, 100.f }); // 100.f, 100.f 矩形参数

    // add oval
    OH_Drawing_Path* path6 = OH_Drawing_PathCreate();
    rect = OH_Drawing_RectCreate(0, 0, 20.f, 60.f); // 20.f, 60.f 矩形参数
    OH_Drawing_PathAddArc(path6, rect, 0, 360.f);   // 360.f 旋转角度
    paths.push_back(path6);
    pathsBounds.push_back({ 0, 0, 20.f, 60.f }); // 20.f, 60.f 矩形参数
    OH_Drawing_RectDestroy(rect);
}

void make_path_star(std::vector<OH_Drawing_Path*>& paths, std::vector<DrawRect>& pathsBounds)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    std::vector<OH_Drawing_Path*> concavePaths;
    std::vector<DrawRect> concavePathsBounds;
    path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0.0, -33.3333);       // 0.0, -33.3333 gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, 9.62, -16.6667);      // 9.62, -16.6667 gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, 28.867f, -16.6667f);  // 28.867f, -16.6667f gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, 19.24f, 0.0f);        // 19.24f, 0.0f gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, 28.867f, 16.6667f);   // 28.867f, 16.6667f gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, 9.62f, 16.6667f);     // 9.62f, 16.6667f gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, 0.0f, 33.3333f);      // 0.0f, 33.3333f gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, -9.62f, 16.6667f);    // -9.62f, 16.6667f gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, -28.867f, 16.6667f);  // -28.867f, 16.6667f gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, -19.24f, 0.0f);       // -19.24f, 0.0f gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, -28.867f, -16.6667f); // -28.867f, -16.6667f gm 要求的图形坐标
    OH_Drawing_PathLineTo(path, -9.62f, -16.6667f);   // -9.62f, -16.6667f gm 要求的图形坐标
    OH_Drawing_PathClose(path);
    concavePaths.push_back(path);
    concavePathsBounds.push_back(
        { -28.867f, -33.3333, 28.867f, 33.3333f }); //  -28.867f, -33.3333, 28.867f, 33.3333f gm 要求的图形坐

    // dumbbell
    OH_Drawing_Path* path1 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 50.0, 0);                 // 50.0, 0 gm 要求的图形坐标
    OH_Drawing_PathCubicTo(path, 100, 25, 60, 50, 50, 0); // 100, 25, 60, 50, 50, 0 gm 要求的图形坐标
    OH_Drawing_PathCubicTo(path, 0, -25, 40, -50, 50, 0); // 0, -25, 40, -50, 50, 0 gm 要求的图形坐标
    concavePaths.push_back(path1);
    concavePathsBounds.push_back({ 0, -50, 100, 50 }); // 0, -50, 100, 50  gm 要求的图形坐标
}

void destory_path(std::vector<OH_Drawing_Path*>& paths, std::vector<OH_Drawing_Path*>& concavePaths,
    std::vector<OH_Drawing_Matrix*>& matrices)
{
    for (auto p : paths) {
        OH_Drawing_PathDestroy(p);
    }
    for (auto p : concavePaths) {
        OH_Drawing_PathDestroy(p);
    }
    for (auto m : matrices) {
        OH_Drawing_MatrixDestroy(m);
    }
}

void draw_rect(OH_Drawing_Canvas* canvas, DRAW_RECT_PARAM param)
{
    ShadowUtils::ShadowMode mode = param.mode;
    OH_Drawing_Point3D lightPos = param.lightPos;
    float kHeight = param.kHeight;
    float kLightR = param.kLightR;
    float kPad = param.kPad;
    float dy = 0;
    float x = 0;
    for (auto m : param.matrices) {
        for (OH_Drawing_CanvasShadowFlags flags : { SHADOW_FLAGS_NONE, SHADOW_FLAGS_TRANSPARENT_OCCLUDER }) {
            int pathCounter = 0;
            for (const auto path : param.paths) {
                // 计算各个path的bound，避免碰撞，并对其进行移动
                DrawRect postMBounds = param.pathsBounds[pathCounter];
                float boundWidth = postMBounds.Width();
                float boundHeight = postMBounds.Height();
                float w = boundWidth + kHeight;
                float dx = w + kPad;
                if (x + dx > K_W - 3 * kPad) { // 3倍间距
                    OH_Drawing_CanvasRestore(canvas);
                    OH_Drawing_CanvasTranslate(canvas, 0, dy);
                    OH_Drawing_CanvasSave(canvas);
                    x = 0;
                    dy = 0;
                }
                OH_Drawing_CanvasSave(canvas);
                OH_Drawing_CanvasConcatMatrix(canvas, m);
                if (SHADOW_FLAGS_TRANSPARENT_OCCLUDER == flags && 0 == pathCounter % 3) { // 3个图形换行
                    OH_Drawing_CanvasSave(canvas);
                    OH_Drawing_CanvasRotate(canvas, 180.f, 25.f, 25.f); // 180.f 旋转角度, 25.f x偏移 , 25.f y偏移
                }
                if (ShadowUtils::K_NO_OCCLUDERS == mode || ShadowUtils::K_NO_OCCLUDERS == mode) {
                    draw_shadow(canvas, path, { kHeight, 0xFFFF0000, lightPos, kLightR, true, flags });
                    draw_shadow(canvas, path, { kHeight, 0xFF0000FF, lightPos, kLightR, false, flags });
                } else if (ShadowUtils::K_GRAY_SCALE == mode) {
                    uint32_t ambientColor = OH_Drawing_ColorSetArgb(0.1 * 255, 0, 0, 0); // 0.1 * 255 ambientColor
                    uint32_t spotColor = OH_Drawing_ColorSetArgb(0.25 * 255, 0, 0, 0);   // 0.25 * 255  spotColor
                    OH_Drawing_CanvasDrawShadow(
                        canvas, path, { 0, 0, kHeight }, lightPos, kLightR, ambientColor, spotColor, flags);
                }
                OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
                OH_Drawing_BrushSetAntiAlias(brush, true);
                OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
                OH_Drawing_PenSetAntiAlias(pen, true);
                if (ShadowUtils::K_NO_OCCLUDERS == mode) {
                    if (flags & SHADOW_FLAGS_TRANSPARENT_OCCLUDER) {
                        OH_Drawing_PenSetColor(pen, 0xFF00FFFF); // SK_ColorCYAN
                    } else {
                        OH_Drawing_PenSetColor(pen, 0xFF00FF00); // SK_ColorGREEN
                    }
                    OH_Drawing_PenSetWidth(pen, 0);
                    OH_Drawing_CanvasAttachPen(canvas, pen);
                } else {
                    OH_Drawing_BrushSetColor(brush,
                        ShadowUtils::K_NO_OCCLUDERS == mode ? 0xFFCCCCCC
                                                            : 0xFFFFFFFF); // SK_ColorLTGRAY : SK_ColorWHITE
                    if (flags & SHADOW_FLAGS_TRANSPARENT_OCCLUDER)
                        OH_Drawing_BrushSetAlpha(brush, 0x80); // 0.5 alpha
                    OH_Drawing_CanvasAttachBrush(canvas, brush);
                }
                OH_Drawing_CanvasDrawPath(canvas, path);
                OH_Drawing_CanvasDetachPen(canvas);
                OH_Drawing_CanvasDetachBrush(canvas);
                OH_Drawing_PenDestroy(pen);
                OH_Drawing_BrushDestroy(brush);
                if (SHADOW_FLAGS_TRANSPARENT_OCCLUDER == flags && 0 == pathCounter % 3) { // 3个图形换行
                    OH_Drawing_CanvasRestore(canvas);
                }
                OH_Drawing_CanvasRestore(canvas);
                OH_Drawing_CanvasTranslate(canvas, dx, 0);
                x += dx;

                dy = std::max(dy, boundHeight + kPad + kHeight);
                ++pathCounter;
            }
        }
    }
}

void draw_star(OH_Drawing_Canvas* canvas, DRAW_RECT_PARAM param)
{
    ShadowUtils::ShadowMode mode = param.mode;
    OH_Drawing_Point3D lightPos = param.lightPos;
    float kHeight = param.kHeight;
    float kLightR = param.kLightR;
    float kPad = param.kPad;
    float x = kPad;
    float dy = 0;
    for (auto m : param.matrices) {
        int pathCounter = 0;
        for (const auto path : param.paths) {
            DrawRect postMBounds = param.pathsBounds[pathCounter];
            float boundWidth = postMBounds.Width();
            float boundHeight = postMBounds.Height();
            float w = boundWidth + kHeight;
            float dx = w + kPad;

            OH_Drawing_CanvasSave(canvas);
            OH_Drawing_CanvasConcatMatrix(canvas, m);
            if (ShadowUtils::K_NO_OCCLUDERS == mode || ShadowUtils::K_NO_OCCLUDERS == mode) {
                draw_shadow(canvas, path, { kHeight, 0xFFFF0000, lightPos, kLightR, true, SHADOW_FLAGS_NONE });
                draw_shadow(canvas, path, { kHeight, 0xFF0000FF, lightPos, kLightR, false, SHADOW_FLAGS_NONE });
            } else if (ShadowUtils::K_GRAY_SCALE == mode) {
                uint32_t ambientColor = OH_Drawing_ColorSetArgb(0.1 * 255, 0, 0, 0); // 0.1 * 255 alphaAmbient
                uint32_t spotColor = OH_Drawing_ColorSetArgb(0.25 * 255, 0, 0, 0);   // 0.25 * 255 alphaSpot
                OH_Drawing_CanvasDrawShadow(
                    canvas, path, { 0, 0, kHeight }, lightPos, kLightR, ambientColor, spotColor, SHADOW_FLAGS_NONE);
            }
            OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
            OH_Drawing_BrushSetAntiAlias(brush, true);
            OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
            OH_Drawing_PenSetAntiAlias(pen, true);
            if (ShadowUtils::K_NO_OCCLUDERS == mode) {
                OH_Drawing_PenSetColor(pen, 0xFF00FF00);
                OH_Drawing_PenSetWidth(pen, 0);
                OH_Drawing_CanvasAttachPen(canvas, pen);
            } else {
                OH_Drawing_BrushSetColor(brush,
                    ShadowUtils::K_NO_OCCLUDERS == mode ? 0xFFCCCCCC : 0xFFFFFFFF); // SK_ColorLTGRAY : SK_ColorWHITE
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
}

void draw_black_point(OH_Drawing_Canvas* canvas, OH_Drawing_Point3D lightPos, float kLightR)
{
    OH_Drawing_Matrix* invCanvasM = OH_Drawing_MatrixCreate();
    OH_Drawing_CanvasGetTotalMatrix(canvas, invCanvasM);
    OH_Drawing_Matrix* invert = OH_Drawing_MatrixCreate();

    bool ret1 = OH_Drawing_MatrixIsIdentity(invCanvasM);
    bool ret2 = OH_Drawing_MatrixInvert(invCanvasM, invert);
    if (ret1 || ret2) {
        if (ret1)
            OH_Drawing_MatrixSetMatrix(invCanvasM, 1, 0, 0, 0, 1, 0, 0, 0, 1); // 1, 0, 0, 0, 1, 0, 0, 0, 1 单位矩阵
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasConcatMatrix(canvas, invert);
        OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
        OH_Drawing_BrushSetAntiAlias(brush, true);
        OH_Drawing_BrushSetColor(brush, 0xFF000000);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_Point* center = OH_Drawing_PointCreate(lightPos.x, lightPos.y);
        float fDiv = 10.f;
        OH_Drawing_CanvasDrawCircle(canvas, center, kLightR / fDiv);
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_BrushDestroy(brush);
        OH_Drawing_CanvasRestore(canvas);
    }
}
ShadowUtils::ShadowUtils(ShadowMode m) : sMode(m)
{
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "shadowutils";
}

void ShadowUtils::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    draw_paths(canvas, sMode);
}

void ShadowUtils::draw_paths(OH_Drawing_Canvas* canvas, ShadowMode mode)
{
    // rect
    std::vector<OH_Drawing_Path*> paths;
    std::vector<DrawRect> pathsBounds;
    make_path_rect(paths, pathsBounds);

    // star
    std::vector<OH_Drawing_Path*> concavePaths;
    std::vector<DrawRect> concavePathsBounds;
    make_path_star(concavePaths, concavePathsBounds);

    static constexpr float kPad = 15.f;
    static constexpr float kLightR = 100.f;
    static constexpr float kHeight = 50.f;

    // transform light position relative to canvas to handle tiling
    OH_Drawing_Point2D lightXY = { 250.f, 400.f };                 // 250.f, 400.f 坐标参数
    OH_Drawing_Point3D lightPos = { lightXY.x, lightXY.y, 500.f }; // 500.f 坐标参数

    OH_Drawing_CanvasTranslate(canvas, 3 * kPad, 3 * kPad); // 3 3 个xy平移单位
    float x = 0;
    float dy = 0;
    std::vector<OH_Drawing_Matrix*> matrices;
    OH_Drawing_Matrix* mI = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixReset(mI);
    matrices.push_back(mI);

    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateRotation(33.0, 25.0, 25.0); // 33.0, 25.0, 25.0 gm 要求的矩阵参数
    OH_Drawing_MatrixPostScale(matrix, 1.2, 0.8, 25.0, 25.0); // 1.2, 0.8, 25.0, 25.0 gm 要求的矩阵参数
    matrices.push_back(matrix);

    float n = 80;
    DRAW_RECT_PARAM param = { paths, pathsBounds, matrices, mode, lightPos, kHeight, kLightR, kPad };
    draw_rect(canvas, param);

    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasTranslate(canvas, 0, 3 * n); // 3个平移单位
    OH_Drawing_CanvasSave(canvas);

    DRAW_RECT_PARAM concaveParam = { concavePaths, concavePathsBounds, matrices, mode, lightPos, kHeight, kLightR,
        kPad };
    draw_star(canvas, concaveParam);

    draw_black_point(canvas, lightPos, kLightR);
    destory_path(paths, concavePaths, matrices);
}