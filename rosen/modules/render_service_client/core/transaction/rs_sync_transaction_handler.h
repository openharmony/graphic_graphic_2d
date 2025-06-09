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

/**
* @file rs_sync_transaction_handle.h
* @brief This file is used to control the execution of synchronous transctios.
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
    /**
     * @brief Open a synchronous transaction.
     * @details This function is used to open a synchronous transaction, which will block other operation until
     * transaction is compelete.
     * @param handler Event handle, used to handle events related to the transaction, if it is empty
     * then no events will be processed.
     */
    void OpenSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr);
    /**
     * @brief Close a synchronous transaction.
     * @details This function is used to close a synchronous transaction, and commit it to the server.
     * @param handler An event handle for the fallback mechanism of synchronous transaction, if it is empty,
     * then no fallback for synchronous transactions will be performed.
     */
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