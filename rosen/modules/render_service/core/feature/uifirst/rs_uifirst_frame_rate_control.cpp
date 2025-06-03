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

#include "rs_uifirst_frame_rate_control.h"

namespace OHOS{
namespace Rosen {
void RSUifirstFrameRateControl::SetAnimationStartInfo(const DataBaseRs& eventInfo)
{
    if (!RSSystemProperties::GetSubThreadControlFrameRate()) {
        return;
    }
    auto sceneId = GetSceneId(eventInfo.sceneId);

    switch (sceneId) {
        case SceneId::LAUNCHER_APP_LAUNCH_FROM_ICON:
            forceRefreshOnce_ = true;
            SetStartAnimation(true);
            break;
        case SceneId::LAUNCHER_APP_SWIPE_TO_HOME:
            forceRefreshOnce_ = true;
            SetStopAnimation(true);
            break;
        case SceneId::GESTURE_TO_RECENTS:
            forceRefreshOnce_ = true;
            SetMultTaskAnimation(true);
            break;
        case SceneId::LAUNCHER_APP_LAUNCH_FROM_RECENT:
            forceRefreshOnce_ = false;
            break;
        case SceneId::AOD_TO_LAUNCHER:
        case SceneId::LOCKSCREEN_TO_LAUNCHER:
            SetStartAnimation(false);
            SetStopAnimation(false);
            SetMultTaskAnimation(false);
        default:
            break;
    }
}

void RSUifirstFrameRateControl::SetAnimationEndInfo(const DataBaseRs& eventInfo)
{
    if (!RSSystemProperties::GetSubThreadControlFrameRate()) {
        return;
    }
    auto sceneId = GetSceneId(eventInfo.sceneId);

    switch (sceneId) {
        case SceneId::LAUNCHER_APP_LAUNCH_FROM_ICON:
            SetStartAnimation(false);
            break;
        case SceneId::LAUNCHER_APP_SWIPE_TO_HOME:
            SetStopAnimation(false);
            break;
        case SceneId::LAUNCHER_APP_LAUNCH_FROM_RECENT:
            forceRefreshOnce_ = true;
            SetMultTaskAnimation(false);
            break;
        case SceneId::EXIT_RECENT_2_HOME_ANI:
        case SceneId::CLEAR_1_RECENT_ANI:
        case SceneId::CLEAR_ALL_RECENT_ANI:
            SetMultTaskAnimation(false);
            break;
        case SceneId::GESTURE_TO_RECENTS:
        default:
            break;
    }
}

bool RSUifirstFrameRateControl::JudgeMultiSubSurface(RSSurfaceRenderNode& node)
{
    return node.GetChildren()->size() > 1;
}

bool RSUifirstFrameRateControl::GetUifirstFrameDropInternal(int frameInterval)
{
    callCount_++;
    if(callCount_ % (frameInterval + 1) == 0) {
        callCount_ = 0;
        return false;
    }
    return true;
}

bool RSUifirstFrameRateControl::SubThreadFrameDropDecision(RSSurfaceRenderNode& node)
{
    bool inAnimation = JudgeStartAnimation() || JudgeStopAnimation() || JudgeMultTaskAnimation();
    bool hasMultipleSubSurfaces = JudgeMultiSubSurface(node);
    bool canDropFrame = GetUifirstFrameDropInternal(RSSystemProperties::GetSubThreadDropFrameInterval());
    
    return inAnimation && (forceRefreshOnce_ || (!hasMultipleSubSurfaces && canDropFrame));
}

bool RSUifirstFrameRateControl::NeedRSUifirstControlFrameDrop(RSSurfaceRenderNode& node)
{
    return SubThreadFrameDropDecision(node);
}
}
}