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

#ifndef CANVAS_TEST_H
#define CANVAS_TEST_H
#include <bits/alltypes.h>
#include <multimedia/player_framework/native_avscreen_capture_base.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_rect.h>

#include "../test_base.h"
#include "test_common.h"
class CanvasDrawRect : public TestBase {
public:
    explicit CanvasDrawRect(int type) : TestBase(type) {};
    ~CanvasDrawRect() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};


class CanvasDrawImageRect : public TestBase {
public:
    explicit CanvasDrawImageRect(int type) : TestBase(type) {};
    ~CanvasDrawImageRect() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

/*
 * 测试void OH_Drawing_CanvasRotate
 */
class CanvasRotate : public TestBase {
public:
    explicit CanvasRotate(int type) : TestBase(type) {};
    ~CanvasRotate() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

// 测试void OH_Drawing_CanvasTranslate
class CanvasTranslate : public TestBase {
public:
    explicit CanvasTranslate(int type) : TestBase(type) {};
    ~CanvasTranslate() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

// void OH_Drawing_CanvasSkew
class CanvasScale : public TestBase {
public:
    explicit CanvasScale(int type) : TestBase(type) {};
    ~CanvasScale() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasConcatMatrix : public TestBase {
public:
    explicit CanvasConcatMatrix(int type) : TestBase(type) {};
    ~CanvasConcatMatrix() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

// 清理画布
class CanvasClear : public TestBase {
public:
    explicit CanvasClear(int type) : TestBase(type) {};
    ~CanvasClear() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

// 设置画布的矩阵状态。
class CanvasSetMatrix : public TestBase {
public:
    explicit CanvasSetMatrix(int type) : TestBase(type) {};
    ~CanvasSetMatrix() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasResetMatrix : public TestBase {
public:
    explicit CanvasResetMatrix(int type) : TestBase(type) {};
    ~CanvasResetMatrix() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawImageRectWithSrc : public TestBase {
public:
    explicit CanvasDrawImageRectWithSrc(int type) : TestBase(type) {};
    ~CanvasDrawImageRectWithSrc() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

// 3
class CanvasReadPixels : public TestBase {
public:
    explicit CanvasReadPixels(int type) : TestBase(type) {};
    ~CanvasReadPixels() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
// 4
class CanvasReadPixelsToBitmap : public TestBase {
public:
    explicit CanvasReadPixelsToBitmap(int type) : TestBase(type) {};
    ~CanvasReadPixelsToBitmap() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawShadow : public TestBase {
public:
    explicit CanvasDrawShadow(int type) : TestBase(type) {};
    ~CanvasDrawShadow() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasSkew : public TestBase {
public:
    explicit CanvasSkew(int type) : TestBase(type) {};
    ~CanvasSkew() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawVertices : public TestBase {
public:
    explicit CanvasDrawVertices(int type) : TestBase(type) {};
    ~CanvasDrawVertices() = default;

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawPath : public TestBase {
public:
    explicit CanvasDrawPath(int type) : TestBase(type) {};
    ~CanvasDrawPath() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasRestoreToCount : public TestBase {
public:
    explicit CanvasRestoreToCount(int type) : TestBase(type) {};
    ~CanvasRestoreToCount() = default;

protected:
    uint32_t fSave_Count = 0; // 0 保存的数量
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawPoints : public TestBase {
public:
    explicit CanvasDrawPoints(int type) : TestBase(type) {};
    ~CanvasDrawPoints() = default;

protected:
    const int n = 30; // 30 可以同时存储的点的数量
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawLine : public TestBase {
public:
    explicit CanvasDrawLine(int type) : TestBase(type) {};
    ~CanvasDrawLine() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawLayer : public TestBase {
public:
    explicit CanvasDrawLayer(int type) : TestBase(type) {};
    ~CanvasDrawLayer() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawBitmap : public TestBase {
public:
    explicit CanvasDrawBitmap(int type) : TestBase(type) {};
    ~CanvasDrawBitmap() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawBackground : public TestBase {
public:
    explicit CanvasDrawBackground(int type) : TestBase(type) {};
    ~CanvasDrawBackground() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawRegion : public TestBase {
public:
    explicit CanvasDrawRegion(int type) : TestBase(type) {};
    ~CanvasDrawRegion() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawPixelMapRect : public TestBase {
public:
    explicit CanvasDrawPixelMapRect(int type) : TestBase(type) {};
    ~CanvasDrawPixelMapRect() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasSave : public TestBase {
public:
    explicit CanvasSave(int type) : TestBase(type) {};
    ~CanvasSave() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawBitmapRect : public TestBase {
public:
    explicit CanvasDrawBitmapRect(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawBitmapRect";
    };
    ~CanvasDrawBitmapRect() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawCircle : public TestBase {
public:
    explicit CanvasDrawCircle(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawCircle";
    };
    ~CanvasDrawCircle() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawOval : public TestBase {
public:
    explicit CanvasDrawOval(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawOval";
    };
    ~CanvasDrawOval() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawArc : public TestBase {
public:
    explicit CanvasDrawArc(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawArc";
    };
    ~CanvasDrawArc() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawRoundRect : public TestBase {
public:
    explicit CanvasDrawRoundRect(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawRoundRect";
    };
    ~CanvasDrawRoundRect() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawTextBlob : public TestBase {
public:
    explicit CanvasDrawTextBlob(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlob";
    };
    ~CanvasDrawTextBlob() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasClipRect : public TestBase {
public:
    explicit CanvasClipRect(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipRect";
    };
    ~CanvasClipRect() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasClipRoundRect : public TestBase {
public:
    explicit CanvasClipRoundRect(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipRoundRect";
    };
    ~CanvasClipRoundRect() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasClipPath : public TestBase {
public:
    explicit CanvasClipPath(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPath";
    };
    ~CanvasClipPath() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // CANVAS_TEST_H
