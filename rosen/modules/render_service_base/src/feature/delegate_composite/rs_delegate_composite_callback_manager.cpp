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
#include "feature/delegate_composite/rs_delegate_composite_callback_manager.h"

#include <charconv>
#include <string>

#include "buffer_extra_data_impl.h"

#include "command/rs_delegate_composite_command.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {

constexpr uint32_t CLIENT_PID_SHIFT_BITS = 48;
constexpr uint32_t BUFFER_QUEUE_SIZE = 16;
static pid_t GenePidBySeqNum(uint64_t seqNum)
{
    return static_cast<pid_t>(seqNum >> CLIENT_PID_SHIFT_BITS);
}

const std::string CLIENT_PID = "RS-WEB-PROXY-COMPOSER-CLIENT-PID";
const std::string NODE_ID = "RS-WEB-PROXY-COMPOSER-NODE-ID";
const uint32_t MAX_MAP_SIZE = 10000;

static bool g_enableDfx = RSSystemProperties::GetRsDelegateCompositeCleanCacheDfxEnable();
#define DFX_LOGD(enableDfx, format, ...) \
    ((enableDfx) ? (void) HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__) : \
                   (void) HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__))

class RsDelegateProducerListener : public IRemoteStub<IProducerListener> {
public:
    RsDelegateProducerListener(sptr<IConsumerSurface> consumer)
    {
        if (consumer) {
            surfaceId_ = consumer->GetUniqueId();
            if (!RsDelegateCompositeCallbackManager::GetInstance().GetInfo(consumer, nodeId_, clientPid_)) {
                ROSEN_LOGE("DelegateModeDebugTag: RsDelegateProducerListener init fail, surfaceName=%{public}s",
                    consumer->GetName().c_str());
            }
        }
    }

    GSError OnBufferReleasedWithSequenceAndFence(uint32_t sequence, const sptr<SyncFence>& fence) override
    {
        RS_TRACE_NAME_FMT("RsDelegate OnBufferReleasedWithSequenceAndFence: sequence=%u", sequence);
        RsDelegateCompositeCallbackManager::GetInstance().AddBufferReleaseInfo(
            nodeId_, surfaceId_, sequence, fence, clientPid_);
        return GSERROR_OK;
    }

    GSError OnBufferReleased() override
    {
        ROSEN_LOGE("DelegateModeDebugTag: RsDelegateProducerListener OnBufferReleased fail");
        return SURFACE_ERROR_NOT_SUPPORT;
    }

    GSError OnBufferReleasedWithFence(const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence) override
    {
        ROSEN_LOGE("DelegateModeDebugTag: RsDelegateProducerListener OnBufferReleasedWithFence fail");
        return SURFACE_ERROR_NOT_SUPPORT;
    }

    GSError OnPropertyChange(const SurfaceProperty& property) override
    {
        ROSEN_LOGE("DelegateModeDebugTag: RsDelegateProducerListener OnPropertyChange fail");
        return SURFACE_ERROR_NOT_SUPPORT;
    }

    void ResetReleaseFunc() override
    {
        ROSEN_LOGE("DelegateModeDebugTag: RsDelegateProducerListener ResetReleaseFunc fail");
    }
private:
    NodeId nodeId_ = 0;
    uint64_t surfaceId_ = 0;
    pid_t clientPid_ = 0;
};

void RsDelegateCompositeCallbackManager::BufferDestructorCallback(uint64_t bufferId)
{
    RsDelegateCompositeCallbackManager::GetInstance().ProcessBufferReleaseInfo(bufferId);
}

