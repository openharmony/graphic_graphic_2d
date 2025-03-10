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

#include "monitor/self_drawing_node_monitor.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
namespace OHOS {
namespace Rosen {

SelfDrawingNodeMonitor &SelfDrawingNodeMonitor::GetInstance()
{
    static SelfDrawingNodeMonitor monitor;
    return monitor;
}

void SelfDrawingNodeMonitor::InsertCurRectMap(NodeId id, std::string nodeName, RectI rect)
{
    curRect_[id] = std::make_pair(nodeName, rect);
}

void SelfDrawingNodeMonitor::EraseCurRectMap(NodeId id)
{
    curRect_.erase(id);
}

void SelfDrawingNodeMonitor::ClearRectMap()
{
    curRect_.clear();
    lastRect_.clear();
}

pid_t SelfDrawingNodeMonitor::GetCallingPid() const
{
    return callingPid_;
}

void SelfDrawingNodeMonitor::RegisterRectChangeCallback(
    pid_t pid, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    if (isListeningEnabled_) {
        RS_LOGI("SelfDrawingNodeRectChangeCallback has registered");
        return;
    }
    callingPid_ = pid;
    isListeningEnabled_ = true;
    rectChangeCallbackListenner_ = std::make_pair(pid, callback);
}

void SelfDrawingNodeMonitor::UnRegisterRectChangeCallback(pid_t pid)
{
    isListeningEnabled_ = false;
}

void SelfDrawingNodeMonitor::TriggerRectChangeCallback()
{
    if (!isListeningEnabled_) {
        return;
    }
    
    if (curRect_ != lastRect_) {
        RS_TRACE_NAME("SelfDrawingNodeMonitor::TriggerRectChangeCallback");
        if (rectChangeCallbackListenner_.second) {
            rectChangeCallbackListenner_.second->OnSelfDrawingNodeRectChange(
                std::make_shared<RSSelfDrawingNodeRectData>(curRect_));
        } else {
            RS_LOGE("SelfDrawingNodeMonitor::TriggerRectChangeCallback callback is null");
        }
        lastRect_ = curRect_;
    }
}
} // namespace Rosen
} // namespace OHOS