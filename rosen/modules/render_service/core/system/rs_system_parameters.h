/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_CORE_SYSTEM_RS_SYSTEM_PARAMETERS_H
#define ROSEN_RENDER_SERVICE_CORE_SYSTEM_RS_SYSTEM_PARAMETERS_H

#include <atomic>
#include <string>
#include <vector>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum class QuickSkipPrepareType {
    DISABLED = 0,           // 0, disable quick skip preparation
    STATIC_PROCESS,         // 1, skip no-command process's preparation
    STATIC_APP_INSTANCE,    // 2, in case of dirty process, skip static app instance's preparation
    STATIC_CACHE,           // 3, in case of dirty instance, simplify static (drawing)cache's preparation
    STATIC_CACHE_SURFACE,   // 4, in case of dirty instance, simplify surface's static cache's preparation
    CONTENT_DIRTY_CACHE_SURFACE,  // 5, simplify dirty cache surface's subtree preparation
};

enum class RsParallelType {
    RS_PARALLEL_TYPE_ASYNC = 0,         // 0, The main thread and render thread execute asynchronous, default type
    RS_PARALLEL_TYPE_SYNC = 1,          // 1, The main thread and render thread execute synchronously
    RS_PARALLEL_TYPE_SINGLE_THREAD = 2, // 2, Render in main thread, execute synchronously
};

enum class RsSurfaceCaptureType {
    // 0, Run SurfaceCapture in render thread, default type
    RS_SURFACE_CAPTURE_TYPE_RENDER_THREAD = 0,
    // 1, Run SurfaceCapture in main thread
    RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD = 1,
    // 2, Run SurfaceCapture in render thread using RSSurfaceCaptureVisitor
    RS_SURFACE_CAPTURE_TYPE_RENDER_THREAD_VISITOR = 2,
};

class RSB_EXPORT RSSystemParameters final {
public:
    ~RSSystemParameters() = default;
    static int GetDumpRSTreeCount();
    static void SetDumpRSTreeCount(int count);
    static bool GetCalcCostEnabled();
    static bool GetDrawingCacheEnabled();
    static bool GetDrawingCacheEnabledDfx();
    static bool GetShowRefreshRateEnabled();
    static QuickSkipPrepareType GetQuickSkipPrepareType();
    static RsParallelType GetRsParallelType();
    static RsSurfaceCaptureType GetRsSurfaceCaptureType();
    static bool GetVSyncControlEnabled();
    static bool GetSystemAnimatedScenesEnabled();
    static bool GetFilterCacheOcculusionEnabled();
    static bool GetSkipCanvasNodeOutofScreenEnabled();
    static bool GetDrawingEffectRegionEnabledDfx();
    static bool GetRenderStop();
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CORE_SYSTEM_RS_SYSTEM_PARAMETERS_H
