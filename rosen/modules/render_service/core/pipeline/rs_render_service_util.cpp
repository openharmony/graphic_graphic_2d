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

#include "common/rs_common_def.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkBitmap.h"
#include "pipeline/rs_render_service_util.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

void RsRenderServiceUtil::ComposeSurface(std::shared_ptr<HdiLayerInfo> layer, sptr<Surface> consumerSurface,
    std::vector<LayerInfoPtr>& layers,  ComposeInfo info, RSSurfaceRenderNode* node)
{
    layer->SetSurface(consumerSurface);
    layer->SetBuffer(info.buffer, info.fence, info.preBuffer, info.preFence);
    layer->SetZorder(info.zOrder);
    layer->SetAlpha(info.alpha);
    layer->SetLayerSize(info.dstRect);
    layer->SetLayerAdditionalInfo(node);
    if (node == nullptr || ROSEN_EQ(node->GetRenderProperties().GetRotation(), 0.f)) {
        layer->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    } else {
        layer->SetCompositionType(CompositionType::COMPOSITION_CLIENT);
    }
    layer->SetVisibleRegion(1, info.srcRect);
    layer->SetDirtyRegion(info.srcRect);
    layer->SetBlendType(info.blendType);
    layer->SetCropRect(info.srcRect);
    layers.emplace_back(layer);
}

void RsRenderServiceUtil::DrawBuffer(SkCanvas* canvas, sptr<OHOS::SurfaceBuffer> buffer,
    const std::shared_ptr<RSObjAbsGeometry> geotry, float alpha)
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
    if (addr == nullptr || buffer->GetWidth() <= 0 || buffer->GetHeight() <= 0) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer this buffer have no vir add or width or height is negative");
        return;
    }
    SkColorType colorType;
    colorType = buffer->GetFormat() == PIXEL_FMT_BGRA_8888 ? kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
    SkImageInfo layerInfo = SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(),
        colorType, kPremul_SkAlphaType);
    SkPixmap pixmap(layerInfo, addr, buffer->GetStride());
    SkBitmap bitmap;
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setAlphaf(alpha);
    if (bitmap.installPixels(pixmap)) {
        canvas->save();
        if (geotry) {
            canvas->setMatrix(geotry->GetAbsMatrix());
        }
        canvas->drawBitmapRect(bitmap, SkRect::MakeXYWH(0, 0, buffer->GetWidth(), buffer->GetHeight()), &paint);
        canvas->restore();
    }
}

void RsRenderServiceUtil::DrawBuffer(SkCanvas* canvas, sptr<OHOS::SurfaceBuffer> buffer,
        RSSurfaceRenderNode& node)
{
    DrawBuffer(canvas, node.GetBuffer(),
        std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry()),
        node.GetAlpha() * node.GetRenderProperties().GetAlpha());
}

} // namespace Rosen
} // namespace OHOS