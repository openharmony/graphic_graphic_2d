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
#include "platform/common/rs_system_properties.h"
#ifdef ROSEN_OHOS
#include "render_context/render_context.h"
#endif

#ifdef RES_BASE_SCHED_ENABLE
#include "qos.h"
#endif

namespace OHOS::Rosen {

RSOffscreenRenderThread& RSOffscreenRenderThread::Instance()
{
    static RSOffscreenRenderThread instance;
    return instance;
}

RSOffscreenRenderThread::RSOffscreenRenderThread()
{
    runner_ = AppExecFwk::EventRunner::Create("RSOffscreenRender");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return;
    }
#ifdef ROSEN_OHOS
    PostTask([this]() {
        renderContext_ = std::make_shared<RenderContext>();
#ifdef RS_ENABLE_GL
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
            renderContext_->InitializeEglContext();
        }
#endif

        renderContext_->SetUpGpuContext(nullptr);
    });
#endif

#ifdef RES_BASE_SCHED_ENABLE
    PostTask([this]() {
        auto ret = OHOS::QOS::SetThreadQos(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE);
        RS_LOGI("RSOffscreenRenderThread: SetThreadQos retcode = %{public}d", ret);
    });
#endif
}

void RSOffscreenRenderThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSOffscreenRenderThread::InSertCaptureTask(NodeId nodeId, std::function<void()>& task)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    taskMap_[nodeId] = task;
}

const std::function<void()> RSOffscreenRenderThread::GetCaptureTask(NodeId nodeId)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (!taskMap_.empty()) {
        auto iter = taskMap_.find(nodeId);
        if (iter != taskMap_.end()) {
            auto task = taskMap_[nodeId];
            taskMap_.erase(nodeId);
            return task;
        }
    }
    return nullptr;
}

#ifdef ROSEN_OHOS
const std::shared_ptr<RenderContext>& RSOffscreenRenderThread::GetRenderContext()
{
    if (!renderContext_) {
        return nullptr;
    }
    return renderContext_;
}

void RSOffscreenRenderThread::CleanGrResource()
{
    PostTask([this]() {
        if (!renderContext_) {
            return;
        }
        auto grContext = renderContext_->GetDrGPUContext();
        if (grContext == nullptr) {
            RS_LOGE("RSOffscreenRenderThread::grContext is nullptr");
            return;
        }
        grContext->FreeGpuResources();
        RS_LOGD("RSOffscreenRenderThread::CleanGrResource() finished");
    });
}
#endif
}
