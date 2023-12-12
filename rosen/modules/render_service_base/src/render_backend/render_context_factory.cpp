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

#include "render_context_factory.h"

#include "utils/log.h"

#include "ohos/render_context_ohos_gl.h"
#include "ohos/render_context_ohos_raster.h"
#ifdef RS_ENABLE_VK
#include "ohos/render_context_ohos_vk.h"
#endif

namespace OHOS {
namespace Rosen {
std::shared_ptr<RenderContextBase> RenderContextBaseFactory::CreateRenderContext(RenderType renderType)
{
    std::shared_ptr<RenderContextBase> renderContext;
#if defined(ROSEN_OHOS)
    if (renderType == RenderType::GLES) {
        renderContext = std::make_shared<RenderContextOhosGl>();
    } else if (renderType == RenderType::RASTER) {
        renderContext = std::make_shared<RenderContextOhosRaster>();
    } else {
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            renderContext = std::make_shared<RenderContextOhosVk>();
        }
#endif
    }
    renderContext->SetPlatformName(PlatformName::OHOS);
    renderContext->SetRenderType(renderType);
#endif
    return renderContext;
}
}
}