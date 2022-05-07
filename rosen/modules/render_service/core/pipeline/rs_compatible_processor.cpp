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

#include "pipeline/rs_compatible_processor.h"

#include "sync_fence.h"

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_util.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"

namespace OHOS {

namespace Rosen {

RSCompatibleProcessor::RSCompatibleProcessor() {}

RSCompatibleProcessor::~RSCompatibleProcessor() {}

void RSCompatibleProcessor::Init(ScreenId id, int32_t offsetX, int32_t offsetY)
{
    offsetX_ = offsetX;
    offsetY_ = offsetY;
    backend_ = HdiBackend::GetInstance();
    screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        RS_LOGE("RSCompatibleProcessor::Init ScreenManager is nullptr");
        return;
    }
    output_ = screenManager_->GetOutput(id);

    consumerSurface_ = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new RSRenderBufferListener(*this);
    SurfaceError ret = consumerSurface_->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSCompatibleProcessor::Init Register Consumer Listener fail");
        return;
    }
    sptr<IBufferProducer> producer = consumerSurface_->GetProducer();
    producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    BufferRequestConfig requestConfig = {
        .width = 2800,
        .height = 1600,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    rsSurface_ = std::make_shared<RSSurfaceOhosRaster>(producerSurface_);
    auto skCanvas = CreateCanvas(rsSurface_, requestConfig);
    if (skCanvas == nullptr) {
        RS_LOGE("RSHardwareProcessor::Redraw: canvas is null.");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(skCanvas);
}

void RSCompatibleProcessor::ProcessSurface(RSSurfaceRenderNode& node)
{
    RS_LOGI("RsDebug RSCompatibleProcessor::ProcessSurface start node:%llu available buffer:%d", node.GetId(),
        node.GetAvailableBufferCount());
    if (!canvas_) {
        RS_LOGE("RsDebug RSCompatibleProcessor::ProcessSurface canvas is nullptr");
        return;
    }
    OHOS::sptr<SurfaceBuffer> cbuffer;
    RSProcessor::SpecialTask task = [&node, &cbuffer] () -> void{
        if (cbuffer != node.GetBuffer() && node.GetBuffer() != nullptr) {
            auto& surfaceConsumer = node.GetConsumer();
            SurfaceError ret = surfaceConsumer->ReleaseBuffer(node.GetBuffer(), SyncFence::INVALID_FENCE);
            if (ret != SURFACE_ERROR_OK) {
                RS_LOGE("RSCompatibleProcessor::ProcessSurface: ReleaseBuffer buffer error! error: %d.", ret);
                return;
            }
        }
    };
    bool ret = ConsumeAndUpdateBuffer(node, task, cbuffer);
    if (!ret) {
        RS_LOGE("RsDebug RSCompatibleProcessor::ProcessSurface consume buffer fail");
        return;
    }

    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGE("RsDebug RSCompatibleProcessor::ProcessSurface geoPtr == nullptr");
        return;
    }

    RS_LOGI("RsDebug RSCompatibleProcessor::ProcessSurface surfaceNode id:%llu [%d %d %d %d] buffer [%d %d]",
        node.GetId(), geoPtr->GetAbsRect().left_, geoPtr->GetAbsRect().top_,
        geoPtr->GetAbsRect().width_, geoPtr->GetAbsRect().height_,
        node.GetDamageRegion().w, node.GetDamageRegion().y);

    SkMatrix matrix;
    matrix.reset();
    auto params = RsRenderServiceUtil::CreateBufferDrawParam(node);
    RsRenderServiceUtil::DrawBuffer(*canvas_, params);
}

void RSCompatibleProcessor::PostProcess()
{
    rsSurface_->FlushFrame(currFrame_);
}

void RSCompatibleProcessor::DoComposeSurfaces()
{
    if (!backend_ || !output_ || !consumerSurface_) {
        RS_LOGE("RSCompatibleProcessor::DoComposeSurfaces either backend output or consumer is nullptr");
        return;
    }

    OHOS::sptr<SurfaceBuffer> cbuffer = nullptr;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp;
    Rect damage;
    auto sret = consumerSurface_->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);
    if (!cbuffer || sret != OHOS::SURFACE_ERROR_OK) {
        RS_LOGE("RSCompatibleProcessor::DoComposeSurfaces: AcquireBuffer failed!");
        return;
    }
    RS_LOGI("RSCompatibleProcessor::DoComposeSurfaces start");

    ComposeInfo info = {
        .srcRect = {
            .x = 0,
            .y = 0,
            .w = cbuffer->GetWidth(),
            .h = cbuffer->GetHeight(),
        },
        .dstRect = {
            .x = 0,
            .y = 0,
            .w = cbuffer->GetWidth(),
            .h = cbuffer->GetHeight(),
        },
        .alpha = alpha_,
        .buffer = cbuffer,
        .fence = std::move(acquireFence),
    };
    std::shared_ptr<HdiLayerInfo> layer = HdiLayerInfo::CreateHdiLayerInfo();
    std::vector<LayerInfoPtr> layers;
    RsRenderServiceUtil::ComposeSurface(layer, consumerSurface_, layers, info);
    output_->SetLayerInfo(layers);
    std::vector<std::shared_ptr<HdiOutput>> outputs{output_};
    backend_->Repaint(outputs);
}

RSCompatibleProcessor::RSRenderBufferListener::~RSRenderBufferListener() {}

RSCompatibleProcessor::RSRenderBufferListener::RSRenderBufferListener(RSCompatibleProcessor& processor) : processor_(processor) {}

void RSCompatibleProcessor::RSRenderBufferListener::OnBufferAvailable()
{
    processor_.DoComposeSurfaces();
}


} // namespace Rosen
} // namespace OHOS