void RsDelegateCompositeCallbackManager::OnCleanCacheForBufferInfoMap(
    std::vector<CleanCacheBufferInfo> &infos, NodeId nodeId, uint64_t queueId)
{
    DFX_LOGD(g_enableDfx, "RsDelegateCompositeCleanCacheDfx: OnCleanCache in, infos size=%{public}u",
        static_cast<uint32_t>(infos.size()));

    RS_TRACE_NAME("RsDelegateCompositeCallbackManager OnCleanCacheForBufferInfoMap begin");
    pid_t clientPid = ExtractPid(nodeId);
    for (auto& info : infos) {
        if (!info.buffer) {
            continue;
        }
        if (!info.isAcquired) {
            DFX_LOGD(g_enableDfx, "RsDelegateCompositeCleanCacheDfx: AddBufferReleaseInfo(1), bufferSeqnum=%{public}u",
                info.buffer->GetSeqNum());
            RsDelegateCompositeCallbackManager::GetInstance().AddBufferReleaseInfo(nodeId, queueId,
                info.buffer->GetSeqNum(), info.fence, clientPid);
            continue;
        }
        info.buffer->RegisterBufferDestructorCallBack(&RsDelegateCompositeCallbackManager::BufferDestructorCallback);
        DFX_LOGD(g_enableDfx, "RsDelegateCompositeCleanCacheDfx: PrepareBufferReleaseInfo, bufferSeqnum=%{public}u",
            info.buffer->GetSeqNum());
        RsDelegateCompositeCallbackManager::GetInstance().PrepareBufferReleaseInfo(nodeId, queueId,
            info.buffer, info.fence, clientPid);
    }
    RsDelegateCompositeCallbackManager::GetInstance().NotifyCurrentSurfaceNodeBufferReleaseCallback();
}

RsDelegateCompositeCallbackManager& RsDelegateCompositeCallbackManager::GetInstance()
{
    static RsDelegateCompositeCallbackManager instance;
    return instance;
}

void RsDelegateCompositeCallbackManager::UnRegisterSurfaceTransactionListener(uint64_t listenerId)
{
    std::lock_guard<std::mutex> lock(surfaceTransactionMapMutex_);
    pid_t pid = GenePidBySeqNum(listenerId);
    auto iter = surfaceTransactionListenerMap_.find(pid);
    if (iter != surfaceTransactionListenerMap_.end()) {
        iter->second.erase(listenerId);
        RS_TRACE_NAME_FMT("UnRegisterSurfaceTransactionListener, pid=%u, id=%llu, size=%u",
            pid, listenerId, surfaceTransactionListenerMap_.size());
    }
}

void RsDelegateCompositeCallbackManager::RegisterSurfaceTransactionListener(
    sptr<RSISurfaceTransactionListener> listener, uint64_t listenerId, pid_t pid, pid_t tid)
{
    std::lock_guard<std::mutex> lock(surfaceTransactionMapMutex_);
    pid_t pidTmp = GenePidBySeqNum(listenerId);
    auto iter = surfaceTransactionListenerMap_.find(pidTmp);
    if (iter != surfaceTransactionListenerMap_.end()) {
        if (iter->second.size() >= MAX_MAP_SIZE) {
            return;
        }
        iter->second[listenerId] = listener;
    } else {
        if (surfaceTransactionListenerMap_.size() >= MAX_MAP_SIZE) {
            return;
        }
        surfaceTransactionListenerMap_[pidTmp][listenerId] = listener;
    }
    RS_TRACE_NAME_FMT("RegisterSurfaceTransactionListener, pidTmp=%u, id=%llu, size=%u",
        pidTmp, listenerId, surfaceTransactionListenerMap_.size());

    RS_TRACE_NAME_FMT("RegisterSurfaceTransactionListener: AddWebProxyComposerCmdInfo size=%u",
        surfaceTransactionIdentityInfoMap_.size());
    auto iter1 = surfaceTransactionIdentityInfoMap_.find(pid);
    if (iter1 == surfaceTransactionIdentityInfoMap_.end()) {
        if (surfaceTransactionIdentityInfoMap_.size() >= MAX_MAP_SIZE) {
            return;
        }
        RS_TRACE_NAME_FMT("pid=%u, tid=%u", pid, tid);
        surfaceTransactionIdentityInfoMap_[pid] = tid;
    }
}

void RsDelegateCompositeCallbackManager::NotifySurfaceTransactionListener(uint64_t timestamp)
{
    std::lock_guard<std::mutex> lock(surfaceTransactionMapMutex_);
    std::map<uint64_t, std::queue<uint64_t>> surfaceTransactionCmdInfoMap;
    GetSurfaceTransactionCmdInfoLocked(surfaceTransactionCmdInfoMap);
    for (auto& item : surfaceTransactionCmdInfoMap) {
        pid_t pid = GenePidBySeqNum(item.first);
        auto iter1 = surfaceTransactionListenerMap_.find(pid);
        if (iter1 != surfaceTransactionListenerMap_.end()) {
            auto iter2 = iter1->second.find(item.first);
            if (iter2 != iter1->second.end()) {
                iter2->second->OnComplete(timestamp, item.first, item.second);
            }
        }
    }
}

