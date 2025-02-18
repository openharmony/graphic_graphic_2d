/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"

#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature/uifirst/rs_ui_first_render_listener.h"
#include "impl_interface/region_impl.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif

namespace OHOS::Rosen::DrawableV2 {
bool RSSurfaceRenderNodeDrawable::UseDmaBuffer()
{
    bool useDmaBuffer = RSUifirstManager::Instance().GetUseDmaBuffer(name_);
    return useDmaBuffer;
}

#ifndef ROSEN_CROSS_PLATFORM
bool RSSurfaceRenderNodeDrawable::CreateSurface()
{
    auto consumer = surfaceHandlerUiFirst_->GetConsumer();
    if (consumer != nullptr && surface_ != nullptr) {
        RS_LOGI("RSSurfaceRenderNodeDrawable::CreateSurface already created, return");
        return true;
    }
    if (auto& params = GetRenderParams()) {
        auto size = params->GetCacheSize();
        boundsWidth_ = size.x_;
        boundsHeight_ = size.y_;
    } else {
        RS_LOGE("RSSurfaceRenderNodeDrawable::CreateSurface cannot get cachesize");
        return false;
    }
    if (consumerListener_ == nullptr) {
        consumerListener_ = new RSUIFirstRenderListener(surfaceHandlerUiFirst_);
    }
    consumer = IConsumerSurface::Create(name_);
    if (consumer == nullptr) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer->RegisterConsumerListener(consumerListener_);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    auto producer = consumer->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    if (surface == nullptr) {
        return false;
    }
    const uint8_t queueSize = 3;
    surface->SetQueueSize(queueSize);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    surface_ = client->CreateRSSurface(surface);
    surfaceCreated_ = true;
    surfaceHandlerUiFirst_->SetConsumer(consumer);
    return true;
}
#endif

BufferRequestConfig RSSurfaceRenderNodeDrawable::GetFrameBufferRequestConfig()
{
    BufferRequestConfig config {};
    config.width = static_cast<int32_t>(boundsWidth_);
    config.height = static_cast<int32_t>(boundsHeight_);
    config.strideAlignment = 0x8;
    config.colorGamut = GRAPHIC_COLOR_GAMUT_SRGB;
    config.format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    config.usage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA;
    config.timeout = 0;
    return config;
}

std::unique_ptr<RSRenderFrame> RSSurfaceRenderNodeDrawable::RequestFrame(
    RenderContext* renderContext, std::shared_ptr<Drawing::GPUContext> skContext)
{
    RS_TRACE_NAME("RSSurfaceRenderNodeDrawable:RequestFrame");
    auto rsSurface = std::static_pointer_cast<RSSurfaceOhos>(surface_);
    if (rsSurface == nullptr) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::RequestFrame: surface is null!");
        return nullptr;
    }
#ifdef RS_ENABLE_VK
    RSTagTracker tagTracker(skContext.get(), RSTagTracker::TAGTYPE::TAG_ACQUIRE_SURFACE);
#endif
    auto config = GetFrameBufferRequestConfig();
    rsSurface->SetColorSpace(config.colorGamut);
    rsSurface->SetSurfacePixelFormat(config.format);

    auto bufferUsage = config.usage;
#if !((defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE))
    bufferUsage |= BUFFER_USAGE_CPU_WRITE;
#endif
    rsSurface->SetSurfaceBufferUsage(bufferUsage);

#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL &&
        renderContext != nullptr) {
        rsSurface->SetRenderContext(renderContext);
    }
#endif
#ifdef RS_ENABLE_VK
    if ((RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) && skContext != nullptr) {
        std::static_pointer_cast<RSSurfaceOhosVulkan>(rsSurface)->SetSkContext(skContext);
    }
#endif
    auto surfaceFrame = rsSurface->RequestFrame(config.width, config.height, 0);
    if (surfaceFrame == nullptr) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::RequestFrame: request SurfaceFrame failed!");
        return nullptr;
    }
    return std::make_unique<RSRenderFrame>(rsSurface, std::move(surfaceFrame));
}

bool RSSurfaceRenderNodeDrawable::DrawUIFirstCacheWithDma(
    RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams)
{
    RS_TRACE_NAME("DrawUIFirstCacheWithDma");
    if (surfaceParams.GetHardwareEnabled()) {
        return true;
    }
    if (!surfaceHandlerUiFirst_->GetBuffer() && surfaceHandlerUiFirst_->GetAvailableBufferCount() <= 0) {
        RS_TRACE_NAME_FMT("HandleSubThreadNode wait %" PRIu64 "", surfaceParams.GetId());
        RSSubThreadManager::Instance()->WaitNodeTask(surfaceParams.GetId());
    }
    if (!RSBaseRenderUtil::ConsumeAndUpdateBuffer(*surfaceHandlerUiFirst_) ||
        !surfaceHandlerUiFirst_->GetBuffer()) {
        RS_LOGE("DrawUIFirstCacheWithDma ConsumeAndUpdateBuffer or GetBuffer return false");
        return false;
    }
    DrawDmaBufferWithGPU(canvas);
    return true;
}

void RSSurfaceRenderNodeDrawable::DrawDmaBufferWithGPU(RSPaintFilterCanvas& canvas)
{
    auto buffer = surfaceHandlerUiFirst_->GetBuffer();
    BufferDrawParam param;
    param.srcRect = {0, 0, buffer->GetWidth(), buffer->GetHeight()};
    param.dstRect = {0, 0, boundsWidth_, boundsHeight_};
    param.threadIndex = 0;
    param.buffer = buffer;
    param.acquireFence = surfaceHandlerUiFirst_->GetAcquireFence();
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    renderEngine->RegisterDeleteBufferListener(surfaceHandlerUiFirst_->GetConsumer());
    renderEngine->RegisterDeleteBufferListener(*surfaceHandlerUiFirst_);
    renderEngine->DrawUIFirstCacheWithParams(canvas, param);
    RSBaseRenderUtil::ReleaseBuffer(*surfaceHandlerUiFirst_);
}

void RSSurfaceRenderNodeDrawable::ClipRoundRect(Drawing::Canvas& canvas)
{
    if (!uifirstRenderParams_) {
        return;
    }
    auto uifirstParams = static_cast<RSSurfaceRenderParams*>(uifirstRenderParams_.get());
    if (!uifirstParams) {
        return;
    }
    RRect rrect = uifirstParams->GetRRect();
    canvas.ClipRoundRect(RSPropertiesPainter::RRect2DrawingRRect(rrect), Drawing::ClipOp::INTERSECT, true);
}

void RSSurfaceRenderNodeDrawable::ClearBufferQueue()
{
    if (surface_ != nullptr) {
        surface_->ClearBuffer();
        surface_ = nullptr;
    }
    auto consumer = surfaceHandlerUiFirst_->GetConsumer();
    if (consumer != nullptr) {
        consumer->GoBackground();
        surfaceHandlerUiFirst_->SetConsumer(nullptr);
    }
    surfaceHandlerUiFirst_->ResetBufferAvailableCount();
    surfaceHandlerUiFirst_->CleanCache();
    surfaceCreated_ = false;
}
} // namespace OHOS::Rosen::DrawableV2
