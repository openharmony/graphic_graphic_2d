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
#include "pipeline/rs_render_service_util.h"

#include <cmath>

#include "common/rs_common_def.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkBitmap.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_blur_filter.h"

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
    layer->SetCompositionType(IsNeedClient(node) ?
        CompositionType::COMPOSITION_CLIENT : CompositionType::COMPOSITION_DEVICE);
    layer->SetVisibleRegion(1, info.srcRect);
    layer->SetDirtyRegion(info.srcRect);
    layer->SetBlendType(info.blendType);
    layer->SetCropRect(info.srcRect);
    layers.emplace_back(layer);
}

bool RsRenderServiceUtil::IsNeedClient(RSSurfaceRenderNode* node)
{
    if (node == nullptr) {
        ROSEN_LOGE("RsRenderServiceUtil::ComposeSurface node is empty");
        return false;
    }
    auto filter = std::static_pointer_cast<RSBlurFilter>(node->GetRenderProperties().GetBackgroundFilter());
    if (filter != nullptr && filter->GetBlurRadiusX() > 0 && filter->GetBlurRadiusY() > 0) {
        return true;
    }
    auto transitionProperties = node->GetAnimationManager().GetTransitionProperties();
    if (!transitionProperties) {
        return false;
    }
    SkMatrix matrix = transitionProperties->GetRotate();
    float value[9];
    matrix.get9(value);
    float rAngle = -round(atan2(value[SkMatrix::kMSkewX], value[SkMatrix::kMScaleX]) * (180 / PI));
    return rAngle > 0;
}

void RsRenderServiceUtil::DealAnimation(SkCanvas* canvas, SkPaint& paint, const RSProperties& property,
    const std::unique_ptr<RSTransitionProperties>& transitionProperties)
{
    if (!canvas || !transitionProperties) {
        ROSEN_LOGD("RsRenderServiceUtil::dealAnimation canvas or properties is nullptr");
        return;
    }
    paint.setAlphaf(paint.getAlphaf() * transitionProperties->GetAlpha());
    auto translate = transitionProperties->GetTranslate();
    canvas->translate(translate.x_, translate.y_);

    // scale and rotate about the center of node, currently scaleZ is not used
    auto center = property.GetBoundsSize() * 0.5f;
    auto scale = transitionProperties->GetScale();
    canvas->translate(center.x_, center.y_);
    canvas->scale(scale.x_, scale.y_);
    canvas->concat(transitionProperties->GetRotate());
    canvas->translate(-center.x_, -center.y_);
}

void RsRenderServiceUtil::DrawBuffer(SkCanvas* canvas, sptr<OHOS::SurfaceBuffer> buffer,
        RSSurfaceRenderNode& node, bool isDrawnOnDisplay)
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
    paint.setAlphaf(node.GetAlpha() * node.GetRenderProperties().GetAlpha());
    if (bitmap.installPixels(pixmap)) {
        canvas->save();
        std::unique_ptr<SkRect> rect =
            std::make_unique<SkRect>(SkRect::MakeXYWH(0, 0, buffer->GetWidth(), buffer->GetHeight()));
        if (isDrawnOnDisplay) {
            const RSProperties& property = node.GetRenderProperties();
            auto geotry = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
            if (geotry) {
                canvas->setMatrix(geotry->GetAbsMatrix());
            }
            DealAnimation(canvas, paint, property, node.GetAnimationManager().GetTransitionProperties());
            auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
            if (filter != nullptr) {
                RSPropertiesPainter::SaveLayerForFilter(property, (*canvas), filter, rect);
                RSPropertiesPainter::RestoreForFilter(*canvas);
            }
        }
        canvas->drawBitmapRect(bitmap, (*rect), &paint);
        canvas->restore();
    }
}

} // namespace Rosen
} // namespace OHOS