/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_surface_capture_task.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::Run()
{
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId_);
    if (node == nullptr) {
        ROSEN_LOGE("RSSurfaceCaptureTask: node == nullptr\n");
        return nullptr;
    }
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = pixmapWidth;
    opts.size.height = pixmapHeight;
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    if (pixelmap == nullptr) {
        ROSEN_LOGE("RSSurfaceCaptureTask: pixelmap == nullptr\n");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        ROSEN_LOGE("RSSurfaceCaptureTask: address == nullptr\n");
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(pixmapWidth, pixmapHeight,
            kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    std::unique_ptr<SkCanvas> canvas = SkCanvas::MakeRasterDirect(info, address, pixelmap->GetRowBytes());

    // get surface capture
    sptr<OHOS::SurfaceBuffer> buffer = node->GetBuffer();
    if (buffer == nullptr) {
        ROSEN_LOGE("RSSurfaceCaptureTask: buffer == nullptr\n");
        return nullptr;
    }
    auto addr = static_cast<uint32_t*>(buffer->GetVirAddr());
    if (addr == nullptr || buffer->GetWidth() <= 0 || buffer->GetHeight() <= 0) {
        ROSEN_LOGE("RSSurfaceCaptureTask: addr is nullptr, or buffer->GetWidth() buffer->GetHeight() is error.\n");
        return nullptr;
    }
    SkImageInfo layerInfo = SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    SkPixmap skpixmap(layerInfo, addr, buffer->GetSize() / buffer->GetHeight());
    SkBitmap skbitmap;
    if (skbitmap.installPixels(skpixmap)) {
        canvas->drawBitmapRect(skbitmap, SkRect::MakeXYWH(0, 0, buffer->GetWidth(), buffer->GetHeight()), nullptr);
    }
    return pixelmap;
}
}
}