bool RsDelegateCompositeCallbackManager::RegisterReleaseListener(sptr<IConsumerSurface> consumer)
{
    if (!consumer || !consumer->GetProducer()) {
        return false;
    }

    RS_TRACE_NAME_FMT("RegisterReleaseListener: surfaceId=%llu", consumer->GetUniqueId());
    auto producer = consumer->GetProducer();
    GSError err = producer->Connect();
    if (err != GSERROR_OK) {
        ROSEN_LOGE("DelegateModeDebugTag: RegisterReleaseListener fail: Connect err=%{public}u", err);
    }
    sptr<IProducerListener> listener = new RsDelegateProducerListener(consumer);
    err = producer->RegisterReleaseListener(listener, true);
    if (err != GSERROR_OK) {
        ROSEN_LOGE("DelegateModeDebugTag: RegisterReleaseListener fail: RegisterReleaseListener err=%{public}u", err);
        return false;
    }
    return true;
}

bool RsDelegateCompositeCallbackManager::SetInfo(sptr<IConsumerSurface> cSurface, uint64_t node, pid_t pid)
{
    if (!cSurface) {
        ROSEN_LOGE("DelegateModeDebugTag: SetInfo fail, invalid cSurface");
        return false;
    }

    RS_TRACE_NAME_FMT("SetInfo: surfaceId=%llu, node=%llu, pid=%u", cSurface->GetUniqueId(), node, pid);
    cSurface->SetQueueSize(BUFFER_QUEUE_SIZE);
    cSurface->SetUserData(CLIENT_PID, std::to_string(pid));
    cSurface->SetUserData(NODE_ID, std::to_string(node));
    return true;
}

bool RsDelegateCompositeCallbackManager::GetInfo(sptr<IConsumerSurface> cSurface, uint64_t& nodeId, pid_t& pid)
{
    if (!cSurface) {
        ROSEN_LOGE("DelegateModeDebugTag: GetInfo fail, invalid cSurface");
        return false;
    }
    std::string pidTmp = cSurface->GetUserData(CLIENT_PID);
    std::string nodeIdTmp = cSurface->GetUserData(NODE_ID);
    if (pidTmp.empty() || nodeIdTmp.empty()) {
        return false;
    }
    auto resultNodeId = std::from_chars(nodeIdTmp.data(), nodeIdTmp.data() + nodeIdTmp.size(), nodeId);
    auto resultPid = std::from_chars(pidTmp.data(), pidTmp.data() + pidTmp.size(), pid);
    if (resultNodeId.ec != std::errc() || resultPid.ec != std::errc()) {
        ROSEN_LOGE("DelegateModeDebugTag: GetInfo parse fail, pid=%{public}s, nodeId=%{public}s", pidTmp.c_str(),
            nodeIdTmp.c_str());
        return false;
    }
    return true;
}

void RsDelegateCompositeCallbackManager::AddBufferReleaseInfo(NodeId nodeId, uint64_t surfaceId,
    uint32_t bufferSeqNum, sptr<SyncFence> releaseFence, pid_t clientPid)
{
    RS_TRACE_NAME_FMT("AddBufferReleaseInfo in: nodeId=%llu, surfaceId=%llu, clientPid=%u, bufferSeqNum=%u",
        nodeId, surfaceId, clientPid, bufferSeqNum);
    OnCompletedRet onCompletedRet = {
        .nodeId = nodeId,
        .clientPid = clientPid,
        .queueId = surfaceId,
#ifdef ROSEN_OHOS
        .releaseFence = releaseFence,
#endif
        .bufferSeqNum = bufferSeqNum,
    };
    AddBufferReleaseInfoInner(clientPid, onCompletedRet);
}

bool RsDelegateCompositeCallbackManager::ProcessBufferReleaseInfo(uint64_t bufferId)
{
    OnCompletedRet ret;
    {
        std::lock_guard<std::mutex> lock(surfaceNodeBufferReleaseCallbackMapMutex_);
        auto iter = currentSurfaceNodeBufferReleaseInfoMapTmp_.find(bufferId);
        if (iter == currentSurfaceNodeBufferReleaseInfoMapTmp_.end()) {
            return false;
        }
        ret = iter->second;
        currentSurfaceNodeBufferReleaseInfoMapTmp_.erase(iter);
    }
    DFX_LOGD(g_enableDfx,
        "RsDelegateCompositeCleanCacheDfx: ProcessBufferReleaseInfo, bufferSeqnum=%{public}u", ret.bufferSeqNum);
    AddBufferReleaseInfoInner(ret.clientPid, ret);
    NotifyCurrentSurfaceNodeBufferReleaseCallback();
    return true;
}

