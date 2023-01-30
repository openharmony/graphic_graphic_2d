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

#include "offscreen_render/rs_offscreen_render_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSOffscreenRenderThread& RSOffscreenRenderThread::Instance()
{
    static RSOffscreenRenderThread instance;
    return instance;
}

void RSOffscreenRenderThread::Start()
{
    std::lock_guard<std::mutex> lock(mutex_);
    runner_ = AppExecFwk::EventRunner::Create("RSOffscreenRender");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void RSOffscreenRenderThread::PostTask(const std::function<void()>& task)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSOffscreenRenderThread::Stop()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handler_) {
            handler_->RemoveAllEvents();
            handler_ = nullptr;
        }

        if (runner_) {
            runner_->Stop();
        }
    }

    ROSEN_LOGD("RSOffscreenRenderThread stopped");
}
}
