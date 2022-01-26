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

#include "include/core/SkPixmap.h"
#include "include/core/SkBitmap.h"
#include "pipeline/rs_render_service_util.h"
#include "platform/common/rs_log.h"

namespace OHOS {

namespace Rosen {

void RsRenderServiceUtil::ComposeSurface(std::shared_ptr<HdiLayerInfo> layer, sptr<Surface> consumerSurface,
    std::vector<LayerInfoPtr>& layers,  ComposeInfo info)
{
    layer->SetSurface(consumerSurface);
    layer->SetBuffer(info.buffer, info.fence, info.preBuffer, info.preFence);
    layer->SetZorder(info.zOrder);
    layer->SetAlpha(info.alpha);
    layer->SetLayerSize(info.dstRect);
    layer->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    layer->SetVisibleRegion(1, info.srcRect);
    layer->SetDirtyRegion(info.srcRect);
    layer->SetBlendType(info.blendType);
    layer->SetCropRect(info.srcRect);
    layers.emplace_back(layer);
}

void RsRenderServiceUtil::DrawBuffer(SkCanvas* canvas, const SkMatrix& matrix, sptr<OHOS::SurfaceBuffer> buffer,
    float tranX, float tranY, float width, float height, int32_t bufferWidth, int32_t bufferHeight)
{
    if (!canvas) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer canvas is nullptr");
        return;
    }
    if (!buffer) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer buffer is nullptr");
        return;
    }
    auto addr = static_cast<uint32_t*>(buffer->GetVirAddr());
    if (addr == nullptr || bufferWidth <= 0 || bufferHeight <= 0) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer this buffer have no vir add or width or height is negative");
        return;
    }
    SkColorType colorType;
    colorType = buffer->GetFormat() == PIXEL_FMT_BGRA_8888 ? kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
    SkImageInfo layerInfo = SkImageInfo::Make(bufferWidth, bufferHeight,
        colorType, kPremul_SkAlphaType);
    SkPixmap pixmap(layerInfo, addr, layerInfo.bytesPerPixel() * bufferWidth);
    SkBitmap bitmap;
    float scaleX = width / static_cast<float>(bufferWidth);
    float scaleY = height / static_cast<float>(bufferHeight);
    if (bitmap.installPixels(pixmap)) {
        canvas->save();
        canvas->setMatrix(matrix);
        canvas->translate(tranX, tranY);
        canvas->scale(scaleX, scaleY);
        canvas->drawBitmapRect(bitmap, SkRect::MakeXYWH(0, 0, bufferWidth, bufferHeight), nullptr);
        canvas->restore();
    }
}

void RsRenderServiceUtil::DrawBuffer(SkCanvas* canvas, const SkMatrix& matrix, sptr<OHOS::SurfaceBuffer> buffer,
        RSSurfaceRenderNode& node)
{
    DrawBuffer(canvas, matrix, node.GetBuffer(),
        node.GetRenderProperties().GetBoundsPositionX(), node.GetRenderProperties().GetBoundsPositionY(),
        node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight(),
        node.GetDamageRegion().w, node.GetDamageRegion().h);
}

} // namespace Rosen
} // namespace OHOS
