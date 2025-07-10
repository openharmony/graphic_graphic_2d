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

#include "common/rs_optional_trace.h"
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

void SelfDrawingNodeMonitor::InsertCurRectMap(NodeId id, RectI rect)
{
    curRect_[id] = rect;
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

void SelfDrawingNodeMonitor::RegisterRectChangeCallback(
    pid_t pid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    RS_TRACE_NAME_FMT("SelfDrawingNodeMonitor::RegisterRectChangeCallback registerPid:%d", pid);
    rectChangeCallbackListenner_[pid] = callback;
    rectChangeCallbackConstraint_[pid] = constraint;
}

void SelfDrawingNodeMonitor::UnRegisterRectChangeCallback(pid_t pid)
{
    RS_TRACE_NAME_FMT("SelfDrawingNodeMonitor::UnRegisterRectChangeCallback unRegisterPid:%d", pid);
    rectChangeCallbackListenner_.erase(pid);
    rectChangeCallbackConstraint_.erase(pid);
    if (rectChangeCallbackListenner_.empty()) {
        ClearRectMap();
    }
}

void SelfDrawingNodeMonitor::TriggerRectChangeCallback()
{
    RS_OPTIONAL_TRACE_NAME_FMT("SelfDrawingNodeMonitor::TriggerRectChangeCallback");
    if (rectChangeCallbackListenner_.empty()) {
        return;
    }
    
    if (curRect_ != lastRect_) {
        for (const auto& iter : rectChangeCallbackListenner_) {
            pid_t pid = iter.first;
            bool shouldTrigger = false;
            SelfDrawingNodeRectCallbackData callbackData;
            auto constraintIter = rectChangeCallbackConstraint_.find(pid);
            if (constraintIter != rectChangeCallbackConstraint_.end()) {
                shouldTrigger = ShouldTrigger(constraintIter->second, callbackData);
            }
            if (shouldTrigger && iter.second) {
                iter.second->OnSelfDrawingNodeRectChange(std::make_shared<RSSelfDrawingNodeRectData>(callbackData));
            } else {
                RS_LOGD("SelfDrawingNodeMonitor::TriggerRectChangeCallback callback is null or shouldnot trigger "
                        "%{public}s", shouldTrigger ? "true" : "false");
            }
        }
        lastRect_ = curRect_;
    }
}

bool SelfDrawingNodeMonitor::ShouldTrigger(RectConstraint& constraint, SelfDrawingNodeRectCallbackData& callbackData)
{
    bool ret = false;
    for (auto& [nodeId, rect] : curRect_) {
        if (constraint.pids.find(ExtractPid(nodeId)) == constraint.pids.end()) {
            continue;
        }
        auto iter = lastRect_.find(nodeId);
        if (iter == lastRect_.end()) {
            if (CheckStatify(rect, constraint)) {
                callbackData.insert(std::make_pair(nodeId, rect));
                ret = true;
            }
        } else if (iter->second != rect) {
            if (CheckStatify(rect, constraint) != CheckStatify(iter->second, constraint)) {
                callbackData.insert(std::make_pair(nodeId, rect));
                ret = true;
            }
        }
    }

    for (auto& [nodeId, lastRect] : lastRect_) {
        if (constraint.pids.find(ExtractPid(nodeId)) == constraint.pids.end()) {
            continue;
        }
        if (curRect_.find(nodeId) == curRect_.end()) {
            if (CheckStatify(lastRect, constraint)) {
                callbackData.insert(std::make_pair(nodeId, RectI(0, 0, 0, 0)));
                ret = true;
            }
        }
    }
    return ret;
}

bool SelfDrawingNodeMonitor::CheckStatify(RectI& rect, RectConstraint& constraint) const
{
    RS_OPTIONAL_TRACE_NAME_FMT("SelfDrawingNodeMonitor::CheckStatify rect %d %d %d %d", rect.GetTop(), rect.GetLeft(),
        rect.GetWidth(), rect.GetHeight());
    if (rect.GetWidth() >= constraint.range.lowLimit.width && rect.GetHeight() >= constraint.range.lowLimit.height &&
        rect.GetWidth() <= constraint.range.highLimit.width && rect.GetHeight() <= constraint.range.highLimit.height) {
            return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS