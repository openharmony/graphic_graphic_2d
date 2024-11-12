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

#ifndef SIMPLE_PERFORMANCE_CANVAS_TEST_H
#define SIMPLE_PERFORMANCE_CANVAS_TEST_H
#include <bits/alltypes.h>
#include <multimedia/player_framework/native_avscreen_capture_base.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"
class SimplePerformanceCanvasDrawRect : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawRect(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawRect() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawImageRect : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawImageRect(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawImageRect() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasRotate : public TestBase {
public:
    explicit SimplePerformanceCanvasRotate(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasRotate() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasTranslate : public TestBase {
public:
    explicit SimplePerformanceCanvasTranslate(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasTranslate() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasScale : public TestBase {
public:
    explicit SimplePerformanceCanvasScale(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasScale() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasConcatMatrix : public TestBase {
public:
    explicit SimplePerformanceCanvasConcatMatrix(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasConcatMatrix() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasClear : public TestBase {
public:
    explicit SimplePerformanceCanvasClear(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasClear() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasSetMatrix : public TestBase {
public:
    explicit SimplePerformanceCanvasSetMatrix(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasSetMatrix() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasResetMatrix : public TestBase {
public:
    explicit SimplePerformanceCanvasResetMatrix(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasResetMatrix() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawImageRectWithSrc : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawImageRectWithSrc(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawImageRectWithSrc() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasReadPixels : public TestBase {
public:
    explicit SimplePerformanceCanvasReadPixels(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasReadPixels() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasReadPixelsToBitmap : public TestBase {
public:
    explicit SimplePerformanceCanvasReadPixelsToBitmap(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasReadPixelsToBitmap() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawShadow : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawShadow(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawShadow() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasSkew : public TestBase {
public:
    explicit SimplePerformanceCanvasSkew(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasSkew() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawVertices : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawVertices(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawVertices() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawPath : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawPath(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawPath() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasRestoreToCount : public TestBase {
public:
    explicit SimplePerformanceCanvasRestoreToCount(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasRestoreToCount() override {};

protected:
    uint32_t fSave_Count = 0; // 0 保存的数量
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawPoints : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawPoints(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawPoints() override {};

protected:
    const int n = 10; // 10 可以同时存储的点的数量
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawLine : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawLine(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawLine() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawLayer : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawLayer(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawLayer() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawBitmap : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawBitmap(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawBitmap() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawBackground : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawBackground(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawBackground() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawRegion : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawRegion(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawRegion() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawPixelMapRect : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawPixelMapRect(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawPixelMapRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasSave : public TestBase {
public:
    explicit SimplePerformanceCanvasSave(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasSave() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawBitmapRect : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawBitmapRect(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawBitmapRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawCircle : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawCircle(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawCircle() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawOval : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawOval(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawOval() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawArc : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawArc(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawArc() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawRoundRect : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawRoundRect(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawRoundRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawTextBlob : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawTextBlob(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawTextBlob() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasClipRect : public TestBase {
public:
    explicit SimplePerformanceCanvasClipRect(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasClipRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasClipRoundRect : public TestBase {
public:
    explicit SimplePerformanceCanvasClipRoundRect(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasClipRoundRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasClipPath : public TestBase {
public:
    explicit SimplePerformanceCanvasClipPath(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasClipPath() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasGetTotalMatrix : public TestBase {
public:
    explicit SimplePerformanceCanvasGetTotalMatrix(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasGetTotalMatrix() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasRestore : public TestBase {
public:
    explicit SimplePerformanceCanvasRestore(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasRestore() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasGetLocalClipBounds : public TestBase {
public:
    explicit SimplePerformanceCanvasGetLocalClipBounds(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasGetLocalClipBounds() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasGetSaveCount : public TestBase {
public:
    explicit SimplePerformanceCanvasGetSaveCount(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasGetSaveCount() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasGetWidth : public TestBase {
public:
    explicit SimplePerformanceCanvasGetWidth(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasGetWidth() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasGetHeight : public TestBase {
public:
    explicit SimplePerformanceCanvasGetHeight(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasGetHeight() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasCreate : public TestBase {
public:
    explicit SimplePerformanceCanvasCreate(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasCreate() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDestroy : public TestBase {
public:
    explicit SimplePerformanceCanvasDestroy(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDestroy() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasAttachPen : public TestBase {
public:
    explicit SimplePerformanceCanvasAttachPen(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasAttachPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDetachPen : public TestBase {
public:
    explicit SimplePerformanceCanvasDetachPen(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDetachPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasAttachBrush : public TestBase {
public:
    explicit SimplePerformanceCanvasAttachBrush(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasAttachBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDetachBrush : public TestBase {
public:
    explicit SimplePerformanceCanvasDetachBrush(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDetachBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawPoint : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawPoint(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawPoint() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawColor : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawColor(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawColor() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasDrawSingleCharacter : public TestBase {
public:
    explicit SimplePerformanceCanvasDrawSingleCharacter(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasDrawSingleCharacter() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasClipRegion : public TestBase {
public:
    explicit SimplePerformanceCanvasClipRegion(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasClipRegion() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasIsClipEmpty : public TestBase {
public:
    explicit SimplePerformanceCanvasIsClipEmpty(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasIsClipEmpty() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SimplePerformanceCanvasGetImageInfo : public TestBase {
public:
    explicit SimplePerformanceCanvasGetImageInfo(int type) : TestBase(type) {};
    ~SimplePerformanceCanvasGetImageInfo() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif // SIMPLE_PERFORMANCE_CANVAS_TEST_H
