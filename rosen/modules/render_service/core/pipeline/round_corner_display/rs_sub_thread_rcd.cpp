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

#include "rs_sub_thread_rcd.h"
#include <string>
#include "memory/rs_tag_tracker.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"
#ifdef RES_SCHED_ENABLE
#include "qos.h"
#endif

namespace OHOS {
namespace Rosen {
RSSubThreadRCD::~RSSubThreadRCD()
{
    RS_LOGD("~RSSubThreadRCD");
}

void RSSubThreadRCD::Start(RenderContext* context)
{
    if (renderContext_ != nullptr) {
        RS_LOGD("RSSubThreadRCD already start!");
        return;
    }
    RS_LOGD("RSSubThreadRCD Started");
    std::string name = "RoundCornerDisplaySubThread";
    runner_ = AppExecFwk::EventRunner::Create(name);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);

#ifdef RES_SCHED_ENABLE
    PostTask([this]() {
        auto ret = OHOS::QOS::SetThreadQos(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE);
        RS_LOGI("RoundCornerDisplay: SetThreadQos retcode = %{public}d", ret);
    });
#endif

    renderContext_ = context;
}

void RSSubThreadRCD::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}
} // namespace Rosen
} // namespace OHOS