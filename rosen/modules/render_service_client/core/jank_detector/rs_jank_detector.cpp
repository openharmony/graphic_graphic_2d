/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "jank_detector/rs_jank_detector.h"

#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif

#ifdef _WIN32
#define getuid() 0
#endif

namespace {
struct FrameMsg {
    uint64_t totalTime = 0;
    uint64_t uiDrawTime = 0;
    uint64_t renderDrawTime = 0;
    int dropUiFrameNum = 0;
    std::string abilityName;
};

void DrawEventReport(const FrameMsg &frameMsg, const std::string &stringId)
{
#ifdef ROSEN_OHOS
    int32_t pid = OHOS::GetRealPid();
    uint32_t uid = getuid();
    std::string msg = "It took " + std::to_string(frameMsg.totalTime) + "ns to draw, "
        + "UI took " + std::to_string(frameMsg.uiDrawTime) + "ns to draw, "
        + "RSRenderThread took " + std::to_string(frameMsg.renderDrawTime) + "ns to draw, "
        + "RSRenderThread dropped " + std::to_string(frameMsg.dropUiFrameNum) + " UI Frames";

    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, stringId,
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "PID", pid,
        "UID", uid,
        "ABILITY_NAME", frameMsg.abilityName,
        "MSG", msg);
#endif
}
}

namespace OHOS {
namespace Rosen {
uint64_t RSJankDetector::GetSysTimeNs()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

void RSJankDetector::SetRefreshPeriod(uint64_t refreshPeriod)
{
    refreshPeriod_ = refreshPeriod;
}

uint64_t RSJankDetector::GetRefreshPeriod() const
{
    return refreshPeriod_;
}

void RSJankDetector::UpdateUiDrawFrameMsg(uint64_t startTimeStamp, uint64_t endTimeStamp,
    const std::string& abilityName)
{
    // In some scenes we don't have startTimeStamp(e.g. in OnSurfaceChanged),
    // so we temporarily don't count this situation.
    if (startTimeStamp == 0) {
        return;
    }

    UiDrawFrameMsg uiFrame;
    uiFrame.startTimeStamp = startTimeStamp;
    uiFrame.endTimeStamp = endTimeStamp;
    uiFrame.abilityName = abilityName;
    uiDrawFrames_.emplace_back(uiFrame);
}

void RSJankDetector::CalculateSkippedFrame(uint64_t renderStartTimeStamp, uint64_t renderEndTimeStamp)
{
    FrameMsg frameMsg;
    uint64_t uiStartTimeStamp = 0;
    uint64_t uiEndTimeStamp = 0;
    if (!uiDrawFrames_.empty()) {
        UiDrawFrameMsg uiDrawFrame = uiDrawFrames_.back();
        frameMsg.dropUiFrameNum = uiDrawFrames_.size() - 1;
        uiStartTimeStamp = uiDrawFrame.startTimeStamp;
        uiEndTimeStamp = uiDrawFrame.endTimeStamp;
        frameMsg.abilityName = uiDrawFrame.abilityName;
    }

    frameMsg.uiDrawTime = uiEndTimeStamp - uiStartTimeStamp;
    frameMsg.renderDrawTime = renderEndTimeStamp - renderStartTimeStamp;
    // Currently, we do not consider the time consumption of UI thread
    frameMsg.totalTime = frameMsg.renderDrawTime;
    uiDrawFrames_.clear();
    int skippedFrame = 0;
    if (refreshPeriod_ != 0) {
        skippedFrame = static_cast<int>(frameMsg.totalTime / refreshPeriod_);
    }
    if ((skippedFrame >= JANK_SKIPPED_THRESHOLD) || (frameMsg.dropUiFrameNum >= JANK_SKIPPED_THRESHOLD)) {
        DrawEventReport(frameMsg, "JANK_FRAME_SKIP");
    }

    if (frameMsg.renderDrawTime >= NO_DRAW_THRESHOLD) {
        DrawEventReport(frameMsg, "NO_DRAW");
    }
}
} // namespace Rosen
} // namespace OHOS
