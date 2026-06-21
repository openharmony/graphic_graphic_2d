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
#ifndef RS_DELEGATE_COMPOSITE_CALLBACK_MANAGER
#define RS_DELEGATE_COMPOSITE_CALLBACK_MANAGER
#ifndef ROSEN_CROSS_PLATFORM
#include <map>
#include <queue>
#include "ipc_callbacks/rs_delegate_composite_callback.h"
#include <surface.h>
#include "transaction/rs_transaction_data.h"
#include "pipeline/rs_context.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RsDelegateCompositeCallbackManager {
public:
    static RsDelegateCompositeCallbackManager& GetInstance();

    bool SetInfo(sptr<IConsumerSurface> cSurface, uint64_t nodeId, pid_t pid);
    bool GetInfo(sptr<IConsumerSurface> cSurface, uint64_t& nodeId, pid_t& pid);

    void AddBufferReleaseInfo(sptr<SurfaceBuffer> buffer, sptr<SyncFence> releaseFence,
        sptr<IConsumerSurface> cSurface);
    void AddBufferReleaseInfo(NodeId nodeId, uint64_t surfaceId, uint32_t bufferSeqNum,
        sptr<SyncFence> releaseFence, pid_t clientPid);

    void AddSurfaceTransactionCmdInfo(const uint64_t &srcId, const uint64_t &seqNum,
        const pid_t& pid, const pid_t& tid);

    void RegisterSurfaceTransactionListener(sptr<RSISurfaceTransactionListener> listener,
        uint64_t listenerId, pid_t pid, pid_t tid);
    void UnRegisterSurfaceTransactionListener(uint64_t listenerId);
    void NotifySurfaceTransactionListener(uint64_t timestamp);

    void RegisterSurfaceNodeBufferReleaseListener(pid_t pid, sptr<RSISurfaceNodeBufferReleaseCallback> listener);
    void UnRegisterSurfaceNodeBufferReleaseListener(pid_t pid);

    void NotifyCurrentSurfaceNodeBufferReleaseCallback();

    bool CheckSurfaceTransactionIdentity(pid_t pid, pid_t tid);
    bool CheckIsDelegateCompositeOnly(std::shared_ptr<TransactionDataMap> transactionDataEffective);
    void RemoveAllListenerbyPid(pid_t pid);
    void PrepareDelegateCompositeCommand(std::unique_ptr<RSTransactionData>& transactionData);
    bool ProcessDelegateCompositeCommand(RSContext& context);

    bool RegisterReleaseListener(sptr<IConsumerSurface> cSurface);
    void DumpInfo(std::string &dumpString);

    static void BufferDestructorCallback(uint64_t bufferId);
    bool PrepareBufferReleaseInfo(NodeId nodeId, uint64_t surfaceId,
        sptr<SurfaceBuffer> buffer, sptr<SyncFence> releaseFence, pid_t clientPid);
    bool ProcessBufferReleaseInfo(uint64_t bufferId);
    void OnCleanCacheForBufferInfoMap(std::vector<CleanCacheBufferInfo> &infos, NodeId nodeId, uint64_t queueId);
    void ProcessEachCommandQueue(std::queue<std::unique_ptr<RSCommand>> &cmdqueue, RSContext& context);
private:
    void AddBufferReleaseInfoInner(pid_t pid, OnCompletedRet& ret);
    void GetBufferReleaseInfo(std::map<pid_t, std::queue<OnCompletedRet>>& outmap);
    void GetSurfaceTransactionCmdInfoLocked(std::map<uint64_t, std::queue<uint64_t>> &targetListenerMap);
    void DumpTotleInfo(std::string &dumpString);
    pid_t webProxyComposerTid_ = 0;

    std::mutex surfaceNodeBufferReleaseCallbackMapMutex_;
    std::map<pid_t, sptr<RSISurfaceNodeBufferReleaseCallback>> surfaceNodeBufferReleaseCallbackMap_;
    std::map<pid_t, std::queue<OnCompletedRet>> currentSurfaceNodeBufferReleaseInfoMap_;
    std::map<uint64_t, OnCompletedRet> currentSurfaceNodeBufferReleaseInfoMapTmp_;

    std::mutex surfaceTransactionMapMutex_;
    std::map<uint64_t, std::queue<uint64_t>> surfaceTransactionCmdMap_;
    std::map<pid_t, pid_t> surfaceTransactionIdentityInfoMap_; // pid and tid
    std::map<pid_t, std::map<uint64_t, sptr<RSISurfaceTransactionListener>>> surfaceTransactionListenerMap_;
    std::mutex cmdMapMutex_;
    using singlePidCmdMap =  std::map<uint64_t, std::queue<std::unique_ptr<RSCommand>>>;
    std::unordered_map<pid_t, singlePidCmdMap> allPidCmdMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_CROSS_PLATFORM
#endif