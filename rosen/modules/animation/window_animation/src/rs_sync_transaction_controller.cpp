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

#include "rs_sync_transaction_controller.h"

#include "rs_process_transaction_controller.h"
#include "rs_window_animation_log.h"
#include "transaction/rs_transaction.h"

namespace OHOS {
namespace Rosen {
namespace {
    const std::string CLOSE_SYNC_TRANSACTION_TIMEOUT_TASK = "RSCloseSyncTransactionTimeoutTask";
    constexpr int64_t CLOSE_SYNC_TRANSACTION_TIMEOUT_MILLISECONDS = 100;
}

std::once_flag RSSyncTransactionController::flag_;
RSSyncTransactionController* RSSyncTransactionController::instance_ = nullptr;

RSSyncTransactionController* RSSyncTransactionController::GetInstance()
{
    std::call_once(flag_, &RSSyncTransactionController::Init);
    return instance_;
}

RSSyncTransactionController::~RSSyncTransactionController()
{}

void RSSyncTransactionController::Init()
{
    instance_ = new RSSyncTransactionController();
    ::atexit(&RSSyncTransactionController::Destroy);
}

void RSSyncTransactionController::Destroy()
{
    instance_ = nullptr;
}

RSSyncTransactionController::RSSyncTransactionController()
{
    runner_ = AppExecFwk::EventRunner::Create("RSSyncTransactionController");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    rsTransaction_ = std::make_shared<RSTransaction>();
}

void RSSyncTransactionController::CreateTransactionFinished()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (processCount_ == 0) {
            return;
        }
        processCount_--;
        transactionCount_++;
    }
    if (processCount_ == 0) {
        WALOGD("RS sync transaction controller CreateTransactionFinished, processCount: %{public}d", processCount_);
        CloseSyncTransaction();
        controllers_.clear();
    }
}

void RSSyncTransactionController::StartTransactionSyncForProcess()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (processCount_ < 0) {
        processCount_ = 0;
    }
    processCount_++;
    WALOGD("RS sync transaction controller StartTransactionSyncForProcess, processCount: %{public}d", processCount_);
}

void RSSyncTransactionController::OpenSyncTransaction()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        needCloseSync_ = true;
    }
    WALOGD("RS sync transaction controller OpenSyncTransaction");
    if (rsTransaction_) {
        rsTransaction_->OpenSyncTransaction();
    }
    if (handler_) {
        auto task = [this]() {
            WALOGD("RS sync transaction controller close timeout task run");
            CloseSyncTransaction();
        };
        handler_->PostTask(task, CLOSE_SYNC_TRANSACTION_TIMEOUT_TASK, CLOSE_SYNC_TRANSACTION_TIMEOUT_MILLISECONDS);
    }
}

void RSSyncTransactionController::CloseSyncTransaction()
{
    if (!needCloseSync_) {
        return;
    }

    WALOGD("RS sync transaction controller CloseSyncTransaction");
    if (rsTransaction_) {
        rsTransaction_->CloseSyncTransaction(transactionCount_);
    }
    {
        std::unique_lock<std::mutex> lock(mutex_);
        processCount_ = 0;
        transactionCount_ = 0;
        needCloseSync_ = false;
    }
}

void RSSyncTransactionController::AddProcessTransactionController(
    const sptr<RSProcessTransactionController>& controller)
{
    std::unique_lock<std::mutex> lock(mutex_);
    controllers_.emplace_back(controller);
    TransactionFinishedCallback callback = [this]() {
        CreateTransactionFinished();
    };
    controller->SetTransactionFinishedCallback(callback);
}
} // namespace Rosen
} // namespace OHOS
