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

#include "transaction/rs_transaction_proxy.h"
#include <mutex>
#include <stdlib.h>

#ifdef ROSEN_OHOS
#include "hisysevent.h"
#include "mem_mgr_client.h"
#endif
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
std::once_flag RSTransactionProxy::flag_;
RSTransactionProxy* RSTransactionProxy::instance_ = nullptr;

RSTransactionProxy* RSTransactionProxy::GetInstance()
{
    std::call_once(flag_, &RSTransactionProxy::Init);
    return instance_;
}

RSTransactionProxy::RSTransactionProxy()
{
}

RSTransactionProxy::~RSTransactionProxy()
{
}

void RSTransactionProxy::Init()
{
    instance_ = new RSTransactionProxy();
    ::atexit(&RSTransactionProxy::Destroy);
}

void RSTransactionProxy::Destroy()
{
    instance_ = nullptr;
}

void RSTransactionProxy::SetRenderThreadClient(std::unique_ptr<RSIRenderClient>& renderThreadClient)
{
    if (renderThreadClient != nullptr) {
        renderThreadClient_ = std::move(renderThreadClient);
    }
}

void RSTransactionProxy::SetRenderServiceClient(const std::shared_ptr<RSIRenderClient>& renderServiceClient)
{
    if (renderServiceClient != nullptr) {
        renderServiceClient_ = renderServiceClient;
    }
}

void RSTransactionProxy::AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand,
                                    FollowType followType, NodeId nodeId)
{
#ifndef SCREENLESS_DEVICE
    if ((renderServiceClient_ == nullptr && renderThreadClient_ == nullptr) || command == nullptr) {
        RS_LOGE("RSTransactionProxy::add command fail, (renderServiceClient_ and renderThreadClient_ is nullptr)"
            " or command is nullptr");
        return;
    }

    std::unique_lock<std::mutex> cmdLock(mutex_);

    RS_LOGI_IF(DEBUG_NODE,
        "RSTransactionProxy::add command nodeId:%{public}" PRIu64 " isRenderServiceCommand:%{public}d"
        " followType:%{public}hu", nodeId, isRenderServiceCommand, followType);
    if (renderServiceClient_ != nullptr && (isRenderServiceCommand || renderThreadClient_ == nullptr)) {
        #ifdef ROSEN_OHOS
        int appPid = ExtractPid(nodeId);
        if (isRenderServiceCommand && command->GetSubType() == RSCommandType::ANIMATION &&
            RSSystemProperties::GetDmaReclaimParam()) {
            RS_TRACE_NAME_FMT("MemoryStatusChanged from proxy pid[%d]", appPid);
            Memory::MemMgrClient::GetInstance().MemoryStatusChanged(appPid,
                static_cast<int32_t>(Memory::MemoryTypeCode::DMABUF),
                static_cast<int32_t>(Memory::MemoryStatusCode::USED));
        }
        #endif
        AddRemoteCommand(command, nodeId, followType);
        return;
    }

    if (!isRenderServiceCommand) {
        AddCommonCommand(command);
        return;
    }
    ROSEN_LOGE("RSTransactionProxy::AddCommand failed, isRenderServiceCommand:%{public}d %{public}s",
        isRenderServiceCommand, command->PrintType().c_str());
#endif
}

void RSTransactionProxy::AddCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType)
{
    if (renderServiceClient_ == nullptr || command == nullptr) {
        return;
    }

    {
        std::unique_lock<std::mutex> cmdLock(mutexForRT_);
        implicitTransactionDataFromRT_->AddCommand(command, nodeId, followType);
    }
}

