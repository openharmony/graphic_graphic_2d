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

#ifndef PERFORMANCE_CANVAS_DRAW_IMAGE_RECT_WITH_SRC_H
#define PERFORMANCE_CANVAS_DRAW_IMAGE_RECT_WITH_SRC_H
#include "performance/performance.h"
#include "test_base.h"
#include "test_common.h"

#include "common/drawing_type.h"
// 超大源绘制性能-OH_Drawing_CanvasDrawImageRectWithSrc
class PerformanceCanvasDrawImageRectWithSrc : public TestBase {
public:
    explicit PerformanceCanvasDrawImageRectWithSrc(int attachType)
    {
        fileName_ = "PerformanceCanvasDrawImageRectWithSrc" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    };
    ~PerformanceCanvasDrawImageRectWithSrc() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_ = nullptr;
};

#endif // PERFORMANCE_CANVAS_DRAW_IMAGE_RECT_WITH_SRC_H
