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
#include "rs_hetero_hdr_buffer_layer.h"

#include "feature/hdr/hetero_hdr/rs_hetero_hdr_manager.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_util.h"
#include "feature/hdr/rs_hdr_util.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_uni_render_listener.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_handler.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "colorspace_converter_display.h"
#include "render/rs_colorspace_convert.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
const uint32_t BUFFER_SIZE = 3;
const uint32_t STRIDE_ALIGNMENT = 0x8;
const std::string HPAE_HETERO_BUFFER_TAG = "hpae_memory_hdrhetero";
}
RSHeteroHDRBufferLayer::RSHeteroHDRBufferLayer(const std::string& name, NodeId layerId)
    : surfaceHandler_(std::make_shared<RSSurfaceHandler>(layerId))
{
    layerName_ = name;
}

RSHeteroHDRBufferLayer::~RSHeteroHDRBufferLayer()
{
}

void RSHeteroHDRBufferLayer::CleanCache()
{
    if (surfaceHandler_ != nullptr) {
        auto consumer = surfaceHandler_->GetConsumer();
        std::lock_guard<std::mutex> lockGuard(surfaceHandleMutex_);
        if (consumer != nullptr && bufferToRelease_ != nullptr) {
            consumer->ReleaseBuffer(bufferToRelease_, surfaceHandler_->GetPreBufferReleaseFence());
            bufferToRelease_ = nullptr;
        }
        surfaceHandler_->CleanCache();
        RS_LOGD("[hdrHetero]:RSHeteroHDRBufferLayer CleanCache surfaceHandler clean done");
    }

    if (surface_ != nullptr) {
        GSError cleanStatus = surface_->CleanCache(true);
        RS_LOGD("[hdrHetero]:RSHeteroHDRBufferLayer CleanCache cleanStatus:%{public}d", cleanStatus);
    }
}

sptr<SurfaceBuffer> RSHeteroHDRBufferLayer::PrepareHDRDstBuffer(RSSurfaceRenderParams* surfaceParams, ScreenId screenId)
{
    if (!RSHeteroHDRUtil::ValidateSurface(surfaceParams)) {
        return nullptr;
    }
    BufferRequestConfig config{};
    ScreenInfo curScreenInfo = CreateOrGetScreenManager()->QueryScreenInfo(screenId);
    auto transform = RSBaseRenderUtil::GetRotateTransform(surfaceParams->GetLayerInfo().transformType);
    config.strideAlignment = STRIDE_ALIGNMENT;
    config.format = GRAPHIC_PIXEL_FMT_RGBA_1010102;
    config.usage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA;
    config.timeout = 0;
    auto buffer = surfaceParams->GetBuffer();
    config.width = buffer->GetSurfaceBufferWidth();
    config.height = buffer->GetSurfaceBufferHeight();
    HdrStatus hdrStatus = RSHeteroHDRManager::Instance().GetCurHandleStatus();
    switch (hdrStatus) {
        case HdrStatus::AI_HDR_VIDEO_GAINMAP:
            config.colorGamut = GRAPHIC_COLOR_GAMUT_SRGB;
            break;
        case HdrStatus::HDR_VIDEO:
            config.colorGamut = GRAPHIC_COLOR_GAMUT_BT2020;
            if (transform == GraphicTransformType::GRAPHIC_ROTATE_90 ||
                transform == GraphicTransformType::GRAPHIC_ROTATE_270) {
                config.width = static_cast<int32_t>(curScreenInfo.height);
                config.height = static_cast<int32_t>(curScreenInfo.width);
            } else {
                config.width = static_cast<int32_t>(curScreenInfo.width);
                config.height = static_cast<int32_t>(curScreenInfo.height);
            }
            break;
        default:
            RS_LOGW("[hdrHetero]:RSHeteroHDRBufferLayer PrepareHDRDstBuffer this hdrStatus is not supported");
            return nullptr;
    }
    int32_t releaseFence = -1;
    sptr<SurfaceBuffer> dstBuffer = RequestSurfaceBuffer(config, releaseFence);
    if (!dstBuffer) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer PrepareHDRDstBuffer request buffer failed");
        return nullptr;
    }
