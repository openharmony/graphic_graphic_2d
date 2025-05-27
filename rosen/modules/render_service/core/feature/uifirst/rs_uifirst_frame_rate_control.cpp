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
void RSUifirstFrameRateControl::SetAnimationInfo(const EventInfo& eventInfo) {
    if (eventInfo.description == "START_APP_ANIMATION") {
        SetStartAnimation(eventInfo.eventStatus);
    } else if (eventInfo.description == "BACK_APP_ANIMATION") {
        SetStopAnimation(eventInfo.eventStatus);
    } else if (eventInfo.description == "MULTIPLE_TASK") {
        SetMultTaskAnimation(eventInfo.eventStatus);
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
    
    return inAnimation && (forceFreashOnce_ || (!hasMultipleSubSurfaces && canDropFrame));
}

bool RSUifirstFrameRateControl::NeedRSUifirstControlFrameDrop(RSSurfaceRenderNode& node)
{
    return SubThreadFrameDropDecision(node);
}
}
}