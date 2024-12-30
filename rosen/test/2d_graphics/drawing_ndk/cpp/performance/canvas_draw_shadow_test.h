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

#ifndef PERFORMANCE_CANVAS_DRAW_SHADOW_TEST_H
#define PERFORMANCE_CANVAS_DRAW_SHADOW_TEST_H
#include "performance/performance.h"
#include "test_base.h"
#include "test_common.h"

#include "common/drawing_type.h"

// 超大折线段光影绘制性能-OH_Drawing_CanvasDrawShadow--attach pen，attach brush
// 超大折线段光影绘制性能-OH_Drawing_CanvasDrawShadow--attach pen
// 超大折线段光影绘制性能-OH_Drawing_CanvasDrawShadow--attach brush
class PerformanceCanvasDrawShadowLine : public TestBase {
public:
    explicit PerformanceCanvasDrawShadowLine(int attachType)
    {
        fileName_ = "PerformanceCanvasDrawShadowLine" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceCanvasDrawShadowLine() override
    {
        delete performance_;
    };

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_ = nullptr;
};

// 超大曲线段光影绘制性能-OH_Drawing_CanvasDrawShadow--attach pen，attach brush
// 超大曲线段光影绘制性能-OH_Drawing_CanvasDrawShadow--attach pen
// 超大曲线段光影绘制性能-OH_Drawing_CanvasDrawShadow--attach brush
class PerformanceCanvasDrawShadowCurve : public TestBase {
public:
    explicit PerformanceCanvasDrawShadowCurve(int attachType)
    {
        fileName_ = "PerformanceCanvasDrawShadowCurve" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceCanvasDrawShadowCurve() override
    {
        delete performance_;
    };

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_ = nullptr;
};

#endif // PERFORMANCE_CANVAS_DRAW_SHADOW_TEST_H
