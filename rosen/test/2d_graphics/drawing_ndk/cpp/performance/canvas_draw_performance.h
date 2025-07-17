/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef TESTCASE_CANVAS_DRAWPOINTS_H
#define TESTCASE_CANVAS_DRAWPOINTS_H

#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>

#include "performance/performance.h"
#include "test_base.h"
#include "test_common.h"

#include "common/drawing_type.h"

// 超多点绘制性能-OH_Drawing_CanvasDrawPoints attach pen and brush
// 超多点绘制性能-OH_Drawing_CanvasDrawPoints attach pen
// 超多点绘制性能-OH_Drawing_CanvasDrawPoints attach brush
class PerformanceDrawPoints : public TestBase {
public:
    explicit PerformanceDrawPoints(int attachType)
    {
        fileName_ = "PerformanceDrawPoints" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceDrawPoints() override
    {
        delete performance_;
    };

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    OH_Drawing_Brush* brush_ = nullptr;
    SpecialPerformance* performance_;
};

// 超大折线段绘制性能-OH_Drawing_Path attach pen and brush
// 超大折线段绘制性能-OH_Drawing_Path attach pen
// 超大折线段绘制性能-OH_Drawing_Path attach brush
class PerformanceDrawBrokenLine : public TestBase {
public:
    explicit PerformanceDrawBrokenLine(int attachType)
    {
        fileName_ = "PerformanceDrawBrokenLine" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceDrawBrokenLine() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_;
};

// 超大曲线段绘制性能-OH_Drawing_Path attach pen and brush
// 超大曲线段绘制性能-OH_Drawing_Path attach pen
// 超大曲线段绘制性能-OH_Drawing_Path attach brush
class PerformanceDrawCurveLine : public TestBase {
public:
    explicit PerformanceDrawCurveLine(int attachType)
    {
        fileName_ = "PerformanceDrawCurveLine" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceDrawCurveLine() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_;
};

// 超大矩形组绘制性能-OH_Drawing_CanvasDrawRegion attach pen and brush
// 超大矩形组绘制性能-OH_Drawing_CanvasDrawRegion attach pen
// 超大矩形组绘制性能-OH_Drawing_CanvasDrawRegion attach brush
class PerformanceDrawRegion : public TestBase {
public:
    explicit PerformanceDrawRegion(int attachType)
    {
        fileName_ = "PerformanceDrawRegion" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceDrawRegion() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_;
};

// 超大位图绘制性能-OH_Drawing_CanvasDrawBitmap attach pen and brush
// 超大位图绘制性能-OH_Drawing_CanvasDrawBitmap attach pen
// 超大位图绘制性能-OH_Drawing_CanvasDrawBitmap attach brush
class PerformanceDrawBitMap : public TestBase {
public:
    explicit PerformanceDrawBitMap(int attachType)
    {
        fileName_ = "PerformanceDrawBitMap" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceDrawBitMap() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_;
};

// 超大位图绘制性能-OH_Drawing_CanvasDrawBitmapRect attach pen and brush
// 超大位图绘制性能-OH_Drawing_CanvasDrawBitmapRect attach pen
// 超大位图绘制性能-OH_Drawing_CanvasDrawBitmapRect attach brush
class PerformanceDrawBitMapRect : public TestBase {
public:
    explicit PerformanceDrawBitMapRect(int attachType)
    {
        fileName_ = "PerformanceDrawBitMapRect" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceDrawBitMapRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_;
};

// 超大图片绘制性能 attach pen and brush
// 超大图片绘制性能 attach pen
// 超大图片绘制性能 attach brush
class PerformanceDrawImage : public TestBase {
public:
    explicit PerformanceDrawImage(int attachType)
    {
        fileName_ = "PerformanceDrawImage" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceDrawImage() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_;
};

// 超大源绘制性能-OH_Drawing_CanvasDrawPixelMapRect attach pen and brush
// 超大源绘制性能-OH_Drawing_CanvasDrawPixelMapRect attach pen
// 超大源绘制性能-OH_Drawing_CanvasDrawPixelMapRect attach brush
class PerformanceDrawPixelMap : public TestBase {
public:
    explicit PerformanceDrawPixelMap(int attachType)
    {
        fileName_ = "PerformanceDrawPixelMap" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceDrawPixelMap() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_;
};

class PerformanceCanvasQuickRejectPath : public TestBase {
public:
    PerformanceCanvasQuickRejectPath()
    {
        fileName_ = "PerformanceCanvasQuickRejectPath";
    };
    ~PerformanceCanvasQuickRejectPath() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCanvasQuickRejectRect : public TestBase {
public:
    PerformanceCanvasQuickRejectRect()
    {
        fileName_ = "PerformanceCanvasQuickRejectRect";
    };
    ~PerformanceCanvasQuickRejectRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCanvasDrawArcWithCenter : public TestBase {
public:
    PerformanceCanvasDrawArcWithCenter()
    {
        fileName_ = "PerformanceCanvasDrawArcWithCenter";
    };
    ~PerformanceCanvasDrawArcWithCenter() override{};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCanvasDrawNestedRoundRect : public TestBase {
public:
    PerformanceCanvasDrawNestedRoundRect()
    {
        fileName_ = "PerformanceCanvasDrawNestedRoundRect";
    };
    ~PerformanceCanvasDrawNestedRoundRect() override{};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCanvasDrawImageNine : public TestBase {
public:
    PerformanceCanvasDrawImageNine()
    {
        fileName_ = "PerformanceCanvasDrawImageNine";
    };
    ~PerformanceCanvasDrawImageNine() override{};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif