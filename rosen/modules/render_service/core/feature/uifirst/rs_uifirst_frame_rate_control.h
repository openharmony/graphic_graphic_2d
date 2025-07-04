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
#include "transaction/rs_render_service_client.h"
#include <mutex>

namespace OHOS::Rosen {
class RSUifirstFrameRateControl {
public:
    enum class SceneId {
        LAUNCHER_APP_LAUNCH_FROM_ICON,
        LAUNCHER_APP_SWIPE_TO_HOME,
        GESTURE_TO_RECENTS,
        LAUNCHER_APP_LAUNCH_FROM_RECENT,
        EXIT_RECENT_2_HOME_ANI,
        CLEAR_1_RECENT_ANI,
        CLEAR_ALL_RECENT_ANI,
        AOD_TO_LAUNCHER,
        LOCKSCREEN_TO_LAUNCHER,
        UNKNOWN
    };

    SceneId GetSceneId(const std::string& sceneId)
    {
        if (sceneId == "LAUNCHER_APP_LAUNCH_FROM_ICON") {
            return SceneId::LAUNCHER_APP_LAUNCH_FROM_ICON;
        } else if (sceneId == "LAUNCHER_APP_SWIPE_TO_HOME") {
            return SceneId::LAUNCHER_APP_SWIPE_TO_HOME;
        } else if (sceneId == "GESTURE_TO_RECENTS") {
            return SceneId::GESTURE_TO_RECENTS;
        } else if (sceneId == "LAUNCHER_APP_LAUNCH_FROM_RECENT") {
            return SceneId::LAUNCHER_APP_LAUNCH_FROM_RECENT;
        } else if (sceneId == "EXIT_RECENT_2_HOME_ANI") {
            return SceneId::EXIT_RECENT_2_HOME_ANI;
        } else if (sceneId == "CLEAR_1_RECENT_ANI") {
            return SceneId::CLEAR_1_RECENT_ANI;
        } else if (sceneId == "CLEAR_All_RECENT_ANI") {
            return SceneId::CLEAR_ALL_RECENT_ANI;
        } else if (sceneId == "AOD_TO_LAUNCHER") {
            return SceneId::AOD_TO_LAUNCHER;
        } else if (sceneId == "LOCKSCREEN_TO_LAUNCHER") {
            return SceneId::LOCKSCREEN_TO_LAUNCHER;
        } else {
            return SceneId::UNKNOWN;
        }
    }

    static RSUifirstFrameRateControl& Instance()
    {
        static RSUifirstFrameRateControl instance;
        return instance;
    }

    void SetAnimationStartInfo(const DataBaseRs& eventInfo);
    void SetAnimationEndInfo(const DataBaseRs& eventInfo);
    bool JudgeMultiSubSurface(const RSSurfaceRenderNode& node);
    bool SubThreadFrameDropDecision(const RSSurfaceRenderNode& node);
    bool NeedRSUifirstControlFrameDrop(const RSSurfaceRenderNode& node);
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
    std::mutex incrementCallCount_;
    int callCount_ = 0;
    bool startAnimationStatus_ = false;
    bool stopAnimationStatus_ = false;
    bool multTaskAnimationStatus_ = false;
    bool forceRefreshOnce_ = true;
};
}