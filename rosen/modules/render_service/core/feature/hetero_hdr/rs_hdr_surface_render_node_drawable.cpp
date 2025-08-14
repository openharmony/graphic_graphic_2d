/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <memory>

#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "draw/brush.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/hetero_hdr/rs_hdr_manager.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "hetero_hdr/rs_hdr_pattern_manager.h"
#include "impl_interface/region_impl.h"
#include "memory/rs_tag_tracker.h"
#include "metadata_helper.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "rs_frame_report.h"
#include "rs_profiler.h"
#include "rs_trace.h"
#include "utils/rect.h"
#include "utils/region.h"

#ifdef RS_ENABLE_VK
#include "include/gpu/GrBackendSurface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

constexpr float GAMMA2_2 = 2.2f;
const int RS_ROTATION_90 = -90;
const int RS_ROTATION_180 = -180;
const int RS_ROTATION_270 = -270;

namespace OHOS::Rosen::DrawableV2 {

bool RSSurfaceRenderNodeDrawable::GetCurHeterogComputingHdr() const
{
    return g_HDRHeterRenderContext.curHeterogComputingHdr_;
}

void RSSurfaceRenderNodeDrawable::SetCurHeterogComputingHdr(bool curCondition)
{
    g_HDRHeterRenderContext.curHeterogComputingHdr_ = curCondition;
}

bool RSSurfaceRenderNodeDrawable::DrawHDRCacheWithDmaFFRT(
    RSPaintFilterCanvas &canvas, RSSurfaceRenderParams &surfaceParams)
{
    g_HDRHeterRenderContext.curHeterogComputingHdr_ = false;
    if (surfaceParams.GetHardwareEnabled() || !(RSHdrManager::Instance().GetCurFrameHeterogHandleCanBeUsed())) {
        return false;
    }
    if (RSHdrManager::Instance().IsHeterogComputingHdrOn() && GetVideoHdrStatus() == HdrStatus::HDR_VIDEO) {
        std::shared_ptr<RSSurfaceHandler> hdrSurfaceHandler =
            g_HDRHeterRenderContext.rsHdrBufferLayer_->GetMutableRSSurfaceHandler();

        if (hdrSurfaceHandler == nullptr) {
            RS_LOGE("hdrSurfaceHandler_ is nullptr!!");
            return false;
        }
        if (!RSBaseRenderUtil::ConsumeAndUpdateBuffer(*hdrSurfaceHandler, CONSUME_DIRECTLY,
            false, 0, true) || !hdrSurfaceHandler->GetBuffer()) {
            RS_LOGE("DrawHDRCacheWithDmaFFRT ConsumeAndUpdateBuffer or GetBuffer return false");
            return false;
        }
        RSHDRPatternManager::Instance().SetThreadId();

        bool isSelfDrawingSurface = (surfaceParams.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE) &&
                                    !surfaceParams.IsSpherizeValid() && !surfaceParams.IsAttractionValid();
        if (isSelfDrawingSurface) {
            SetSkip(surfaceParams.GetBuffer() != nullptr ? SkipType::SKIP_BACKGROUND_COLOR : SkipType::NONE);
            canvas.Save();
            DrawHDRBufferWithGPU(canvas);
            canvas.Restore();
        } else {
            DrawHDRBufferWithGPU(canvas);
        }

        g_HDRHeterRenderContext.curHeterogComputingHdr_ = true;
        return true;
    }
    return false;
}

void RSSurfaceRenderNodeDrawable::SetHpaeDstRect(RectI boundSize)
{
    g_HDRHeterRenderContext.aaeDstRect_ = {boundSize.GetLeft(), boundSize.GetTop(),
        boundSize.GetRight(), boundSize.GetBottom()};
}

void RSSurfaceRenderNodeDrawable::SetNodeDrawableNodeDstRect(bool isFixedDstBuffer, RectI boundSize)
{
    g_HDRHeterRenderContext.isFixedDstBuffer_ = isFixedDstBuffer;
    g_HDRHeterRenderContext.boundSize_ = boundSize;
}

BufferDrawParam RSSurfaceRenderNodeDrawable::InitBufferDrawParam(RSSurfaceRenderParams* surfaceParams)
{
    BufferDrawParam param;
    param.threadIndex = gettid();

    if (surfaceParams == nullptr) {
        RS_LOGE("surfaceParams is nullptr!");
        return param;
    }
    auto dstRect = surfaceParams->GetLayerInfo().dstRect;
    GraphicTransformType bufferTransform = surfaceParams->GetBuffer()->GetSurfaceBufferTransform();

    if (g_HDRHeterRenderContext.isFixedDstBuffer_) {
        param.srcRect = {0, 0, g_HDRHeterRenderContext.aaeDstRect_.right, g_HDRHeterRenderContext.aaeDstRect_.bottom};
        param.dstRect = {0, 0, surfaceParams->GetCacheSize().x_, surfaceParams->GetCacheSize().y_};
        if (bufferTransform == GraphicTransformType::GRAPHIC_ROTATE_90 ||
            bufferTransform == GraphicTransformType::GRAPHIC_ROTATE_270) {
            param.srcRect = {0, 0, g_HDRHeterRenderContext.aaeDstRect_.bottom,
                g_HDRHeterRenderContext.aaeDstRect_.right};
        }
    } else {
        param.srcRect = {0, 0, g_HDRHeterRenderContext.aaeDstRect_.right, g_HDRHeterRenderContext.aaeDstRect_.bottom};
        param.dstRect = {dstRect.x, dstRect.y, dstRect.x + dstRect.w, dstRect.y + dstRect.h};
    }

    if ((param.srcRect.GetWidth() == 0.0f) || (param.srcRect.GetHeight() == 0.0f)) {
        RS_LOGE("Skipping SetScaleTranslate due to zero division");
    } else {
        auto sx = param.dstRect.GetWidth() / param.srcRect.GetWidth();
        auto sy = param.dstRect.GetHeight() / param.srcRect.GetHeight();
        auto tx = param.dstRect.GetLeft() - param.srcRect.GetLeft() * sx;
        auto ty = param.dstRect.GetTop() - param.srcRect.GetTop() * sy;
        param.matrix.SetScaleTranslate(sx, sy, tx, ty);
    }

    switch (bufferTransform) {
        case GraphicTransformType::GRAPHIC_ROTATE_90: {
            param.matrix.PreTranslate(0, param.srcRect.GetHeight());
            param.matrix.PreRotate(RS_ROTATION_90);  // rotate 90 degrees anti-clockwise at last.
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_180: {
            param.matrix.PreTranslate(param.srcRect.GetWidth(), param.srcRect.GetHeight());
            param.matrix.PreRotate(RS_ROTATION_180);  // rotate 180 degrees anti-clockwise at last.
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_270: {
            param.matrix.PreTranslate(param.srcRect.GetWidth(), 0);
            param.matrix.PreRotate(RS_ROTATION_270);  // rotate 270 degrees anti-clockwise at last.
            break;
        }
        default: break;
    }

    return param;
}

void RSSurfaceRenderNodeDrawable::DrawHDRBufferWithGPU(RSPaintFilterCanvas &canvas)
{
    if (g_HDRHeterRenderContext.rsHdrBufferLayer_ == nullptr) {
        RS_LOGE("g_HDRHeterRenderContext.rsHdrBufferLayer_ is nullptr!");
        return;
    }
    std::shared_ptr<RSSurfaceHandler> hdrSurfaceHandler =
        g_HDRHeterRenderContext.rsHdrBufferLayer_->GetMutableRSSurfaceHandler();

    if (hdrSurfaceHandler == nullptr) {
        RS_LOGE("hdrSurfaceHandler is nullptr!");
        return;
    }
    auto buffer = hdrSurfaceHandler->GetBuffer();

    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(GetRenderParams().get());
    BufferDrawParam param = InitBufferDrawParam(surfaceParams);
    Drawing::Matrix matrix = surfaceParams->GetLayerInfo().matrix;

    auto bgColor = surfaceParams->GetBackgroundColor();
    if (surfaceParams->GetHardwareEnabled() && surfaceParams->GetIsHwcEnabledBySolidLayer()) {
        bgColor = surfaceParams->GetSolidLayerColor();
        RS_LOGD("solidLayer enabled, %{public}s, brush set color: %{public}08x", __func__, bgColor.AsArgbInt());
    }
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));

    canvas.AttachBrush(brush);
    canvas.DrawRect(surfaceParams->GetBounds());
    canvas.DetachBrush();

    canvas.Save();
    if (!g_HDRHeterRenderContext.isFixedDstBuffer_) {
        Drawing::Matrix matrixInv;
        matrix.Invert(matrixInv);
        canvas.ConcatMatrix(matrixInv);
    }

    canvas.ConcatMatrix(param.matrix);
    matrix = canvas.GetTotalMatrix();

    param.dstRect = {0, 0, g_HDRHeterRenderContext.aaeDstRect_.right, g_HDRHeterRenderContext.aaeDstRect_.bottom};
    param.buffer = buffer;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    param.sdrNits = surfaceParams->GetSdrNit();
    param.tmoNits = surfaceParams->GetDisplayNit();
    param.displayNits = param.tmoNits / std::pow(surfaceParams->GetBrightnessRatio(), GAMMA2_2);
#endif

    param.acquireFence = hdrSurfaceHandler->GetAcquireFence();
    param.isHeterog = true;

    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    renderEngine->RegisterDeleteBufferListener(hdrSurfaceHandler->GetConsumer());
    renderEngine->DrawHDRCacheWithParams(canvas, param);
    
    RSBaseRenderUtil::ReleaseBuffer(*hdrSurfaceHandler);
    canvas.Restore();
}

std::shared_ptr<RSHDRBUfferLayer> RSSurfaceRenderNodeDrawable::GetRsHdrBUfferLayer()
{
    return g_HDRHeterRenderContext.rsHdrBufferLayer_;
}

void RSSurfaceRenderNodeDrawable::SetVideoHdrStatus(HdrStatus hasHdrVideoSurface)
{
    g_HDRHeterRenderContext.hdrSatus_ = hasHdrVideoSurface;
}

HdrStatus RSSurfaceRenderNodeDrawable::GetVideoHdrStatus()
{
    return g_HDRHeterRenderContext.hdrSatus_;
}
}  // namespace OHOS::Rosen::DrawableV2