bool RsDelegateCompositeCallbackManager::PrepareBufferReleaseInfo(NodeId nodeId, uint64_t surfaceId,
    sptr<SurfaceBuffer> buffer, sptr<SyncFence> releaseFence, pid_t clientPid)
{
    std::lock_guard<std::mutex> lock(surfaceNodeBufferReleaseCallbackMapMutex_);
    if (currentSurfaceNodeBufferReleaseInfoMapTmp_.size() >= MAX_MAP_SIZE) {
        ROSEN_LOGE("PrepareBufferReleaseInfo: currentSurfaceNodeBufferReleaseInfoMapTmp_ size exceed limit");
        return false;
    }
    if (!buffer) {
        ROSEN_LOGE("PrepareBufferReleaseInfo: buffer is null");
        return false;
    }
    uint64_t bufferId = buffer->GetBufferId();
    auto iter = currentSurfaceNodeBufferReleaseInfoMapTmp_.find(bufferId);
    if (iter != currentSurfaceNodeBufferReleaseInfoMapTmp_.end()) {
        ROSEN_LOGE(
            "PrepareBufferReleaseInfo: bufferId=%{public}" PRIu64
            " already exist in currentSurfaceNodeBufferReleaseInfoMapTmp_", bufferId);
        return false;
    }
    OnCompletedRet onCompletedRet = {
        .nodeId = nodeId,
        .clientPid = clientPid,
        .queueId = surfaceId,
#ifdef ROSEN_OHOS
        .releaseFence = releaseFence,
#endif
        .bufferSeqNum = buffer->GetSeqNum(),
    };
    currentSurfaceNodeBufferReleaseInfoMapTmp_[bufferId] = onCompletedRet;
    return true;
}

void RsDelegateCompositeCallbackManager::AddBufferReleaseInfo(sptr<SurfaceBuffer> buffer, sptr<SyncFence> releaseFence,
    sptr<IConsumerSurface> cSurface)
{
    RS_TRACE_NAME("RsDelegateCompositeCallbackManager AddBufferReleaseInfo by cSurface begin");
    if (!buffer || !releaseFence || !cSurface) {
        RS_TRACE_NAME_FMT("AddBufferReleaseInfo: invalid param, buffer:%d, releaseFence=%d, cSurface=%d",
            buffer != nullptr, releaseFence != nullptr, cSurface != nullptr);
        return;
    }
    pid_t clientPid = 0;
    uint64_t nodeId = 0;
    if (!GetInfo(cSurface, nodeId, clientPid)) {
        RS_TRACE_NAME_FMT("AddBufferReleaseInfo: GetInfo failed for cSurface=%s", cSurface->GetName().c_str());
        return;
    }
    AddBufferReleaseInfo(nodeId, cSurface->GetUniqueId(), buffer->GetSeqNum(), releaseFence, clientPid);
}

void RsDelegateCompositeCallbackManager::AddBufferReleaseInfoInner(pid_t pid, OnCompletedRet& ret)
{
    std::lock_guard<std::mutex> lock(surfaceNodeBufferReleaseCallbackMapMutex_);
    RS_TRACE_NAME_FMT("AddBufferReleaseInfo:%d, CompletedCallBackRetMap size=%u, info: %d %llu %u",
        pid, currentSurfaceNodeBufferReleaseInfoMap_.size(), ret.clientPid, ret.queueId, ret.bufferSeqNum);
    auto iter = currentSurfaceNodeBufferReleaseInfoMap_.find(pid);
    if (iter != currentSurfaceNodeBufferReleaseInfoMap_.end()) {
        iter->second.push(std::move(ret));
        return;
    }
    std::queue<OnCompletedRet> newqueue;
    newqueue.push(std::move(ret));
    if (currentSurfaceNodeBufferReleaseInfoMap_.size() >= MAX_MAP_SIZE) {
        ROSEN_LOGE("AddBufferReleaseInfoInner: currentSurfaceNodeBufferReleaseInfoMap_ size exceed limit");
        return;
    }
    currentSurfaceNodeBufferReleaseInfoMap_[pid] = std::move(newqueue);
}

