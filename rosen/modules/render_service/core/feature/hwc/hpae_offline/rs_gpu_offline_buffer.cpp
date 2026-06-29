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

#include "feature/hwc/hpae_offline/rs_gpu_offline_buffer.h"

#include "pipeline/main_thread/rs_uni_render_listener.h"
#include "pipeline/render_thread/rs_base_surface_util.h"
#include "surface_buffer_impl.h"

namespace OHOS::Rosen {
static const std::string GPU_BUFFER_NAME = "gpu_offline_memory";

bool NativeWindowBufferInfo::ReadFromParcel(MessageParcel &parcel)
{
    static uint32_t sequenceNumberTotal = 0;
    // 0xFFFF is pid mask. 16 is pid offset.
    uint32_t sequenceNumber = (static_cast<uint32_t>(getpid()) & 0xFFFF) << 16;
    if (mUseAutoSeq) {
        sequenceNumber |= (0x8000 & 0xFFFF);
        // 0x7FFF is seqnum mask.
        sequenceNumber |= (sequenceNumberTotal++ & 0x7FFF);
    }

    uint32_t seqNum = 0;
    if (!parcel.ReadUint32(seqNum)) {
        return false;
    }
    baseInfo.sfbuffer = new SurfaceBufferImpl(mUseAutoSeq ? sequenceNumber : seqNum);
    if (baseInfo.sfbuffer == nullptr) {
        return false;
    }
    if (baseInfo.sfbuffer->ReadFromMessageParcel(parcel) != GSERROR_OK) {
        return false;
    }
    if (baseInfo.sfbuffer->ReadBufferRequestConfig(parcel) != GSERROR_OK) {
        return false;
    }
    if (!parcel.ReadInt64(baseInfo.uiTimestamp)) {
        return false;
    }
    return true;
}

bool NativeWindowBufferInfo::Marshalling(Parcel &parcel) const
{
    if (baseInfo.sfbuffer == nullptr) {
        return false;
    }
    MessageParcel* msgParcel = reinterpret_cast<MessageParcel*>(&parcel);
    if (msgParcel == nullptr) {
        return false;
    }
    if (!parcel.WriteUint32(baseInfo.sfbuffer->GetSeqNum())) {
        return false;
    }
    auto ret = baseInfo.sfbuffer->WriteToMessageParcel(*msgParcel);
    if (ret != GSERROR_OK) {
        return false;
    }
    ret = baseInfo.sfbuffer->WriteBufferRequestConfig(*msgParcel);
    if (ret != GSERROR_OK) {
        return false;
    }
    if (!parcel.WriteInt64(baseInfo.uiTimestamp)) {
        return false;
    }
    return true;
}

OHOS::sptr<NativeWindowBufferInfo> NativeWindowBufferInfo::Unmarshalling(Parcel &parcel, bool autoSeq)
{
    MessageParcel* msgParcel = reinterpret_cast<MessageParcel*>(&parcel);
    if (msgParcel == nullptr) {
        return nullptr;
    }
    OHOS::sptr<NativeWindowBufferInfo> ret = new NativeWindowBufferInfo(autoSeq);
    if ((ret != nullptr) && (!ret->ReadFromParcel(*msgParcel))) {
        ret = nullptr;
    }
    return ret;
}

RSGPUOfflineBuffer::RSGPUOfflineBuffer(const std::string& name, NodeId layerId)
    : surfaceHandler_(std::make_shared<RSSurfaceHandler>(layerId)), layerName_(name) {}

RSGPUOfflineBuffer::~RSGPUOfflineBuffer()
{
    CleanCache(true);
}

bool RSGPUOfflineBuffer::PreAllocBuffers(const BufferRequestConfig& config)
{
    if (!surfaceCreated_) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler_);
        if (!CreateSurface(listener)) {
            return false;
        }
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSGPUOfflineBuffer::PreAllocBuffers");
    GSError ret = pSurface_->PreAllocBuffers(config, 1); // single buffer
    if (ret != GSERROR_OK) {
        pSurface_->Connect();
        auto cleanRet = pSurface_->CleanCache(true);
        RS_LOGW("RSGPUOfflineBuffer::PreAllocBuffers failed[%{public}d], cleanRet: %{public}d", ret, cleanRet);
        return false;
    }
    RS_LOGD("RSGPUOfflineBuffer::PreAllocBuffers success.");
    return true;
}

