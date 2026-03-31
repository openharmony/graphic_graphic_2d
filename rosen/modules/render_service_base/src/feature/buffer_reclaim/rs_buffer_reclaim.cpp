/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ROSEN_CROSS_PLATFORM
#include "feature/buffer_reclaim/rs_buffer_reclaim.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
const uint32_t MAX_BUFFER_RECLAIM_NUMS = 50;

RSBufferReclaim& RSBufferReclaim::GetInstance()
{
    static RSBufferReclaim instance;
    return instance;
}

void RSBufferReclaim::BufferDestructorCallBack(uint64_t bufferId)
{
    RSBufferReclaim::GetInstance().RemoveBufferReclaim(bufferId);
}

bool RSBufferReclaim::DoBufferReclaim(sptr<SurfaceBuffer> buffer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = bufferReclaimNumsSet_.find(buffer->GetBufferId());
    if (iter != bufferReclaimNumsSet_.end()) {
        return false;
    }
    if (bufferReclaimNumsSet_.size() >= MAX_BUFFER_RECLAIM_NUMS) {
        return false;
    }
    bufferReclaimNumsSet_.insert(buffer->GetBufferId());
    RS_TRACE_NAME_FMT("DoBufferReclaim: bufferReclaimNumsSet_=%lu", bufferReclaimNumsSet_.size());
    RS_LOGI("DoBufferReclaim: bufferReclaimNumsSet_=%{public}u", bufferReclaimNumsSet_.size());

    bool ret = false;
    if (buffer->TryReclaim() == GSERROR_OK) {
        buffer->RegisterBufferDestructorCallBack(&RSBufferReclaim::BufferDestructorCallBack);
        ret = true;
    } else {
        bufferReclaimNumsSet_.erase(buffer->GetBufferId());
    }
    return ret;
}


bool RSBufferReclaim::DoBufferResume(sptr<SurfaceBuffer> buffer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = bufferReclaimNumsSet_.find(buffer->GetBufferId());
    if (iter == bufferReclaimNumsSet_.end()) {
        return false;
    }
    bufferReclaimNumsSet_.erase(iter);
    RS_TRACE_NAME_FMT("DoBufferResume: bufferReclaimNumsSet_=%lu", bufferReclaimNumsSet_.size());
    RS_LOGI("DoBufferResume: bufferReclaimNumsSet_=%{public}u", bufferReclaimNumsSet_.size());
    bool ret = false;
    if (buffer->TryResumeIfNeeded() == GSERROR_OK) {
        buffer->UnRegisterBufferDestructorCallBack();
        ret = true;
    } else {
        bufferReclaimNumsSet_.insert(buffer->GetBufferId());
    }
    return ret;
}

bool RSBufferReclaim::CheckBufferReclaim()
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_TRACE_NAME_FMT("CheckBufferReclaim: bufferReclaimNumsSet_=%lu", bufferReclaimNumsSet_.size());
    RS_LOGI("CheckBufferReclaim: bufferReclaimNumsSet_=%{public}u", bufferReclaimNumsSet_.size());
    return bufferReclaimNumsSet_.size() < MAX_BUFFER_RECLAIM_NUMS;
}


void RSBufferReclaim::RemoveBufferReclaim(uint64_t bufferId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bufferReclaimNumsSet_.erase(bufferId);
    RS_TRACE_NAME_FMT("RemoveBufferReclaim: bufferReclaimNumsSet_=%lu", bufferReclaimNumsSet_.size());
    RS_LOGI("RemoveBufferReclaim: bufferReclaimNumsSet_=%{public}u", bufferReclaimNumsSet_.size());
}

void RSBufferReclaim::AddUICaptureNode(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(uiCaptureNodeMapMutex_);
    pid_t pid = ExtractPid(nodeId);
    RS_TRACE_NAME_FMT("UICaptureNode: add nodeid=%llu, pid=%d, mapSize=%u", nodeId, pid, uiCaptureNodeMap_.size());
    auto iter1 = uiCaptureNodeMap_.find(pid);
    if (iter1 == uiCaptureNodeMap_.end()) {
        std::map<NodeId, uint32_t> nodeIdCountMap;
        nodeIdCountMap[nodeId] = 1;
        uiCaptureNodeMap_[pid] = nodeIdCountMap;
        RS_TRACE_NAME("new pid and node");
        return;
    }

    auto& nodeIdCountMap = iter1->second;
    auto iter2 = nodeIdCountMap.find(nodeId);
    if (iter2 != nodeIdCountMap.end()) {
        iter2->second++;
        RS_TRACE_NAME_FMT("same node, count=%u", iter2->second);
    } else {
        nodeIdCountMap[nodeId] = 1;
        RS_TRACE_NAME("new node");
    }
}

void RSBufferReclaim::RemoveUICaptureNode(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(uiCaptureNodeMapMutex_);
    pid_t pid = ExtractPid(nodeId);
    RS_TRACE_NAME_FMT("UICaptureNode: remove nodeid=%llu, pid=%d, mapSize=%u", nodeId, pid, uiCaptureNodeMap_.size());
    auto iter1 = uiCaptureNodeMap_.find(pid);
    if (iter1 == uiCaptureNodeMap_.end()) {
        RS_TRACE_NAME("invalid pid");
        return;
    }

    auto& nodeIdCountMap = iter1->second;
    auto iter2 = nodeIdCountMap.find(nodeId);
    if (iter2 != nodeIdCountMap.end()) {
        iter2->second--;
        RS_TRACE_NAME_FMT("node count=%u", iter2->second);
        if (iter2->second == 0) {
            nodeIdCountMap.erase(iter2);
        }
    }

    if (nodeIdCountMap.empty()) {
        RS_TRACE_NAME_FMT("pid=%d, all node is removed", pid);
        uiCaptureNodeMap_.erase(iter1);
    }
}

bool RSBufferReclaim::CheckSameProcessUICaptureNode(NodeId id)
{
    std::lock_guard<std::mutex> lock(uiCaptureNodeMapMutex_);
    pid_t pid = ExtractPid(id);
    return (uiCaptureNodeMap_.find(pid) != uiCaptureNodeMap_.end());
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_CROSS_PLATFORM
