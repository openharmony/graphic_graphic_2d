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

#include "rs_ui_context_manager.h"

#include <atomic>

#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "transaction/rs_transaction.h"

namespace OHOS {
namespace Rosen {

namespace {
static std::atomic_bool g_instanceValid = false;
}

RSUIContextManager::RSUIContextManager()
{
    g_instanceValid.store(true);
    isMultiInstanceOpen_ = RSSystemProperties::GetRSClientMultiInstanceEnabled();
    handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
    RS_LOGI("multi-instance, create RSUIContextManager, isMultiInstanceOpen_ %{public}d", isMultiInstanceOpen_);
}

RSUIContextManager::~RSUIContextManager() noexcept
{
    std::unique_lock<std::mutex> lock(mutex_);
    rsUIContextMap_.clear();
    g_instanceValid.store(false);
}

RSUIContextManager& RSUIContextManager::MutableInstance()
{
    static RSUIContextManager rsUIContextManager;
    return rsUIContextManager;
}

const RSUIContextManager& RSUIContextManager::Instance()
{
    return MutableInstance();
}

std::shared_ptr<RSUIContext> RSUIContextManager::CreateRSUIContext()
{
    if (!isMultiInstanceOpen_ || !g_instanceValid.load()) {
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    int32_t tid = gettid();
    uint64_t token = GenerateToken(tid);
    auto iter = rsUIContextMap_.find(token);
    if (iter != rsUIContextMap_.end()) {
        ROSEN_LOGW("RSUIContextManager::CreateRSUIContext: context token %{public}" PRIu64 " already exists", token);
        return iter->second;
    }
    auto newContext = std::shared_ptr<RSUIContext>(new RSUIContext(token));
    rsUIContextMap_[token] = newContext;
    return newContext;
}

const std::shared_ptr<RSUIContext> RSUIContextManager::GetRSUIContext(uint64_t token) const
{
    if (!isMultiInstanceOpen_ || !g_instanceValid.load()) {
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = rsUIContextMap_.find(token);
    if (iter == rsUIContextMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

void RSUIContextManager::DestroyContext(uint64_t token)
{
    if (!isMultiInstanceOpen_ || !g_instanceValid.load()) {
        return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = rsUIContextMap_.find(token);
    if (iter != rsUIContextMap_.end()) {
        rsUIContextMap_.erase(iter);
    } else {
        ROSEN_LOGW("RSUIContextManager::DestroyContext: context token %{public}" PRIu64 " does not exist", token);
    }
}

uint64_t RSUIContextManager::GenerateToken(int32_t tid)
{
    ++instanceIdCounter_;
    return (static_cast<uint64_t>(tid) << 32) | instanceIdCounter_; // 32 for 64-bit unsignd number shift
}

std::shared_ptr<RSUIContext> RSUIContextManager::GetRandomUITaskRunnerCtx() const
{
    if (!isMultiInstanceOpen_ || !g_instanceValid.load() || rsUIContextMap_.empty()) {
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    for (const auto& [_, ctx] : rsUIContextMap_) {
        if (ctx->HasTaskRunner()) {
            return ctx;
        }
    }
    return nullptr;
}

void RSUIContextManager::CloseAllSyncTransaction(const uint64_t syncId) const
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (!isMultiInstanceOpen_ || !g_instanceValid.load() || rsUIContextMap_.empty()) {
        ROSEN_LOGE("RSUIContextManager::CloseAllSyncTransaction isMultiInstanceOpen_, g_instanceValid, rsUIContextMap_ "
            "is not match syncId:%{public}" PRIu64 "", syncId);
        return;
    }
    for (const auto& [_, ctx] : rsUIContextMap_) {
        auto syncHandler = ctx->GetSyncTransactionHandler();
        if (syncHandler == nullptr) {
            ROSEN_LOGE("RSUIContextManager::CloseAllSyncTransaction syncHandler is nullptr syncId:%{public}" PRIu64 "",
                syncId);
            continue;
        }
        auto transaction = syncHandler->GetCommonRSTransaction();
        if (transaction == nullptr) {
            ROSEN_LOGE("RSUIContextManager::CloseAllSyncTransaction transaction is nullptr syncId:%{public}" PRIu64 "",
                syncId);
            continue;
        }
        auto transactionHandler = transaction->GetTransactionHandler();
        if (transactionHandler == nullptr) {
            ROSEN_LOGE(
                "RSUIContextManager::CloseAllSyncTransaction transactionHandler is nullptr syncId:%{public}" PRIu64 "",
                syncId);
            continue;
        }
        transactionHandler->CommitSyncTransaction(syncId);
        transactionHandler->CloseSyncTransaction();
    }
}

void RSUIContextManager::StartCloseSyncTransactionFallbackTask(bool isOpen, const uint64_t syncId)
{
    std::unique_lock<std::mutex> lock(closeSyncFallBackMutex_);
    static uint32_t num = 0;
    const std::string name = "CloseSyncTransactionFallbackTask";
    const int timeOutDelay = 5000;
    if (isOpen) {
        num++;
        auto taskName = name + std::to_string(num);
        taskNames_.push(taskName);
        auto task = [this, syncId, taskName]() {
            RS_TRACE_NAME_FMT("CloseSyncTransaction timeout syncId:%" PRIu64 "", syncId);
            ROSEN_LOGE("CloseSyncTransaction timeout syncId:%{public}" PRIu64 "", syncId);
            CloseAllSyncTransaction(syncId);
            if (!taskNames_.empty() && taskNames_.front() == taskName) {
                taskNames_.pop();
            }
        };
        handler_->PostTask(task, taskName, timeOutDelay);
    } else {
        if (!taskNames_.empty()) {
            handler_->RemoveTask(taskNames_.front());
            taskNames_.pop();
        }
    }
}

} // namespace Rosen
} // namespace OHOS