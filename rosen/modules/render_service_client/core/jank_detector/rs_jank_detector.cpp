/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"

namespace {
struct FrameMsg {
    uint64_t totalTime = 0;
    uint64_t uiDrawTime = 0;
    uint64_t renderDrawTime = 0;
    int dropUiFrameNum = 0;
    std::string abilityName;
};

void DrawEventReport(FrameMsg& frameMsg, std::string stringId)
{
    int32_t pid = getpid();
    uint32_t uid = getuid();
    std::string domain = "GRAPHIC";
    std::string msg = "It took " + std::to_string(frameMsg.totalTime) + "ns to draw, "
        + "UI took " + std::to_string(frameMsg.uiDrawTime) + "ns to draw, "
        + "RSRenderThread took " + std::to_string(frameMsg.renderDrawTime) + "ns to draw, "
        + "RSRenderThread dropped " + std::to_string(frameMsg.dropUiFrameNum) + " UI Frames";

    OHOS::HiviewDFX::HiSysEvent::Write(domain, stringId,
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "PID", pid,
        "UID", uid,
        "ABILITY_NAME", frameMsg.abilityName,
        "MSG", msg);
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
    {
        std::unique_lock<std::mutex> frameLock(frameMutex_);
        uiDrawFrames_.emplace_back(uiFrame);
    }
}

void RSJankDetector::ProcessUiDrawFrameMsg()
{
    preUiDrawFrames_.clear();
    {
        std::unique_lock<std::mutex> frameLock(frameMutex_);
        std::swap(preUiDrawFrames_, uiDrawFrames_);
    }
}

void RSJankDetector::CalculateSkippedFrame(uint64_t renderStartTimeStamp, uint64_t renderEndTimeStamp)
{
    FrameMsg frameMsg;
    uint64_t uiStartTimeStamp = 0;
    uint64_t uiEndTimeStamp = 0;
    if (!preUiDrawFrames_.empty()) {
        UiDrawFrameMsg uiDrawFrame = preUiDrawFrames_.front();
        frameMsg.dropUiFrameNum = preUiDrawFrames_.size() - 1;
        uiStartTimeStamp = uiDrawFrame.startTimeStamp;
        uiEndTimeStamp = uiDrawFrame.endTimeStamp;
        frameMsg.abilityName = uiDrawFrame.abilityName;
    }

    frameMsg.totalTime = renderEndTimeStamp - uiStartTimeStamp;
    frameMsg.uiDrawTime = uiEndTimeStamp - uiStartTimeStamp;
    frameMsg.renderDrawTime = renderEndTimeStamp - renderStartTimeStamp;
    // In some animation scenes, only RSRenderthread is working.
    if (preUiDrawFrames_.empty()) {
        frameMsg.totalTime = frameMsg.renderDrawTime;
    }

    // Currently a frame takes two vsync times
    uint64_t skippedFrame = frameMsg.totalTime / (refreshPeriod_ * 2);
    if ((skippedFrame >= JANK_SKIPPED_THRESHOLD) || (frameMsg.dropUiFrameNum >= JANK_SKIPPED_THRESHOLD)) {
        DrawEventReport(frameMsg, "JANK_FRAME_SKIP");
    }

    if (frameMsg.totalTime > NO_DRAW_THRESHOLD) {
        DrawEventReport(frameMsg, "NO_DRAW");
    }
}
} // namespace Rosen
} // namespace OHOS