void RsDelegateCompositeCallbackManager::GetBufferReleaseInfo(std::map<pid_t, std::queue<OnCompletedRet>>& outmap)
{
    std::lock_guard<std::mutex> lock(surfaceNodeBufferReleaseCallbackMapMutex_);
    RS_TRACE_NAME_FMT("GetBufferReleaseInfo CompletedCallBackRetMap size=%u",
        currentSurfaceNodeBufferReleaseInfoMap_.size());
    currentSurfaceNodeBufferReleaseInfoMap_.swap(outmap);
}

void RsDelegateCompositeCallbackManager::AddSurfaceTransactionCmdInfo(
    const uint64_t &srcId, const uint64_t &seqNum, const pid_t& pid, const pid_t& tid)
{
    std::lock_guard<std::mutex> lock(surfaceTransactionMapMutex_);
    {
        RS_OPTIONAL_TRACE_FMT("AddSurfaceTransactionCmdInfo: srcId=%llu, seqNum=%llu, pid=%u, tid=%u",
            srcId, seqNum, pid, tid);
        auto iter = surfaceTransactionCmdMap_.find(srcId);
        if (iter != surfaceTransactionCmdMap_.end()) {
            iter->second.push(seqNum);
        } else {
            std::queue<uint64_t> seqNums;
            seqNums.push(seqNum);
            if (surfaceTransactionCmdMap_.size() >= MAX_MAP_SIZE) {
                ROSEN_LOGE("AddSurfaceTransactionCmdInfo: surfaceTransactionCmdMap_ size exceed limit");
                return;
            }
            surfaceTransactionCmdMap_[srcId] = seqNums;
        }
    }

    {
        RS_OPTIONAL_TRACE_NAME_FMT("AddSurfaceTransactionCmdInfo: AddWebProxyComposerCmdInfo size=%u, pid=%u, tid=%u",
            surfaceTransactionIdentityInfoMap_.size(), pid, tid);
        auto iter = surfaceTransactionIdentityInfoMap_.find(pid);
        if (iter == surfaceTransactionIdentityInfoMap_.end()) {
            if (surfaceTransactionIdentityInfoMap_.size() >= MAX_MAP_SIZE) {
                ROSEN_LOGE("AddSurfaceTransactionCmdInfo: surfaceTransactionIdentityInfoMap_ size exceed limit");
                return;
            }
            surfaceTransactionIdentityInfoMap_[pid] = tid;
        }
    }
}

void RsDelegateCompositeCallbackManager::GetSurfaceTransactionCmdInfoLocked(
    std::map<uint64_t, std::queue<uint64_t>>& outmap)
{
    RS_TRACE_NAME_FMT("surfaceTransactionCmdMap_ size=%u", surfaceTransactionCmdMap_.size());
    surfaceTransactionCmdMap_.swap(outmap);
}

bool RsDelegateCompositeCallbackManager::ProcessDelegateCompositeCommand(RSContext& context)
{
    std::unordered_map<pid_t, singlePidCmdMap> allPidProcessCmdMap;
    bool ret = false;
    {
        std::lock_guard<std::mutex> lock(cmdMapMutex_);
        if (allPidCmdMap_.empty()) {
            return false;
        }
        for (auto it = allPidCmdMap_.begin(); it != allPidCmdMap_.end();) {
            auto &sendingPid = it->first;
            auto &sendingPidCmdMap = it->second;
            if (sendingPidCmdMap.empty()) {
                it = allPidCmdMap_.erase(it);
                continue;
            }

            auto iter = sendingPidCmdMap.begin();
            if (iter != sendingPidCmdMap.end()) {
                allPidProcessCmdMap[sendingPid][iter->first] = std::move(iter->second);
                sendingPidCmdMap.erase(iter);
            }
            if (sendingPidCmdMap.empty()) {
                it = allPidCmdMap_.erase(it);
                continue;
            }
            it++;
        }
        if (!allPidCmdMap_.empty()) {
            RS_TRACE_NAME("ProcessDelegateCompositeCommand: RequestNextVsync");
            ret = true;
        }
    }
    RS_TRACE_NAME_FMT("ProcessDelegateCompositeCommand: allPidProcessCmdMap size=%u", allPidProcessCmdMap.size());
    for (auto it1 = allPidProcessCmdMap.begin(); it1 != allPidProcessCmdMap.end(); ++it1) {
        auto &sendingPid = it1->first;
        auto &sendingPidCmdMap = it1->second;
        for (auto it2 = sendingPidCmdMap.begin(); it2 != sendingPidCmdMap.end(); ++it2) {
            auto &index = it2->first;
            auto &cmdqueue = it2->second;
            RS_TRACE_NAME_FMT("ProcessDelegateCompositeCommand: [%d, %" PRIu64 "], cmdNums=%u",
                sendingPid, index, cmdqueue.size());
            ProcessEachCommandQueue(cmdqueue, context);
        }
    }
    return ret;
}

