/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "utils/log.h"

#include "render_backend_utils.h"

namespace OHOS {
namespace Rosen {
bool RenderBackendUtils::RenderBackendUtils::IsValidFrame(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (frame == nullptr) {
        LOGE("Frame is nullptr");
        return false;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame->frameConfig;
    if (frameConfig == nullptr) {
        LOGE("FrameConfig is nullptr");
        return false;
    }
    std::shared_ptr<SurfaceConfig> surfaceConfig = frame->surfaceConfig;
    if (surfaceConfig == nullptr) {
        LOGE("SurfaceConfig is nullptr");
        return false;
    }
    std::shared_ptr<EGLState> eglState = frame->eglState;
    if (eglState == nullptr) {
        LOGE("EGLState is nullptr");
        return false;
    }
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        VulkanState* vulkanState = frame->vulkanState;
        if (vulkanState == nullptr) {
            LOGE("VulkanState is nullptr");
            return false;
        }
    }
#endif
    return true;
}
}
}