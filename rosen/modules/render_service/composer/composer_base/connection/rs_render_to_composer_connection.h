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
#ifndef RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_H
#define RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_H

#include "rs_render_composer_agent.h"
#include "rs_render_to_composer_connection_stub.h"

namespace OHOS::Rosen {
class RSRenderToComposerConnection : public RSRenderToComposerConnectionStub {
public:
    RSRenderToComposerConnection(const std::string& name, uint64_t screenId,
        std::shared_ptr<RSRenderComposerAgent> rsRenderComposerAgent);
    ~RSRenderToComposerConnection() noexcept override = default;

    void CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData) override;
    void ClearFrameBuffers() override;
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId) override;
    void OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp);
    void ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds) override;
    void SetScreenBacklight(uint32_t level) override;
private:
    uint64_t screenId_ = 0;
    std::shared_ptr<RSRenderComposerAgent> rsRenderComposerAgent_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_H