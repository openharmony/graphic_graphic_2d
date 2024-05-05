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

#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_transaction.h"

namespace OHOS {
namespace Rosen {
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

RSSyncTransactionController::RSSyncTransactionController() : rsTransaction_(std::make_shared<RSTransaction>()) {}

std::shared_ptr<RSTransaction> RSSyncTransactionController::GetRSTransaction()
{
    if (!needCloseSync_) {
        return nullptr;
    }
    return rsTransaction_;
}

void RSSyncTransactionController::OpenSyncTransaction()
{
    if (!RSSystemProperties::GetSyncTransactionEnabled()) {
        return;
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (needCloseSync_) {
            return;
        }
        needCloseSync_ = true;
    }
    ROSEN_LOGD("RS sync transaction controller OpenSyncTransaction");
    if (rsTransaction_) {
        rsTransaction_->OpenSyncTransaction();
    }
}

void RSSyncTransactionController::CloseSyncTransaction()
{
    if (!needCloseSync_) {
        return;
    }

    ROSEN_LOGD("RS sync transaction controller CloseSyncTransaction");
    if (rsTransaction_) {
        rsTransaction_->CloseSyncTransaction();
    }
    {
        std::unique_lock<std::mutex> lock(mutex_);
        needCloseSync_ = false;
    }
}
} // namespace Rosen
} // namespace OHOS
