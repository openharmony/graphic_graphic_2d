/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "transaction/rs_unmarshal_thread.h"

#include "app_mgr_client.h"
#include "hisysevent.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_unmarshal_task_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_transaction_data.h"
#include "res_sched_client.h"
#include "res_type.h"
#include "rs_frame_report.h"
#include "rs_profiler.h"
#include "command/rs_node_command.h"
#include "command/rs_canvas_node_command.h"
#include "recording/draw_cmd_list.h"
#include "rs_trace.h"
#include "platform/common/rs_hisysevent.h"

#ifdef RES_SCHED_ENABLE
#include "qos.h"
#endif

namespace OHOS::Rosen {
namespace {
constexpr int REQUEST_FRAME_AWARE_ID = 100001;
constexpr int REQUEST_SET_FRAME_LOAD_ID = 100006;
constexpr int REQUEST_FRAME_AWARE_LOAD = 85;
constexpr int REQUEST_FRAME_AWARE_NUM = 4;
constexpr int REQUEST_FRAME_STANDARD_LOAD = 50;
constexpr size_t TRANSACTION_DATA_ALARM_COUNT = 10000;
constexpr size_t TRANSACTION_DATA_KILL_COUNT = 20000;
const char* TRANSACTION_REPORT_NAME = "IPC_DATA_OVER_ERROR";

const std::shared_ptr<AppExecFwk::AppMgrClient> GetAppMgrClient()
{
    static std::shared_ptr<AppExecFwk::AppMgrClient> appMgrClient =
        std::make_shared<AppExecFwk::AppMgrClient>();
    return appMgrClient;
}
}

RSUnmarshalThread& RSUnmarshalThread::Instance()
{
    static RSUnmarshalThread instance;
    return instance;
}

void RSUnmarshalThread::Start()
{
    runner_ = AppExecFwk::EventRunner::Create("RSUnmarshalThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
#ifdef RES_SCHED_ENABLE
    PostTask([this]() {
        auto ret = OHOS::QOS::SetThreadQos(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE);
        unmarshalTid_ = gettid();
        RS_LOGI("RSUnmarshalThread: SetThreadQos retcode = %{public}d", ret);
    });
#endif
}

void RSUnmarshalThread::PostTask(const std::function<void()>& task, const std::string& name)
{
    if (handler_) {
        handler_->PostTask(
            [task, taskName = name]() mutable {
                auto handle = RSUnmarshalTaskManager::Instance().BeginTask(std::move(taskName));
                std::invoke(task);
                RSUnmarshalTaskManager::Instance().EndTask(handle);
            }, name, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSUnmarshalThread::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask(name);
    }
}

void RSUnmarshalThread::RecvParcel(std::shared_ptr<MessageParcel>& parcel, bool isNonSystemAppCalling, pid_t callingPid,
    std::unique_ptr<AshmemFdWorker> ashmemFdWorker, std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit,
    uint32_t parcelNumber)
{
    if (!handler_ || !parcel) {
        RS_LOGE("RSUnmarshalThread::RecvParcel has nullptr, handler: %{public}d, parcel: %{public}d",
            (!handler_), (!parcel));
        return;
    }
    bool isPendingUnmarshal = (parcel->GetDataSize() > MIN_PENDING_REQUEST_SYNC_DATA_SIZE);
    RSTaskMessage::RSTask task = [this, parcel = parcel, isPendingUnmarshal, isNonSystemAppCalling, callingPid,
        ashmemFdWorker = std::shared_ptr(std::move(ashmemFdWorker)), ashmemFlowControlUnit, parcelNumber]() {
        RSMarshallingHelper::SetCallingPid(callingPid);
        AshmemFdContainer::SetIsUnmarshalThread(true);
        if (ashmemFdWorker) {
            ashmemFdWorker->PushFdsToContainer();
        }
        SetFrameParam(REQUEST_FRAME_AWARE_ID, REQUEST_FRAME_AWARE_LOAD, REQUEST_FRAME_AWARE_NUM, 0);
        SetFrameLoad(REQUEST_FRAME_AWARE_LOAD);
        auto transData = RSBaseRenderUtil::ParseTransactionData(*parcel, parcelNumber);
        SetFrameLoad(REQUEST_FRAME_STANDARD_LOAD);
        if (ashmemFdWorker) {
            // ashmem parcel fds will be closed in ~AshmemFdWorker() instead of ~MessageParcel()
            parcel->FlushBuffer();
            ashmemFdWorker->EnableManualCloseFds();
        }
        if (!transData) {
            return;
        }
        if (isNonSystemAppCalling) {
            const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
            if (!transData->IsCallingPidValid(callingPid, nodeMap)) {
                RS_LOGE("RSUnmarshalThread::RecvParcel IsCallingPidValid check failed");
            }
            bool shouldDrop = ReportTransactionDataStatistics(callingPid, transData.get(), isNonSystemAppCalling);
            if (shouldDrop) {
                RS_LOGW("RSUnmarshalThread::RecvParcel data droped");
                return;
            }
        }
        RS_PROFILER_ON_PARCEL_RECEIVE(parcel.get(), transData.get());
        int64_t time = transData == nullptr ? 0 : static_cast<int64_t>(transData->GetTimestamp());
        {
            std::lock_guard<std::mutex> lock(transactionDataMutex_);
            cachedTransactionDataMap_[transData->GetSendingPid()].emplace_back(std::move(transData));
        }
        if (isPendingUnmarshal) {
            RSMainThread::Instance()->RequestNextVSync("UI", time);
        } else {
            const auto &now = std::chrono::steady_clock::now().time_since_epoch();
            int64_t currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
            constexpr int64_t ONE_PERIOD = 8000000;
            if (currentTime - time > ONE_PERIOD && time != 0) {
                RSMainThread::Instance()->RequestNextVSync("UI", time);
            }
        }
        RSMainThread::Instance()->NotifyUnmarshalTask(time);
        // ashmem parcel flow control ends in the destructor of ashmemFlowControlUnit
    };
    {
        ffrt::task_handle handle;
        if (RSSystemProperties::GetUnmarshParallelFlag()) {
            handle = ffrt::submit_h(task, {}, {}, ffrt::task_attr().qos(ffrt::qos_user_interactive));
        } else {
            PostTask(task, std::to_string(callingPid));
        }
        /* a task has been posted, it means cachedTransactionDataMap_ will not been empty.
         * so set willHaveCachedData_ to true
         */
        std::lock_guard<std::mutex> lock(transactionDataMutex_);
        willHaveCachedData_ = true;
        if (RSSystemProperties::GetUnmarshParallelFlag()) {
            cachedDeps_.push_back(std::move(handle));
        }
    }
    if (!isPendingUnmarshal) {
        RSMainThread::Instance()->RequestNextVSync();
    }
}

TransactionDataMap RSUnmarshalThread::GetCachedTransactionData()
{
    TransactionDataMap transactionData;
    {
        std::lock_guard<std::mutex> lock(transactionDataMutex_);
        std::swap(transactionData, cachedTransactionDataMap_);
        willHaveCachedData_ = false;
    }
    return transactionData;
}

bool RSUnmarshalThread::CachedTransactionDataEmpty()
{
    std::lock_guard<std::mutex> lock(transactionDataMutex_);
    /* we need consider both whether cachedTransactionDataMap_ is empty now
     * and whether cachedTransactionDataMap_ will be empty later
     */
    return cachedTransactionDataMap_.empty() && !willHaveCachedData_;
}
void RSUnmarshalThread::SetFrameParam(int requestId, int load, int frameNum, int value)
{
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().SetFrameParam(requestId, load, frameNum, value);
    }
}
void RSUnmarshalThread::SetFrameLoad(int load)
{
    if (load == REQUEST_FRAME_STANDARD_LOAD && unmarshalLoad_ > REQUEST_FRAME_STANDARD_LOAD) {
        unmarshalLoad_ = load;
        SetFrameParam(REQUEST_SET_FRAME_LOAD_ID, load, 0, unmarshalTid_);
        return;
    }
    SetFrameParam(REQUEST_SET_FRAME_LOAD_ID, load, 0, unmarshalTid_);
    unmarshalLoad_ = load;
}

void RSUnmarshalThread::Wait()
{
    std::vector<ffrt::dependence> deps;
    {
        std::lock_guard<std::mutex> lock(transactionDataMutex_);
        std::swap(deps, cachedDeps_);
    }
    ffrt::wait(deps);
}

bool RSUnmarshalThread::IsHaveCmdList(const std::unique_ptr<RSCommand>& cmd) const
{
    if (!cmd) {
        return false;
    }
    bool haveCmdList = false;
    switch (cmd->GetType()) {
        case RSCommandType::RS_NODE:
#if defined(MODIFIER_NG)
            if (cmd->GetSubType() == RSNodeCommandType::UPDATE_MODIFIER_DRAW_CMD_LIST_NG ||
                cmd->GetSubType() == RSNodeCommandType::ADD_MODIFIER_NG) {
#else
            if (cmd->GetSubType() == RSNodeCommandType::UPDATE_MODIFIER_DRAW_CMD_LIST ||
                cmd->GetSubType() == RSNodeCommandType::ADD_MODIFIER) {
#endif
                haveCmdList = true;
            }
            break;
        case RSCommandType::CANVAS_NODE:
            if (cmd->GetSubType() == RSCanvasNodeCommandType::CANVAS_NODE_UPDATE_RECORDING) {
                haveCmdList = true;
            }
            break;
        default:
            break;
    }
    return haveCmdList;
}

bool RSUnmarshalThread::ReportTransactionDataStatistics(pid_t pid,
                                                        RSTransactionData* transactionData,
                                                        bool isNonSystemAppCalling)
{
    size_t preCount = 0;
    size_t totalCount = 0;
    size_t opCount = 0;
    if (!transactionData) {
        return false;
    }
    opCount = transactionData->GetCommandCount();
    auto& payload_temp = transactionData->GetPayload();
    for (auto& item_temp : payload_temp) {
        auto& cmd = std::get<2>(item_temp);
        if (!cmd) {
            continue;
        }
        if (IsHaveCmdList(cmd)) {
            auto drawCmdList = cmd->GetDrawCmdList();
            if (drawCmdList) {
                opCount += drawCmdList->GetOpItemSize();
            }
        }
    }
    {
        std::unique_lock<std::mutex> lock(statisticsMutex_);
        preCount = transactionDataStatistics_[pid];
        totalCount = preCount + opCount;
        transactionDataStatistics_[pid] = totalCount;

        if (totalCount < TRANSACTION_DATA_ALARM_COUNT) {
            return false;
        }
    }
    const auto appMgrClient = GetAppMgrClient();
    if (!appMgrClient) {
        RS_LOGW("Get global variable AppMgrClient failed");
        return false;
    }
    int32_t uid = 0;
    std::string bundleName;
    appMgrClient->GetBundleNameByPid(pid, bundleName, uid);

    RS_TRACE_NAME_FMT("RSUnmarshalThread::ReportTransactionDataStatistics HiSysEventWrite pid[%d] uid[%d]"
        " bundleName[%s] opCount[%zu] exceeded[%d]",
        pid, uid, bundleName.c_str(), totalCount, totalCount > TRANSACTION_DATA_KILL_COUNT);
    RSHiSysEvent::EventWrite(RSEventName::IPC_DATA_OVER_ERROR, RSEventType::RS_STATISTIC, "PID", pid, "UID", uid,
        "BUNDLE_NAME", bundleName, "TRANSACTION_DATA_SIZE", totalCount);
    RS_LOGW("TransactionDataStatistics pid[%{public}d] uid[%{public}d]"
            " bundleName[%{public}s] opCount[%{public}zu] exceeded[%{public}d]",
        pid, uid, bundleName.c_str(), totalCount, totalCount > TRANSACTION_DATA_KILL_COUNT);

    bool terminateEnabled = RSSystemProperties::GetTransactionTerminateEnabled();
    if (!isNonSystemAppCalling || !terminateEnabled) {
        return false;
    }
    if (totalCount > TRANSACTION_DATA_KILL_COUNT && preCount <= TRANSACTION_DATA_KILL_COUNT) {
        int res = appMgrClient->KillApplicationByUid(bundleName, uid);
        return res == AppExecFwk::RESULT_OK;
    }
    return false;
}

void RSUnmarshalThread::ClearTransactionDataStatistics()
{
    std::unique_lock<std::mutex> lock(statisticsMutex_);
    transactionDataStatistics_.clear();
}
}
