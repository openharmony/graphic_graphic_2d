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

#include "hgm_task_handle_thread.h"

namespace OHOS::Rosen {
HgmTaskHandleThread& HgmTaskHandleThread::Instance()
{
    static HgmTaskHandleThread instance;
    return instance;
}

HgmTaskHandleThread::HgmTaskHandleThread() : runner_(AppExecFwk::EventRunner::Create("HgmTaskHandleThread"))
{
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void HgmTaskHandleThread::PostTask(const std::function<void()>& task, int64_t delayTime)
{
    if (handler_) {
        handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}
}
