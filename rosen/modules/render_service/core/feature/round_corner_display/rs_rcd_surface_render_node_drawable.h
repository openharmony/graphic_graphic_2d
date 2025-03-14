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

#ifndef RENDER_SERVICE_RCD_SURFACE_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_RCD_SURFACE_RENDER_NODE_DRAWABLE_H

#include <memory>
#include "common/rs_common_def.h"
#include "platform/drawing/rs_surface.h"
#include "drawable/rs_render_node_drawable.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {

struct CldInfo {
    uint32_t cldDataOffset = 0;
    uint32_t cldSize = 0;
    uint32_t cldWidth = 0;
    uint32_t cldHeight = 0;
    uint32_t cldStride = 0;
    uint32_t exWidth = 0;
    uint32_t exHeight = 0;
    uint32_t baseColor = 0;
};

namespace DrawableV2 {

// use normal hardware composer node drawable instead when it supports rcd feature in OH 6.0 rcd refactoring
class RSRcdSurfaceRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSRcdSurfaceRenderNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);

    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    bool IsSurfaceCreated() const;
    std::shared_ptr<RSSurface> GetRSSurface() const;
    sptr<IBufferConsumerListener> GetConsumerListener() const;

    const std::shared_ptr<RSSurfaceHandler> GetSurfaceHandler() const
    {
        return surfaceHandler_;
    }
    void SetSurfaceType(uint32_t surfaceType);
    bool IsTopSurface() const;
    bool IsBottomSurface() const;
    bool IsInvalidSurface() const;

    bool IsRcdEnabled() const;
    bool IsResourceChanged() const;

    BufferRequestConfig GetHardenBufferRequestConfig() const;
    bool ProcessRcdSurfaceRenderNode(std::shared_ptr<RSProcessor> processor);

    bool ConsumeAndUpdateBuffer();
    void ClearBufferCache();
    bool SetHardwareResourceToBuffer();

    const CldInfo& GetCldInfo() const;

    void SetRenderTargetId(NodeId id);

    bool PrepareResourceBuffer();
    bool FillHardwareResource(int width, int height);

    bool DoProcessRenderTask(std::shared_ptr<RSProcessor> processor);

private:
    explicit RSRcdSurfaceRenderNodeDrawable(std::shared_ptr<const RSRenderNode> &&node);

    NodeId GetRenderTargetId() const;
    std::shared_ptr<Drawing::Bitmap> GetRcdBitmap() const;

    uint32_t GetRcdBufferWidth() const;
    uint32_t GetRcdBufferHeight() const;
    uint32_t GetRcdBufferSize() const;

    CldInfo cldInfo_;

    NodeId renderTargetId_ = 0;
    uint32_t surfaceType_ = 0;

    bool surfaceCreated_ = false;
    std::shared_ptr<RSSurface> surface_ = nullptr;
    sptr<IBufferConsumerListener> consumerListener_;

    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::ROUND_CORNER_NODE, OnGenerate>;
    static Registrar instance_;

    std::shared_ptr<RSSurfaceHandler> surfaceHandler_;

    // dirty manager
    std::shared_ptr<RSDirtyRegionManager> syncDirtyManager_ = nullptr;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_RCD_SURFACE_RENDER_NODE_DRAWABLE_H
