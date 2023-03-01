/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H
#define RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H

#include <atomic>
#include <string>
#include <vector>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum class DirtyRegionDebugType {
    DISABLED = 0,
    CURRENT_SUB,
    CURRENT_WHOLE,
    MULTI_HISTORY,
    CURRENT_SUB_AND_WHOLE,
    CURRENT_WHOLE_AND_MULTI_HISTORY,
    EGL_DAMAGE
};

enum class PartialRenderType {
    DISABLED = 0,                               // 0, disable partial render
    SET_DAMAGE,                                 // 1, set damageregion, without draw_op dropping
    SET_DAMAGE_AND_DROP_OP,                     // 2, drop draw_op if node is not in dirty region
    SET_DAMAGE_AND_DROP_OP_OCCLUSION,           // 3, drop draw_op if node is not in visible region (unirender)
    SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY     // 4, drop draw_op if node is not in visible dirty region (unirender)
};

enum class DumpSurfaceType {
    DISABLED = 0,
    SINGLESURFACE,
    ALLSURFACES,
    PIXELMAP,
};

/* ContainerWindowConfigType
// -1, disable round corner's transparent region
// 0, roundcorner surfacenode has a strip size transparent region
// 1, on the base of ENABLED_LEVEL_0, unfocused surface's side boundary is opaque
// 2, on the base of ENABLED_LEVEL_0, unfocused surface only has 4 little squares as transparent region
*/
enum class ContainerWindowConfigType {
    DISABLED = -1,
    ENABLED_LEVEL_0 = 0,
    ENABLED_UNFOCUSED_WINDOW_LEVEL_1 = 1,
    ENABLED_UNFOCUSED_WINDOW_LEVEL_2 = 2,
};

using OnSystemPropertyChanged = void(*)(const char*, const char*, void*);

class RSB_EXPORT RSSystemProperties final {
public:
    ~RSSystemProperties() = default;

    // used by clients
    static bool GetUniRenderEnabled();
    static bool GetRenderNodeTraceEnabled();
    static DirtyRegionDebugType GetDirtyRegionDebugType();
    static PartialRenderType GetPartialRenderEnabled();
    static PartialRenderType GetUniPartialRenderEnabled();
    static ContainerWindowConfigType GetContainerWindowConfig();
    static bool GetOcclusionEnabled();
    static std::string GetRSEventProperty(const std::string &paraName);
    static bool GetDirectClientCompEnableStatus();
    static bool GetHighContrastStatus();
    static uint32_t GetCorrectionMode();
    static bool IsUniRenderMode();
    static void SetRenderMode(bool isUni);
    static DumpSurfaceType GetDumpSurfaceType();
    static uint64_t GetDumpSurfaceId();
    static bool GetTargetDirtyRegionDfxEnabled(std::vector<std::string>& dfxTargetSurfaceNames_);

    static void SetDrawTextAsBitmap(bool flag);
    static bool GetDrawTextAsBitmap();

    static bool GetColdStartThreadEnabled();
    static float GetAnimationScale();

    static bool GetBoolSystemProperty(const char* name, bool defaultValue);
    static int WatchSystemProperty(const char* name, OnSystemPropertyChanged func, void* context);
private:
    RSSystemProperties() = default;

    static inline bool isUniRenderEnabled_ = false;
    static inline std::atomic_bool isUniRenderMode_ = false;
    inline static bool isDrawTextAsBitmap_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H
