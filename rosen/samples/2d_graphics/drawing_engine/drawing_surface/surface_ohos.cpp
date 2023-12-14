/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "surface_ohos.h"

#include "surface_frame_ohos.h"
#if defined(ACE_ENABLE_VK)
#include "surface_ohos_vulkan.h"
#endif
#if defined(ACE_ENABLE_GL)
#include "surface_ohos_gl.h"
#endif
#include "surface_ohos_raster.h"

namespace OHOS {
namespace Rosen {
SurfaceOhos::SurfaceOhos(const sptr<Surface>& producer)
    : producer_(producer)
{
    producer_->SetQueueSize(5);
}

std::shared_ptr<SurfaceBase> SurfaceOhos::CreateSurface(sptr<Surface> surface)
{
    auto type = Setting::GetRenderBackendType();
    std::shared_ptr<SurfaceBase> producer = nullptr;
    switch (type) {
        case RenderBackendType::VULKAN:
#ifdef ACE_ENABLE_VK
            if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
                LOGI("SurfaceOhos::CreateSurface with vulkan backend");
                producer = std::make_shared<SurfaceOhosVulkan>(surface);
            }
#endif
            break;
        case RenderBackendType::GLES:
#ifdef ACE_ENABLE_GL
            if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
                LOGI("SurfaceOhos::CreateSurface with gles backend");
                producer = std::make_shared<SurfaceOhosGl>(surface);
            }
#endif
            break;
        case RenderBackendType::SOFTWARE:
            LOGI("SurfaceOhos::CreateSurface with software backend");
            producer = std::make_shared<SurfaceOhosRaster>(surface);
            break;
        default:
            break;
    }
    return producer;
}
} // namespace Rosen
} // namespace OHOS