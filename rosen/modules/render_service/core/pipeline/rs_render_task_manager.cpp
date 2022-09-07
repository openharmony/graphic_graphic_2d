/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifdef RS_ENABLE_GL
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#include <string>
#include <numeric>
#include "include/core/SkImage.h"
#include "include/gpu/GrBackendSurface.h"
#include "graphic_error.h"
#include "rs_trace.h"
#include "rs_render_task_manager.h"

namespace OHOS {
namespace Rosen {
#define SUPER_TASK_LOAD_LIMIT 180
#define BACKGROUND_TASK_LOAD 10
RSRenderTaskTexture::RSRenderTaskTexture(int idx, int offsetX, int offsetY)
    : taskIdx_(idx), isFinished_(false), offsetX_(offsetX), offsetY_(offsetY)
{
    eglDisplay_ = eglGetCurrentDisplay();
}
    
RSRenderTaskTexture::~RSRenderTaskTexture() {}

void RSRenderTaskTexture::SetTexture(sk_sp<SkImage> texture)
{
    std::lock_guard<std::mutex> lock(mutex_);
    texture_ = texture;
    isFinished_ = true;
    cv_.notify_all();
}

void RSRenderTaskTexture::WaitTexture(int textureIndex)
{
    std::string waitMuteName = "WaitMutex_";
    std::string waitMuteTrace = waitMuteName + std::to_string(textureIndex);
    RS_TRACE_BEGIN(waitMuteTrace);
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return isFinished_;});
    RS_TRACE_END();
    std::string waitFenceName = "waitFence_";
    std::string waitFenceTrace = waitFenceName + std::to_string(textureIndex);
    RS_TRACE_BEGIN(waitFenceTrace);
    WaitReleaseFence();
    RS_TRACE_END();
}

bool RSRenderTaskTexture::CreateFence()
{
    using CreateFenceFunc = bool (*)(std::function<bool()>);
    auto CreateFenceFuncSo = (CreateFenceFunc)RSInnovation::_s_createFence;
    
    auto create = [this]() -> bool {
        ROSEN_LOGD("eglClientWaitSyncKHR create");
        if (this->eglSync_ != EGL_NO_SYNC_KHR) {
            EGLint ret = eglClientWaitSyncKHR(this->eglDisplay_, this->eglSync_, 0, 1000000000);
            if (ret == EGL_FALSE) {
                ROSEN_LOGE("eglClientWaitSyncKHR error 0x%{public}x", eglGetError());
                return false;
            } else if (ret == EGL_TIMEOUT_EXPIRED_KHR) {
                ROSEN_LOGE("create eglClientWaitSyncKHR timeout");
                return false;
            }
            eglDestroySyncKHR(this->eglDisplay_, this->eglSync_);
        }
        this->eglSync_ = eglCreateSyncKHR(this->eglDisplay_, EGL_SYNC_FENCE_KHR, NULL);
        if (this->eglSync_ == EGL_NO_SYNC_KHR) {
            ROSEN_LOGE("eglCreateSyncKHR error 0x%{public}x", eglGetError());
            return false;
        }
        glFlush();
        return true;
    };
    
    return (*CreateFenceFuncSo)(create);
}

bool RSRenderTaskTexture::WaitReleaseFence()
{
    ROSEN_LOGD("eglClientWaitSyncKHR release");
    if (eglSync_ != EGL_NO_SYNC_KHR) {
        EGLint ret = eglClientWaitSyncKHR(eglDisplay_, eglSync_, 0, 1000000000);
        if (ret == EGL_FALSE) {
            ROSEN_LOGE("eglClientWaitSyncKHR error 0x%{public}x", eglGetError());
            return false;
        } else if (ret == EGL_TIMEOUT_EXPIRED_KHR) {
            ROSEN_LOGE("create eglClientWaitSyncKHR timeout");
            return false;
        }
        eglDestroySyncKHR(eglDisplay_, eglSync_);
    }
    eglSync_ = EGL_NO_SYNC_KHR;
    return true;
}

