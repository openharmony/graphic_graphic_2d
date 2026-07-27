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

#ifndef RENDER_SERVICE_FEATURE_RS_TV_SHUTTER_3D_MANAGER_H
#define RENDER_SERVICE_FEATURE_RS_TV_SHUTTER_3D_MANAGER_H

#include <memory>
#include <mutex>
#include "common/rs_common_def.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_screen_render_params.h"

namespace OHOS::Rosen {
class RSTvShutter3DManager {
public:
    static RSTvShutter3DManager& Instance();

    void UpdateSurfaceNodeCompositionType(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, UIMode3D uiMode3D);

    bool Prepare3DForDraw(const RSScreenRenderParams& params,
        const std::shared_ptr<Drawing::Surface>& drSurface, std::shared_ptr<RSPaintFilterCanvas>& curCanvas);

    bool Process3DForFlush(UIMode3D uiMode3D, std::shared_ptr<RSPaintFilterCanvas>& curCanvas);

    void SetVideoDimType(VideoDimType type);
    VideoDimType GetVideoDimType() const;

private:
    RSTvShutter3DManager() = default;
    ~RSTvShutter3DManager() = default;
    RSTvShutter3DManager(const RSTvShutter3DManager&) = delete;
    RSTvShutter3DManager& operator=(const RSTvShutter3DManager&) = delete;

    bool Is3DEnabled(UIMode3D uiMode3D) const;
    bool ShouldSet3DShutterComposition(UIMode3D uiMode3D, VideoDimType videoDimType) const;

    void Init3DContext(UIMode3D uiMode3D, int32_t width, int32_t height,
        std::shared_ptr<RSPaintFilterCanvas>& curCanvas);

    void Process3DImage(std::shared_ptr<RSPaintFilterCanvas> targetCanvas,
        std::shared_ptr<Drawing::Image> snapshot, VideoDimType type);

    void Release3DContext();

    std::shared_ptr<RSPaintFilterCanvas> GetOffscreenCanvas() const;

    std::shared_ptr<RSPaintFilterCanvas> GetBackupCanvas() const;

    mutable std::mutex mutex_;
    std::shared_ptr<Drawing::Surface> offscreenSurface3D_ = nullptr;
    mutable std::shared_ptr<RSPaintFilterCanvas> offscreenCanvas3D_ = nullptr;
    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvasBak_ = nullptr;
    VideoDimType videoDimType_ = VideoDimType::VIDEO_DIM_TYPE_2D;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_FEATURE_RS_TV_SHUTTER_3D_MANAGER_H
