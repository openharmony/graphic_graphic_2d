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

#include "rs_render_composer_client.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t COMPOSER_THREAD_TASK_NUM = 2;
constexpr uint32_t WAIT_FOR_COMPOSER_THREAD_TASK_TIMEOUT = 3000;
};

RSRenderComposerClient::RSRenderComposerClient(
    const sptr<IRSRenderToComposerConnection>& renderToComposerConn,
    const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent)
{
    rsComposerContext_ = std::make_shared<RSComposerContext>(renderToComposerConn);
    renderToComposerConn_ = renderToComposerConn;
    rsVsyncManagerAgent_ = rsVsyncManagerAgent;
}

std::shared_ptr<RSRenderComposerClient> RSRenderComposerClient::Create(
    const sptr<IRSRenderToComposerConnection>& renderToComposerConn,
    const sptr<IRSComposerToRenderConnection>& composerToRenderConn,
    const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent)
{
    RS_TRACE_NAME_FMT("RSRenderComposerClient::Create");
    if (renderToComposerConn != nullptr) {
        renderToComposerConn->SetComposerToRenderConnection(composerToRenderConn);
    }
    return std::make_shared<RSRenderComposerClient>(renderToComposerConn, rsVsyncManagerAgent);
}

void RSRenderComposerClient::SetOutput(const std::shared_ptr<HdiOutput>& output)
{
    output_ = output;
}

std::shared_ptr<HdiOutput> RSRenderComposerClient::GetOutput() const
{
    return output_;
}

std::shared_ptr<RSLayer> RSRenderComposerClient::GetRSLayer(RSLayerId rsLayerId)
{
    return rsComposerContext_->GetRSLayer(rsLayerId);
}

void RSRenderComposerClient::CommitLayers(ComposerInfo& composerInfo)
{
    std::unique_lock<std::mutex> lock(clientMutex_);
    if (!WaitComposerThreadTaskExecute(lock)) {
        RS_LOGW("CommitLayers task has too many to Execute TaskNum:[%{public}d]", GetUnExecuteTaskNum());
    }
    IncUnExecuteTaskNum();
    composerInfo.pipelineParam = pipelineParam_;
    pipelineParam_.ResetSurfaceFpsOp();
    if (!rsComposerContext_->CommitLayers(composerInfo)) {
        SubUnExecuteTaskNum();
        RS_LOGE("CommitRSLayer failed, restore task count");
    }
    if (rsVsyncManagerAgent_ != nullptr) {
        rsVsyncManagerAgent_->SetHardwareTaskNum(GetUnExecuteTaskNum());
    }
}

PipelineParam RSRenderComposerClient::GetPipelineParam()
{
    return pipelineParam_;
}

void RSRenderComposerClient::UpdatePipelineParam(const PipelineParam& pipelineParam)
{
    pipelineParam_ = pipelineParam;
}

void RSRenderComposerClient::RegisterOnReleaseLayerBuffersCB(OnReleaseLayerBuffersCB cb)
{
    rsComposerContext_->RegisterOnReleaseLayerBuffersCB(cb);
}

void RSRenderComposerClient::ReleaseLayerBuffers(uint64_t screenId,
    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>>& timestampVec,
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec)
{
    std::unique_lock<std::mutex> lock(clientMutex_);
    SubUnExecuteTaskNum();
    if (GetUnExecuteTaskNum() <= COMPOSER_THREAD_TASK_NUM) {
        NotifyComposerThreadCanExecuteTask();
    }
    rsComposerContext_->ReleaseLayerBuffers(screenId, timestampVec, releaseBufferFenceVec);
}

std::shared_ptr<RSComposerContext> RSRenderComposerClient::GetComposerContext()
{
    return rsComposerContext_;
}

void RSRenderComposerClient::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    rsComposerContext_->CleanLayerBufferBySurfaceId(surfaceId);
}

