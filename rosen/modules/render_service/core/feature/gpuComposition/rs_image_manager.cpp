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

#include "rs_image_manager.h"

#ifdef RS_ENABLE_VK
#include "rs_vk_image_manager.h"
#endif // RS_ENABLE_VK

#include "EGL/egl.h"
#include "platform/common/rs_system_properties.h"
#include "rs_egl_image_manager.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSImageManager> RSImageManager::Create(std::shared_ptr<RenderContext>& renderContext)
{
    std::shared_ptr<RSImageManager> imageManager;
    if (RSSystemProperties::IsUseVulkan()) {
#ifdef RS_ENABLE_VK
        imageManager = std::make_shared<RSVkImageManager>();
#endif
    } else {
        if (renderContext != nullptr) {
            imageManager = std::make_shared<RSEglImageManager>(renderContext->GetEGLDisplay());
        }
    }
    return imageManager;
}
} // namespace Rosen
} // namespace OHOS