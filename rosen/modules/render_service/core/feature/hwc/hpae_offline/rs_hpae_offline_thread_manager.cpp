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

#include "rs_hpae_offline_thread_manager.h"
#include "rs_hpae_offline_util.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"
#ifdef RES_SCHED_ENABLE
#include "qos.h"
#endif

namespace OHOS::Rosen {
bool RSHpaeOfflineThreadManager::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        return true;
    }
    return false;
}

void RSHpaeOfflineThreadManager::CreateThread()
{
    if (!runner_ || !handler_) {
        runner_ = AppExecFwk::EventRunner::Create("RSHpaeOfflineThread");
        handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
        if (runner_) {
            runner_->Run();
        }
#ifdef RES_SCHED_ENABLE
        PostTask([this]() {
            auto ret = OHOS::QOS::SetThreadQos(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE);
            RS_OFFLINE_LOGI("RSHpaeOfflineThreadManager: SetThreadQos ret = %{public}d", ret);
        });
#endif
    }
}

void RSHpaeOfflineThreadManager::DestroyThread()
{
    if (handler_) {
        handler_->RemoveAllEvents();
        handler_ = nullptr;
    }
    if (runner_) {
        runner_->Stop();
        runner_ = nullptr;
    }
}
}