void RSTransactionProxy::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task, bool isRenderServiceTask)
{
    if (!task) {
        ROSEN_LOGE("RSTransactionProxy::ExecuteSynchronousTask failed, the task is not exist.");
        return;
    }

    if (renderServiceClient_ && isRenderServiceTask) {
        renderServiceClient_->ExecuteSynchronousTask(task);
        return;
    }

    if (renderThreadClient_ && (!isRenderServiceTask)) {
        renderThreadClient_->ExecuteSynchronousTask(task);
        return;
    }

    ROSEN_LOGE("RSTransactionProxy::ExecuteSynchronousTask failed, isRenderServiceTask is %{public}d.",
        isRenderServiceTask);
}

void RSTransactionProxy::FlushImplicitTransaction(uint64_t timestamp, const std::string& abilityName)
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    if (!implicitRemoteTransactionDataStack_.empty() && needSync_) {
        RS_LOGE_LIMIT(__func__, __line__, "FlushImplicitTransaction failed, DataStack not empty");
        return;
    }
    timestamp_ = std::max(timestamp, timestamp_);
#ifdef RS_ENABLE_VK
    thread_local pid_t tid = gettid();
#endif
    if (renderThreadClient_ != nullptr && !implicitCommonTransactionData_->IsEmpty()) {
        implicitCommonTransactionData_->timestamp_ = timestamp_;
#ifdef RS_ENABLE_VK
        implicitCommonTransactionData_->tid_ = tid;
#endif
        implicitCommonTransactionData_->abilityName_ = abilityName;
        renderThreadClient_->CommitTransaction(implicitCommonTransactionData_);
        implicitCommonTransactionData_ = std::make_unique<RSTransactionData>();
    } else {
        if (flushEmptyCallback_) {
            flushEmptyCallback_(timestamp_);
        }
    }

    if (renderServiceClient_ == nullptr || implicitRemoteTransactionData_->IsEmpty()) {
        RS_LOGE_LIMIT(__func__, __line__, "FlushImplicitTransaction return, [renderServiceClient_:%{public}d," \
            " transactionData empty:%{public}d]",
            renderServiceClient_ != nullptr, implicitRemoteTransactionData_->IsEmpty());
        RS_TRACE_NAME("UI skip");
        uiSkipCount_++;
        return;
    }

    auto transactionData = std::make_unique<RSTransactionData>();
    std::swap(implicitRemoteTransactionData_, transactionData);
    transactionData->timestamp_ = timestamp_;
#ifdef RS_ENABLE_VK
    transactionData->tid_ = tid;
    if (RSSystemProperties::GetHybridRenderEnabled() && commitTransactionCallback_ != nullptr) {
        commitTransactionCallback_(renderServiceClient_,
            std::move(transactionData), transactionDataIndex_);
        return;
    }
#endif
    renderServiceClient_->CommitTransaction(transactionData);
    transactionDataIndex_ = transactionData->GetIndex();
}

void RSTransactionProxy::ReportUiSkipEvent(const std::string& ability, int64_t nowMs, int64_t lastTimestamp)
{
    static uint32_t lastCount_ = 0;
    static std::mutex mtx;
    uint32_t nowCount = 0;
    uint32_t lastCount = 0;
    {
        std::lock_guard<std::mutex> lock(mtx);
        nowCount = uiSkipCount_;
        lastCount = lastCount_;
        lastCount_ = nowCount;
    }
    int64_t duration = (nowMs > lastTimestamp) ? nowMs - lastTimestamp : 0;
    if (duration > 60 * 1000 && nowCount > lastCount && nowCount - lastCount > 100) { // 60 * 1000 ms, gt 100 frame
        int32_t count = static_cast<int32_t>(nowCount - lastCount);
        RS_LOGE_LIMIT(__func__, __line__, "ui_skip count:%{public}d", count);
#ifdef ROSEN_OHOS
        using namespace OHOS::HiviewDFX;
        HiSysEventWrite(HiSysEvent::Domain::GRAPHIC, "RS_UI_SKIP_ANOMALY",
            HiSysEvent::EventType::STATISTIC, "ABILITY", ability, "COUNT", count, "DURATION", duration);
#endif
    }
}

