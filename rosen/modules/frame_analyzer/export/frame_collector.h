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

#ifndef ROSEN_MODULE_FRAME_ANALYZER_EXPORT_FRAME_COLLECTOR_H
#define ROSEN_MODULE_FRAME_ANALYZER_EXPORT_FRAME_COLLECTOR_H

#include <functional>
#include <memory>
#include <mutex>

#include "frame_info.h"

namespace OHOS {
namespace Rosen {
class FrameSaver;

class FrameCollector {
public:
    static FrameCollector &GetInstance();

    ~FrameCollector() = default;

    void SetRepaintCallback(std::function<void()> repaint);
    const FrameInfoQueue &LockGetFrameQueue();
    void UnlockFrameQueue();
    bool IsEnabled() const;
    void SetEnabled(bool enable);
    void MarkFrameEvent(const FrameEventType &type, int64_t timeNs = 0);
    void ClearEvents();

private:
    FrameCollector();
    FrameCollector(const FrameCollector &collector) = delete;
    FrameCollector(FrameCollector &&collector) = delete;
    FrameCollector &operator =(const FrameCollector &collector) = delete;
    FrameCollector &operator =(FrameCollector &&collector) = delete;

    void ProcessFrameEvent(int32_t index, int64_t timeNs);
    bool ProcessUIMarkLocked(int32_t index, int64_t timeNs);
    static void SwitchFunction(const char *key, const char *value, void *context);

    // pending
    std::mutex pendingMutex_;
    int32_t currentFrameNumber_ = 0;
    struct UIMarks pendingUIMarks_ = {};
    struct UIMarks currentUIMarks_ = {};
    struct FrameInfo *pbefore_ = nullptr;
    struct FrameInfo *pafter_ = nullptr;
    bool haveAfterVsync_ = false;

    // frame queue
    std::mutex frameQueueMutex_;
    FrameInfoQueue frameQueue_;

    // param
    bool enabled_ = false;
    bool usingSaver_ = false;
    std::shared_ptr<FrameSaver> saver_ = nullptr;
    std::function<void()> repaint_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULE_FRAME_ANALYZER_EXPORT_FRAME_COLLECTOR_H