void RsDelegateCompositeCallbackManager::ProcessEachCommandQueue(std::queue<std::unique_ptr<RSCommand>> &cmdqueue,
    RSContext& context)
{
    while (!cmdqueue.empty()) {
        std::unique_ptr<RSCommand> cmd = std::move(cmdqueue.front());
        cmdqueue.pop();
        if (cmd) {
            cmd->Process(context);
        }
    }
}

void RsDelegateCompositeCallbackManager::PrepareDelegateCompositeCommand(
    std::unique_ptr<RSTransactionData>& transactionData)
{
    if (!transactionData ||
        !CheckSurfaceTransactionIdentity(transactionData->GetSendingPid(), transactionData->GetSendingTid())) {
        ROSEN_LOGE("ProcessDelegateCompositeCommand transactionData is null or CheckSurfaceTransactionIdentity failed");
        return;
    }

    RS_OPTIONAL_TRACE_NAME("ProcessDelegateCompositeCommand begin");
    auto& processPayload = transactionData->GetPayload();
    std::queue<std::unique_ptr<RSCommand>> cmdQueue;
    for (auto it = processPayload.begin(); it != processPayload.end();) {
        auto& command = std::get<2>(*it);
        if (!command) {
            continue;
        }
        command->SetCallingPidValid(true);
        cmdQueue.push(std::move(command));
        it = processPayload.erase(it);
        if (cmdQueue.size() >= MAX_MAP_SIZE) {
            ROSEN_LOGE("ProcessDelegateCompositeCommand: cmdQueue size exceed limit");
            break;
        }
    }
    {
        std::lock_guard<std::mutex> lock(cmdMapMutex_);
        pid_t sendingPid = transactionData->GetSendingPid();
        auto it = allPidCmdMap_.find(sendingPid);
        if (it == allPidCmdMap_.end()) {
            if (allPidCmdMap_.size() >= MAX_MAP_SIZE) {
                return;
            }
            singlePidCmdMap cmdMap;
            cmdMap[transactionData->GetIndex()] = std::move(cmdQueue);
            allPidCmdMap_[sendingPid] = std::move(cmdMap);
            return;
        }

        if (it->second.size() >= MAX_MAP_SIZE) {
            ROSEN_LOGE("ProcessDelegateCompositeCommand: cmdQueue size exceed limit before swap");
            return;
        }
        it->second[transactionData->GetIndex()] = std::move(cmdQueue);
    }
}

bool RsDelegateCompositeCallbackManager::CheckSurfaceTransactionIdentity(pid_t pid, pid_t tid)
{
    bool ret = false;
    std::lock_guard<std::mutex> lock(surfaceTransactionMapMutex_);
    auto iter = surfaceTransactionIdentityInfoMap_.find(pid);
    if (iter == surfaceTransactionIdentityInfoMap_.end()) {
        RS_OPTIONAL_TRACE_FMT("CheckSurfaceTransactionIdentity: not find pid=%u, tid=%u, ret=%d", pid, tid, ret);
        return ret;
    }
    ret = iter->second == tid;
    RS_OPTIONAL_TRACE_FMT("CheckSurfaceTransactionIdentity: pid=%u, tid=%u, ret=%d", pid, tid, ret);
    return ret;
}

