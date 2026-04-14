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

#include "rcd/rs_render_rcd_draw.h"
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "feature/hdr/rs_colorspace_util.h"
#include "platform/common/rs_log.h"
#include "pipeline/sk_resource_manager.h"
#include "render/rs_pixel_map_util.h"
#include "rs_trace.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "native_buffer_inner.h"
#include "native_window.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

namespace OHOS {
namespace Rosen {
using namespace Media;

Drawing::ColorType RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::RGB_565:
            return Drawing::ColorType::COLORTYPE_RGB_565;
        case PixelFormat::RGBA_8888:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        case PixelFormat::BGRA_8888:
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        case PixelFormat::ALPHA_8:
            return Drawing::ColorType::COLORTYPE_ALPHA_8;
        case PixelFormat::RGBA_F16:
            return Drawing::ColorType::COLORTYPE_RGBA_F16;
        case PixelFormat::RGBA_1010102:
            return Drawing::ColorType::COLORTYPE_RGBA_1010102;
        case PixelFormat::UNKNOWN:
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGB_888:
        case PixelFormat::NV21:
        case PixelFormat::NV12:
        case PixelFormat::CMYK:
        default:
            return Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}

Drawing::AlphaType RSRenderRcdDraw::AlphaTypeToDrawingAlphaType(AlphaType alphaType)
{
    switch (alphaType) {
        case AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return Drawing::AlphaType::ALPHATYPE_PREMUL;
        case AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        case AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
        default:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}

void RSRenderRcdDraw::DrawRoundCorner(RSPaintFilterCanvas& canvas, const std::vector<RSLayerPtr>& layers)
{
    RS_TRACE_NAME("RSRenderRcdDraw::DrawRoundCorner");
    for (auto& layer : layers) {
        if (layer == nullptr || !layer->IsScreenRCDLayer()) {
            continue;
        }
        DrawRSRCDLayer(canvas, layer);
    }
}

void RSRenderRcdDraw::DrawRSRCDLayer(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSLayer>& layer)
{
    if (layer == nullptr) {
        RS_LOGE("RSRenderRcdDraw::DrawRsRCDLayer layer is null");
        return;
    }
    auto rcdLayer = std::static_pointer_cast<RSRenderSurfaceRCDLayer>(layer);
    rcdLayer->SetRedrawState(true);
    auto pixelMap = rcdLayer->GetPixelMap();
    if (pixelMap == nullptr || pixelMap->GetPixels() == nullptr ||
        pixelMap->GetWidth() < 1 || pixelMap->GetHeight() < 1) {
        RS_LOGE("RSRenderRcdDraw::DrawRsRCDLayer pixelmap error");
        return;
    }
    auto rect = rcdLayer->GetLayerSize();
    if (!BindPixelMapToDrawingImage(canvas, rcdLayer) || rcdLayer->GetCacheImage() == nullptr) {
        RS_LOGE("RSRenderRcdDraw::DrawRsRCDLayer BindPixelMapToDrawingImage error");
        return;
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    canvas.DrawImage(*(rcdLayer->GetCacheImage()), rect.x, rect.y, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

bool RSRenderRcdDraw::BindPixelMapToDrawingImage(Drawing::Canvas& canvas,
    const std::shared_ptr<RSRenderSurfaceRCDLayer>& rcdLayer)
{
    if (rcdLayer == nullptr) {
        return false;
    }
    auto pixelMap = rcdLayer->GetPixelMap();
    if (pixelMap == nullptr) {
        return false;
    }
    if (pixelMap->GetUniqueId() == rcdLayer->GetCachedId()) {
        return true;
    }

    return StoreWithoutDMA(canvas, rcdLayer, pixelMap);
}

bool RSRenderRcdDraw::StoreWithoutDMA(Drawing::Canvas& canvas,
    const std::shared_ptr<RSRenderSurfaceRCDLayer>& layer, RSRenderRcdDraw::PixelMapPtr& pixelMap)
{
    if (!pixelMap) {
        return false;
    }
    ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        PixelFormatToDrawingColorType(imageInfo.pixelFormat),
        AlphaTypeToDrawingAlphaType(imageInfo.alphaType),
        RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(pixelMap->InnerGetGrColorSpace().GetColorSpaceName()) };
    Drawing::Bitmap pixelBitmap;
    pixelBitmap.InstallPixels(
        drawingImageInfo, reinterpret_cast<void*>(pixelMap->GetWritablePixels()),
        static_cast<uint32_t>(pixelMap->GetRowBytes()));
    auto image = std::make_shared<Drawing::Image>();
    if (image->BuildFromBitmap(pixelBitmap)) {
        RS_TRACE_NAME_FMT("%s cache Rcd image : %d, %d", __func__, image->GetWidth(), image->GetHeight());
        layer->CacheImageWithId(pixelMap->GetUniqueId(), image);
        return true;
    }
    RS_LOGE("RSRenderRcdDraw::StoreWithoutDMA BuildFromBitmap error");
    return false;
}
}
}