/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "feature/hpae/rs_hpae_buffer.h"

#include "feature/hpae/rs_hpae_render_listener.h"
#include "hpae_base/rs_hpae_log.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_surface_handler.h"
#include "platform/common/rs_log.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif

namespace OHOS::Rosen {
namespace DrawableV2 {

RSHpaeBuffer::RSHpaeBuffer(const std::string& name, const int layerId)
{
    layerName_ = name;
    surfaceHandler_ = std::make_shared<RSSurfaceHandler>(layerId);
}

RSHpaeBuffer::~RSHpaeBuffer()
{
}

// reference to RSDisplayRenderNodeDrawable::RequestFrame
std::unique_ptr<RSRenderFrame> RSHpaeBuffer::RequestFrame(const BufferRequestConfig& config, bool isHebc)
{
#if defined(ROSEN_OHOS)&&defined(ENABLE_HPAE_BLUR)
    HPAE_TRACE_NAME("RSHpaeBuffer:RequestFrame");
    bufferConfig_ = config;
    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (UNLIKELY(!uniRenderEngine)) {
        RS_LOGE("RSHpaeBuffer::RequestFrame RenderEngine is null!");
        return nullptr;
    }

    if (grContext_ == nullptr) {
        if (uniRenderEngine->GetRenderContext()) {
            grContext_ = uniRenderEngine->GetRenderContext()->GetSharedDrGPUContext();
        } else {
            HPAE_TRACE_NAME("Exception: context is nullptr");
            return nullptr;
        }
    }

    if (!surfaceCreated_) {
        sptr<IBufferConsumerListener> listener = new RSHpaeRenderListener(surfaceHandler_);
        HPAE_TRACE_NAME("create layer surface");
        if (!CreateSurface(listener)) {
            RS_LOGE("RSHpaeBuffer::RequestFrame CreateSurface failed");
            return nullptr;
        }
    }

    if (rsSurface_ == nullptr) {
        RS_LOGE("RSHpaeBuffer::RequestFrame: surface is null!");
        return nullptr;
    }

    auto renderFrame = uniRenderEngine->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface_),
        config, false, isHebc);
    if (!renderFrame) {
        RS_LOGE("RSHpaeBuffer::RequestFrame renderEngine requestFrame is null");
        return nullptr;
    }

    return renderFrame;
#else
    return nullptr;
#endif
}

bool RSHpaeBuffer::FlushFrame()
{
#if defined(ROSEN_OHOS)&&defined(ENABLE_HPAE_BLUR)
    if (!producerSurface_ || !surfaceHandler_) {
        RS_LOGE("RSHpaeBuffer::FlushFrame producerSurface_ or surfaceHandler_ is nullptr");
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
    HPAE_TRACE_NAME_FMT("RSHpaeBuffer::FlushFrame: %p", surfaceHandler_->GetBuffer().GetRefPtr());

    auto fbBuffer = rsSurface_->GetCurrentBuffer();

    SurfaceError err = producerSurface_->FlushBuffer(fbBuffer, -1, flushConfig_);
    if (err != SURFACE_ERROR_OK) {
        int errval = err;
        RS_LOGE("RSHpaeBuffer::Flushframe Failed, error is : %{public}s, errval:%{public}d",
            SurfaceErrorStr(err).c_str(), errval);
        return false;
    }
    return true;
#else
    return false;
#endif
}

GSError RSHpaeBuffer::ForceDropFrame(uint64_t presentWhen)
{
#if defined(ROSEN_OHOS)&&defined(ENABLE_HPAE_BLUR)
    if (!surfaceHandler_) {
        RS_LOGE("RSHpaeBuffer::ForceDropFrame surfaceHandler_ is nullptr");
        return OHOS::GSERROR_NOT_INIT;
    }
    const auto surfaceConsumer = surfaceHandler_->GetConsumer();
    if (surfaceConsumer == nullptr) {
        RS_LOGE("RsDebug RSHpaeBuffer::DropFrame (node: %{public}" PRIu64"): surfaceConsumer is null!",
        surfaceHandler_->GetNodeId());
        return OHOS::GSERROR_NO_CONSUMER;
    }

    IConsumerSurface::AcquireBufferReturnValue returnValue;
    returnValue.fence = SyncFence::InvalidFence();
    int32_t ret = surfaceConsumer->AcquireBuffer(returnValue, static_cast<int64_t>(presentWhen), false);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        RS_LOGE("RSHpaeBuffer::DropFrameProcess(node: %{public}" PRIu64"): AcquireBuffer failed("
            " ret: %{public}d), do nothing ", surfaceHandler_->GetNodeId(), ret);
        return OHOS::GSERROR_NO_BUFFER;
    }

    HPAE_TRACE_NAME_FMT("Force drop: DropFrame: %p, %p", returnValue.buffer.GetRefPtr(),
        surfaceHandler_->GetBuffer().GetRefPtr());
    ret = surfaceConsumer->ReleaseBuffer(returnValue.buffer, returnValue.fence);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        RS_LOGE("RSHpaeBuffer::DropFrameProcess(node: %{public}" PRIu64
            "): ReleaseBuffer failed(ret: %{public}d), Acquire done ",
            surfaceHandler_->GetNodeId(), ret);
    }
    surfaceHandler_->SetAvailiableBufferCount(static_cast<int32_t>(surfaceConsumer->GetAvailableBufferCount()));
    RS_LOGD("RsDebug RSHpaeBuffer::DropFrameProcess (node: %{public}" PRIu64 "), drop one frame",
        surfaceHandler_->GetNodeId());

    return OHOS::GSERROR_OK;
#else
    return GSERROR_NOT_INIT;
#endif
}

#if defined(ROSEN_OHOS)
// reference to RSDisplayRenderNodeDrawable::CreateSurface
bool RSHpaeBuffer::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer != nullptr && rsSurface_ != nullptr) {
        RS_LOGI("RSHpaeBuffer::CreateSurface already created, return");
        return true;
    }
    consumer = IConsumerSurface::Create(layerName_);
    if (consumer == nullptr) {
        RS_LOGE("RSHpaeBuffer::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSHpaeBuffer::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    auto producer = consumer->GetProducer();
    producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (!producerSurface_) {
        RS_LOGE("RSHpaeBuffer::CreateSurface CreateSurfaceAsProducer fail");
        return false;
    }
    producerSurface_->SetQueueSize(HPAE_BUFFER_SIZE);

    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    auto surface = client->CreateRSSurface(producerSurface_);
    rsSurface_ = std::static_pointer_cast<RSSurfaceOhos>(surface);
    RS_LOGI("RSHpaeBuffer::CreateSurface end");
    surfaceCreated_ = true;
    surfaceHandler_->SetConsumer(consumer);

    return true;
}
#endif

void* RSHpaeBuffer::GetBufferHandle()
{
#if defined(ROSEN_OHOS)&&defined(ENABLE_HPAE_BLUR)
    if (UNLIKELY(!rsSurface_)) {
        HPAE_LOGE("surface not exist");
        return nullptr;
    }

    auto buffer = rsSurface_->GetCurrentBuffer();
    if (buffer) {
        bufferHandle_ = buffer->GetBufferHandle();
        HPAE_TRACE_NAME_FMT("getBufferHandle: %p", bufferHandle_);
        return bufferHandle_;
    }
#endif
    return nullptr;
}

} // DrawableV2
} // OHOS::Rosen