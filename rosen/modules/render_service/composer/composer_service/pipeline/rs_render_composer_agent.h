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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_PIPELINE_RS_RENDER_COMPOSER_AGENT_H
#define RENDER_SERVICE_COMPOSER_SERVICE_PIPELINE_RS_RENDER_COMPOSER_AGENT_H

#include "rs_render_composer.h"
#include "rs_layer_transaction_data.h"
#include "sync_fence.h"

namespace OHOS::Rosen {
class RSRenderComposerAgent : public std::enable_shared_from_this<RSRenderComposerAgent> {
public:
    explicit RSRenderComposerAgent(const std::shared_ptr<RSRenderComposer>& rsRenderComposer);
    ~RSRenderComposerAgent() = default;

    void SetComposerToRenderConnection(const sptr<IRSComposerToRenderConnection>& composerToRenderConn);
    bool ComposerProcess(const std::shared_ptr<RSLayerTransactionData>& transactionData);
    void OnScreenConnected(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    void OnScreenDisconnected();
    void OnHwcRestored(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    void OnHwcDead();
    GSError ClearFrameBuffers(bool isNeedResetContext = true);
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId);
    void ClearRedrawGPUCompositionCache(const std::unordered_set<uint64_t>& bufferIds);
    void SetScreenBacklight(uint32_t level);
    void OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp);
    void SurfaceDump(std::string& dumpString);
    void GetRefreshInfoToSP(std::string& dumpString, NodeId nodeId);
    void FpsDump(std::string& dumpString, const std::string& layerName);
    void ClearFpsDump(std::string& dumpString, std::string& layerName);
    void HitchsDump(std::string& dumpString, std::string& layerArg);
    void RefreshRateCounts(std::string& dumpString);
    void ClearRefreshRateCounts(std::string& dumpString);
    void PreAllocProtectedFrameBuffers(const sptr<SurfaceBuffer>& buffer);

private:
    std::shared_ptr<RSRenderComposer> rsRenderComposer_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_SERVICE_PIPELINE_RS_RENDER_COMPOSER_AGENT_H