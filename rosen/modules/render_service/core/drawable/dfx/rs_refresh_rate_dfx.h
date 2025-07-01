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

#ifndef RENDER_SERVICE_DRAWABLE_DFX_RS_REFRESH_RATE_DFX_H
#define RENDER_SERVICE_DRAWABLE_DFX_RS_REFRESH_RATE_DFX_H

#include <string>

#include "system/rs_system_parameters.h"
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "params/rs_logical_display_render_params.h"

namespace OHOS::Rosen {
class RSRefreshRateDfx {
public:
    explicit RSRefreshRateDfx(DrawableV2::RSLogicalDisplayRenderNodeDrawable& targetDrawable)
        : logicalDisplayDrawable_(targetDrawable), logicalDisplayParams_(targetDrawable.GetRenderParams())
    {}
    ~RSRefreshRateDfx() = default;

    void OnDraw(RSPaintFilterCanvas& canvas);

private:
    bool RefreshRateRotationProcess(RSPaintFilterCanvas& canvas,
        ScreenRotation rotation, uint32_t translateWidth, uint32_t translateHeight);
    
    const DrawableV2::RSLogicalDisplayRenderNodeDrawable& logicalDisplayDrawable_;
    const std::unique_ptr<RSRenderParams>& logicalDisplayParams_;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_DFX_RS_REFRESH_RATE_DFX_H
