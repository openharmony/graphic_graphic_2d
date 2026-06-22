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

#ifndef RENDER_SERVICE_CORE_FEATURE_MULTI_SCREEN_RS_MULTI_SCREEN_UTIL_H
#define RENDER_SERVICE_CORE_FEATURE_MULTI_SCREEN_RS_MULTI_SCREEN_UTIL_H

#include <bitset>
#include <memory>
#include <string>

#include "common/rs_common_def.h"
#include "params/rs_logical_display_render_params.h"
#include "params/rs_render_thread_params.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/rs_processor.h"

namespace OHOS::Rosen {

class RSUniRenderVirtualProcessor;

namespace DrawableV2 {

class RSScreenRenderNodeDrawable;
class RSLogicalDisplayRenderNodeDrawable;

enum class RenderStrategy : int {
    INVALID = -1,  // Initial state
    DRAW_AS_MAIN_SCREEN = 0,
    DRAW_VIRTUAL_EXTEND = 1,
    DRAW_VIRTUAL_MIRROR_FROM_CACHE = 2,
    DRAW_VIRTUAL_MIRROR_REBUILD = 3,
    DRAW_PHYSICAL_MIRROR_FROM_CACHE = 4,
    DRAW_PHYSICAL_MIRROR_REBUILD = 5
};

enum RebuildReason : size_t {
    PROCESS_SPECIAL_LAYER = 0,  // Need to process special layer
    MIRROR_SCREEN_HDR_PRESENT,  // Mirror screen has HDR content
    MIRROR_SOURCE_HDR_PRESENT,  // Mirror source screen has HDR content
    CACHE_IMAGE_NULL,           // CacheImage is not available
    VIRTUAL_SCREEN_MUTE,        // Virtual screen is muted
    SCREEN_OFF,                 // Screen is off
    COLOR_FILTER_MODE,          // Color filter mode is enabled

    // Add new reasons above this line
    MAX_VALUE
};

struct MultiScreenParams {
    std::shared_ptr<RSScreenRenderNodeDrawable> screenDrawable;
    RSScreenRenderParams* screenParams = nullptr;
    std::shared_ptr<RSLogicalDisplayRenderNodeDrawable> mirrorSourceDisplayDrawable;
    RSLogicalDisplayRenderParams* mirrorSourceDisplayParams = nullptr;
    std::shared_ptr<RSScreenRenderNodeDrawable> mirrorSourceScreenDrawable;
    RSScreenRenderParams* mirrorSourceScreenParams = nullptr;
};

class RSMultiScreenUtil {
public:
    static void HandleMirrorDisplay(
        RSLogicalDisplayRenderNodeDrawable& drawable,
        RSLogicalDisplayRenderParams& params,
        const std::shared_ptr<RSProcessor>& processor);

    static void HandleMirrorScreen(
        RSScreenRenderNodeDrawable& drawable,
        RSScreenRenderParams& mirrorSourceParams,
        RSScreenRenderParams& params,
        const std::shared_ptr<RSProcessor>& processor);

    static void HandleVirtualExtendDisplay(
        RSLogicalDisplayRenderNodeDrawable& drawable,
        RSLogicalDisplayRenderParams& params,
        const std::shared_ptr<RSProcessor>& processor);

    static void HandleVirtualExtendScreen(
        RSScreenRenderNodeDrawable& drawable,
        RSScreenRenderParams& params,
        const std::shared_ptr<RSProcessor>& processor);
    
    static void DumpRenderStrategy(
        RSLogicalDisplayRenderNodeDrawable& drawable,
        ScreenId screenId,
        RenderStrategy path,
        const std::string& reason);

private:
    static void DrawPhysicalMirrorDisplay(
        RSLogicalDisplayRenderNodeDrawable& drawable,
        RSLogicalDisplayRenderParams& params,
        const std::shared_ptr<RSProcessor>& processor);

    static void DrawPhysicalMirrorFromCache(
        RSLogicalDisplayRenderNodeDrawable& drawable,
        RSLogicalDisplayRenderParams& params);

    static void DrawPhysicalMirrorRebuild(
        RSLogicalDisplayRenderNodeDrawable& drawable,
        RSLogicalDisplayRenderParams& params,
        const std::shared_ptr<RSProcessor>& processor);

    static void DrawVirtualMirrorDisplay(
        RSLogicalDisplayRenderNodeDrawable& drawable,
        RSLogicalDisplayRenderParams& params,
        const std::shared_ptr<RSProcessor>& processor);

    static void DrawVirtualMirrorFromCache(
        RSLogicalDisplayRenderNodeDrawable& drawable,
        RSLogicalDisplayRenderParams& params,
        const std::shared_ptr<RSUniRenderVirtualProcessor>& processor,
        RSRenderThreadParams& uniParam);

    static void DrawVirtualMirrorRebuild(
        RSLogicalDisplayRenderNodeDrawable& drawable,
        RSLogicalDisplayRenderParams& params,
        const std::shared_ptr<RSUniRenderVirtualProcessor>& processor,
        RSRenderThreadParams& uniParam);

    static void DrawVirtualExtend(
        RSLogicalDisplayRenderNodeDrawable& drawable,
        RSLogicalDisplayRenderParams& params,
        const std::shared_ptr<RSUniRenderVirtualProcessor>& processor);

    static std::pair<MultiScreenParams, bool> GetMultiScreenParams(RSLogicalDisplayRenderParams& params);
};

} // namespace OHOS::Rosen::DrawableV2
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_CORE_FEATURE_MULTI_SCREEN_RS_MULTI_SCREEN_UTIL_H
