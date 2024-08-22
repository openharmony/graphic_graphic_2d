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

#include "hgm_log.h"
#include "hgm_task_handle_thread.h"
#include "xcollie/watchdog.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t WATCHDOG_TIMEVAL = 5000;
}

HgmTaskHandleThread& HgmTaskHandleThread::Instance()
{
    static HgmTaskHandleThread instance;
    return instance;
}

HgmTaskHandleThread::HgmTaskHandleThread() : runner_(AppExecFwk::EventRunner::Create("HgmTaskHandleThread"))
{
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread("HgmTaskHandle", handler_, WATCHDOG_TIMEVAL);
    if (ret != 0) {
        HGM_LOGW("Add watchdog thread failed");
    }
}

std::shared_ptr<AppExecFwk::EventHandler> HgmTaskHandleThread::CreateHandler()
{
    return std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void HgmTaskHandleThread::PostTask(const std::function<void()>& task, int64_t delayTime)
{
    if (handler_) {
        handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

bool HgmTaskHandleThread::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        return handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::VIP);
    }
    return false;
}

void HgmTaskHandleThread::PostEvent(std::string eventId, const std::function<void()>& task, int64_t delayTime)
{
    if (handler_) {
        handler_->PostTask(task, eventId, delayTime);
    }
}

void HgmTaskHandleThread::RemoveEvent(std::string eventId)
{
    if (handler_) {
        handler_->RemoveTask(eventId);
    }
}
}
