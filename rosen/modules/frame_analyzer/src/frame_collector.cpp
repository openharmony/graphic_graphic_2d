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

#include <chrono>
#include <mutex>
#include <map>
#include <string>

#include "hilog/log.h"
#include "hitrace_meter.h"
#include "parameter.h"

#include "frame_saver.h"

namespace OHOS {
namespace Rosen {

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400

#undef LOG_TAG
#define LOG_TAG "FrameCollector"

#define LOGW(fmt, ...) HILOG_WARN(LOG_CORE, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)
namespace {
constexpr int32_t uimarksStart = static_cast<int32_t>(FrameEventType::UIMarksStart);
constexpr int32_t uimarksEnd = static_cast<int32_t>(FrameEventType::UIMarksEnd) - 1;
constexpr int32_t loopEnd = static_cast<int32_t>(FrameEventType::LoopEnd) - 1;
constexpr int32_t vsyncStart = static_cast<int32_t>(FrameEventType::WaitVsyncStart);
constexpr int32_t vsyncEnd = static_cast<int32_t>(FrameEventType::WaitVsyncEnd);
} // namespace

FrameCollector &FrameCollector::GetInstance()
{
    static FrameCollector instance;
    return instance;
}

void FrameCollector::SetRepaintCallback(std::function<void()> repaint)
{
    repaint_ = repaint;
}

const FrameInfoQueue &FrameCollector::LockGetFrameQueue()
{
    frameQueueMutex_.lock();
    return frameQueue_;
}

void FrameCollector::UnlockFrameQueue()
{
    frameQueueMutex_.unlock();
}

bool FrameCollector::IsEnabled() const
{
    return enabled_;
}

void FrameCollector::SetEnabled(bool enable)
{
    enabled_ = enable;
}

void FrameCollector::MarkFrameEvent(const FrameEventType &type, int64_t timeNs)
{
    const auto &index = static_cast<int32_t>(type);
    if (index >= static_cast<int32_t>(FrameEventType::Max)) {
        LOGW("FrameCollector::MarkFrameEvent index(%{public}d) not exists", static_cast<int32_t>(index));
        return;
    }

    if (timeNs == 0) {
        timeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    if (usingSaver_) {
        saver_->SaveFrameEvent(type, timeNs);
    }

    if (!enabled_) {
        return;
    }

    LOGD("FrameCollector::MarkFrameEvent index(%{public}d) occur at %{public}s", index, std::to_string(timeNs).c_str());
    ProcessFrameEvent(index, timeNs);
}

void FrameCollector::ProcessFrameEvent(int32_t index, int64_t timeNs)
{
    std::lock_guard lockPending(pendingMutex_);
    // lockFrameQueue: lock for {pbefore_, pafter_}
    std::lock_guard lockFrameQueue(frameQueueMutex_);
    if (ProcessUIMarkLocked(index, timeNs)) {
        return;
    }

    if (index == vsyncStart) {
        pbefore_ = &frameQueue_.Push(FrameInfo());
        pbefore_->frameNumber = currentUIMarks_.frameNumber;
        for (auto i = uimarksStart; i <= uimarksEnd; i++) {
            pbefore_->times[i] = currentUIMarks_.times[i];
        }
        pbefore_->times[index] = timeNs;

        if (haveAfterVsync_) {
            pbefore_->skiped = true;
            pbefore_->times[vsyncEnd] = pbefore_->times[vsyncStart];
        } else {
            StartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, GetAsyncNameByFrameEventType(index), pbefore_->frameNumber);
        }
        return;
    }

    if (!haveAfterVsync_) {
        haveAfterVsync_ = true;
        pafter_ = pbefore_;
    }

    if (pafter_ != nullptr) {
        pafter_->times[index] = timeNs;

        if (IsStartFrameEventType(index)) {
            StartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, GetAsyncNameByFrameEventType(index), pafter_->frameNumber);
        } else {
            FinishAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, GetAsyncNameByFrameEventType(index), pafter_->frameNumber);
        }
    }

    if (index == loopEnd) {
        haveAfterVsync_ = false;
    }
}

bool FrameCollector::ProcessUIMarkLocked(int32_t index, int64_t timeNs)
{
    if (index > uimarksEnd) {
        return false;
    }

    if (IsStartFrameEventType(index)) {
        StartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, GetAsyncNameByFrameEventType(index), currentFrameNumber_);
    } else {
        FinishAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, GetAsyncNameByFrameEventType(index), currentFrameNumber_);
    }

    if (index < uimarksEnd) {
        pendingUIMarks_.times[index] = timeNs;
        return true;
    }

    // index == uimarksEnd
    pendingUIMarks_.times[index] = timeNs;
    currentUIMarks_ = pendingUIMarks_;
    pendingUIMarks_.frameNumber = ++currentFrameNumber_;
    return true;
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
    auto oldEnable = that.enabled_;
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

    if (that.enabled_ != oldEnable && that.repaint_ != nullptr) {
        that.repaint_();
    }
}
} // namespace Rosen
} // namespace OHOS
