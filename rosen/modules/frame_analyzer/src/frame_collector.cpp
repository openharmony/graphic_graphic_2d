/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "frame_collector.h"

#include <cinttypes>
#include <chrono>
#include <mutex>
#include <map>

#include <hilog/log.h>
#include <parameter.h>

#include "frame_saver.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001400, "FrameCollector" };
} // namespace

FrameCollector &FrameCollector::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard lock(mutex);
        if (instance == nullptr) {
            instance = std::unique_ptr<FrameCollector>(new FrameCollector());
        }
    }
    return *instance;
}

void FrameCollector::MarkFrameEvent(const FrameEventType &type, int64_t timeNs)
{
    const auto &index = static_cast<int32_t>(type);
    if (index >= static_cast<int32_t>(FrameEventType::Max)) {
        ::OHOS::HiviewDFX::HiLog::Warn(LABEL,
            "FrameCollector::MarkFrameEvent index(%{public}d) not exists", static_cast<int32_t>(index));
        return;
    }

    if (timeNs == 0) {
        timeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    if (usingSaver_ == true) {
        saver_->SaveFrameEvent(type, timeNs);
    }

    if (enabled_ == false) {
        return;
    }

    constexpr int32_t uimarksStart = static_cast<int32_t>(FrameEventType::UIMarksStart);
    constexpr int32_t uimarksEnd = static_cast<int32_t>(FrameEventType::UIMarksEnd) - 1;
    constexpr int32_t loopEnd = static_cast<int32_t>(FrameEventType::LoopEnd) - 1;
    constexpr int32_t vsyncMark = static_cast<int32_t>(FrameEventType::WaitVsyncEnd);

    std::lock_guard lockPending(pendingMutex_);
    std::lock_guard lockFrameQueue(frameQueueMutex_);
    if (index <= uimarksEnd) {
        pendingUIMarks_.times[index] = timeNs;
    } else if (index < vsyncMark) {
        pbefore_ = &frameQueue_.Push({});
        pbefore_->frameNumber = currentUIMarks_.frameNumber;
        for (auto i = uimarksStart; i <= uimarksEnd; i++) {
            pbefore_->times[i] = currentUIMarks_.times[i];
        }
        pbefore_->times[index] = timeNs;
    } else {
        if (haveAfterVsync_ == false) {
            haveAfterVsync_ = true;
            pafter_ = pbefore_;
        }
        pafter_->times[index] = timeNs;
    }

    if (index == uimarksEnd) {
        currentUIMarks_ = pendingUIMarks_;
        pendingUIMarks_.frameNumber = ++currentFrameNumber_;
        ::OHOS::HiviewDFX::HiLog::Info(LABEL, "currentUIMarks_");
    }

    if (index == vsyncMark - 1 && haveAfterVsync_) {
        pbefore_->skiped = true;
        pbefore_->times[vsyncMark + 1] = pbefore_->times[vsyncMark];
    }

    ::OHOS::HiviewDFX::HiLog::Debug(LABEL,
        "FrameCollector::MarkFrameEvent index(%{public}d) occur at %{public}" PRIi64,
        static_cast<int32_t>(index), timeNs);
    if (index == loopEnd) {
        haveAfterVsync_ = false;
    }
}

void FrameCollector::ClearEvents()
{
    std::lock_guard lock(frameQueueMutex_);
    frameQueue_.Clear();
}

FrameCollector::FrameCollector()
{
    char value[0x20];
    GetParameter(switchRenderingText, "disable", value, sizeof(value));
    SwitchFunction(switchRenderingText, value, this);
    WatchParameter(switchRenderingText, SwitchFunction, this);
}

void FrameCollector::SwitchFunction(const char *key, const char *value, void *context)
{
    auto &that = *reinterpret_cast<FrameCollector *>(context);
    std::string str = value;
    if (str == switchRenderingPaintText) {
        that.ClearEvents();
        that.usingSaver_ = false;
        that.enabled_ = true;
    }

    if (str == switchRenderingSaverText) {
        that.ClearEvents();
        that.usingSaver_ = true;
        that.enabled_ = false;
        that.saver_ = std::make_unique<FrameSaver>();
    }

    if (str == switchRenderingDisableText) {
        that.usingSaver_ = false;
        that.enabled_ = false;
    }
}
} // namespace Rosen
} // namespace OHOS
