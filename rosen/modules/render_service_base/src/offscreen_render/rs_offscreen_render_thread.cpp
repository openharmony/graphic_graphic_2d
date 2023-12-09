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
#ifdef ROSEN_OHOS
#include "render_context/render_context.h"
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
#ifdef ROSEN_OHOS
    PostTask([this]() {
        renderContext_ = std::make_shared<RenderContext>();
        renderContext_->InitializeEglContext();
#if defined(RS_ENABLE_VK)
        renderContext_->SetUpGrContext(nullptr);
#else
        renderContext_->SetUpGrContext(nullptr);
#endif // RS_ENABLE_VK
    });
#endif
}

void RSOffscreenRenderThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

#ifdef ROSEN_OHOS
const std::shared_ptr<RenderContext>& RSOffscreenRenderThread::GetRenderContext()
{
    return renderContext_;
}

void RSOffscreenRenderThread::CleanGrResource()
{
    PostTask([this]() {
        auto grContext = renderContext_->GetGrContext();
        if (grContext == nullptr) {
            RS_LOGE("RSOffscreenRenderThread::grContext is nullptr");
            return;
        }
        grContext->freeGpuResources();
        RS_LOGI("RSOffscreenRenderThread::CleanGrResource() finished");
    });
}
#endif
}
