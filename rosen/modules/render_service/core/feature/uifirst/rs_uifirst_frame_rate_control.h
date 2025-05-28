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

#include <parameter.h>
#include <parameters.h>
#include <string>
#include "pipeline/rs_surface_render_node.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"

namespace OHOS::Rosen {
class RSUifirstFrameRateControl {
public:
    static RSUifirstFrameRateControl& Instance()
    {
        static RSUifirstFrameRateControl instance;
        return instance;
    }

    void SetAnimationInfo(const EventInfo& eventInfo);
    bool JudgeMultiSubSurface(RSSurfaceRenderNode& node);
    bool SubThreadFrameDropDecision(RSSurfaceRenderNode& node);
    bool NeedRSUifirstControlFrameDrop(RSSurfaceRenderNode& node);
    bool GetUifirstFrameDropInternal(int);
    bool JudgeStartAnimation() const
    {
        return startAnimationStatus_;
    }

    void SetStartAnimation(bool status)
    {
        startAnimationStatus_ = status;
    }

    bool JudgeStopAnimation() const
    {
        return stopAnimationStatus_;
    }

    void SetStopAnimation(bool status)
    {
        stopAnimationStatus_ = status;
    }

    bool JudgeMultTaskAnimation() const
    {
        return multTaskAnimationStatus_;
    }

    void SetMultTaskAnimation(bool status)
    {
        multTaskAnimationStatus_ = status;
    }
private:
    RSUifirstFrameRateControl() {}
    int callCount_ = 0;
    bool startAnimationStatus_ = false;
    bool stopAnimationStatus_ = false;
    bool multTaskAnimationStatus_ = false;
    bool forceFreashOnce_ = true;
};
}