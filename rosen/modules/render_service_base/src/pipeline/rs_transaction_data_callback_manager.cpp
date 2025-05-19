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
#include "pipeline/rs_transaction_data_callback_manager.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSTransactionDataCallbackManager& RSTransactionDataCallbackManager::Instance()
{
    static RSTransactionDataCallbackManager mgr;
    return mgr;
}

void RSTransactionDataCallbackManager::RegisterTransactionDataCallback(pid_t pid,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    RS_TRACE_NAME_FMT("789 test 7. manager save data, timeStamp: %"
        PRIu64 " pid: %d", timeStamp, pid);
    RS_LOGD("789 test 7. manager save data, timeStamp: %{public}"
        PRIu64 " pid: %{public}d", timeStamp, pid);
    if (!PushTransactionDataCallback(pid, timeStamp, callback)) {
        RS_LOGE("RegisterTransactionDataCallback register callback err");
    }
}

void RSTransactionDataCallbackManager::TriggerTransactionDataCallback(pid_t pid, uint64_t timeStamp)
{
    if (auto callback = PopTransactionDataCallback(pid, timeStamp)) {
        RS_TRACE_NAME_FMT("789 test 8. manager trigger data, timeStamp: %"
            PRIu64 " pid: %d", timeStamp, pid);
        RS_LOGD("789 test 8. manager trigger data, timeStamp: %{public}"
            PRIu64 " pid: %{public}d", timeStamp, pid);
        callback->OnAfterProcess(pid, timeStamp);
    } else {
        RS_LOGE("RegisterTransactionDataCallback trigger callback err");
    }
}

bool RSTransactionDataCallbackManager::PushTransactionDataCallback(pid_t pid,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    std::lock_guard<std::mutex> lock { transactionDataCbMutex_ };
    if (transactionDataCallbacks_.find(std::make_pair(pid, timeStamp)) == std::end(transactionDataCallbacks_)) {
        RS_TRACE_NAME_FMT("789 test 7x. manager save data, timeStamp: %"
            PRIu64 " pid: %d", timeStamp, pid);
        RS_LOGD("789 test 7x. manager save data, timeStamp: %{public}"
            PRIu64 " pid: %{public}d", timeStamp, pid);
        transactionDataCallbacks_.emplace(std::make_pair(pid, timeStamp), callback);
        return true;
    }
    return false;
}

sptr<RSITransactionDataCallback> RSTransactionDataCallbackManager::PopTransactionDataCallback(pid_t pid,
    uint64_t timeStamp)
{
    std::lock_guard<std::mutex> lock { transactionDataCbMutes_ };
    auto iter = transactionDataCallbacks_.find(std::make_pair(pid, timeStamp));
    if (iter != std::end(transactionDataCallbacks_)) {
        RS_TRACE_NAME_FMT("789 test 8x. manager trigger data, timeStamp: %" PRIu64 " pid: %d", timeStamp, pid);
        RS_LOGD("789 test 8x. manager trigger data, timeStamp: %{public}" PRIu64 " pid: %{public}d", timeStamp, pid);
        auto callback = iter->second;
        transactionDataCallbacks_.erase(iter);
        return callback;
    }
    return nullptr;
}
} //namespace Rosen
} //namespace OHOS