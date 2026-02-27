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

#include "rs_uni_dirty_occlusion_util.h"

#include <parameter.h>
#include <parameters.h>
#include <string>

#include "common/rs_optional_trace.h"
#include "rs_trace.h"

#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "feature/uifirst/rs_uifirst_frame_rate_control.h"
#include "graphic_feature_param_manager.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
std::pair<bool, time_t> RSUniDirtyOcclusionUtil::isAnimationOcclusion_;

void RSUniDirtyOcclusionUtil::SetAnimationOcclusionInfo(const std::string& sceneId, bool isStart)
{
    if (!DirtyRegionParam::IsAnimationOcclusionEnable() || !RSSystemProperties::GetAnimationOcclusionEnabled()) {
        return;
    }
    int64_t curTime = static_cast<int64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    auto id = RSUifirstFrameRateControl::Instance().GetSceneId(sceneId);
    switch (id) {
        // currently, occlusion is only enabled in these three animation scenes
        case RSUifirstFrameRateControl::SceneId::LAUNCHER_APP_LAUNCH_FROM_ICON:
        case RSUifirstFrameRateControl::SceneId::LAUNCHER_APP_LAUNCH_FROM_DOCK:
        case RSUifirstFrameRateControl::SceneId::LAUNCHER_APP_SWIPE_TO_HOME:
            isAnimationOcclusion_.first = isStart;
            isAnimationOcclusion_.second = curTime;
            break;
        default:
            break;
    }
}

void RSUniDirtyOcclusionUtil::CheckResetAccumulatedOcclusionRegion(RSSurfaceRenderNode& node,
    RectI& screenRect, Occlusion::Region& accumulatedOcclusionRegion)
{
    auto visibleFilterRect = RSUniFilterDirtyComputeUtil::GetVisibleFilterRect(node);
    bool needReset = RSUniDirtyOcclusionUtil::GetIsAnimationOcclusion() &&
        node.IsTransparent() && visibleFilterRect == screenRect;
    if (needReset) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSUniDirtyOcclusionUtil::CheckResetAccumulatedOcclusionRegion surface node[%s]"
            "accumulated occlusion region needs to be reset", node.GetName().c_str());
        accumulatedOcclusionRegion.Reset();
    }
}

bool RSUniDirtyOcclusionUtil::IsParticipateInOcclusion(RSSurfaceRenderNode& node,
    bool isFocus, bool ancestorNodeHasAnimation, bool isAllSurfaceVisibleDebugEnabled)
{
    auto mainThread = RSMainThread::Instance();

    // specific animation effects can enable occlusion, including three-finger swipe up and quick app launch/exit.
    bool isAnimationOcclusionScenes =
        (isFocus && RSUniDirtyOcclusionUtil::GetIsAnimationOcclusion()) || mainThread->GetIsRegularAnimation();
    bool occlusionInAnimation = !node.IsAttractionAnimation() &&
        (!ancestorNodeHasAnimation || isAnimationOcclusionScenes);
    bool isParticipateInOcclusion = node.CheckParticipateInOcclusion(isAnimationOcclusionScenes) &&
        occlusionInAnimation && !isAllSurfaceVisibleDebugEnabled;
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniDirtyOcclusionUtil::IsParticipateInOcclusion:[%s] isFocus:[%d] "
        "isAnimationOcclusionScenes:[%d] isRegularAnimation:[%d] ancestorNodeHasAnimation:[%d] "
        "isParticipateInOcclusion:[%d] isAttractionAnimation:[%d]",
        node.GetName().c_str(), isFocus, isAnimationOcclusionScenes, mainThread->GetIsRegularAnimation(),
        ancestorNodeHasAnimation, isParticipateInOcclusion, node.IsAttractionAnimation());
    return isParticipateInOcclusion;
}

} // namespace Rosen
} // namespace OHOS