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
#include "ffrt.h"
#include "ffrt_inner.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
const uint32_t MAX_BUFFER_RECLAIM_NUMS = 50;
constexpr uint32_t RECLAIM_TIMEOUT_MS = 300;

RSBufferReclaim& RSBufferReclaim::GetInstance()
{
    static RSBufferReclaim instance;
    return instance;
}

void RSBufferReclaim::BufferDestructorCallback(uint64_t bufferId)
{
    RSBufferReclaim::GetInstance().RemoveBufferReclaim(bufferId);
}

void RSBufferReclaim::ReclaimBufferOnFfrt(sptr<SurfaceBuffer> buffer)
{
    const auto bufferId = buffer->GetBufferId();
    RS_TRACE_NAME_FMT("TryBufferReclaim bufId: %" PRIu64 ", seq: %u, hasReclaimed: %d",
        bufferId, buffer->GetSeqNum(), buffer->IsReclaimed());
    {
        std::lock_guard<std::mutex> lock(bufferReclaimMapMutex_);
        auto iter = bufferReclaimMap_.find(bufferId);
        if (iter == bufferReclaimMap_.end() || iter->second != ReclaimState::PENDING) {
            bufferReclaimMap_.erase(bufferId);
            return;
        }
        iter->second = ReclaimState::RECLAIMING;
        if (!buffer->RegisterBufferDestructorCallbackFunc(&RSBufferReclaim::BufferDestructorCallback)) {
            RS_LOGE("DoBufferReclaim register destructor callback failed");
            bufferReclaimMap_.erase(iter);
            return;
        }
    }
    RS_TRACE_NAME_FMT("DoBufferReclaim");
    auto startTime = std::chrono::steady_clock::now();
    bool needNotify = false;
    auto ret = buffer->TryReclaim();
    if (ret == GSERROR_OK) {
        std::lock_guard<std::mutex> lock(bufferReclaimMapMutex_);
        auto iter = bufferReclaimMap_.find(bufferId);
        if (iter != bufferReclaimMap_.end()) {
            if (iter->second == ReclaimState::RECLAIMING_RESUME) {
                needNotify = true;
            }
            iter->second = ReclaimState::RECLAIMED;
        }
    } else {
        buffer->UnRegisterBufferDestructorCallbackFunc(&RSBufferReclaim::BufferDestructorCallback);
        std::lock_guard<std::mutex> lock(bufferReclaimMapMutex_);
        bufferReclaimMap_.erase(bufferId);
    }
    if (needNotify) {
        resumeCv_.notify_one();
    }
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::lock_guard<std::mutex> lock(bufferReclaimMapMutex_);
    RS_TRACE_NAME_FMT("DoBufferReclaim done bufferReclaimMap_: %zu, hasReclaimed: %d, ret: %d",
        bufferReclaimMap_.size(), buffer->IsReclaimed(), ret);
    RS_LOGI("DoBufferReclaim bufId:%{public}" PRIu64 ", seq:%{public}u, bufferReclaimMap_:%{public}zu, hasReclaimed: "
        "%{public}d, cost:%{public}" PRIu64 " ms, ret: %{public}d", buffer->GetBufferId(), buffer->GetSeqNum(),
        bufferReclaimMap_.size(), buffer->IsReclaimed(), static_cast<uint64_t>(duration.count()), ret);
}

bool RSBufferReclaim::DoBufferReclaim(sptr<SurfaceBuffer> buffer)
{
    if (!buffer) {
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(bufferReclaimMapMutex_);
        if (bufferReclaimMap_.size() >= MAX_BUFFER_RECLAIM_NUMS) {
            return false;
        }
        auto iter = bufferReclaimMap_.find(buffer->GetBufferId());
        if (iter != bufferReclaimMap_.end()) {
            return false;
        }
        bufferReclaimMap_[buffer->GetBufferId()] = ReclaimState::PENDING;
        RS_TRACE_NAME_FMT("DoBufferReclaim(post to ffrt) bufId: %" PRIu64 ", seq: %u, bufferReclaimMap_: %zu",
            buffer->GetBufferId(), buffer->GetSeqNum(), bufferReclaimMap_.size());
    }
    ffrt::submit([buffer = std::move(buffer)]() {
        RSBufferReclaim::GetInstance().ReclaimBufferOnFfrt(buffer);
        }, ffrt::task_attr().name("LastBufferReclaim").qos(ffrt::qos_user_interactive));
    return true;
}

