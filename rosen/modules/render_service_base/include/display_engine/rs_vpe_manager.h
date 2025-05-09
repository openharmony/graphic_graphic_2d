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
#ifndef RS_VPE_MANAGER_H
#define RS_VPE_MANAGER_H

#include <surface.h>

#include "command/rs_command.h"
#include "detail_enhancer_video.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSVpeManager {
public:
    RSB_EXPORT static RSVpeManager& GetInstance();

    RSB_EXPORT sptr<Surface> CheckAndGetSurface(const sptr<Surface>& surface, const RSSurfaceRenderNodeConfig& config);
    RSB_EXPORT void ReleaseVpeVideo(uint64_t nodeId);

private:
    sptr<Surface> GetVpeVideoSurface(uint32_t type, const sptr<Surface>& RSSurface,
        const RSSurfaceRenderNodeConfig& config);
    std::shared_ptr<OHOS::Media::VideoProcessingEngine::VpeVideo> GetVpeVideo(uint32_t type,
        const RSSurfaceRenderNodeConfig& config);

    mutable std::mutex vpeVideoLock_{};
    // Guarded by vpeVideoLock_ begin
    std::unordered_map<uint64_t, std::shared_ptr<OHOS::Media::VideoProcessingEngine::VpeVideo>> allVpeVideo_{};
    // Guarded by vpeVideoLock_ end
};
} // namespace Rosen

namespace Media {
namespace VideoProcessingEngine {
class VpeVideoCallbackImpl : public VpeVideoCallback {
public:
    VpeVideoCallbackImpl() = default;
    ~VpeVideoCallbackImpl() override = default;
    explicit VpeVideoCallbackImpl(const std::shared_ptr<VpeVideo>& videoResizeFilter)
        : videoFilter_(videoResizeFilter)
    {
    }
    VpeVideoCallbackImpl(const VpeVideoCallbackImpl&) = delete;
    VpeVideoCallbackImpl& operator=(const VpeVideoCallbackImpl&) = delete;
    VpeVideoCallbackImpl(VpeVideoCallbackImpl&&) = delete;
    VpeVideoCallbackImpl& operator=(VpeVideoCallbackImpl&&) = delete;

    void OnOutputBufferAvailable(uint32_t index, VpeBufferFlag flag) override;

private:
    std::weak_ptr<VpeVideo> videoFilter_;
};
} // namespace VideoProcessingEngine
} // namespace Media
} // namespace OHOS

#endif