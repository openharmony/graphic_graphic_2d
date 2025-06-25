/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_HIGH_PERFORMANCE_VISUAL_ENGINE_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_HIGH_PERFORMANCE_VISUAL_ENGINE_H

#include "draw/canvas.h"
#include "image/image.h"
#include <mutex>
#include "pipeline/rs_paint_filter_canvas.h"
#include "utils/matrix.h"
#include "utils/rect.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
struct SurfaceNodeInfo {
    std::shared_ptr<Drawing::Image> surfaceImage_ = nullptr;
    Drawing::Matrix matrix_ = {};
    Drawing::Rect srcRect_;
    Drawing::Rect dstRect_;
    Color solidLayerColor_;
};
class RSB_EXPORT HveFilter {
public:
    ~HveFilter() = default;
    HveFilter(const HveFilter&) = delete;
    HveFilter& operator=(const HveFilter&) = delete;

    static HveFilter& GetHveFilter();

    void ClearSurfaceNodeInfo();
    void PushSurfaceNodeInfo(SurfaceNodeInfo& surfaceNodeInfo);
    std::vector<SurfaceNodeInfo> GetSurfaceNodeInfo() const;
    int GetSurfaceNodeSize() const;

    std::shared_ptr<Drawing::Image> SampleLayer(RSPaintFilterCanvas& canvas, const Drawing::RectI& srcRect);
private:
    HveFilter() = default;
    std::vector<SurfaceNodeInfo> surfaceNodeInfo_;
    mutable std::mutex hveFilterMtx_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_HIGH_PERFORMANCE_VISUAL_ENGINE_H