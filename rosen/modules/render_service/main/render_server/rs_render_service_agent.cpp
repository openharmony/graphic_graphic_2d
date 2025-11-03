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

#include "rs_render_service_agent.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderServiceAgent"

namespace OHOS {
namespace Rosen {
void RSRenderServiceAgent::PostTaskImmediate(const std::function<void()>& task)
{
    auto& handler = renderService_.handler_;
    if (handler == nullptr) {
        return;
    }
    handler->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void RSRenderServiceAgent::PostTaskImmediateInPlace(const std::function<void()>& task)
{
    auto& handler = renderService_.handler_;
    if (handler == nullptr) {
        return;
    }
    if (handler->GetEventRunner()->IsCurrentRunnerThread()) {
        task();
        return;
    }
    handler->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void RSRenderServiceAgent::PostSyncTaskImmediate(const std::function<void()>& task)
{
    auto& handler = renderService_.handler_;
    if (handler == nullptr) {
        return;
    }
    handler->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}
} // namespace Rosen
} // namespace OHOS
