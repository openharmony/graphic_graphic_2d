/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RS_UNMARSHAL_THREAD_H
#define RS_UNMARSHAL_THREAD_H

#include <mutex>
#include <unordered_map>

#include "event_handler.h"
#include "ffrt_inner.h"
#include "message_parcel.h"

#include "memory/rs_memory_flow_control.h"
#include "transaction/rs_ashmem_helper.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS::Rosen {
class RSUnmarshalThread {
public:
    static RSUnmarshalThread& Instance();
    void Start();
    void PostTask(const std::function<void()>& task, const std::string& name = "");
    void RemoveTask(const std::string& name = "");
    void RecvParcel(std::shared_ptr<MessageParcel>& parcel, bool isNonSystemAppCalling = false, pid_t callingPid = 0,
        std::unique_ptr<AshmemFdWorker> ashmemFdWorker = nullptr,
        std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit = nullptr, uint32_t parcelNumber = 0);
    TransactionDataMap GetCachedTransactionData();
    bool CachedTransactionDataEmpty();
    void Wait();

    bool ReportTransactionDataStatistics(pid_t pid, RSTransactionData* transactionData,
        bool isNonSystemAppCalling = false);
    void ClearTransactionDataStatistics();

private:
    RSUnmarshalThread() = default;
    ~RSUnmarshalThread() = default;
    RSUnmarshalThread(const RSUnmarshalThread&);
    RSUnmarshalThread(const RSUnmarshalThread&&);
    RSUnmarshalThread& operator=(const RSUnmarshalThread&);
    RSUnmarshalThread& operator=(const RSUnmarshalThread&&);

    bool IsHaveCmdList(const std::unique_ptr<RSCommand>& cmd) const;
    static constexpr uint32_t MIN_PENDING_REQUEST_SYNC_DATA_SIZE = 32 * 1024;

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;

    std::mutex transactionDataMutex_;
    TransactionDataMap cachedTransactionDataMap_;
    bool willHaveCachedData_ = false;
    int unmarshalTid_ = -1;
    int unmarshalLoad_ = 0;
    std::vector<ffrt::dependence> cachedDeps_;

    std::mutex statisticsMutex_;
    std::unordered_map<pid_t, size_t> transactionDataStatistics_;
};
}
#endif // RS_UNMARSHAL_THREAD_H