void RsDelegateCompositeCallbackManager::RemoveAllListenerbyPid(pid_t pid)
{
    RS_TRACE_NAME_FMT("RemoveAllListenerbyPid: pid=%u", pid);
    {
        std::lock_guard<std::mutex> lock(surfaceTransactionMapMutex_);
        auto iter1 = surfaceTransactionIdentityInfoMap_.find(pid);
        if (iter1 != surfaceTransactionIdentityInfoMap_.end()) {
            surfaceTransactionIdentityInfoMap_.erase(pid);
            RS_TRACE_NAME_FMT(
                "RemoveAllListenerbyPid: erase surfaceTransactionIdentityInfoMap_ for pid=%u, size=%u",
                pid, surfaceTransactionIdentityInfoMap_.size());
        }
        auto iter2 = surfaceTransactionListenerMap_.find(pid);
        if (iter2 != surfaceTransactionListenerMap_.end()) {
            surfaceTransactionListenerMap_.erase(pid);
            RS_TRACE_NAME_FMT(
                "RemoveAllListenerbyPid: erase surfaceTransactionListenerMap_ for pid=%u, size=%u",
                pid, surfaceTransactionListenerMap_.size());
        }
    }
    UnRegisterSurfaceNodeBufferReleaseListener(pid);
}

bool RsDelegateCompositeCallbackManager::CheckIsDelegateCompositeOnly(
    std::shared_ptr<TransactionDataMap> transactionDataEffective)
{
    if (!transactionDataEffective || transactionDataEffective->size() != 1) {
        return false;
    }
    pid_t pid = transactionDataEffective->begin()->first;
    std::lock_guard<std::mutex> lock(surfaceTransactionMapMutex_);
    auto iter = surfaceTransactionIdentityInfoMap_.find(pid);
    if (iter == surfaceTransactionIdentityInfoMap_.end()) {
        return false;
    }
    return true;
}

void RsDelegateCompositeCallbackManager::UnRegisterSurfaceNodeBufferReleaseListener(pid_t pid)
{
    RS_TRACE_NAME_FMT("UnRegisterSurfaceNodeBufferReleaseListener: pid=%u", pid);
    std::lock_guard<std::mutex> lock(surfaceNodeBufferReleaseCallbackMapMutex_);
    auto iter = surfaceNodeBufferReleaseCallbackMap_.find(pid);
    if (iter == surfaceNodeBufferReleaseCallbackMap_.end()) {
        RS_TRACE_NAME_FMT("UnRegisterSurfaceNodeBufferReleaseListener fail, no listener: pid=%d, size=%u",
            pid, surfaceNodeBufferReleaseCallbackMap_.size());
        return;
    }

    surfaceNodeBufferReleaseCallbackMap_.erase(pid);
    RS_TRACE_NAME_FMT("UnRegisterSurfaceNodeBufferReleaseListener success pid=%u, size=%u",
        pid, surfaceNodeBufferReleaseCallbackMap_.size());
}

void RsDelegateCompositeCallbackManager::RegisterSurfaceNodeBufferReleaseListener(
    pid_t pid, sptr<RSISurfaceNodeBufferReleaseCallback> listener)
{
    std::lock_guard<std::mutex> lock(surfaceNodeBufferReleaseCallbackMapMutex_);
    auto iter = surfaceNodeBufferReleaseCallbackMap_.find(pid);
    if (iter != surfaceNodeBufferReleaseCallbackMap_.end()) {
        return;
    }
    if (surfaceNodeBufferReleaseCallbackMap_.size() >= MAX_MAP_SIZE) {
        return;
    }
    surfaceNodeBufferReleaseCallbackMap_[pid] = listener;
    RS_TRACE_NAME_FMT("RegisterSurfaceNodeBufferReleaseListener: pid=%u, size=%u", pid,
        surfaceNodeBufferReleaseCallbackMap_.size());
}

void RsDelegateCompositeCallbackManager::NotifyCurrentSurfaceNodeBufferReleaseCallback()
{
    std::map<pid_t, std::queue<OnCompletedRet>> bufferReleaseInfoMap;
    GetBufferReleaseInfo(bufferReleaseInfoMap);

    std::lock_guard<std::mutex> lock(surfaceNodeBufferReleaseCallbackMapMutex_);
    for (auto& item : bufferReleaseInfoMap) {
        auto iter = surfaceNodeBufferReleaseCallbackMap_.find(item.first);
        if (iter == surfaceNodeBufferReleaseCallbackMap_.end()) {
            continue;
        }
        RS_TRACE_NAME_FMT("OnBufferComplete(RsDelegateCompositeCallbackManager), pid=%d, size=%u",
            item.first, item.second.size());
        iter->second->OnBufferComplete(item.second);
    }
    bufferReleaseInfoMap.clear();
}

