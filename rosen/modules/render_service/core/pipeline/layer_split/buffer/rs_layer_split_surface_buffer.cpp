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

#include "pipeline/layer_split/buffer/rs_layer_split_surface_buffer.h"

#if defined(ROSEN_OHOS)

#include "common/rs_optional_trace.h"
#include "engine/rs_base_render_engine.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_surface_handler.h"
#include "platform/common/rs_log.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif
#include "rs_trace.h"
#include "ffrt_inner.h"

namespace OHOS::Rosen {

const std::string SPLIT_SURFACE_BUFFER_NAME = "hpae_memory_split";
constexpr int FFRT_QOS_HPAE_HIGH = 5;

RSSplitSurfaceBuffer::RSSplitSurfaceBuffer(const std::string& name, const int layerId, int32_t width, int32_t height)
{
    layerName_ = name;
    surfaceHandler_ = std::make_shared<RSSurfaceHandler>(layerId);
    bufferConfig_ = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8, // default stride is 8 Bytes.
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = 0,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
}

RSSplitSurfaceBuffer::~RSSplitSurfaceBuffer()
{
    if (producerSurface_ != nullptr && isBufferPreAllocated_.load()) {
        producerSurface_->Connect();
        GSError cleanRet = producerSurface_->CleanCache(true);
        LAYER_SPLIT_LOGD("RSSplitSurfaceBuffer cleanRet:%{public}d", cleanRet);
    }
}

void RSSplitSurfaceBuffer::Init(bool isHebc)
{
    RS_OPTIONAL_TRACE_NAME("Init");
    isHebc_ = isHebc;
    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (UNLIKELY(!uniRenderEngine)) {
        LAYER_SPLIT_LOGE("Init RenderEngine is null!");
        return;
    }

    if (grContext_ == nullptr) {
        if (!uniRenderEngine->GetRenderContext()) {
            LAYER_SPLIT_LOGE("Exception: context is nullptr");
            return;
        }
        grContext_ = uniRenderEngine->GetRenderContext()->GetSharedDrGPUContext();
    }

    if (!surfaceCreated_) {
        sptr<IBufferConsumerListener> listener = new RSSplitSurfaceRenderListener(surfaceHandler_);
        RS_OPTIONAL_TRACE_NAME("create layer surface");
        if (!CreateSurface(listener)) {
            LAYER_SPLIT_LOGE("Init CreateSurface failed");
            return;
        }
    }

    if (rsSurface_ == nullptr) {
        LAYER_SPLIT_LOGE("surface is null!");
        return;
    }

#ifdef RS_ENABLE_VK
    if ((RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
         RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) && grContext_ != nullptr) {
        auto vulkanSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(rsSurface_);
        vulkanSurface->SetSkContext(grContext_);
    }
#endif
    rsSurface_->SetColorSpace(bufferConfig_.colorGamut);
    rsSurface_->SetSurfacePixelFormat(bufferConfig_.format);
    rsSurface_->SetSurfaceBufferUsage(bufferConfig_.usage);
}

void RSSplitSurfaceBuffer::PreAllocateBuffer()
{
    RS_OPTIONAL_TRACE_NAME("RSSplitSurfaceBuffer::PreAllocateBuffer");

    if (isBufferPreAllocated_.load(std::memory_order_acquire)) {
        return;
    }

    bool expected = false;
    bool desired = true;
    if (!isPreAllocInProgress_.compare_exchange_strong(expected, desired)) {
        return;
    }

    if (!surfaceCreated_) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::PreAllocateBuffer surface not created");
        isPreAllocInProgress_.store(false, std::memory_order_release);
        return;
    }