#ifdef USE_VIDEO_PROCESSING_ENGINE
    RSHDRUtilConst::CM_ColorSpaceInfo inputColorSpaceInfo;
    bool ret = MetadataHelper::GetColorSpaceInfo(buffer, inputColorSpaceInfo) == GSERROR_OK &&
        MetadataHelper::SetColorSpaceInfo(dstBuffer, inputColorSpaceInfo) == GSERROR_OK;
    if (ret) {
        return dstBuffer;
    }
#endif
    return nullptr;
}

void RSHeteroHDRBufferLayer::ConsumeAndUpdateBuffer()
{
    if (surfaceHandler_ == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer ConsumeAndUpdateBuffer surfaceHandler is nullptr");
        return;
    }
    // The log information related to the failure has been printed within the function.
    ReleaseBuffer();
    RSBaseRenderUtil::ConsumeAndUpdateBuffer(*surfaceHandler_);
}

sptr<SurfaceBuffer> RSHeteroHDRBufferLayer::RequestSurfaceBuffer(
    BufferRequestConfig& config, int32_t& releaseFence)
{
    if (!surfaceCreated_) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler_);
        if (!CreateSurface(listener)) {
            RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer RequestSurfaceBuffer CreateSurface failed");
            return nullptr;
        }
    }

    sptr<SurfaceBuffer> buffer = nullptr;
    GSError ret = surface_->RequestBuffer(buffer, releaseFence, config);
    if (ret != GSERROR_OK) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer RequestSurfaceBuffer request failed, %{public}s",
            GSErrorStr(ret).c_str());
        return nullptr;
    }
    if (buffer == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer RequestSurfaceBuffer request buffer is nullptr");
    }
    return buffer;
}

bool RSHeteroHDRBufferLayer::CreateSurface(sptr<IBufferConsumerListener>& listener)
{
    if (surfaceHandler_ == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer CreateSurface surfaceHandler is nullptr");
        return false;
    }
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer != nullptr && surface_ != nullptr) {
        RS_LOGD("[hdrHetero]:RSHeteroHDRBufferLayer CreateSurface surface is already created");
        return true;
    }
    consumer = IConsumerSurface::Create(layerName_);
    if (consumer == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer CreateSurface consumer is nullptr");
        return false;
    }
    SurfaceError ret = consumer->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer CreateSurface RegisterConsumerListener failed");
        return false;
    }
    auto producer = consumer->GetProducer();
    surface_ = Surface::CreateSurfaceAsProducer(producer);
    if (!surface_) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer CreateSurface CreateSurfaceAsProducer failed");
        return false;
    }
    surface_->SetQueueSize(BUFFER_SIZE);
    surface_->SetBufferTypeLeak(HPAE_HETERO_BUFFER_TAG);
    surfaceCreated_ = true;
    surfaceHandler_->SetConsumer(consumer);
    return true;
}

void RSHeteroHDRBufferLayer::FlushSurfaceBuffer(sptr<SurfaceBuffer>& buffer, int32_t acquireFence,
    BufferFlushConfig& flushConfig)
{
    sptr<SyncFence> acquireFenceSp(new SyncFence(acquireFence));
    surface_->FlushBuffer(buffer, acquireFenceSp, flushConfig);
}

bool RSHeteroHDRBufferLayer::ReleaseBuffer()
{
    if (surfaceHandler_ == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer ReleaseBuffer surfaceHandler is nullptr");
        return false;
    }
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer == nullptr) {
        return false;
    }
    auto preBuffer = surfaceHandler_->GetPreBuffer();
    std::lock_guard<std::mutex> lockGuard(surfaceHandleMutex_);
    if (bufferToRelease_ == preBuffer) {
        return true;
    }
    if (bufferToRelease_ != nullptr) {
        auto ret = consumer->ReleaseBuffer(bufferToRelease_, surfaceHandler_->GetPreBufferReleaseFence());
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGE("[hdrHetero]:RSHeteroHDRBufferLayer ReleaseBuffer consumer failed, maybe normal");
            return false;
        }
    }
    bufferToRelease_ = preBuffer;
    return true;
}

void RSHeteroHDRBufferLayer::FlushHDRDstBuffer(sptr<SurfaceBuffer>& dstBuffer)
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
    return;
}
} // OHOS::Rosen
} // namespace OHOS