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

#ifndef RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_SURFACE_REGION_CONFIG_H
#define RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_SURFACE_REGION_CONFIG_H

#ifndef ROSEN_CROSS_PLATFORM

#include <surface.h>

#include "common/rs_rect.h"

namespace OHOS::Rosen {

// Multi-surface virtual screen: surface with its render region.
struct SurfaceRegionConfig {
    sptr<Surface> surface = nullptr;
    RectI region;

    bool operator==(const SurfaceRegionConfig& config) const
    {
        if (surface == nullptr || config.surface == nullptr) {
            return surface == config.surface && region == config.region;
        }
        return surface->GetUniqueId() == config.surface->GetUniqueId() && region == config.region;
    }
};

} // namespace OHOS::Rosen

#endif // ROSEN_CROSS_PLATFORM
#endif // RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_SURFACE_REGION_CONFIG_H
