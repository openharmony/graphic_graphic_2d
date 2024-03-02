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

#include "benchmarks/rs_recording_thread.h"

#include <thread>
#include "common/rs_thread_handler.h"
#include "platform/common/rs_system_properties.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "transaction/rs_marshalling_helper.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

namespace OHOS::Rosen {
RSRecordingThread::~RSRecordingThread()
{
    RS_LOGI("RSRecordingThread::~RSRecordingThread()");
    PostTask([this]() {
        DestroyShareEglContext();
    });
}

RSRecordingThread &RSRecordingThread::Instance(RenderContext* context)
{
    static RSRecordingThread instance(context);
    return instance;
}

void RSRecordingThread::Start()
{
    runner_ = AppExecFwk::EventRunner::Create("RSRecordingThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    PostTask([this]() {
        grContext_ = CreateShareGrContext();
    });
}

std::shared_ptr<Drawing::GPUContext> RSRecordingThread::CreateShareGrContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        CreateShareEglContext();
        Drawing::GPUContextOptions options;
        auto handler = std::make_shared<MemoryHandler>();
        auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        auto size = glesVersion ? strlen(glesVersion) : 0;
        handler->ConfigureContext(&options, glesVersion, size);
        if (!gpuContext->BuildFromGL(options)) {
            RS_LOGE("nullptr gpuContext is null");
            return nullptr;
        }
        return gpuContext;
    }
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        Drawing::GPUContextOptions options;
        auto handler = std::make_shared<MemoryHandler>();
        std::string vulkanVersion = RsVulkanContext::GetSingleton().GetVulkanVersion();
        auto size = vulkanVersion.size();
        handler->ConfigureContext(&options, vulkanVersion.c_str(), size);
        if (!gpuContext->BuildFromVK(RsVulkanContext::GetSingleton().GetGrVkBackendContext(), options)) {
            RS_LOGE("nullptr gpuContext is null");
            return nullptr;
        }
        return gpuContext;
    }
#endif
    return nullptr;
}

void RSRecordingThread::CreateShareEglContext()
{
    if (renderContext_ == nullptr) {
        RS_LOGE("renderContext_ is nullptr");
        return;
    }
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return;
    }
    eglShareContext_ = renderContext_->CreateShareContext();
    if (eglShareContext_ == EGL_NO_CONTEXT) {
        RS_LOGE("eglShareContext_ is EGL_NO_CONTEXT");
        return;
    }
    if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
        RS_LOGE("eglMakeCurrent failed");
        return;
    }
#endif
}

void RSRecordingThread::DestroyShareEglContext()
{
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return;
    }
    if (renderContext_ != nullptr) {
        eglDestroyContext(renderContext_->GetEGLDisplay(), eglShareContext_);
        eglShareContext_ = EGL_NO_CONTEXT;
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
#endif
}
bool RSRecordingThread::IsIdle()
{
    return handler_ && handler_->IsIdle();
}

void RSRecordingThread::PostTask(const std::function<void()> &task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

bool RSRecordingThread::CheckAndRecording()
{
    if (!handler_) {
        Start();
    }
    RSTaskMessage::RSTask task = [this]() {
        std::string line = "RSRecordingThread::CheckAndRecording curDumpFrame = " + std::to_string(curDumpFrame_) +
            ", dumpFrameNum = " + std::to_string(dumpFrameNum_);
        RS_LOGD("%{public}s", line.c_str());
        mode_ = RecordingMode(RSSystemProperties::GetRecordingEnabled());
        // init curDumpFrame
        if (GetRecordingEnabled() && curDumpFrame_ == 0) {
            RS_TRACE_NAME(line);
            dumpFrameNum_ = RSSystemProperties::GetDumpFrameNum();
            fileDir_ = RSSystemProperties::GetRecordingFile();
            if (access(fileDir_.c_str(), F_OK) != 0) {
                mkdir(fileDir_.c_str(), (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH));
            }
        }
    };
    PostTask(task);
    return GetRecordingEnabled();
}

void RSRecordingThread::FinishRecordingOneFrameTask(RecordingMode modeSubThread)
{
    for (int curFrameIndex = 0; curFrameIndex < dumpFrameNum_; curFrameIndex++) {
        std::shared_ptr<MessageParcel> messageParcel = std::make_shared<MessageParcel>();
        std::string opsDescription = "drawing ops no description";
        if (modeSubThread == RecordingMode::HIGH_SPPED_RECORDING) {
            RS_LOGI("RSRecordingThread::High speed!");
            messageParcel->SetMaxCapacity(RECORDING_PARCEL_CAPCITY);
            RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
            RSMarshallingHelper::Marshalling(*messageParcel, drawCmdListVec_[curFrameIndex]);
            RSMarshallingHelper::EndNoSharedMem();
            opsDescription = drawCmdListVec_[curFrameIndex]-> GetOpsWithDesc();
        } else if (modeSubThread == RecordingMode::LOW_SPEED_RECORDING) {
            messageParcel = messageParcelVec_[curFrameIndex];
            opsDescription = opsDescriptionVec_[curFrameIndex];
        }
        OHOS::Rosen::Benchmarks::WriteMessageParcelToFile(messageParcel, opsDescription, curFrameIndex, fileDir_);
    }
    drawCmdListVec_.clear();
    messageParcelVec_.clear();
    opsDescriptionVec_.clear();
    curDumpFrame_ = 0;
    dumpFrameNum_ = 0;
    fileDir_ = "";
    RSSystemProperties::SetRecordingDisenabled();
    RS_LOGD("RSRecordingThread::FinishRecordingOneFrame isRecordingEnabled = false");
}

void RSRecordingThread::FinishRecordingOneFrame()
{
    std::string line = "RSRecordingThread::FinishRecordingOneFrame curDumpFrame = " + std::to_string(curDumpFrame_) +
        ", dumpFrameNum = " + std::to_string(dumpFrameNum_);
    RS_LOGD("%{public}s", line.c_str());
    RS_TRACE_NAME(line);
    if (curDumpFrame_ < dumpFrameNum_) {
        curDumpFrame_++;
        return;
    }
    auto modeSubThread = mode_;
    mode_ = RecordingMode::STOP_RECORDING;
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    RSTaskMessage::RSTask task = [this, modeSubThread]() {
        FinishRecordingOneFrameTask(modeSubThread);
    };
    PostTask(task);
#else
    FinishRecordingOneFrameTask(modeSubThread);
#endif
}

void RSRecordingThread::RecordingToFile(const std::shared_ptr<Drawing::DrawCmdList>& drawCmdList)
{
    if (curDumpFrame_ < 0) {
        return;
    }
    if (mode_ == RecordingMode::HIGH_SPPED_RECORDING) {
        drawCmdListVec_.push_back(drawCmdList);
    } else if (mode_ == RecordingMode::LOW_SPEED_RECORDING) {
        std::shared_ptr<MessageParcel> messageParcel = std::make_shared<MessageParcel>();
        messageParcel->SetMaxCapacity(RECORDING_PARCEL_CAPCITY);
        RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
        RSMarshallingHelper::Marshalling(*messageParcel, drawCmdList);
        RSMarshallingHelper::EndNoSharedMem();
        opsDescriptionVec_.push_back(drawCmdList->GetOpsWithDesc());
        messageParcelVec_.push_back(messageParcel);
    }

    FinishRecordingOneFrame();
}
}
