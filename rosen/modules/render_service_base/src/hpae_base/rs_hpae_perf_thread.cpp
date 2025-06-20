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

#include <unistd.h>
#include "hpae_base/rs_hpae_perf_thread.h"
#include "hpae_base/rs_hpae_log.h"

namespace OHOS::Rosen {

RSHpaePerfThread& RSHpaePerfThread::Instance()
{
    static RSHpaePerfThread instance;
    return instance;
}

RSHpaePerfThread::RSHpaePerfThread() : runner_(AppExecFwk::EventRunner::Create("RSHpaePerfThread"))
{
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    if (handler_) {
        handler_->PostSyncTask([this]() {
#if defined(ROSEN_OHOS)
            this->curThreadId_ = gettid();
#else
            this->curThreadId_ = 0;
#endif
        });
    }
}

std::shared_ptr<AppExecFwk::EventHandler> RSHpaePerfThread::CreateHandler()
{
    return std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void RSHpaePerfThread::PostTask(const std::function<void()>& task, int64_t delayTime)
{
    if (handler_) {
        handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSHpaePerfThread::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

} // OHOS::Rosen