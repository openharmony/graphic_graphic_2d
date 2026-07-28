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
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
constexpr uint64_t MAX_TRANSACTION_DATA_CALLBACKS = 65535;
constexpr const char* GRAPHIC_TEST_MODE_TRACE_NAME = "sys.graphic.openTestModeTrace";
bool RSTransactionDataCallbackManager::isDebugEnabled_ = false;
RSTransactionDataCallbackManager& RSTransactionDataCallbackManager::Instance()
{
    static RSTransactionDataCallbackManager mgr;
    return mgr;
}

void RSTransactionDataCallbackManager::RegisterTransactionDataCallback(uint64_t token,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback, pid_t callingPid)
{
    RS_LOGD("RSTransactionDataCallbackManager save data, timeStamp: %{public}"
        PRIu64 " token: %{public}" PRIu64, timeStamp, token);
    if (!PushTransactionDataCallback(token, timeStamp, callback, callingPid)) {
        RS_LOGE("RegisterTransactionDataCallback register callback err");
    }
}

void RSTransactionDataCallbackManager::TriggerTransactionDataCallback(uint64_t token, uint64_t timeStamp,
    pid_t callingPid)
{
    if (auto callback = PopTransactionDataCallback(token, timeStamp, callingPid)) {
        RS_LOGD("RSTransactionDataCallbackManager trigger data, timeStamp: %{public}"
            PRIu64 " token: %{public}" PRIu64, timeStamp, token);
        callback->OnAfterProcess(token, timeStamp);
    } else {
        RS_LOGD("RSTransactionDataCallbackManager trigger callback error, timeStamp: %{public}"
            PRIu64 " token: %{public}" PRIu64, timeStamp, token);
    }
}

bool RSTransactionDataCallbackManager::PushTransactionDataCallback(uint64_t token,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback, pid_t callingPid)
{
    std::lock_guard<std::mutex> lock{ transactionDataCbMutex_ };
    if (transactionDataCallbacks_.size() >= MAX_TRANSACTION_DATA_CALLBACKS) {
        RS_LOGE("RSTransactionDataCallbackManager: transactionDataCallbacks_ has reached maximus size, cannot add new "
                "callback");
        return false;
    }
    if (transactionDataCallbacks_.find(std::make_pair(token, timeStamp)) == std::end(transactionDataCallbacks_)) {
        RS_LOGD("RSTransactionDataCallbackManager push data, timeStamp: %{public}"
            PRIu64 " token: %{public}" PRIu64, timeStamp, token);
        transactionDataCallbacks_.emplace(std::make_pair(token, timeStamp),
            TransactionDataCallbackInfo { callback, callingPid });
        return true;
    }
    return false;
}

sptr<RSITransactionDataCallback> RSTransactionDataCallbackManager::PopTransactionDataCallback(uint64_t token,
    uint64_t timeStamp, pid_t callingPid)
{
    std::lock_guard<std::mutex> lock { transactionDataCbMutex_ };
    auto iter = transactionDataCallbacks_.find(std::make_pair(token, timeStamp));
    if (iter == std::end(transactionDataCallbacks_)) {
        return nullptr;
    }
    // both pids come from trusted binder callingPid; skip the check only when either side is unknown,
    // so that a forged token/pid from another process cannot pop this entry
    if (iter->second.callingPid > 0 && callingPid > 0 && iter->second.callingPid != callingPid) {
        RS_LOGW("RSTransactionDataCallbackManager pop rejected, pid mismatch, timeStamp: %{public}"
            PRIu64 " token: %{public}" PRIu64 " registerPid: %{public}d triggerPid: %{public}d",
            timeStamp, token, static_cast<int>(iter->second.callingPid), static_cast<int>(callingPid));
        return nullptr;
    }
    RS_LOGD("RSTransactionDataCallbackManager pop data, timeStamp: %{public}"
        PRIu64 " token: %{public}" PRIu64, timeStamp, token);
    auto callback = iter->second.callback;
    transactionDataCallbacks_.erase(iter);
    return callback;
}

RSTransactionDataCallbackManager::RSTransactionDataCallbackManager()
{
    isDebugEnabled_ = RSSystemProperties::GetTransactionDataTraceEnabled();
    RSSystemProperties::WatchSystemProperty(
        GRAPHIC_TEST_MODE_TRACE_NAME, TransactionChangedCallback, nullptr);
}

void RSTransactionDataCallbackManager::TransactionChangedCallback(const char* key, const char* value, void* context)
{
    isDebugEnabled_ = RSSystemProperties::GetTransactionDataTraceEnabled();
}

bool RSTransactionDataCallbackManager::GetTransactionDataTestEnabled()
{
    return isDebugEnabled_;
}
} //namespace Rosen
} //namespace OHOS