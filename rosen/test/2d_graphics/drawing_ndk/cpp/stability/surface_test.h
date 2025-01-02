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

#ifndef STABILITY_SURFACE_TEST_H
#define STABILITY_SURFACE_TEST_H

#include <map>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

typedef enum {
    // operation function
    OPERATION_FUNCTION_SURFACE_GET_CANVAS = 0,
    OPERATION_FUNCTION_SURFACE_MAX
} OperationFunctionSurface;

// SurfaceCreateFromGpuContext-SurfaceDestroy循环
class StabilitySurfaceCreateFromGpuContext : public TestBase {
public:
    StabilitySurfaceCreateFromGpuContext()
    {
        fileName_ = "StabilitySurfaceCreateFromGpuContext";
    }
    ~StabilitySurfaceCreateFromGpuContext() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// SurfaceCreateFromGpuContext-相关配置接口全调用-SurfaceDestroy循环,无配置接口，改为操作接口全调用
class StabilitySurfaceFuncAll : public TestBase {
public:
    StabilitySurfaceFuncAll()
    {
        fileName_ = "StabilitySurfaceFuncAll";
    }
    ~StabilitySurfaceFuncAll() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;

private:
    TestRend rand_;
    void SurfaceGetCanvasTest(OH_Drawing_Surface* surface);
    using HandlerFunc = std::function<void(OH_Drawing_Surface*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_SURFACE_GET_CANVAS,
            [this](OH_Drawing_Surface* surface) { return SurfaceGetCanvasTest(surface); } },
    };
};

// GpuContextCreateFromGL-GpuContextDestroy循环
class StabilityGpuContextCreate : public TestBase {
public:
    StabilityGpuContextCreate()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityGpuContextCreate";
    }
    ~StabilityGpuContextCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

#endif // STABILITY_SURFACE_TEST_H