uint32_t RSTransactionProxy::GetTransactionDataIndex() const
{
    return transactionDataIndex_;
}

bool RSTransactionProxy::IsEmpty() const
{
    bool isEmpty = true;
    std::unique_lock<std::mutex> cmdLock(mutex_);
    if (implicitCommonTransactionData_) {
        isEmpty &= implicitCommonTransactionData_->IsEmpty();
    }
    if (implicitRemoteTransactionData_) {
        isEmpty &= implicitRemoteTransactionData_->IsEmpty();
    }
    isEmpty &= implicitCommonTransactionDataStack_.empty();
    isEmpty &= implicitRemoteTransactionDataStack_.empty();
    return isEmpty;
}

void RSTransactionProxy::FlushImplicitTransactionFromRT(uint64_t timestamp)
{
    std::unique_lock<std::mutex> cmdLock(mutexForRT_);
    if (renderServiceClient_ != nullptr && !implicitTransactionDataFromRT_->IsEmpty()) {
        implicitTransactionDataFromRT_->timestamp_ = timestamp;
        renderServiceClient_->CommitTransaction(implicitTransactionDataFromRT_);
        implicitTransactionDataFromRT_ = std::make_unique<RSTransactionData>();
    }
}

void RSTransactionProxy::StartSyncTransaction()
{
    needSync_ = true;
}

void RSTransactionProxy::CloseSyncTransaction()
{
    needSync_ = false;
}

void RSTransactionProxy::StartCloseSyncTransactionFallbackTask(
    std::shared_ptr<AppExecFwk::EventHandler> handler, bool isOpen)
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    static uint32_t num = 0;
    const std::string name = "CloseSyncTransactionFallbackTask";
    const int timeOutDelay = 5000;
    if (!handler) {
        ROSEN_LOGD("StartCloseSyncTransactionFallbackTask handler is null");
        return;
    }
    if (isOpen) {
        num++;
        auto taskName = name + std::to_string(num);
        taskNames_.push(taskName);
        auto task = [this]() {
            RS_TRACE_NAME("CloseSyncTransaction timeout");
            ROSEN_LOGE("CloseSyncTransaction timeout");
            auto transactionProxy = RSTransactionProxy::GetInstance();
            if (transactionProxy != nullptr) {
                transactionProxy->CommitSyncTransaction();
                transactionProxy->CloseSyncTransaction();
            }
            if (!taskNames_.empty()) {
                taskNames_.pop();
            }
        };
        handler->PostTask(task, taskName, timeOutDelay);
    } else {
        if (!taskNames_.empty()) {
            handler->RemoveTask(taskNames_.front());
            taskNames_.pop();
        }
    }
}

void RSTransactionProxy::Begin()
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    implicitCommonTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    implicitRemoteTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    if (needSync_) {
        implicitCommonTransactionDataStack_.top()->MarkNeedSync();
        implicitRemoteTransactionDataStack_.top()->MarkNeedSync();
    }
}

void RSTransactionProxy::Commit(uint64_t timestamp)
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    if (!implicitCommonTransactionDataStack_.empty()) {
        implicitCommonTransactionDataStack_.pop();
    }

    if (!implicitRemoteTransactionDataStack_.empty()) {
        if (renderServiceClient_ != nullptr && !implicitRemoteTransactionDataStack_.top()->IsEmpty()) {
            implicitRemoteTransactionDataStack_.top()->timestamp_ = timestamp;
            renderServiceClient_->CommitTransaction(implicitRemoteTransactionDataStack_.top());
        }
        implicitRemoteTransactionDataStack_.pop();
    }
}

