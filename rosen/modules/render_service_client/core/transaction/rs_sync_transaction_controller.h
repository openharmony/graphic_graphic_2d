/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SYNC_TRANSACTION_CONTROLLER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SYNC_TRANSACTION_CONTROLLER_H

#include <mutex>
#include <vector>

#include "common/rs_macros.h"
#include "event_handler.h"

namespace OHOS {
namespace Rosen {
class RSTransaction;

class RSC_EXPORT RSSyncTransactionController {
public:
    static RSSyncTransactionController* GetInstance();

    void OpenSyncTransaction();

    std::shared_ptr<RSTransaction> GetRSTransaction()
    {
        return rsTransaction_;
    }

private:
    RSSyncTransactionController();
    virtual ~RSSyncTransactionController();

    static void Init();
    static void Destroy();

    RSSyncTransactionController(const RSSyncTransactionController&) = delete;
    RSSyncTransactionController(const RSSyncTransactionController&&) = delete;
    RSSyncTransactionController& operator=(const RSSyncTransactionController&) = delete;
    RSSyncTransactionController& operator=(const RSSyncTransactionController&&) = delete;

    void CloseSyncTransaction();
    void CreateTransactionFinished();
    void StartCreateTransaction();

    int32_t processCount_ { 0 };
    int32_t transactionCount_ { 0 };
    std::mutex mutex_;
    bool needCloseSync_ { false };
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<RSTransaction> rsTransaction_;
    static std::once_flag flag_;
    static RSSyncTransactionController* instance_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SYNC_TRANSACTION_CONTROLLER_H
