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

#include "common/rs_upload_texture_thread.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "pipeline/rs_task_dispatcher.h"
#include "rs_trace.h"
#ifdef NEW_SKIA
#include "src/gpu/GrSurfaceProxy.h"
#endif
#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL)
#include "render_context/render_context.h"
#endif

namespace OHOS::Rosen {
RSUploadTextureThread& RSUploadTextureThread::Instance()
{
    static RSUploadTextureThread instance;
    return instance;
}

RSUploadTextureThread::RSUploadTextureThread()
{
    runner_ = AppExecFwk::EventRunner::Create("RSUploadTextureThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void RSUploadTextureThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSUploadTextureThread::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSUploadTextureThread::PostTask(const std::function<void()>& task, const std::string& name)
{
    if (handler_) {
        handler_->PostImmediateTask(task, name);
    }
}

void RSUploadTextureThread::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask(name);
    }
}
 
bool RSUploadTextureThread::IsEnable() const
{
    return uploadProperity_ && isTargetPlatform_;
}

void RSUploadTextureThread::OnRenderEnd()
{
    if (IsEnable()) {
        std::unique_lock<std::mutex> lock(uploadTaskMutex_);
        enableTime_ = true;
        uploadTaskCond_.notify_all();
    }
}

void RSUploadTextureThread::OnProcessBegin()
{
    if (IsEnable()) {
        std::unique_lock<std::mutex> lock(uploadTaskMutex_);
        enableTime_ = false;
    }
    frameCount_.fetch_add(1);
}

int64_t RSUploadTextureThread::GetFrameCount() const
{
    return frameCount_.load();
}

void RSUploadTextureThread::WaitUntilRenderEnd()
{
    RS_TRACE_NAME("Waitfor render_service finish");
    std::unique_lock<std::mutex> lock(uploadTaskMutex_);
    uploadTaskCond_.wait(lock, [this]() { return enableTime_; });
}

bool RSUploadTextureThread::TaskIsValid(int64_t count)
{
    if (count < frameCount_.load()) {
        return false;
    }
    WaitUntilRenderEnd();
    return true;
}

bool RSUploadTextureThread::ImageSupportParallelUpload(int w, int h)
{
    return (w < IMG_WIDTH_MAX) && (h < IMG_HEIGHT_MAX);
}

#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL)
#endif
}
