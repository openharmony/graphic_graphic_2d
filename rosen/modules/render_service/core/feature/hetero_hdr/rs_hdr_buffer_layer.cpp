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

#include "rs_trace.h"
#include "platform/common/rs_log.h"

#include "memory/rs_tag_tracker.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_uni_render_listener.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_handler.h"
#include "rs_hetero_hdr_util.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif

namespace OHOS::Rosen::DrawableV2 {
RSHDRBUfferLayer::RSHDRBUfferLayer(const std::string& name, NodeId layerId)
    : surfaceHandler_(std::make_shared<RSSurfaceHandler>(layerId))
{
    layerName_ = name;
}

RSHDRBUfferLayer::~RSHDRBUfferLayer()
{
}

sptr<SurfaceBuffer> RSHDRBUfferLayer::PrepareHDRDstBuffer(RSSurfaceRenderParams *surfaceParams)
{
    auto layerTransform = surfaceParams->GetLayerInfo().transformType;
    int realRotation = RSBaseRenderUtil::RotateEnumToInt(RSBaseRenderUtil::GetRotateTransform(layerTransform));
    RS_LOGE("Yaolin GetTransform1 %{public}d", realRotation);
    RS_TRACE_NAME_FMT("Yaolin GetTransform1 [%d]", realRotation);

    ScreenId paramScreenId = surfaceParams->GetScreenId();
    ScreenInfo curScreenInfo = CreateOrGetScreenManager()->QueryScreenInfo(paramScreenId);

    BufferRequestConfig config{};
    const int ROTATION_90 = 90;
    const int ROTATION_270 = 270;
    if (realRotation == ROTATION_90 || realRotation == ROTATION_270) {
        config.width = static_cast<int32_t>(curScreenInfo.height);
        config.height = static_cast<int32_t>(curScreenInfo.width);
    } else {
        config.width = static_cast<int32_t>(curScreenInfo.width);
        config.height = static_cast<int32_t>(curScreenInfo.height);
    }
    
    config.strideAlignment = 0x8;
    config.colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    config.format = GRAPHIC_PIXEL_FMT_RGBA_1010102;
    config.usage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA;
    config.timeout = 0;

    int32_t releaseFence = -1;
    sptr<SurfaceBuffer> dstBuffer = RequestSurfaceBuffer(config, releaseFence);
    if (!dstBuffer) {
        RS_LOGE("QM: RS dstSurfaceHandler get buffer failed! ");
        return nullptr;
    }
    return dstBuffer;
}

void RSHDRBUfferLayer::ConsumeAndUpdateBuffer()
{
    if (surfaceHandler_ == nullptr) {
        RS_LOGE("QM hdrSurfaceHandler_ is nullptr!");
        return;
    }
    RSBaseRenderUtil::ConsumeAndUpdateBuffer(*surfaceHandler_);
    RSBaseRenderUtil::ReleaseBuffer(*surfaceHandler_);
}

sptr<SurfaceBuffer> RSHDRBUfferLayer::RequestSurfaceBuffer(
    BufferRequestConfig& config, int32_t &releaseFence)
{
    if (!surfaceCreated_) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler_);
        RS_TRACE_NAME("XXF: create layer surface");
        if (!CreateSurface(listener)) {
            RS_LOGE(" RSRenderLayer::CreateSurface failed");
            return nullptr;
        }
    }

    sptr<SurfaceBuffer> buffer = nullptr;
    GSError ret = surface_->RequestBuffer(buffer, releaseFence, config);
    if (ret != GSERROR_OK) {
        RS_LOGE(" Surface RequestBuffer failed, %{public}s", GSErrorStr(ret).c_str());
    }
    if (buffer == nullptr) {
        RS_LOGE(" RSRenderLayer::RequestBuffer failed");
    }
    return buffer;
}

#ifndef ROSEN_CROSS_PLATFORM
// reference to RSDisplayRenderNodeDrawable::CreateSurface
bool RSHDRBUfferLayer::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer != nullptr && surface_ != nullptr) {
        RS_LOGI("RSRenderLayer::CreateSurface already created, return");
        return true;
    }
    consumer = IConsumerSurface::Create(layerName_);
    if (consumer == nullptr) {
        RS_LOGE("RSRenderLayer::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSRenderLayer::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    auto producer = consumer->GetProducer();
    surface_ = Surface::CreateSurfaceAsProducer(producer);
    if (!surface_) {
        RS_LOGE("RSRenderLayer::CreateSurface CreateSurfaceAsProducer fail");
        return false;
    }
    surface_->SetQueueSize(BUFFER_SIZE);
    surfaceCreated_ = true;
    surfaceHandler_->SetConsumer(consumer);
    return true;
}
#endif

void RSHDRBUfferLayer::FlushSurfaceBuffer(sptr<SurfaceBuffer> &buffer, int32_t acquireFence,
    BufferFlushConfig &flushConfig)
{
    sptr<SyncFence> acquireFenceSp(new SyncFence(acquireFence));
    surface_->FlushBuffer(buffer, acquireFenceSp, flushConfig);
}

void RSHDRBUfferLayer::FlushHDRDstBuffer(sptr<SurfaceBuffer> dstBuffer)
{
    BufferFlushConfig flushConfig;
    flushConfig.damage = {
        .x = 0,
        .y = 0,
        .w = dstBuffer->GetSurfaceBufferWidth(),
        .h = dstBuffer->GetSurfaceBufferHeight()
    };
    flushConfig.timestamp = 0;
    FlushSurfaceBuffer(dstBuffer, -1, flushConfig);
    surfaceHandler_ = GetMutableRSSurfaceHandler();
    return;
}
} // OHOS::Rosen
