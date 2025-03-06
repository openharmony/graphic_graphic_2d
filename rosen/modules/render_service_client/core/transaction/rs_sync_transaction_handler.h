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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SYNC_TRANSACTION_HANDLER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SYNC_TRANSACTION_HANDLER_H

#include <mutex>
#include <vector>

#include "event_handler.h"

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSTransaction;
class RSTransactionHandler;

class RSC_EXPORT RSSyncTransactionHandler {
public:
    RSSyncTransactionHandler() = default;
    virtual ~RSSyncTransactionHandler();
    void OpenSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr);
    void CloseSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr);

    std::shared_ptr<RSTransaction> GetRSTransaction();

private:
    RSSyncTransactionHandler(const RSSyncTransactionHandler&) = delete;
    RSSyncTransactionHandler(const RSSyncTransactionHandler&&) = delete;
    RSSyncTransactionHandler& operator=(const RSSyncTransactionHandler&) = delete;
    RSSyncTransactionHandler& operator=(const RSSyncTransactionHandler&&) = delete;
 
    void SetTransactionHandler(std::shared_ptr<RSTransactionHandler> rsTransactionHandler);

    std::mutex mutex_;
    bool needCloseSync_ { false };
    std::shared_ptr<RSTransaction> rsTransaction_;

    friend class RSUIContext;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SYNC_TRANSACTION_HANDLER_H