void RsDelegateCompositeCallbackManager::DumpInfo(std::string &dumpString)
{
    dumpString.append("\n-------RsDelegateCompositeCallbackManager::DumpInfo begin----------\n");
    {
        std::lock_guard<std::mutex> lock(cmdMapMutex_);
        dumpString.append("allPidCmdMap size:" + std::to_string(allPidCmdMap_.size()) + "\n");
        for (auto it = allPidCmdMap_.begin(); it != allPidCmdMap_.end(); it++) {
            auto &sendingPid = it->first;
            auto &sendingPidCmdMap = it->second;
            dumpString.append("----sendingPid=" + std::to_string(sendingPid) +
                ", CmdMapSize=" + std::to_string(sendingPidCmdMap.size()) + "\n");
        }
    }
    {
        std::lock_guard<std::mutex> lock(surfaceTransactionMapMutex_);
        dumpString.append("surfaceTransactionListener size:" +
            std::to_string(surfaceTransactionListenerMap_.size()) + "\n");
        dumpString.append("surfaceTransactionIdentityInfoMap size:" +
            std::to_string(surfaceTransactionIdentityInfoMap_.size()) + "\n");
        dumpString.append("surfaceTransactionCmdMap size:" +
            std::to_string(surfaceTransactionCmdMap_.size()) + "\n");
    }
    {
        std::lock_guard<std::mutex> lock(surfaceNodeBufferReleaseCallbackMapMutex_);
        dumpString.append("surfaceNodeBufferReleaseCallbackMap size:" +
            std::to_string(surfaceNodeBufferReleaseCallbackMap_.size()) + "\n");
        dumpString.append("currentSurfaceNodeBufferReleaseInfoMap size:" +
            std::to_string(currentSurfaceNodeBufferReleaseInfoMap_.size()) + "\n");
    }
    DumpTotleInfo(dumpString);
    dumpString.append("\n-------RsDelegateCompositeCallbackManager::DumpInfo end----------\n");
}

void RsDelegateCompositeCallbackManager::DumpTotleInfo(std::string &dumpString)
{
    dumpString.append("\n Totle Info:\n");
    {
        std::lock_guard<std::mutex> lock(surfaceTransactionMapMutex_);
        dumpString.append("\n surfaceTransactionListener:\n");
        for (auto iter = surfaceTransactionListenerMap_.begin();
            iter != surfaceTransactionListenerMap_.end(); ++iter) {
            auto subMap = iter->second;
            dumpString.append("info:pid=" + std::to_string(iter->first) +
                ", size=" + std::to_string(subMap.size()) + "\n");
            for (auto itersubMap = subMap.begin(); itersubMap != subMap.end(); ++itersubMap) {
                dumpString.append("----listenerId=" + std::to_string(itersubMap->first) + "\n");
            }
        }
        dumpString.append("\n surfaceTransactionIdentityInfoMap:\n");
        for (auto iter = surfaceTransactionIdentityInfoMap_.begin();
            iter != surfaceTransactionIdentityInfoMap_.end(); ++iter) {
            dumpString.append("info: pid=" + std::to_string(iter->first) +
                ", tid=" + std::to_string(iter->second) + "\n");
        }
        dumpString.append("\n surfaceTransactionCmdMap info:\n");
        for (auto iter = surfaceTransactionCmdMap_.begin(); iter != surfaceTransactionCmdMap_.end(); ++iter) {
            dumpString.append(" info: ListenerId=" + std::to_string(iter->first) +
                ", queueSize=" + std::to_string(iter->second.size()) + "\n");
        }
    }

    {
        std::lock_guard<std::mutex> lock(surfaceNodeBufferReleaseCallbackMapMutex_);
        dumpString.append("\n surfaceNodeBufferReleaseCallBackMap:\n");
        for (auto iter = surfaceNodeBufferReleaseCallbackMap_.begin();
            iter != surfaceNodeBufferReleaseCallbackMap_.end(); ++iter) {
            dumpString.append("info:pid=" + std::to_string(iter->first) + "\n");
        }
        dumpString.append("\n currentSurfaceNodeBufferReleaseInfoMap:\n");
        for (auto iter = currentSurfaceNodeBufferReleaseInfoMap_.begin();
            iter != currentSurfaceNodeBufferReleaseInfoMap_.end(); ++iter) {
            dumpString.append("info: pid=" + std::to_string(iter->first) +
                ", queueSize=" + std::to_string(iter->second.size()) + "\n");
        }
    }
}
} // namespace Rosen
} // namespace OHOS
#endif
