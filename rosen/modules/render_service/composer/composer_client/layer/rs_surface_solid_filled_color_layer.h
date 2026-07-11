/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_LAYER_RS_SURFACE_SOLID_FILLED_COLOR_LAYER_H
#define RENDER_SERVICE_COMPOSER_CLIENT_LAYER_RS_SURFACE_SOLID_FILLED_COLOR_LAYER_H

#include <string>
#include "common/rs_anco_type.h"
#include "hdi_display_type.h"
#include "iconsumer_surface.h"
#include "pixel_map.h"
#include "rs_render_layer_cmd.h"
#include "rs_render_layer_cmd_property.h"
#include "rs_surface_layer.h"
#include "surface.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceSolidFilledColorLayer : public RSSurfaceLayer {
public:
    ~RSSurfaceSolidFilledColorLayer() = default;
    static std::shared_ptr<RSLayer> Create(RSLayerId rsLayerId, const std::shared_ptr<RSComposerContext>& context);

    bool IsSolidFilledColorLayer() const override { return true; };

private:
    RSSurfaceSolidFilledColorLayer(RSLayerId rsLayerId, std::shared_ptr<RSComposerContext> rsComposerContext);
    friend std::shared_ptr<RSLayer> Create(RSLayerId rsLayerId, const std::shared_ptr<RSComposerContext>& context);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_CLIENT_LAYER_RS_SURFACE_SOLID_FILLED_COLOR_LAYER_H