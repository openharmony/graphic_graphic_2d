/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_BASE_SURFACE_UTIL_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_BASE_SURFACE_UTIL_H
#include <atomic>
#include <vector>
#include "engine/rs_base_render_util.h"
#include "pipeline/rs_surface_handler.h"

namespace OHOS {
namespace Rosen {
class RSBaseSurfaceUtil {
public:
    // Configuration for drop frame by PID feature
    struct DropFrameConfig {
        bool enable = false;        // Enable drop frame by PID
        int32_t level = 0;          // Drop frame level: 0=no drop, >0=keep latest N frames

        // Check if dropping should occur
        bool ShouldDrop() const { return enable && level > 0; }

        // Factory methods for common configurations
        static DropFrameConfig NoDrop() { return {false, 0}; }
        static DropFrameConfig Level(int32_t l) { return {true, l}; }
    };

    static Rect MergeBufferDamages(const std::vector<Rect>& damages);
    static void MergeBufferDamages(Rect& surfaceDamage, const std::vector<Rect>& damages);
    static GSError DropFrameProcess(RSSurfaceHandler& surfaceHandler, uint64_t presentWhen = 0);
    static bool ConsumeAndUpdateBuffer(RSSurfaceHandler& surfaceHandler, uint64_t presentWhen = CONSUME_DIRECTLY,
        const DropFrameConfig& dropFrameConfig = DropFrameConfig::NoDrop(),
        uint64_t parentNodeId = 0, bool dropFrameByScreenFrozen = false);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_BASE_SURFACE_UTIL_H
