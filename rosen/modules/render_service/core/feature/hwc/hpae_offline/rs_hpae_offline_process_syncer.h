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

#ifndef RS_HPAE_OFFLINE_PROCESS_SYNC_H
#define RS_HPAE_OFFLINE_PROCESS_SYNC_H

#include <utility>
#include "feature/hwc/hpae_offline/rs_offline_result.h"

namespace OHOS::Rosen {

using offlineTaskId = std::pair<uint64_t, uint64_t>;  // vsyncId, nodeId

class RSB_EXPORT RSHpaeOfflineProcessSyncer final {
public:
    RSHpaeOfflineProcessSyncer() = default;

    std::shared_ptr<ProcessOfflineFuture> RegisterPostedTask(offlineTaskId taskId);
    void MarkTaskDoneAndSetResult(std::shared_ptr<ProcessOfflineFuture> futurePtr,
        const ProcessOfflineResult& processOfflineResult);
    bool WaitForTaskAndGetResult(offlineTaskId taskId, std::chrono::milliseconds timeout,
        ProcessOfflineResult& processOfflineResult);
    int32_t GetResultPoolSize();
    int32_t GetResultPoolSizeByNode(NodeId nodeId);
    void ClearResultPool();
    void SetDirectResult(offlineTaskId taskId, const ProcessOfflineResult& result);

private:
    std::shared_ptr<ProcessOfflineFuture> GetTaskFuture(offlineTaskId taskId);
    std::map<offlineTaskId, std::shared_ptr<ProcessOfflineFuture>> resultPool_;
    std::map<offlineTaskId, ProcessOfflineResult> directResultCache_;
    std::mutex poolMtx_;
};
}
#endif // RS_HPAE_OFFLINE_SYNC_H