RSRenderTaskManager::RSRenderTaskManager()
{
    cachedSuperTask_ = std::make_unique<RSRenderSuperTask>(0);
    superTaskNum_ = 0;
    pushTaskCount_ = 0;
}

RSRenderTaskManager::~RSRenderTaskManager() {}

std::unique_ptr<RSRenderSuperTask> RSRenderTaskManager::GetTask(int threadIndex)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto pred = [this, threadIndex] { return taskList_.size() > threadIndex && taskList_[threadIndex] != nullptr;};
    cv_.wait(lock, pred);
    auto assignTask = std::move(taskList_[threadIndex]);
    taskList_[threadIndex] = nullptr;
    return assignTask;
}

void RSRenderTaskManager::CreateTaskFence(int32_t taskIdx)
{
    std::unique_lock<std::mutex> lock(mutex_);
    taskTextures_[taskIdx]->CreateFence();
}

void RSRenderTaskManager::WaitReleaseTaskFence(int32_t taskIdx)
{
    std::unique_lock<std::mutex> lock(mutex_);
    taskTextures_[taskIdx]->WaitReleaseFence();
}

void RSRenderTaskManager::SaveTexture(int taskIdx, sk_sp<SkImage> texture)
{
    std::lock_guard<std::mutex> lock(mutex_);
    taskTextures_[taskIdx]->SetTexture(texture);
}

void RSRenderTaskManager::InitTaskManager()
{
    std::unique_lock<std::mutex> lock(mutex_);
    superTaskNum_ = 0;
    pushTaskCount_ = 0;
    std::vector<std::unique_ptr<RSRenderSuperTask>>().swap(taskList_);
    std::vector<std::unique_ptr<RSRenderTaskTexture>>().swap(taskTextures_);
    cachedSuperTask_->InitalizeSuperTask();
    ManagerCallBackRegister();
}

void RSRenderTaskManager::ManagerCallBackRegister()
{
    RegisWrapAndPushSuperTaskCallBack();
}

void RSRenderTaskManager::RegisWrapAndPushSuperTaskCallBack()
{
    auto task = [this]() {
        std::unique_lock<std::mutex> lock(mutex_);
        auto texture = std::make_unique<RSRenderTaskTexture>(this->superTaskNum_, 0, 0);
        this->taskTextures_.push_back(std::move(texture));
        this->superTaskNum_++;
        auto superTask = std::make_unique<RSRenderSuperTask>(this->superTaskNum_);
        std::swap(superTask, this->cachedSuperTask_);
        this->taskList_.push_back(std::move(superTask));
        this->cv_.notify_all();
    };
    
    if (RSInnovation::_s_wrapAndPushSuperTaskRegister) {
        using WrapAndPushSuperTaskFunc = void (*)(std::function<void()>);
        auto WrapAndPushSuperTaskRegister = (WrapAndPushSuperTaskFunc)RSInnovation::_s_wrapAndPushSuperTaskRegister;
        (*WrapAndPushSuperTaskRegister)(task);
    }
}

void RSRenderTaskManager::WrapAndPushSuperTask()
{
    using WrapAndPushSuperTaskFunc = void (*)();
    auto WrapAndPushSuperTaskFuncCall = (WrapAndPushSuperTaskFunc)RSInnovation::_s_wrapAndPushSuperTask;
    (*WrapAndPushSuperTaskFuncCall)();
}

void RSRenderTaskManager::PushTask(std::unique_ptr<RSRenderTask> &&task)
{
    if (cachedSuperTask_->GetTaskSize() != 0) {
        WrapAndPushSuperTask();
    }
    cachedSuperTask_->AddTask(std::move(task));
}

void RSRenderTaskManager::LoadBalancePushTask(std::unique_ptr<RSRenderTask> &&task)
{
    uint64_t loadId = task->GetSurfaceNode()->GetId();
    
    using LoadBalancePushTaskFunc = void (*)(uint64_t, int);
    auto LoadBalancePushTask = (LoadBalancePushTaskFunc)RSInnovation::_s_loadBalancePushTask;
    (*LoadBalancePushTask)(loadId, pushTaskCount_);
    
    taskLoad_[loadId] = std::move(task);
    pushTaskCount_++;
}

