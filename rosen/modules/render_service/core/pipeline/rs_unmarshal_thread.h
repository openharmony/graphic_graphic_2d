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

#include "event_handler.h"
#include "message_parcel.h"

#include "transaction/rs_transaction_data.h"

namespace OHOS::Rosen {
class RSUnmarshalThread {
public:
    static RSUnmarshalThread& Instance();
    void Start();
    void PostTask(const std::function<void()>& task);
    void RecvParcel(std::shared_ptr<MessageParcel>& parcel);
    TransactionDataMap GetCachedTransactionData();
    bool CachedTransactionDataEmpty();

private:
    RSUnmarshalThread() = default;
    ~RSUnmarshalThread() = default;
    RSUnmarshalThread(const RSUnmarshalThread&);
    RSUnmarshalThread(const RSUnmarshalThread&&);
    RSUnmarshalThread& operator=(const RSUnmarshalThread&);
    RSUnmarshalThread& operator=(const RSUnmarshalThread&&);

    static constexpr uint32_t MIN_PENDING_REQUEST_SYNC_DATA_SIZE = 32 * 1024;

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;

    std::mutex transactionDataMutex_;
    TransactionDataMap cachedTransactionDataMap_;
    bool willHaveCachedData_ = false;
};
}
#endif // RS_UNMARSHAL_THREAD_H
