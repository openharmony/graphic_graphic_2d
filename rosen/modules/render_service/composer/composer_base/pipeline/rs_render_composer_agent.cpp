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

#include "rs_render_composer_agent.h"
#include "platform/common/rs_log.h"
#include "rs_layer_transaction_data.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "ComposerAgent"

namespace OHOS {
namespace Rosen {
RSRenderComposerAgent::RSRenderComposerAgent(const std::shared_ptr<RSRenderComposer>& rsRenderComposer)
    : rsRenderComposer_(rsRenderComposer)
{}

void RSRenderComposerAgent::ComposerProcess(const std::shared_ptr<RSLayerTransactionData>& transactionData)
{
    if (rsRenderComposer_ == nullptr) {
        RS_LOGE("rsRenderComposer is nullptr");
        return;
    }
    if (transactionData == nullptr) {
        RS_LOGE("transactionData is nullptr");
        return;
    }
    uint32_t currentRate = 0;
    int64_t delayTime = 0;
    RefreshRateParam param;
    bool hasGameScene = false;

    rsRenderComposer_->ComposerPrepare(param, currentRate, hasGameScene, delayTime);
    rsRenderComposer_->ComposerProcess(param, currentRate, hasGameScene, delayTime, transactionData);
}

void RSRenderComposerAgent::OnScreenConnected(const std::shared_ptr<HdiOutput>& output)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->OnScreenConnected(output);
}

void RSRenderComposerAgent::OnScreenDisconnected()
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->OnScreenDisconnected();
}

void RSRenderComposerAgent::PreAllocateProtectedBuffer(sptr<SurfaceBuffer> buffer)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->PreAllocateProtectedBuffer(buffer);
}

uint32_t RSRenderComposerAgent::GetUnExecuteTaskNum()
{
    if (rsRenderComposer_ == nullptr) {
        return 0;
    }
    return rsRenderComposer_->GetUnExecuteTaskNum();
}

int32_t RSRenderComposerAgent::GetAccumulatedBufferCount()
{
    if (rsRenderComposer_ == nullptr) {
        return 0;
    }
    return rsRenderComposer_->GetAccumulatedBufferCount();
}

void RSRenderComposerAgent::PostTask(const std::function<void()>& task)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->PostTask(task);
}

void RSRenderComposerAgent::PostTaskWithInnerDelay(const std::function<void()>& task)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->PostTaskWithInnerDelay(task);
}

GSError RSRenderComposerAgent::ClearFrameBuffers(bool isNeedResetContext)
{
    if (rsRenderComposer_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return rsRenderComposer_->ClearFrameBuffers(isNeedResetContext);
}

void RSRenderComposerAgent::OnScreenVBlankIdleCallback(uint64_t timestamp)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->OnScreenVBlankIdleCallback(timestamp);
}

void RSRenderComposerAgent::RefreshRateCounts(std::string& dumpString)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->RefreshRateCounts(dumpString);
}

void RSRenderComposerAgent::ClearRefreshRateCounts(std::string& dumpString)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->ClearRefreshRateCounts(dumpString);
}

sptr<SyncFence> RSRenderComposerAgent::GetReleaseFence()
{
    if (rsRenderComposer_ == nullptr) {
        return nullptr;
    }
    return rsRenderComposer_->GetReleaseFence();
}

bool RSRenderComposerAgent::WaitComposerTaskExecute()
{
    if (rsRenderComposer_ == nullptr) {
        return false;
    }
    return rsRenderComposer_->WaitComposerTaskExecute();
}

void RSRenderComposerAgent::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    return rsRenderComposer_->CleanLayerBufferBySurfaceId(surfaceId);
}

void RSRenderComposerAgent::FpsDump(std::string& dumpString, std::string& layerName)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->FpsDump(dumpString, layerName);
}

void RSRenderComposerAgent::ClearFpsDump(std::string& dumpString, std::string& layerName)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->ClearFpsDump(dumpString, layerName);
}

void RSRenderComposerAgent::DumpCurrentFrameLayers()
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->DumpCurrentFrameLayers();
}

void RSRenderComposerAgent::HitchsDump(std::string& dumpString, std::string& layerArg)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->HitchsDump(dumpString, layerArg);
}

void RSRenderComposerAgent::DumpVkImageInfo(std::string &dumpString)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->DumpVkImageInfo(dumpString);
}

void RSRenderComposerAgent::ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->ClearRedrawGPUCompositionCache(bufferIds);
}

void RSRenderComposerAgent::SetScreenPowerOnChanged(bool flag)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->SetScreenPowerOnChanged(flag);
}
} // namespace Rosen
} // namespace OHOS
