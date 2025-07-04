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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_HANDLER_H
#define ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_HANDLER_H

#include <event_handler.h>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>

#include "command/rs_command.h"
#include "command/rs_node_showing_command.h"
#include "common/rs_macros.h"
#include "common/rs_singleton.h"
#include "transaction/rs_irender_client.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
class RSSyncTask;
using FlushEmptyCallback = std::function<bool(const uint64_t)>;
#ifdef RS_ENABLE_VK
using CommitTransactionCallback =
    std::function<void(std::shared_ptr<RSIRenderClient>&, std::unique_ptr<RSTransactionData>&&, uint32_t&)>;
#endif
class RSB_EXPORT RSTransactionHandler final {
public:
    RSTransactionHandler() = default;
    RSTransactionHandler(uint64_t token) : token_(token) {}
    virtual ~RSTransactionHandler() = default;
    void SetRenderThreadClient(std::unique_ptr<RSIRenderClient>& renderThreadClient);
    void SetRenderServiceClient(const std::shared_ptr<RSIRenderClient>& renderServiceClient);

    void AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand = false,
        FollowType followType = FollowType::NONE, NodeId nodeId = 0);
    void AddCommandFromRT(
        std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType = FollowType::FOLLOW_TO_PARENT);
    void MoveCommandByNodeId(std::shared_ptr<RSTransactionHandler> transactionHandler, NodeId nodeId);

    void FlushImplicitTransaction(uint64_t timestamp = 0, const std::string& abilityName = "");
    void FlushImplicitTransactionFromRT(uint64_t timestamp);

    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task, bool isRenderServiceTask = false);

    void Begin();
    void Commit(uint64_t timestamp = 0);
    void CommitSyncTransaction(uint64_t timestamp = 0, const std::string& abilityName = "");
    void MarkTransactionNeedSync();
    void MarkTransactionNeedCloseSync(const int32_t transactionCount);
    void SetSyncTransactionNum(const int32_t transactionCount);

    void StartSyncTransaction();
    void CloseSyncTransaction();
    void SetFlushEmptyCallback(FlushEmptyCallback flushEmptyCallback)
    {
        flushEmptyCallback_ = flushEmptyCallback;
    }

#ifdef RS_ENABLE_VK
    static void SetCommitTransactionCallback(CommitTransactionCallback commitTransactionCallback);
#endif

    void SetSyncId(const uint64_t syncId)
    {
        syncId_ = syncId;
    }

    void SetParentPid(const int32_t parentPid);

    uint32_t GetTransactionDataIndex() const;

    bool IsEmpty() const;

    void StartCloseSyncTransactionFallbackTask(std::shared_ptr<AppExecFwk::EventHandler> handler, bool isOpen);

private:
    RSTransactionHandler(const RSTransactionHandler&) = delete;
    RSTransactionHandler(const RSTransactionHandler&&) = delete;
    RSTransactionHandler& operator=(const RSTransactionHandler&) = delete;
    RSTransactionHandler& operator=(const RSTransactionHandler&&) = delete;
    void AddCommonCommand(std::unique_ptr<RSCommand>& command);
    void MoveCommonCommandByNodeId(std::shared_ptr<RSTransactionHandler> transactionHandler, NodeId nodeId);
    void AddRemoteCommand(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType);
    void MoveRemoteCommandByNodeId(std::shared_ptr<RSTransactionHandler> transactionHandler, NodeId nodeId);
    // Command Transaction Triggered by UI Thread.
    mutable std::mutex mutex_;
    std::unique_ptr<RSTransactionData> implicitCommonTransactionData_ { std::make_unique<RSTransactionData>() };
    std::unique_ptr<RSTransactionData> implicitRemoteTransactionData_ { std::make_unique<RSTransactionData>() };

    std::stack<std::unique_ptr<RSTransactionData>> implicitCommonTransactionDataStack_;
    std::stack<std::unique_ptr<RSTransactionData>> implicitRemoteTransactionDataStack_;

    // Command Transaction Triggered by Render Thread which is definitely send to Render Service.
    std::mutex mutexForRT_;
    std::unique_ptr<RSTransactionData> implicitTransactionDataFromRT_ { std::make_unique<RSTransactionData>() };

    std::shared_ptr<RSIRenderClient> renderServiceClient_ = RSIRenderClient::CreateRenderServiceClient();
    std::unique_ptr<RSIRenderClient> renderThreadClient_ = nullptr;
    uint64_t timestamp_ = 0;
    uint64_t token_ = 0;

    bool needSync_ { false };
    uint64_t syncId_ { 0 };
    FlushEmptyCallback flushEmptyCallback_ = nullptr;
#ifdef RS_ENABLE_VK
    static CommitTransactionCallback commitTransactionCallback_;
#endif
    uint32_t transactionDataIndex_ = 0;
    std::queue<std::string> taskNames_ {};
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_HANDLER_H