    if (producerSurface_ == nullptr) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::PreAllocateBuffer producerSurface_ is nullptr");
        isPreAllocInProgress_.store(false, std::memory_order_release);
        return;
    }

    BufferRequestConfig config = bufferConfig_;
    if ((config.format == GRAPHIC_PIXEL_FMT_RGBA_8888 || config.format == GRAPHIC_PIXEL_FMT_RGBA_1010108) && isHebc_) {
        config.usage =
            BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA;
    }

    ffrt::submit_h([config, weakSelf = weak_from_this()]() {
            auto self = weakSelf.lock();
            if (!self) {
                LAYER_SPLIT_LOGE("%{public}s self is nullptr", __func__);
                return;
            }
            self->PreAllocateBufferImpl(config);
        }, {}, {}, ffrt::task_attr().qos(FFRT_QOS_HPAE_HIGH));
}

void RSSplitSurfaceBuffer::PreAllocateBufferImpl(const BufferRequestConfig& bufferConfig)
{
    RS_OPTIONAL_TRACE_NAME("RSSplitSurfaceBuffer::PreAllocateBufferImpl");

    if (isBufferPreAllocated_.load(std::memory_order_acquire)) {
        isPreAllocInProgress_.store(false, std::memory_order_release);
        return;
    }

    if (producerSurface_ == nullptr) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::PreAllocateBufferImpl producerSurface_ is nullptr");
        isPreAllocInProgress_.store(false, std::memory_order_release);
        return;
    }

    GSError ret = producerSurface_->PreAllocBuffers(bufferConfig, SPLIT_SURFACE_BUFFER_SIZE);
    if (ret != GSERROR_OK) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::PreAllocateBufferImpl PreAllocBuffers failed, ret = %{public}d", ret);
        producerSurface_->Connect();
        GSError cleanRet = producerSurface_->CleanCache(true);
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::PreAllocateBufferImpl clean cache ret = %{public}d", cleanRet);
        isPreAllocInProgress_.store(false, std::memory_order_release);
        return;
    }

    isBufferPreAllocated_.store(true, std::memory_order_release);
    isPreAllocInProgress_.store(false, std::memory_order_release);
    LAYER_SPLIT_LOGD("RSSplitSurfaceBuffer::PreAllocateBufferImpl PreAllocBuffers success");
}

// reference to RSDisplayRenderNodeDrawable::RequestFrame
std::unique_ptr<RSRenderFrame> RSSplitSurfaceBuffer::RequestFrame(GraphicColorGamut colorSpace)
{
    RS_OPTIONAL_TRACE_NAME("RSSplitSurfaceBuffer:RequestFrame");
    if (!isBufferPreAllocated_.load(std::memory_order_acquire)) {
        RS_TRACE_NAME("RSSplitSurfaceBuffer::RequestFrame buffer not pre-allocated");
        return nullptr;
    }

    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (UNLIKELY(!uniRenderEngine)) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::RequestFrame RenderEngine is null!");
        return nullptr;
    }

    if (grContext_ == nullptr) {
        if (uniRenderEngine->GetRenderContext()) {
            grContext_ = uniRenderEngine->GetRenderContext()->GetSharedDrGPUContext();
        } else {
            LAYER_SPLIT_LOGE("Exception: context is nullptr");
            return nullptr;
        }
    }

    if (!surfaceCreated_) {
        sptr<IBufferConsumerListener> listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(surfaceHandler_);
        RS_OPTIONAL_TRACE_NAME("create layer surface");
        if (!CreateSurface(listener)) {
            LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::RequestFrame CreateSurface failed");
            return nullptr;
        }
    }

    if (rsSurface_ == nullptr) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::RequestFrame: surface is null!");
        return nullptr;
    }

    bufferConfig_.colorGamut = colorSpace;
    auto renderFrame = uniRenderEngine->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface_),
        bufferConfig_, false, isHebc_);
    if (!renderFrame) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::RequestFrame renderEngine requestFrame is null");
        return nullptr;
    }

    return renderFrame;
}

