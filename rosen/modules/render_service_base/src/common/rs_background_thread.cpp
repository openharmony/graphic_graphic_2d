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

#include "common/rs_background_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSBackgroundThread& RSBackgroundThread::Instance()
{
    static RSBackgroundThread instance;
    return instance;
}

RSBackgroundThread::RSBackgroundThread()
{
    runner_ = AppExecFwk::EventRunner::Create("RSBackgroundThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void RSBackgroundThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}
}