bool RSRenderTaskManager::GetEnableLoadBalance()
{
    return true;
}

void RSRenderTaskManager::LoadBalance()
{
    using LoadBalanceFunc = void* (*)(bool, bool);
    auto LoadBalanceFuncCall = (LoadBalanceFunc)RSInnovation::_s_loadBalanceCall;
    renderLoad_ = *(RenderLoad*)(*LoadBalanceFuncCall)(bMainThreadUsed_, bMainThreadLoadAssigned_);
    for (uint32_t i = 0; i < renderLoad_.orderedLoadIds.size(); i++) {
        if (bMainThreadUsed_ && !bMainThreadLoadAssigned_ &&
            (renderLoad_.loadNumSum.size() > 0) && (i < renderLoad_.loadNumSum.back())) {
            auto taskNode = std::move(taskLoad_[renderLoad_.orderedLoadIds[i]]);
            surfaceNodesQueue_.push(taskNode->GetSurfaceNode());
        } else {
            if (bMainThreadUsed_ && !bMainThreadLoadAssigned_) {
                renderLoad_.loadNumSum.pop_back();
                bMainThreadLoadAssigned_ = true;
            } else if ((renderLoad_.loadNumSum.size() > 0) && (i == renderLoad_.loadNumSum.back())) {
                WrapAndPushSuperTask();
                renderLoad_.loadNumSum.pop_back();
            }
            cachedSuperTask_->AddTask(std::move(taskLoad_[renderLoad_.orderedLoadIds[i]]));
        }
    }
    if (cachedSuperTask_->GetTaskSize() > 0) {
        WrapAndPushSuperTask();
    }
    renderLoad_.isWindowUpdated = false;
}

void RSRenderTaskManager::SetSubThreadRenderLoad(uint32_t threadIdx, uint64_t loadId, float costing)
{
    using SetSubThreadRenderLoadFunc = void (*)(uint32_t, uint64_t, float);
    auto SetSubThreadRenderLoad = (SetSubThreadRenderLoadFunc)RSInnovation::_s_setSubThreadRenderLoad;
    
    (*SetSubThreadRenderLoad)(threadIdx, loadId, costing);
}

void RSRenderTaskManager::MergeTextures(SkCanvas* canvas)
{
    RS_TRACE_BEGIN("MergeTextures");
    for (int i = 0; i < superTaskNum_; i++) {
        taskTextures_[i]->WaitTexture(i);
        auto name = "Merge Texture" + std::to_string(i);
        RS_TRACE_BEGIN(name);
        auto texture = taskTextures_[i]->GetTexture();
        if (!texture) {
            continue;
        }
        canvas->drawImage(texture, 0, 0);
        RS_TRACE_END();
    }
    RS_TRACE_END();
}

void RSRenderTaskManager::DeleteTextures()
{
    for (int32_t i = 0; i < superTaskNum_; ++i) {
        if (taskTextures_[i]->GetTexture()) {
            taskTextures_[i]->SetTexture(nullptr);
        }
    }
}

void RSRenderTaskManager::ResetLoadBalanceParam()
{
    using ClearLoadFunc = void (*)();
    auto ClearLoadFuncCall = (ClearLoadFunc)RSInnovation::_s_clearLoad;
    (*ClearLoadFuncCall)();
    renderLoad_.orderedLoadIds.clear();
    
    pushTaskCount_ = 0;
}

void RSRenderTaskManager::UninitTaskManager()
{
    DeleteTextures();
    ResetLoadBalanceParam();
    ResetMainThreadRenderingParam();
}

void RSRenderTaskManager::ResetMainThreadRenderingParam()
{
    bMainThreadLoadAssigned_ = false;
    while (surfaceNodesQueue_.size() > 0) {
        surfaceNodesQueue_.pop();
    }
}
} // namespace Rosen
} // namespace OHOS
#endif