bool RSSplitSurfaceBuffer::FlushFrame()
{
    if (!producerSurface_ || !surfaceHandler_ || !rsSurface_) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::FlushFrame producerSurface_ or surfaceHandler_ is nullptr");
        return false;
    }
    BufferFlushConfig flushConfig_ = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = bufferConfig_.width,
            .h = bufferConfig_.height,
        },
    };
    RS_OPTIONAL_TRACE_NAME("RSSplitSurfaceBuffer::FlushFrame");

    auto fbBuffer = rsSurface_->GetCurrentBuffer();

    SurfaceError err = producerSurface_->FlushBuffer(fbBuffer, -1, flushConfig_);
    if (err != SURFACE_ERROR_OK) {
        int errval = err;
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::Flushframe Failed, error is : %{public}s, errval:%{public}d",
            SurfaceErrorStr(err).c_str(), errval);
        return false;
    }
    return true;
}

bool RSSplitSurfaceBuffer::ReleaseBuffer()
{
    if (!surfaceHandler_) {
        LAYER_SPLIT_LOGE("%{public}s surfaceHandler_ is nullptr", __func__);
        return false;
    }
    const auto surfaceConsumer = surfaceHandler_->GetConsumer();
    if (surfaceConsumer == nullptr) {
        LAYER_SPLIT_LOGE("RsDebug %{public}s (node: %{public}" PRIu64 "): surfaceConsumer is null!", __func__,
            surfaceHandler_->GetNodeId());
        return false;
    }

    auto fbBuffer = rsSurface_->GetCurrentBuffer();
    RS_OPTIONAL_TRACE_NAME("ReleaseBuffer");
    auto ret = surfaceConsumer->ReleaseBuffer(fbBuffer, -1);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        LAYER_SPLIT_LOGE("%{public}s (node: %{public}" PRIu64 "): ReleaseBuffer failed(ret: %{public}d)", __func__,
            surfaceHandler_->GetNodeId(), ret);
    }
    surfaceHandler_->SetAvailableBufferCount(static_cast<int32_t>(surfaceConsumer->GetAvailableBufferCount()));
    LAYER_SPLIT_LOGD("RsDebug %{public}s (node: %{public}" PRIu64 "), drop one frame", __func__,
        surfaceHandler_->GetNodeId());

    return true;
}

// reference to RSDisplayRenderNodeDrawable::CreateSurface
bool RSSplitSurfaceBuffer::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer != nullptr && rsSurface_ != nullptr) {
        LAYER_SPLIT_LOGD("RSSplitSurfaceBuffer::CreateSurface already created, return");
        return true;
    }
    consumer = IConsumerSurface::Create(layerName_);
    if (consumer == nullptr) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    auto producer = consumer->GetProducer();
    producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (!producerSurface_) {
        LAYER_SPLIT_LOGE("RSSplitSurfaceBuffer::CreateSurface CreateSurfaceAsProducer fail");
        return false;
    }
    producerSurface_->SetQueueSize(SPLIT_SURFACE_BUFFER_SIZE);
    producerSurface_->SetBufferTypeLeak(SPLIT_SURFACE_BUFFER_NAME);

    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    auto surface = client->CreateRSSurface(producerSurface_);
    rsSurface_ = std::static_pointer_cast<RSSurfaceOhos>(surface);
    LAYER_SPLIT_LOGD("RSSplitSurfaceBuffer::CreateSurface end");
    surfaceCreated_ = true;
    surfaceHandler_->SetConsumer(consumer);

    return true;
}

void* RSSplitSurfaceBuffer::GetBufferHandle()
{
    if (UNLIKELY(!rsSurface_)) {
        LAYER_SPLIT_LOGE("surface not exist");
        return nullptr;
    }

    auto buffer = rsSurface_->GetCurrentBuffer();
    if (buffer) {
        bufferHandle_ = buffer->GetBufferHandle();
        return bufferHandle_;
    }

    return nullptr;
}

} // OHOS::Rosen

#endif // (ROSEN_OHOS)