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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_RENDER_LAYER_RS_SURFACE_RENDER_RCD_LAYER_H
#define RENDER_SERVICE_COMPOSER_SERVICE_RENDER_LAYER_RS_SURFACE_RENDER_RCD_LAYER_H

#include <string>
#include "common/rs_anco_type.h"
#include "hdi_display_type.h"
#include "iconsumer_surface.h"
#include "pixel_map.h"
#include "rs_layer_cmd_type.h"
#include "rs_render_surface_layer.h"
#include "surface.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSRenderSurfaceRCDLayer : public RSRenderSurfaceLayer {
public:
    RSRenderSurfaceRCDLayer() = default;
    virtual ~RSRenderSurfaceRCDLayer() = default;
    virtual bool IsScreenRCDLayer() const override { return true; };
    void UpdateRSLayerCmd(const std::shared_ptr<RSRenderLayerCmd>& command) override;
    void SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap)
    {
        pixelMap_ = pixelMap;
    }

    std::shared_ptr<Media::PixelMap> GetPixelMap() const
    {
        return pixelMap_;
    }

    void CopyLayerInfo(const std::shared_ptr<RSLayer>& rsLayer) override;

    void CacheImageWithId(uint32_t id, const std::shared_ptr<Drawing::Image>& image)
    {
        image_ = image;
        id_ = id;
    }

    void ClearCacheImage()
    {
        image_ = nullptr;
        id_ = 0;
    }

    std::shared_ptr<Drawing::Image> GetCacheImage() const
    {
        return image_;
    }

    uint32_t GetCachedId() const
    {
        return id_;
    }

    void SetRedrawState(bool isRedraw)
    {
        isRedraw_ = isRedraw;
    }

    bool GetRedrawState() const
    {
        return isRedraw_;
    }
private:
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    std::shared_ptr<Drawing::Image> image_ = nullptr; // cached for redraw
    bool isRedraw_ = false;
    uint32_t id_ = 0; // cached pixelmapId
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_SERVICE_RENDER_LAYER_RS_SURFACE_RENDER_RCD_LAYER_H
