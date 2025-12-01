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

#ifndef RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_AGENT_H
#define RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_AGENT_H

#include "rs_render_composer.h"
#include "rs_layer_transaction_data.h"
#include "sync_fence.h"

namespace OHOS::Rosen {
class RSRenderComposerAgent {
public:
    explicit RSRenderComposerAgent(const std::shared_ptr<RSRenderComposer>& rsRenderComposer);
    ~RSRenderComposerAgent() = default;

    void ComposerProcess(const std::shared_ptr<RSLayerTransactionData>& transactionData);
    void OnScreenConnected(const std::shared_ptr<HdiOutput>& output);
    void OnScreenDisconnected();

    void PreAllocateProtectedBuffer(sptr<SurfaceBuffer> buffer);
    uint32_t GetUnExecuteTaskNum();
    int32_t GetAccumulatedBufferCount();
    void PostTask(const std::function<void()>& task);
    GSError ClearFrameBuffers(bool isNeedResetContext = true);
    void OnScreenVBlankIdleCallback(uint64_t timestamp);
    void RefreshRateCounts(std::string& dumpString);
    void ClearRefreshRateCounts(std::string& dumpString);
    sptr<SyncFence> GetReleaseFence();
    bool WaitComposerTaskExecute();
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId);
    void FpsDump(std::string& dumpString, std::string& layerName);
    void ClearFpsDump(std::string& dumpString, std::string& layerName);
    void DumpCurrentFrameLayers();
    void HitchsDump(std::string& dumpString, std::string& layerArg);
    void DumpVkImageInfo(std::string &dumpString);
    void ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds);
    void SetScreenPowerOnChanged(bool flag);

private:
    std::shared_ptr<RSRenderComposer> rsRenderComposer_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_AGENT_H