bool RSGPUOfflineBuffer::ConvertColorGamutToSpaceType(const GraphicColorGamut& colorGamut,
    HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceType)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    static const std::map<GraphicColorGamut, CM_ColorSpaceType> RS_TO_COMMON_COLOR_SPACE_TYPE_MAP {
        {GRAPHIC_COLOR_GAMUT_STANDARD_BT601, CM_BT601_EBU_FULL},
        {GRAPHIC_COLOR_GAMUT_STANDARD_BT709, CM_BT709_FULL},
        {GRAPHIC_COLOR_GAMUT_SRGB, CM_SRGB_FULL},
        {GRAPHIC_COLOR_GAMUT_ADOBE_RGB, CM_ADOBERGB_FULL},
        {GRAPHIC_COLOR_GAMUT_DISPLAY_P3, CM_P3_FULL},
        {GRAPHIC_COLOR_GAMUT_BT2020, CM_DISPLAY_BT2020_SRGB},
        {GRAPHIC_COLOR_GAMUT_BT2100_PQ, CM_BT2020_PQ_FULL},
        {GRAPHIC_COLOR_GAMUT_BT2100_HLG, CM_BT2020_HLG_FULL},
        {GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020, CM_DISPLAY_BT2020_SRGB},
    };
    if (RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.find(colorGamut) == RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.end()) {
        return false;
    }
    colorSpaceType = RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.at(colorGamut);
    return true;
}

std::unique_ptr<RSRenderFrame> RSGPUOfflineBuffer::RequestFrame(std::shared_ptr<RSBaseRenderEngine> renderEngine,
    const BufferRequestConfig& config, bool isHebc, SingleBufferMode switchType)
{
    RS_OPTIONAL_TRACE_NAME("RSGPUOfflineBuffer::RequestFrame");
    if (!surfaceCreated_) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler_);
        RS_OPTIONAL_TRACE_NAME("RSGPUOfflineBuffer::CreateSurface");
        if (!CreateSurface(listener)) {
            RS_LOGE("RSGPUOfflineBuffer::RequestFrame CreateSurface failed");
            return nullptr;
        }
    }
#ifdef USE_VIDEO_PROCESSING_ENGINE
    // set color space to surface buffer metadata
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceType colorSpace = CM_SRGB_FULL;
    if (ConvertColorGamutToSpaceType(config.colorGamut, colorSpace)) {
        if (pSurface_->SetUserData("ATTRKEY_COLORSPACE_INFO", std::to_string(colorSpace)) != GSERROR_OK) {
            RS_LOGD("RSGPUOfflineBuffer::SetUserData failed");
        }
    }
#endif
    // clean cache when config changed, for single buffer
    bool needFillSingleBuffer = false;
    if (currentConfig_ != config || switchType != SingleBufferMode::SINGLE_BUFFER_MODE_NONE) {
        RS_LOGD("RSGPUOfflineBuffer::CleanCache when config changed");
        CleanCache(true);
        isFirstRequest_ = true;
        needFillSingleBuffer = (switchType != SingleBufferMode::SINGLE_BUFFER_MODE_TO_MULTI);
    }
    auto renderFrame = renderEngine->RequestFrame(
        std::static_pointer_cast<RSSurfaceOhos>(rsSurface_), config, false, isHebc);
    if (!renderFrame) {
        RS_LOGE("RSGPUOfflineBuffer::RequestFrame renderEngine requestFrame is null");
        return nullptr;
    }
    if (needFillSingleBuffer) {
        AttachSingleBuffer(renderFrame);
    }
    currentConfig_ = config;
    return renderFrame;
}

bool RSGPUOfflineBuffer::AttachSingleBuffer(std::unique_ptr<RSRenderFrame>& renderFrame)
{
    auto info = NativeWindowBufferInfo(false);
    auto rsSurface = renderFrame->GetSurface();
    if (rsSurface == nullptr) {
        RS_LOGD("RSGPUOfflineBuffer::AttachSingleBuffer rsSurface is null");
        return false;
    }
    auto dstBuffer = rsSurface->GetCurrentBuffer();
    if (dstBuffer == nullptr) {
        RS_LOGD("RSGPUOfflineBuffer::AttachSingleBuffer dstbuffer is null");
        return false;
    }
    uint64_t bufferQueueId = pSurface_->GetUniqueId();
    info.baseInfo.sfbuffer = dstBuffer;
    info.baseInfo.uiTimestamp = 0;
    RS_LOGI("RSGPUOfflineBuffer::AttachSingleBuffer queueId=%llx",
        static_cast<unsigned long long>(bufferQueueId));
    for (uint i = 0; i < bufferSize_ - 1; i++) {
        if (!CopyAndAttachBufferToQueue(info, pSurface_, bufferQueueId)) {
            return false;
        }
    }
    return true;
}

