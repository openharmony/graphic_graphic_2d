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
#include "ipc_callbacks/rs_transaction_data_callback.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "transaction/rs_transaction_data_callback_manager.h"

namespace OHOS {
namespace Rosen {
RSTransactionDataCallbackManager& RSTransactionDataCallbackManager::Instance()
{
    static RSTransactionDataCallbackManager mgr;
    return mgr;
}

void RSTransactionDataCallbackManager::RegisterTransactionDataCallback(int32_t pid,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    RS_LOGD("RSTransactionDataCallbackManager save data, timeStamp: %{public}"
        PRIu64 " pid: %{public}d", timeStamp, pid);
    if (!PushTransactionDataCallback(pid, timeStamp, callback)) {
        RS_LOGE("RegisterTransactionDataCallback register callback err");
    }
}

void RSTransactionDataCallbackManager::TriggerTransactionDataCallback(int32_t pid, uint64_t timeStamp)
{
    if (auto callback = PopTransactionDataCallback(pid, timeStamp)) {
        RS_LOGD("RSTransactionDataCallbackManager trigger data, timeStamp: %{public}"
            PRIu64 " pid: %{public}d", timeStamp, pid);
        callback->OnAfterProcess(pid, timeStamp);
    } else {
        RS_LOGD("RSTransactionDataCallbackManager trigger callback error, timeStamp: %{public}"
            PRIu64 " pid: %{public}d", timeStamp, pid);
    }
}

bool RSTransactionDataCallbackManager::PushTransactionDataCallback(int32_t pid,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    std::lock_guard<std::mutex> lock{ transactionDataCbMutex_ };
    if (transactionDataCallbacks_.find(std::make_pair(pid, timeStamp)) == std::end(transactionDataCallbacks_)) {
        RS_LOGD("RSTransactionDataCallbackManager push data, timeStamp: %{public}"
            PRIu64 " pid: %{public}d", timeStamp, pid);
        transactionDataCallbacks_.emplace(std::make_pair(pid, timeStamp), callback);
        return true;
    }
    return false;
}

sptr<RSITransactionDataCallback> RSTransactionDataCallbackManager::PopTransactionDataCallback(int32_t pid,
    uint64_t timeStamp)
{
    std::lock_guard<std::mutex> lock { transactionDataCbMutex_ };
    auto iter = transactionDataCallbacks_.find(std::make_pair(pid, timeStamp));
    if (iter != std::end(transactionDataCallbacks_)) {
        RS_LOGD("RSTransactionDataCallbackManager pop data, timeStamp: %{public}"
            PRIu64 " pid: %{public}d", timeStamp, pid);
        auto callback = iter->second;
        transactionDataCallbacks_.erase(iter);
        return callback;
    }
    return nullptr;
}
} //namespace Rosen
} //namespace OHOS