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

RSRenderComposerClient::RSRenderComposerClient(bool isMultiProcess, const sptr<IRSRenderToComposerConnection>& conn)
    : isMultiProcess_(isMultiProcess)
{
    rsLayerContext_ = std::make_shared<RSLayerContext>();
    rsLayerContext_->SetRenderComposerClientConnection(conn);
    connection_ = conn;
}

std::shared_ptr<RSRenderComposerClient> RSRenderComposerClient::Create(bool isMultiProcess,
    const sptr<IRSRenderToComposerConnection>& conn)
{
    RS_TRACE_NAME_FMT("RSRenderComposerClient::Create");
    return std::make_shared<RSRenderComposerClient>(isMultiProcess, conn);
}

void RSRenderComposerClient::InitRsVsyncManagerAgent(const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent)
{
    rsVsyncManagerAgent_ = rsVsyncManagerAgent;
}

void RSRenderComposerClient::AddRSLayer(const std::shared_ptr<RSLayer>& rsLayer)
{
    std::unique_lock<std::mutex> lock(clientMutex_);
    rsLayerContext_->AddRSLayer(rsLayer);
}

void RSRenderComposerClient::RemoveRSLayer(RSLayerId layerId)
{
    std::unique_lock<std::mutex> lock(clientMutex_);
    rsLayerContext_->RemoveRSLayer(layerId);
}

void RSRenderComposerClient::ClearAllRSLayers()
{
    RS_TRACE_NAME_FMT("RSRenderComposerClient::ClearAllLayers");
	std::unique_lock<std::mutex> lock(clientMutex_);
    rsLayerContext_->ClearAllRSLayers();
}

std::shared_ptr<RSLayer> RSRenderComposerClient::GetRSLayer(RSLayerId rsLayerId)
{
    std::unique_lock<std::mutex> lock(clientMutex_);
    return rsLayerContext_->GetRSLayer(rsLayerId);
}

void RSRenderComposerClient::CommitRSLayer(CommitLayerInfo& commitLayerInfo)
{
    std::unique_lock<std::mutex> lock(clientMutex_);
    if (!WaitComposerThreadTaskExecute(lock)) {
        RS_LOGW("CommitRSLayer task has too many to Execute TaskNum:[%{public}d]", GetUnExecuteTaskNum());
    }
    IncUnExecuteTaskNum();
    commitLayerInfo.pipelineParam = pipelineParam_;
    pipelineParam_.ResetSurfaceFpsOp();
    if (rsVsyncManagerAgent_ != nullptr) {
        rsVsyncManagerAgent_->SetHardwareTaskNum(GetUnExecuteTaskNum());
    }
    rsLayerContext_->CommitRSLayer(commitLayerInfo);
}

PipelineParam RSRenderComposerClient::GetPipelineParam()
{
    std::unique_lock<std::mutex> lock(clientMutex_);
    return pipelineParam_;
}

void RSRenderComposerClient::UpdatePipelineParam(const PipelineParam& pipelineParam)
{
    std::unique_lock<std::mutex> lock(clientMutex_);
    pipelineParam_ = pipelineParam;
}

bool RSRenderComposerClient::RegistOnBufferReleaseFunc(OnBufferReleaseFunc onBufferReleaseFunc)
{
    std::unique_lock<std::mutex> lock(clientMutex_);
    return rsLayerContext_->RegistOnBufferReleaseFunc(onBufferReleaseFunc);
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
    rsLayerContext_->ReleaseLayerBuffers(screenId, timestampVec, releaseBufferFenceVec);

}

std::shared_ptr<RSLayerContext> RSRenderComposerClient::GetRSLayerContext() const
{
    return rsLayerContext_;
}

void RSRenderComposerClient::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    std::lock_guard<std::mutex> lock(clientMutex_);
    rsLayerContext_->CleanLayerBufferBySurfaceId(surfaceId);
}

void RSRenderComposerClient::ClearFrameBuffers()
{
    std::lock_guard<std::mutex> lock(clientMutex_);
    rsLayerContext_->ClearFrameBuffers();
}

int RSRenderComposerClient::GetAccumulatedBufferCount()
{
    return std::max(acquiredBufferCount_.load() - 1, 0);
}

void RSRenderComposerClient::ClearRedrawGPUCompositionCache(const std::set<uint32_t>& bufferIds)
{
    std::lock_guard<std::mutex> lock(clientMutex_);
    if (connection_ != nullptr) {
        connection_->ClearRedrawGPUCompositionCache(bufferIds);
    }
}

void RSRenderComposerClient::SetScreenBacklight(uint32_t level)
{
    std::lock_guard<std::mutex> lock(clientMutex_);
    if (connection_ != nullptr) {
        connection_->SetScreenBacklight(level);
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
    std::lock_guard<std::mutex> lock(clientMutex_);
    rsLayerContext_->DumpLayersInfo(dumpString);
}

void RSRenderComposerClient::DumpCurrentFrameLayers()
{
    std::lock_guard<std::mutex> lock(clientMutex_);
    rsLayerContext_->DumpCurrentFrameLayers();
}
} // namespace Rosen
} // namespace OHOS