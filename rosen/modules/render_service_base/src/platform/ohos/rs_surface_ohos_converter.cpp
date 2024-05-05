/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/drawing/rs_surface_converter.h"
#if defined(NEW_RENDER_CONTEXT)
#include "render_backend/ohos/rs_render_surface_ohos.h"
#else
#include "rs_surface_ohos.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif
#endif

namespace OHOS {
namespace Rosen {
#if defined(NEW_RENDER_CONTEXT)
sptr<Surface> RSSurfaceConverter::ConvertToOhosSurface(std::shared_ptr<RSRenderSurface> surface)
{
    if (surface == nullptr) {
        ROSEN_LOGE("nullptr input");
        return nullptr;
    }
    auto derivedPtr = std::static_pointer_cast<RSRenderSurfaceOhos>(surface);
    return derivedPtr->GetSurfaceOhos();
}
#else
sptr<Surface> RSSurfaceConverter::ConvertToOhosSurface(std::shared_ptr<RSSurface> surface)
{
    if (surface == nullptr) {
        ROSEN_LOGE("nullptr input");
        return nullptr;
    }
#if defined(ACE_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan()) {
        auto derivedPtr = std::static_pointer_cast<RSSurfaceOhosVulkan>(surface); // gpu render
        return derivedPtr->GetSurface();
    }
#endif

#if defined(ACE_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        auto derivedPtr = std::static_pointer_cast<RSSurfaceOhosGl>(surface); // gpu render
        return derivedPtr->GetSurface();
    }
#endif

    auto derivedPtr = std::static_pointer_cast<RSSurfaceOhosRaster>(surface); // cpu render
    return derivedPtr->GetSurface();
}
#endif

} // namespace Rosen
} // namespace OHOS