void RSRenderComposerClient::ClearFrameBuffers()
{
    std::lock_guard<std::mutex> lock(clientMutex_);
    rsComposerContext_->ClearFrameBuffers();
    isPreAllocProtectedFrameBuffer_ = false;
}

int RSRenderComposerClient::GetAccumulatedBufferCount()
{
    return std::max(acquiredBufferCount_.load() - 1, 0);
}

void RSRenderComposerClient::ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds)
{
    if (renderToComposerConn_ != nullptr) {
        renderToComposerConn_->ClearRedrawGPUCompositionCache(bufferIds);
    }
}

void RSRenderComposerClient::SetScreenBacklight(uint32_t level)
{
    if (renderToComposerConn_ != nullptr) {
        renderToComposerConn_->SetScreenBacklight(level);
    }
}

uint32_t RSRenderComposerClient::GetUnExecuteTaskNum()
{
    return unExecuteTaskNum_.load();
}

void RSRenderComposerClient::IncUnExecuteTaskNum()
{
    ++acquiredBufferCount_;
    RS_TRACE_NAME_FMT("Inc Acq BufferCount %d", acquiredBufferCount_.load());
    unExecuteTaskNum_.fetch_add(1);
}

void RSRenderComposerClient::SubUnExecuteTaskNum()
{
    --acquiredBufferCount_;
    RS_TRACE_NAME_FMT("Dec Acq BufferCount %d", acquiredBufferCount_.load());
    unExecuteTaskNum_.fetch_sub(1);
}

bool RSRenderComposerClient::WaitComposerThreadTaskExecute(std::unique_lock<std::mutex>& lock)
{
#ifdef RS_ENABLE_GPU
    RS_TRACE_NAME_FMT("RSRenderComposerClient::WaitComposerThreadTaskExecute task num: %d", GetUnExecuteTaskNum());
    return composerThreadTaskCond_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_COMPOSER_THREAD_TASK_TIMEOUT),
        [this]() { return GetUnExecuteTaskNum() <= COMPOSER_THREAD_TASK_NUM; });
#else
    return false;
#endif
}

void RSRenderComposerClient::NotifyComposerThreadCanExecuteTask()
{
    RS_TRACE_NAME("RSRenderComposerClient::NotifyComposerThreadCanExecuteTask");
    composerThreadTaskCond_.notify_one();
}

void RSRenderComposerClient::DumpLayersInfo(std::string &dumpString)
{
    rsComposerContext_->DumpLayersInfo(dumpString);
}

void RSRenderComposerClient::DumpCurrentFrameLayers()
{
    rsComposerContext_->DumpCurrentFrameLayers();
}

void RSRenderComposerClient::ConvertScreenInfo(const ScreenInfo& screenInfo, ComposerScreenInfo& composerScreenInfo)
{
    composerScreenInfo.id = screenInfo.id;
    composerScreenInfo.width = screenInfo.width;
    composerScreenInfo.height = screenInfo.height;
    composerScreenInfo.phyWidth = screenInfo.phyWidth;
    composerScreenInfo.phyHeight = screenInfo.phyHeight;
    composerScreenInfo.isSamplingOn = screenInfo.isSamplingOn;
    composerScreenInfo.samplingTranslateX = screenInfo.samplingTranslateX;
    composerScreenInfo.samplingTranslateY = screenInfo.samplingTranslateY;
    composerScreenInfo.samplingScale = screenInfo.samplingScale;
    composerScreenInfo.activeRect = screenInfo.activeRect;
    composerScreenInfo.maskRect = screenInfo.maskRect;
    composerScreenInfo.reviseRect = screenInfo.reviseRect;
}

void RSRenderComposerClient::PreAllocProtectedFrameBuffers(const sptr<SurfaceBuffer>& buffer)
{
    std::lock_guard<std::mutex> lock(clientMutex_);
    if (isPreAllocProtectedFrameBuffer_) {
        return;
    }
    rsComposerContext_->PreAllocProtectedFrameBuffers(buffer);
    isPreAllocProtectedFrameBuffer_ = true;
}
} // namespace Rosen
} // namespace OHOS