bool RSBufferReclaim::TryResumeBuffer(sptr<SurfaceBuffer> buffer, uint64_t bufferId)
{
    {
        std::lock_guard<std::mutex> lock(bufferReclaimMapMutex_);
        auto iter = bufferReclaimMap_.find(bufferId);
        if (iter == bufferReclaimMap_.end() || iter->second != ReclaimState::RECLAIMED) {
            return false;
        }
        RS_TRACE_NAME_FMT("DoBufferResume bufferReclaimMap_: %zu", bufferReclaimMap_.size());
    }
    auto startTime = std::chrono::steady_clock::now();
    auto ret = buffer->TryResumeIfNeeded();
    if (ret == GSERROR_OK) {
        if (!buffer->UnRegisterBufferDestructorCallbackFunc(&RSBufferReclaim::BufferDestructorCallback)) {
            RS_LOGW("DoBufferResume: destructor callback is not registered");
        }
        std::lock_guard<std::mutex> lock(bufferReclaimMapMutex_);
        bufferReclaimMap_.erase(bufferId);
    }
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    RS_TRACE_NAME_FMT("DoBufferResume ret: %d", ret);
    RS_LOGI("DoBufferResume bufId:%{public}" PRIu64 ", seq:%{public}u, hasReclaimed:%{public}d, cost:%{public}" PRIu64
        " ms, ret:%{public}d", bufferId, buffer->GetSeqNum(), buffer->IsReclaimed(),
        static_cast<uint64_t>(duration.count()), ret);
    return ret == GSERROR_OK;
}

bool RSBufferReclaim::DoBufferResume(sptr<SurfaceBuffer> buffer)
{
    if (!buffer) {
        return false;
    }
    const auto bufferId = buffer->GetBufferId();
    bool needWaitingForReclaimDone = false;
    {
        std::lock_guard<std::mutex> lock(bufferReclaimMapMutex_);
        auto iter = bufferReclaimMap_.find(bufferId);
        if (iter == bufferReclaimMap_.end()) {
            return false;
        }
        if (iter->second == ReclaimState::PENDING) {
            bufferReclaimMap_.erase(iter);
            RS_LOGI("%{public}s PENDING cancel bufId=%{public}" PRIu64, __func__, bufferId);
            return true;
        }
        if (iter->second == ReclaimState::RECLAIMING || iter->second == ReclaimState::RECLAIMING_RESUME) {
            iter->second = ReclaimState::RECLAIMING_RESUME;
            needWaitingForReclaimDone = true;
            RS_LOGI("%{public}s RECLAIMING deferred bufId=%{public}" PRIu64, __func__, bufferId);
        }
    }
    if (needWaitingForReclaimDone) {
        RS_TRACE_NAME_FMT("DoBufferResume waiting bufId:%" PRIu64 ", seq:%u, hasReclaimed:%d",
            bufferId, buffer->GetSeqNum(), buffer->IsReclaimed());
        std::unique_lock<std::mutex> lock(resumeMutex_);
        if (!resumeCv_.wait_for(
            lock, std::chrono::milliseconds(RECLAIM_TIMEOUT_MS), [bufferId, this]() {
            std::lock_guard<std::mutex> mapLock(bufferReclaimMapMutex_);
            auto it = bufferReclaimMap_.find(bufferId);
            return it == bufferReclaimMap_.end() || it->second == ReclaimState::RECLAIMED;
        })) {
            RS_LOGE("%{public}s wait for reclaim timeout", __func__);
            return false;
        }
    }
    return TryResumeBuffer(buffer, bufferId);
}

bool RSBufferReclaim::CheckBufferReclaim()
{
    std::lock_guard<std::mutex> lock(bufferReclaimMapMutex_);
    RS_TRACE_NAME_FMT("CheckBufferReclaim: bufferReclaimMap_=%zu", bufferReclaimMap_.size());
    RS_LOGI("CheckBufferReclaim: bufferReclaimMap_=%{public}zu", bufferReclaimMap_.size());
    return bufferReclaimMap_.size() < MAX_BUFFER_RECLAIM_NUMS;
}

void RSBufferReclaim::RemoveBufferReclaim(uint64_t bufferId)
{
    std::lock_guard<std::mutex> lock(bufferReclaimMapMutex_);
    bufferReclaimMap_.erase(bufferId);
    RS_TRACE_NAME_FMT("RemoveBufferReclaim: bufferReclaimMap_=%zu", bufferReclaimMap_.size());
    RS_LOGI("RemoveBufferReclaim: bufferReclaimMap_=%{public}zu", bufferReclaimMap_.size());
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
