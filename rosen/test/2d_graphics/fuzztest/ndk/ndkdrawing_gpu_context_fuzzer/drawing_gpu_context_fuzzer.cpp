/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drawing_gpu_context_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_gpu_context_manager.h"
#include "drawing_surface.h"
#include "drawing_gpu_context.h"
#include "pipeline/rs_recording_canvas.h"
#include "image/gpu_context.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_display_node.h"
#include "native_window.h"
#include "drawing_canvas.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int MAX_ARRAY_SIZE = 10;
} // namespace
namespace Drawing {
void DrawGpuContextFuzzerTest001(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;
    OH_Drawing_GpuContextOptions ops1;
    ops1.allowPathMaskCaching = GetObject<bool>();
    OH_Drawing_GpuContext* gpuContext1 = OH_Drawing_GpuContextCreateFromGL(ops1);

    const int32_t width = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    const int32_t height = GetObject<int32_t>() % MAX_ARRAY_SIZE;

    OH_Drawing_Image_Info imageInfo = {width, height,
        GetObject<OH_Drawing_ColorFormat>(), GetObject<OH_Drawing_AlphaFormat>()};
    OH_Drawing_Surface* surface =
        OH_Drawing_SurfaceCreateFromGpuContext(gpuContext1, GetObject<bool>(), imageInfo);

    OH_Drawing_GpuContextDestroy(gpuContext1);
    OH_Drawing_SurfaceDestroy(surface);
    OH_Drawing_GpuContext* gpuContext2 =  OH_Drawing_GpuContextCreate();
    OH_Drawing_GpuContextDestroy(gpuContext2);
}

void DrawGpuContextFuzzerTest002(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;
    OH_Drawing_GpuContextOptions ops1 {GetObject<bool>()};
    OH_Drawing_GpuContext* gpuContext1 =  OH_Drawing_GpuContextCreateFromGL(ops1);

    int32_t width = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    OH_Drawing_Image_Info imageInfo = {width, height,
        GetObject<OH_Drawing_ColorFormat>(), GetObject<OH_Drawing_AlphaFormat>()};
    OH_Drawing_Surface* surface =
        OH_Drawing_SurfaceCreateOnScreen(gpuContext1, imageInfo, nullptr);
    OH_Drawing_GpuContextDestroy(gpuContext1);
    OH_Drawing_SurfaceDestroy(surface);

    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode != nullptr) {
        sptr<OHOS::Surface> surf = surfaceNode->GetSurface();
        OHNativeWindow *window = CreateNativeWindowFromSurface(&surf);
        OH_Drawing_GpuContextOptions ops2 {GetObject<bool>()};
        OH_Drawing_GpuContext* gpuContext2 =  OH_Drawing_GpuContextCreateFromGL(ops2);

        int32_t width1 = GetObject<int32_t>() % MAX_ARRAY_SIZE;
        int32_t height1 = GetObject<int32_t>() % MAX_ARRAY_SIZE;
        OH_Drawing_Image_Info imageInfo2 = {width1, height1,
            GetObject<OH_Drawing_ColorFormat>(), GetObject<OH_Drawing_AlphaFormat>()};

        OH_Drawing_Surface* surface2 =
            OH_Drawing_SurfaceCreateOnScreen(gpuContext2, imageInfo2, window);
        OH_Drawing_Canvas* canvas = OH_Drawing_SurfaceGetCanvas(surface2);
        OH_Drawing_GpuContextDestroy(gpuContext2);

        OH_Drawing_SurfaceDestroy(surface2);
        if (window != nullptr) {
            delete window;
        }
        OH_Drawing_CanvasDestroy(canvas);
    }

    OH_Drawing_SurfaceFlush(nullptr);
    OH_Drawing_SurfaceDestroy(nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::DrawGpuContextFuzzerTest001(data, size);
    OHOS::Rosen::Drawing::DrawGpuContextFuzzerTest002(data, size);
    return 0;
}
