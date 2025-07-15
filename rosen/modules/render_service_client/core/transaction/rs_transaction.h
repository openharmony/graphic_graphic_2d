/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
* @file rs_transaction.h
* @brief Process transaction messages.
*/

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_TRANSACTION_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_TRANSACTION_H

#include <event_handler.h>
#include <message_parcel.h>
#include <mutex>
#include <parcel.h>
#include <refbase.h>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "transaction/rs_transaction_handler.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSTransaction : public Parcelable {
public:
    RSTransaction() = default;
    RSTransaction(const RSTransaction&) = delete;
    RSTransaction(const RSTransaction&&) = delete;
    RSTransaction& operator=(const RSTransaction&) = delete;
    RSTransaction& operator=(const RSTransaction&&) = delete;
    ~RSTransaction() override = default;

    static RSTransaction* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

    /**
     * @brief Send messages to RT or RS.
     * @details This function is used to flush message to RT(in divided render) or RS(in uniRender).
     */
    static void FlushImplicitTransaction();   // planing
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
    /**
     * @brief Start a synchronous transaction.
     * @details This function is used to close a synchronous transaction, and commit it to the server.
     */
    void Begin();
    /**
     * @brief Close a synchronous transaction.
     * @details This function is used to close a synchronous transaction, and commit it to the server.
     */
    void Commit();

    void SetDuration(int32_t duration) { duration_ = duration; }
    int32_t GetDuration() const { return duration_; }

    void SetParentPid(int32_t parentPid)
    {
        parentPid_ = parentPid;
    }

    int32_t GetParentPid() const
    {
        return parentPid_;
    }

    bool IsOpenSyncTransaction() const
    {
        return isOpenSyncTransaction_;
    }

    uint64_t GetSyncId() const
    {
        return syncId_;
    }

    void SetTransactionHandler(std::shared_ptr<RSTransactionHandler> rsTransactionHandler)
    {
        rsTransactionHandler_ = rsTransactionHandler;
    }

private:
    uint64_t GenerateSyncId();
    void ResetSyncTransactionInfo();
    bool UnmarshallingParam(Parcel& parcel);

    uint64_t syncId_ { 0 };
    std::mutex mutex_;
    mutable int32_t transactionCount_ { 0 };
    int32_t duration_ = 0;
    int32_t parentPid_ { -1 };
    bool isOpenSyncTransaction_ = false;
    std::shared_ptr<RSTransactionHandler> rsTransactionHandler_ = nullptr;

    friend class RSSyncTransactionController;
    friend class RSSyncTransactionHandler;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_TRANSACTION_H
