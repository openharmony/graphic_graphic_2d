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

#include "pipeline/rs_unmarshal_thread.h"

#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_data.h"
#include "rs_frame_report.h"
#include "rs_profiler.h"

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

void RSUnmarshalThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSUnmarshalThread::RecvParcel(std::shared_ptr<MessageParcel>& parcel)
{
    if (!handler_) {
        RS_LOGE("RSUnmarshalThread::RecvParcel handler_ is nullptr");
        return;
    }
    bool isPendingUnmarshal = (parcel->GetDataSize() > MIN_PENDING_REQUEST_SYNC_DATA_SIZE);
    RSTaskMessage::RSTask task = [this, parcel = parcel, isPendingUnmarshal]() {
        SetFrameParam(REQUEST_FRAME_AWARE_ID, REQUEST_FRAME_AWARE_LOAD, REQUEST_FRAME_AWARE_NUM, 0);
        SetFrameLoad(REQUEST_FRAME_AWARE_LOAD);
        auto transData = RSBaseRenderUtil::ParseTransactionData(*parcel);
        SetFrameLoad(REQUEST_FRAME_STANDARD_LOAD);
        if (!transData) {
            return;
        }
        RS_PROFILER_ON_PARCEL_RECEIVE(parcel.get(), transData.get());
        {
            std::lock_guard<std::mutex> lock(transactionDataMutex_);
            cachedTransactionDataMap_[transData->GetSendingPid()].emplace_back(std::move(transData));
        }
        if (isPendingUnmarshal) {
            RSMainThread::Instance()->RequestNextVSync();
        }
    };
    {
        PostTask(task);
        /* a task has been posted, it means cachedTransactionDataMap_ will not been empty.
         * so set willHaveCachedData_ to true
         */
        std::lock_guard<std::mutex> lock(transactionDataMutex_);
        willHaveCachedData_ = true;
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
}
