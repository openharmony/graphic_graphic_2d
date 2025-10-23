/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_DRAWING_RS_SURFACE_H
#define RENDER_SERVICE_BASE_DRAWING_RS_SURFACE_H

#include <memory>

#include "rs_surface_frame.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {
class RenderContext;
class RSSurfaceExt;
using RSSurfaceExtPtr = std::shared_ptr<RSSurfaceExt>;

class RSSurface {
public:
    RSSurface() = default;

    virtual ~RSSurface() = default;

    virtual bool IsValid() const = 0;

    virtual void SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp = 0) = 0;

    virtual std::unique_ptr<RSSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp = 0, bool useAFBC = true, bool isProtected = false) = 0;

    virtual bool FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp = 0) = 0;

    virtual RenderContext* GetRenderContext() = 0;
    virtual void SetRenderContext(RenderContext* context) = 0;
    virtual GraphicColorGamut GetColorSpace() const = 0;
    virtual void SetColorSpace(GraphicColorGamut colorSpace) = 0;
    virtual uint32_t GetQueueSize() const = 0;
    virtual void ClearBuffer() = 0; // clear cache only for producer

    // clear buffer for both producer and consumer and will receive OnGoBackground callback
    virtual void ClearAllBuffer() = 0;
    virtual void ResetBufferAge() = 0;
#ifdef USE_SURFACE_TEXTURE
    virtual RSSurfaceExtPtr CreateSurfaceExt(const RSSurfaceExtConfig& config) = 0;
    virtual RSSurfaceExtPtr GetSurfaceExt(const RSSurfaceExtConfig& config) = 0;
#endif
protected:
private:
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DRAWING_RS_SURFACE_H
