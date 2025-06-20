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

#ifndef HPAE_RS_HPAE_BUFFER_H
#define HPAE_RS_HPAE_BUFFER_H

#include <string>

#include "common/rs_common_def.h"
#include "image/gpu_context.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/rs_surface_handler.h"

namespace OHOS::Rosen {

constexpr uint32_t HPAE_BUFFER_SIZE = 5;

namespace DrawableV2 {
class RSHpaeBuffer {
public:
    RSHpaeBuffer(const std::string& name, const int layerId);
    ~RSHpaeBuffer();

    std::unique_ptr<RSRenderFrame> RequestFrame(const BufferRequestConfig& config, bool isHebc);
    bool FlushFrame();
    GSError ForceDropFrame(uint64_t presentWhen = 0);

    std::shared_ptr<Drawing::GPUContext> GetSkContext() const // 如果不调用flush，暂时可以不需要
    {
        return grContext_;
    }

    void* GetBufferHandle();
    std::shared_ptr<RSSurfaceHandler> GetSurfaceHandler() const { return surfaceHandler_; }

private:
#if defined(ROSEN_OHOS)
    bool CreateSurface(sptr<IBufferConsumerListener> listener);
#endif
    std::shared_ptr<RSSurfaceHandler> surfaceHandle_;
    bool surfaceCreated_ = false;
    std::string layerName_;
    std::shared_ptr<Drawing::GPUContext> grContext_;
    sptr<Surface> producerSurface_;
    BufferRequestConfig bufferConfig_;
    void* bufferHandle_ = nullptr;
    std::shared_ptr<RSSurfaceOhos> rsSurface_ = nullptr;
};

} // DrawableV2
} // OHOS::Rosen

#endif