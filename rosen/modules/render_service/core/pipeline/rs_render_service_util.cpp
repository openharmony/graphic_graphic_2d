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
    layer->SetBuffer(info.buffer, info.fence);
    layer->SetZorder(info.zOrder);
    layer->SetAlpha(info.alpha);
    layer->SetLayerSize(info.dstRect);
    layer->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    layer->SetVisibleRegion(1, info.srcRect);
    layer->SetDirtyRegion(info.srcRect);
    layer->SetBlendType(BlendType::BLEND_SRCOVER);
    layer->SetCropRect(info.srcRect);
    layers.emplace_back(layer);
}

void RsRenderServiceUtil::DrawBuffer(SkCanvas* canvas, const SkMatrix& matrix, sptr<OHOS::SurfaceBuffer> buffer,
    float tranX, float tranY, float width, float height)
{
    if (!canvas) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer canvas is nullptr");
        return;
    }
    auto addr = static_cast<uint32_t*>(buffer->GetVirAddr());
    if (addr == nullptr || buffer->GetWidth() <= 0 || buffer->GetHeight() <= 0) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer this buffer have no vir add or width or height is negative");
        return;
    }
    SkImageInfo layerInfo = SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    SkPixmap pixmap(layerInfo, addr, buffer->GetSize() / buffer->GetHeight());
    SkBitmap bitmap;
    float scaleX = width / static_cast<float>(buffer->GetWidth());
    float scaleY = height / static_cast<float>(buffer->GetHeight());
    if (bitmap.installPixels(pixmap)) {
        canvas->save();
        canvas->setMatrix(matrix);
        canvas->translate(tranX, tranY);
        canvas->scale(scaleX, scaleY);
        canvas->drawBitmapRect(bitmap, SkRect::MakeXYWH(0, 0, buffer->GetWidth(), buffer->GetHeight()), nullptr);
        canvas->restore();
    }
}

void RsRenderServiceUtil::DrawBuffer(SkCanvas* canvas, const SkMatrix& matrix, sptr<OHOS::SurfaceBuffer> buffer,
        RSSurfaceRenderNode& node)
{
    DrawBuffer(canvas, matrix, node.GetBuffer(),
        node.GetRenderProperties().GetBoundsPositionX(), node.GetRenderProperties().GetBoundsPositionY(),
        node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight());
}

} // namespace Rosen
} // namespace OHOS