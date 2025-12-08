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

void RSRenderComposerAgent::SetComposerToRenderConnection(sptr<RSIComposerToRenderConnection> conn)
{
    if (rsRenderComposer_ == nullptr) {
        RS_LOGE("rsRenderComposer is nullptr");
        return;
    }
    rsRenderComposer_->SetComposerToRenderConnection(conn);
}

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

    rsRenderComposer_->ComposerPrepare(currentRate, delayTime, transactionData->GetPipelineParam());
    rsRenderComposer_->ComposerProcess(currentRate, delayTime, transactionData);
}

void RSRenderComposerAgent::ClearFrameBuffers(bool isNeedResetContext)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->ClearFrameBuffers(isNeedResetContext);
}

void RSRenderComposerAgent::OnScreenConnected(const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->OnScreenConnected(output, property);
}

void RSRenderComposerAgent::OnScreenDisconnected()
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->OnScreenDisconnected();
}

void RSRenderComposerAgent::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->CleanLayerBufferBySurfaceId(surfaceId);
}

void RSRenderComposerAgent::ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->ClearRedrawGPUCompositionCache(bufferIds);
}

void RSRenderComposerAgent::SetScreenBacklight(uint32_t level)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->SetScreenBacklight(level);
}

void RSRenderComposerAgent::OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->OnScreenVBlankIdleCallback(screenId, timestamp);
}

void RSRenderComposerAgent::SurfaceDump(std::string& dumpString)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->SurfaceDump(dumpString);
}

void RSRenderComposerAgent::GetRefreshInfoToSP(std::string& dumpString, NodeId& nodeId)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->GetRefreshInfoToSP(dumpString, nodeId);
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

void RSRenderComposerAgent::HitchsDump(std::string& dumpString, std::string& layerArg)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->HitchsDump(dumpString, layerArg);
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

void RSRenderComposerAgent::SetScreenPowerOnChanged(bool flag)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    rsRenderComposer_->SetScreenPowerOnChanged(flag);
}
} // namespace Rosen
} // namespace OHOS
