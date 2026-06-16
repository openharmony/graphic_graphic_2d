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

#include "feature/hwc/hpae_offline/rs_gpu_offline_thread.h"
#include "feature/hwc/hpae_offline/rs_offline_util.h"
#include "pipeline/render_thread/rs_render_engine.h"
#ifdef RES_SCHED_ENABLE
#include "qos.h"
#endif

namespace OHOS {
namespace Rosen {

RSGPUOfflineThread::RSGPUOfflineThread()
{
    RS_LOGD("RSGPUOfflineThread::Constructed");
}

RSGPUOfflineThread::~RSGPUOfflineThread()
{
    if (handler_ != nullptr) {
        handler_->PostSyncTask([this]() {
            Stop();
        });
    }
    RS_LOGD("RSGPUOfflineThread::Destructed");
}

bool RSGPUOfflineThread::Start()
{
    RS_LOGD("RSGPUOfflineThread::Start");
    runner_ = AppExecFwk::EventRunner::Create("RSGPUOfflineThread");
    if (!runner_) {
        RS_LOGE("RSGPUOfflineThread::Start runner null");
        return false;
    }
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();

    auto result = handler_->PostSyncTask([this]() {
#ifdef RES_SCHED_ENABLE
        auto ret = OHOS::QOS::SetThreadQos(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE);
        RS_LOGI("RSGPUOfflineThread: SetThreadQos retcode = %{public}d", ret);
#endif
        InitRenderEngine();
    });
    if (!result) {
        RS_LOGE("RSGPUOfflineThread::Init render engine failed");
        return false;
    }
    isInitialized_ = true;
    RS_LOGD("RSGPUOfflineThread::Started successfully");
    return true;
}

void RSGPUOfflineThread::Stop()
{
    RS_LOGD("RSGPUOfflineThread::Stop");
    isInitialized_ = false;
    if (renderEngine_) {
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
        auto renderContext = renderEngine_->GetRenderContext();
        if (renderContext) {
            auto drGpuContext = renderContext->GetDrGPUContext();
            if (drGpuContext) {
                drGpuContext->FreeGpuResources();
                drGpuContext->PurgeUnlockedResources(true);
            }
        }
#endif
    }
}

void RSGPUOfflineThread::InitRenderEngine()
{
    RS_LOGD("RSGPUOfflineThread::InitRenderEngine");
    renderEngine_ = std::make_shared<RSRenderEngine>();
    if (renderEngine_ == nullptr) {
        RS_LOGE("RSGPUOfflineThread::RenderEngine_ is null");
        return;
    }
    renderEngine_->Init();
    RS_LOGD("RSGPUOfflineThread::RenderEngine_ init done");
}

bool RSGPUOfflineThread::PostTask(const std::function<void()>& task, const std::string& name)
{
    if (!isInitialized_) {
        Start();
    }
    if (!isInitialized_) {
        RS_LOGE("RSGPUOfflineThread::Not initialized");
        return false;
    }
    if (handler_) {
        auto result = handler_->PostImmediateTask(task, name);
        if (!result) {
            RS_LOGE("RSGPUOfflineThread::PostTask failed. name:%{public}s", name.c_str());
            return false;
        }
        return true;
    }
    return false;
}

} // namespace Rosen
} // namespace OHOS