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

#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_TRANSACTION_DATA_CALLBACK_MANAGER_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_TRANSACTION_DATA_CALLBACK_MANAGER_H

#include <functional>
#include <map>
#include <utility>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

#include "refbase.h"

namespace OHOS {
namespace Rosen {

class RSITransactionDataCallback;

class RSB_EXPORT RSTransactionDataCallbackManager {
public:
    void RegisterTransactionDataCallback(pid_t pid, uint64_t timeStamp,
        sptr<RSITransactionDataCallback> callback);
    void TriggerTransactionDataCallback(pid_t pid, uint64_t timeStamp);

    static RSTransactionDataCallbackManager& Instance();
private:
    RSTransactionDataCallbackManager() = default;
    ~RSTransactionDataCallbackManager() noexcept = default;

    RSTransactionDataCallbackManager(const RSTransactionDataCallbackManager&) = delete;
    RSTransactionDataCallbackManager(RSTransactionDataCallbackManager&&) = delete;
    RSTransactionDataCallbackManager& operator=(const RSTransactionDataCallbackManager&) = delete;
    RSTransactionDataCallbackManager& operator=(RSTransactionDataCallbackManager&&) = delete;

    sptr<RSITransactionDataCallback> PopTransactionDataCallback(pid_t pid, uint64_t timeStamp);
    bool PushTransactionDataCallback(pid_t pid, uint64_t timeStamp,
        sptr<RSITransactionDataCallback> callback);

    std::map<std::pair<pid_t, uint64_t>, sptr<RSITransactionDataCallback>> transactionDataCallbacks_;
    std::mutex transactionDataCbMutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIPELINE_RS_TRANSACTION_DATA_CALLBACK_MANAGER_H