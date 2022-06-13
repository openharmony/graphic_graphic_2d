/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "pipeline/rs_uni_render_service_util.h"

#include "common/rs_obj_abs_geometry.h"
#include "display_type.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_service_util.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"
#include "rs_trace.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
bool RsUniRenderServiceUtil::isUniRender()
{
    auto uniRenderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    return uniRenderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL ||
        uniRenderType == UniRenderEnabledType::UNI_RENDER_PARTIALLY_ENABLED;
}

void RsUniRenderServiceUtil::DrawBufferOnCanvas(RSSurfaceRenderNode& node, std::unique_ptr<RSPaintFilterCanvas>& canvas)
{
    if (!canvas) {
        RS_LOGE("RSUniRenderVisitor::DrawBufferOnCanvas canvas is nullptr");
    }
    auto buffer = node.GetBuffer();
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setAlphaf(node.GetContextAlpha() * node.GetRenderProperties().GetAlpha());
    canvas->save();
    const RSProperties& property = node.GetRenderProperties();
    auto params = RsRenderServiceUtil::CreateBufferDrawParam(node, SkMatrix(), ScreenRotation::ROTATION_0, paint);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(node.GetRenderProperties().GetBoundsPositionX(),
            node.GetRenderProperties().GetBoundsPositionY(),
            node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas, filter, skRectPtr);
    }
    RsRenderServiceUtil::DrawBuffer(*canvas, params);
}

#ifdef RS_ENABLE_EGLIMAGE
void RsUniRenderServiceUtil::DrawImageOnCanvas(RSSurfaceRenderNode& node, std::unique_ptr<RSPaintFilterCanvas>& canvas)
{
    if (!canvas) {
        RS_LOGE("DrawImageOnCanvas canvas is nullptr");
    }
    auto buffer = node.GetBuffer();
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setAlphaf(node.GetContextAlpha() * node.GetRenderProperties().GetAlpha());
    canvas->save();
    const RSProperties& property = node.GetRenderProperties();
    auto params = RsRenderServiceUtil::CreateBufferDrawParam(node, SkMatrix(), ScreenRotation::ROTATION_0, paint);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(node.GetRenderProperties().GetBoundsPositionX(),
            node.GetRenderProperties().GetBoundsPositionY(),
            node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas, filter, skRectPtr);
    }
    if (buffer->GetFormat() == PIXEL_FMT_YCRCB_420_SP || buffer->GetFormat() == PIXEL_FMT_YCBCR_420_SP) {
        RsRenderServiceUtil::DrawBuffer(*canvas, params);
    } else {
        auto mainThread = RSMainThread::Instance();
        std::shared_ptr<RenderContext> renderContext;
        std::shared_ptr<RSEglImageManager> eglImageManager;
        if (mainThread != nullptr) {
            renderContext = mainThread->GetRenderContext();
            eglImageManager =  mainThread->GetRSEglImageManager();
        }
        RsRenderServiceUtil::DrawImage(eglImageManager, renderContext->GetGrContext(), *canvas, params, nullptr);
        auto consumerSurface = node.GetConsumer();
        GSError error = consumerSurface->RegisterDeleteBufferListener([eglImageManager_ = eglImageManager]
            (int32_t bufferId) {
                eglImageManager_->UnMapEglImageFromSurfaceBuffer(bufferId);
        });
        if (error != GSERROR_OK) {
            RS_LOGE("DrawImageOnCanvas: fail to register UnMapEglImage callback.");
        }
    }
}
#endif // RS_ENABLE_EGLIMAGE
} // namespace Rosen
} // namespace OHOS