void RSTransactionProxy::CommitSyncTransaction(uint64_t timestamp, const std::string& abilityName)
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    timestamp_ = std::max(timestamp, timestamp_);
    if (!implicitCommonTransactionDataStack_.empty()) {
        if (renderThreadClient_ != nullptr && (!implicitCommonTransactionDataStack_.top()->IsEmpty() ||
            implicitCommonTransactionDataStack_.top()->IsNeedSync())) {
            implicitCommonTransactionDataStack_.top()->timestamp_ = timestamp;
            implicitCommonTransactionDataStack_.top()->abilityName_ = abilityName;
            implicitCommonTransactionDataStack_.top()->SetSyncId(syncId_);
            renderThreadClient_->CommitTransaction(implicitCommonTransactionDataStack_.top());
        }
        implicitCommonTransactionDataStack_.pop();
    }

    if (!implicitRemoteTransactionDataStack_.empty()) {
        if (renderServiceClient_ != nullptr && (!implicitRemoteTransactionDataStack_.top()->IsEmpty() ||
            implicitRemoteTransactionDataStack_.top()->IsNeedSync())) {
            implicitRemoteTransactionDataStack_.top()->timestamp_ = timestamp;
            implicitRemoteTransactionDataStack_.top()->SetSyncId(syncId_);
            renderServiceClient_->CommitTransaction(implicitRemoteTransactionDataStack_.top());
        }
        implicitRemoteTransactionDataStack_.pop();
    }
}

void RSTransactionProxy::MarkTransactionNeedSync()
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    if (!implicitCommonTransactionDataStack_.empty()) {
        implicitCommonTransactionDataStack_.top()->MarkNeedSync();
    }

    if (!implicitRemoteTransactionDataStack_.empty()) {
        implicitRemoteTransactionDataStack_.top()->MarkNeedSync();
    }
}

void RSTransactionProxy::MarkTransactionNeedCloseSync(const int32_t transactionCount)
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    if (!implicitCommonTransactionDataStack_.empty()) {
        implicitCommonTransactionDataStack_.top()->MarkNeedSync();
        implicitCommonTransactionDataStack_.top()->MarkNeedCloseSync();
        implicitCommonTransactionDataStack_.top()->SetSyncTransactionNum(transactionCount);
    }

    if (!implicitRemoteTransactionDataStack_.empty()) {
        implicitRemoteTransactionDataStack_.top()->MarkNeedSync();
        implicitRemoteTransactionDataStack_.top()->MarkNeedCloseSync();
        implicitRemoteTransactionDataStack_.top()->SetSyncTransactionNum(transactionCount);
    }
}

void RSTransactionProxy::SetSyncTransactionNum(const int32_t transactionCount)
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    if (!implicitCommonTransactionDataStack_.empty()) {
        implicitCommonTransactionDataStack_.top()->SetSyncTransactionNum(transactionCount);
    }

    if (!implicitRemoteTransactionDataStack_.empty()) {
        implicitRemoteTransactionDataStack_.top()->SetSyncTransactionNum(transactionCount);
    }
}

void RSTransactionProxy::SetParentPid(const int32_t parentPid)
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    if (!implicitCommonTransactionDataStack_.empty()) {
        implicitCommonTransactionDataStack_.top()->SetParentPid(parentPid);
    }

    if (!implicitRemoteTransactionDataStack_.empty()) {
        implicitRemoteTransactionDataStack_.top()->SetParentPid(parentPid);
    }
}

void RSTransactionProxy::AddCommonCommand(std::unique_ptr<RSCommand> &command)
{
    if (!implicitCommonTransactionDataStack_.empty()) {
        implicitCommonTransactionDataStack_.top()->AddCommand(command, 0, FollowType::NONE);
        return;
    }
    implicitCommonTransactionData_->AddCommand(command, 0, FollowType::NONE);
}

void RSTransactionProxy::AddRemoteCommand(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType)
{
    if (!implicitRemoteTransactionDataStack_.empty()) {
        implicitRemoteTransactionDataStack_.top()->AddCommand(command, nodeId, followType);
        return;
    }
    implicitRemoteTransactionData_->AddCommand(command, nodeId, followType);
}

} // namespace Rosen
} // namespace OHOS
