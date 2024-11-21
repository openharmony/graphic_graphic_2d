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
#ifndef PERFORMANCE_CANVAS_CLIP_PATH_TEST_H
#define PERFORMANCE_CANVAS_CLIP_PATH_TEST_H
#include "performance/performance.h"
#include "test_base.h"

#include "common/drawing_type.h"

// 超大折线段裁剪性能-OH_Drawing_CanvasClipPath--attach pen，attach brush
// 超大折线段裁剪性能-OH_Drawing_CanvasClipPath--attach pen
// 超大折线段裁剪性能-OH_Drawing_CanvasClipPath--attach brush
class PerformanceCanvasClipPathLine : public TestBase {
public:
    explicit PerformanceCanvasClipPathLine(int attachType)
    {
        fileName_ = "PerformanceCanvasClipPathLine" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceCanvasClipPathLine() override
    {
        delete performance_;
    };

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_ = nullptr;
};

// 超大曲线段裁剪性能-OH_Drawing_CanvasClipPath--attach pen，attach brush
// 超大曲线段裁剪性能-OH_Drawing_CanvasClipPath--attach pen
// 超大曲线段裁剪性能-OH_Drawing_CanvasClipPath--attach brush
class PerformanceCanvasClipPathCurve : public TestBase {
public:
    explicit PerformanceCanvasClipPathCurve(int attachType)
    {
        fileName_ = "PerformanceCanvasClipPathCurve" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceCanvasClipPathCurve() override
    {
        delete performance_;
    };

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_ = nullptr;
};

#endif // PERFORMANCE_CANVAS_CLIP_PATH_TEST_H
