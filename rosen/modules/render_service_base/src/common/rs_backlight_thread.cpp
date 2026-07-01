/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "common/rs_backlight_thread.h"

#ifdef RES_BASE_SCHED_ENABLE
#include "qos.h"

#include "platform/common/rs_log.h"
#endif

namespace OHOS::Rosen {
RSBacklightThread& RSBacklightThread::Instance()
{
    static RSBacklightThread instance;
    return instance;
}

RSBacklightThread::RSBacklightThread()
{
    runner_ = AppExecFwk::EventRunner::Create("RSBacklightThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
#ifdef RES_BASE_SCHED_ENABLE
    PostTask([]() {
        auto ret = OHOS::QOS::SetThreadQos(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE);
        RS_LOGI("RSBacklightThread: SetThreadQos retcode = %{public}d", ret);
    });
#endif
}

void RSBacklightThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}
} // namespace OHOS::Rosen