bool RSGPUOfflineBuffer::CopyAndAttachBufferToQueue(NativeWindowBufferInfo& info,
    OHOS::sptr<OHOS::Surface>& producerSurface, uint64_t bufferQueueId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSGPUOfflineBuffer::CopyAndAttachBufferToQueue");
    bool addSuccess = false;
    MessageParcel msgParcel;
    info.Marshalling(msgParcel);
    OHOS::sptr<NativeWindowBufferInfo> tempInfo = NativeWindowBufferInfo::Unmarshalling(msgParcel, true);
    if (!tempInfo) {
        return false;
    }
    auto sBuffer = tempInfo->baseInfo.sfbuffer;
    do {
        if (!sBuffer) {
            break;
        }
        // some buffer under use, cannot delete, will delete later
        if (producerSurface->AttachBufferToQueue(sBuffer) != 0) {
            break;
        }
        if (producerSurface->CancelBuffer(sBuffer) != 0) {
            break;
        }
        RS_LOGI("RSGPUOfflineBuffer::CopyAndAttachBufferToQueue %s %llx %x",
            producerSurface->GetName().c_str(),
            (unsigned long long)bufferQueueId, sBuffer->GetSeqNum());
        addSuccess = true;
    } while (false);
    RS_LOGI("RSGPUOfflineBuffer::CopyAndAttachBufferToQueue succeeded!");
    return addSuccess;
}

std::shared_ptr<RSSurfaceOhos> RSGPUOfflineBuffer::GetRSSurfaceOhos()
{
    if (!surfaceCreated_) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler_);
        RS_OPTIONAL_TRACE_NAME("RSGPUOfflineBuffer::CreateSurface");
        if (!CreateSurface(listener)) {
            RS_LOGE("RSGPUOfflineBuffer::GetRSSurfaceOhos CreateSurface failed");
            return nullptr;
        }
    }

    if (rsSurface_ == nullptr) {
        RS_LOGE("RSGPUOfflineBuffer::GetRSSurfaceOhos surface is null");
        return nullptr;
    }
    return std::static_pointer_cast<RSSurfaceOhos>(rsSurface_);
}

// reference to RSGPUOfflineBuffer::CreateSurface
bool RSGPUOfflineBuffer::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer != nullptr && rsSurface_ != nullptr) {
        RS_LOGI("RSGPUOfflineBuffer::CreateSurface already created, return");
        return true;
    }
    consumer = IConsumerSurface::Create(layerName_);
    if (consumer == nullptr) {
        RS_LOGE("RSGPUOfflineBuffer::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSGPUOfflineBuffer::CreateSurface RegisterConsumerListener fail");
        return false;
    }

    auto producer = consumer->GetProducer();
    pSurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (!pSurface_) {
        RS_LOGE("RSGPUOfflineBuffer::CreateSurface CreateSurfaceAsProducer fail");
        return false;
    }
    pSurface_->SetQueueSize(bufferSize_);
    pSurface_->SetBufferTypeLeak(GPU_BUFFER_NAME);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    rsSurface_ = client->CreateRSSurface(pSurface_);
    RS_LOGI("RSGPUOfflineBuffer::CreateSurface end");
    surfaceCreated_ = true;
    surfaceHandler_->SetConsumer(consumer);
    return true;
}

std::shared_ptr<RSSurfaceHandler> RSGPUOfflineBuffer::GetSurfaceHandler()
{
    return surfaceHandler_;
}

sptr<SurfaceBuffer> RSGPUOfflineBuffer::ConsumeAndGetBuffer()
{
    if (!RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*surfaceHandler_) ||
        !surfaceHandler_->GetBuffer()) {
            RS_LOGE("RSGPUOfflineBuffer::ConsumeAndGetBuffer failed. %{public}d",
                !surfaceHandler_->GetBuffer());
            return nullptr;
        }
    if (surfaceHandler_->IsCurrentFrameBufferConsumed()) {
        auto offlinePreBufferCount = surfaceHandler_->GetPreBufferOwnerCount();
        if (offlinePreBufferCount) {
            offlinePreBufferCount->DecRef();
        }
    }
    return surfaceHandler_->GetBuffer();
}

void RSGPUOfflineBuffer::CleanCache(bool cleanAll)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSGPUOfflineBuffer::CleanCache");
    if (pSurface_ != nullptr) {
        GSError ret = pSurface_->CleanCache(cleanAll);
        surfaceHandler_->CleanCache();
        RS_LOGD("RSGPUOfflineBuffer::CleanCache, ret = %{public}d.", ret);
    }
}

} // namespace OHOS::Rosen