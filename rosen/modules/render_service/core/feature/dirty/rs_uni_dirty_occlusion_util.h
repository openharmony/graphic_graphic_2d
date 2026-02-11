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

#ifndef RENDER_SERVICE_CORE_FEATURE_RS_UNI_DIRTY_OCCLUSION_UTIL_H
#define RENDER_SERVICE_CORE_FEATURE_RS_UNI_DIRTY_OCCLUSION_UTIL_H

#include <string>
#include <unordered_map>

#include "common/rs_occlusion_region.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
class RSUniDirtyOcclusionUtil {
public:
    RSUniDirtyOcclusionUtil() = delete;

    static void SetAnimationOcclusionInfo(const std::string& sceneId, bool isStart);

    static std::pair<bool, time_t>& GetIsAnimationOcclusionRef()
    {
        return isAnimationOcclusion_;
    }

    static bool GetIsAnimationOcclusion()
    {
        return isAnimationOcclusion_.first;
    }

    static void CheckResetAccumulatedOcclusionRegion(RSSurfaceRenderNode& node,
        RectI& screenRect, Occlusion::Region& accumulatedOcclusionRegion);

    static bool IsParticipateInOcclusion(RSSurfaceRenderNode& node,
        bool isFocus, bool ancestorNodeHasAnimation, bool isAllSurfaceVisibleDebugEnabled);

private:
    static std::pair<bool, time_t> isAnimationOcclusion_;
};
}
}
#endif // RENDER_SERVICE_CORE_FEATURE_RS_UNI_DIRTY_OCCLUSION_UTIL_H