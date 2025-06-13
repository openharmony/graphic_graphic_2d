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

#ifndef RS_IMAGE_MANAGER_H
#define RS_IMAGE_MANAGER_H

#include <memory>
#include <mutex>
#include <queue>
#include <variant>
#include <unordered_map>
#include <surface.h>

#include "render_context/render_context.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {
class RSImageManager {
public:
    static std::shared_ptr<RSImageManager> Create(std::shared_ptr<RenderContext>& renderContext);

    virtual ~RSImageManager() = default;

    virtual void UnMapImageFromSurfaceBuffer(int32_t seqNum) = 0;
    virtual std::shared_ptr<Drawing::Image> CreateImageFromBuffer(
        RSPaintFilterCanvas& canvas, const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence,
        const pid_t threadIndex, const std::shared_ptr<Drawing::ColorSpace>& drawingColorSpace) = 0;
    virtual std::shared_ptr<Drawing::Image> GetIntersectImage(Drawing::RectI& imgCutRect,
        const std::shared_ptr<Drawing::GPUContext>& context, const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence, pid_t threadIndex = 0) = 0;

    // Vulkan specific functions
    virtual void DumpVkImageInfo(std::string &dumpString) { return; }

    // EGL specific functions
    virtual void ShrinkCachesIfNeeded(bool isForUniRedraw = false) { return; }
protected:
    RSImageManager() = default;
    mutable std::mutex opMutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_IMAGE_MANAGER_H
