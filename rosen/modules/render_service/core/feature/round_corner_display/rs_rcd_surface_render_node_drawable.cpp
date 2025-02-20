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


#include "rs_rcd_surface_render_node_drawable.h"
#include <fstream>
#include <filesystem>
#include "common/rs_singleton.h"
#include "common/rs_optional_trace.h"
#include "params/rs_rcd_render_params.h"
#include "platform/common/rs_log.h"
#include "feature/round_corner_display/rs_rcd_render_listener.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_rcd_render_visitor.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"

namespace OHOS::Rosen::DrawableV2 {

const unsigned long long PRIV_USAGE_FBC_CLD_LAYER = 1ULL << 56; // 56 means the buffer usage is hardware
const int32_t BUFFER_TIME_OUT = 500;

RSRcdSurfaceRenderNodeDrawable::Registrar RSRcdSurfaceRenderNodeDrawable::instance_;

RSRcdSurfaceRenderNodeDrawable::RSRcdSurfaceRenderNodeDrawable(std::shared_ptr<const RSRenderNode> &&node)
    : RSRenderNodeDrawable(std::move(node)), syncDirtyManager_(std::make_shared<RSDirtyRegionManager>(true))
{
    auto nodeSp = std::const_pointer_cast<RSRenderNode>(node);
    auto rcdNode = std::static_pointer_cast<RSRcdSurfaceRenderNode>(nodeSp);
    surfaceHandler_ = rcdNode->GetMutableRSSurfaceHandler();
}

bool RSRcdSurfaceRenderNodeDrawable::PrepareResourceBuffer()
{
    if (!IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceHandler_);
        if (listener == nullptr || (!CreateSurface(listener))) {
            RS_LOGE("RSRcdSurfaceRenderNodeDrawable::PrepareResourceBuffer CreateSurface failed");
            return false;
        }
    }
    return true;
}

bool RSRcdSurfaceRenderNodeDrawable::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    RS_LOGD("RCD: Start RSRcdSurfaceRenderNodeDrawable CreateSurface");
    if (surfaceHandler_ == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable::CreateSurface surfacehandler is null");
        return false;
    }
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer != nullptr && surface_ != nullptr) {
        RS_LOGD("RSRcdSurfaceRenderNodeDrawable::CreateSurface already created, return");
        return true;
    }
    std::string surfaceName = "";
    RoundCornerDisplayManager::RCDLayerType type = RoundCornerDisplayManager::RCDLayerType::INVALID;
    if (IsTopSurface()) {
        surfaceName = "RCDTopSurfaceNode" + std::to_string(renderTargetId_);
        type = RoundCornerDisplayManager::RCDLayerType::TOP;
    } else {
        surfaceName = "RCDBottomSurfaceNode" + std::to_string(renderTargetId_);
        type = RoundCornerDisplayManager::RCDLayerType::BOTTOM;
    }
    consumer = IConsumerSurface::Create(surfaceName.c_str());
    RSSingleton<RoundCornerDisplayManager>::GetInstance().AddLayer(surfaceName, renderTargetId_, type);
    if (consumer == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    consumerListener_ = listener;
    auto producer = consumer->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    surface_ = client->CreateRSSurface(surface);
    surfaceCreated_ = true;
    surfaceHandler_->SetConsumer(consumer);
    return true;
}

bool RSRcdSurfaceRenderNodeDrawable::IsSurfaceCreated() const
{
    return surfaceCreated_;
}

std::shared_ptr<RSSurface> RSRcdSurfaceRenderNodeDrawable::GetRSSurface() const
{
    return surface_;
}

sptr<IBufferConsumerListener> RSRcdSurfaceRenderNodeDrawable::GetConsumerListener() const
{
    return consumerListener_;
}


void RSRcdSurfaceRenderNodeDrawable::ClearBufferCache()
{
    if (surface_ != nullptr) {
        surface_->ClearBuffer();
    }
    if (surfaceHandler_!= nullptr && surfaceHandler_->GetConsumer() != nullptr) {
        surfaceHandler_->GetConsumer()->GoBackground();
    }
}

NodeId RSRcdSurfaceRenderNodeDrawable::GetRenderTargetId() const
{
    return renderTargetId_;
}

std::shared_ptr<Drawing::Bitmap> RSRcdSurfaceRenderNodeDrawable::GetRcdBitmap() const
{
    auto params = static_cast<RSRcdRenderParams*>(renderParams_.get());
    if (params == nullptr) {
        return nullptr;
    }
    return params->GetRcdBitmap();
}


uint32_t RSRcdSurfaceRenderNodeDrawable::RSRcdSurfaceRenderNodeDrawable::GetRcdBufferWidth() const
{
    return GetRcdBitmap() != nullptr ? GetRcdBitmap()->GetWidth() : 0;
}

uint32_t RSRcdSurfaceRenderNodeDrawable::GetRcdBufferHeight() const
{
    return GetRcdBitmap() != nullptr ? GetRcdBitmap()->GetHeight() : 0;
}

uint32_t RSRcdSurfaceRenderNodeDrawable::GetRcdBufferSize() const
{
    auto params = static_cast<RSRcdRenderParams*>(renderParams_.get());
    if (params == nullptr) {
        return 0;
    }
    return params->GetBufferSize();
}

void RSRcdSurfaceRenderNodeDrawable::SetSurfaceType(uint32_t surfaceType)
{
    surfaceType_ = surfaceType;
}

bool RSRcdSurfaceRenderNodeDrawable::IsTopSurface() const
{
    return surfaceType_ == static_cast<uint32_t>(RCDSurfaceType::TOP);
}

bool RSRcdSurfaceRenderNodeDrawable::IsBottomSurface() const
{
    return surfaceType_ == static_cast<uint32_t>(RCDSurfaceType::BOTTOM);
}

bool RSRcdSurfaceRenderNodeDrawable::IsInvalidSurface() const
{
    return surfaceType_ == static_cast<uint32_t>(RCDSurfaceType::INVALID);
}

bool RSRcdSurfaceRenderNodeDrawable::IsRcdEnabled() const
{
    auto params = static_cast<RSRcdRenderParams*>(renderParams_.get());
    if (params == nullptr) {
        return false;
    }
    return params->GetRcdEnabled();
}

bool RSRcdSurfaceRenderNodeDrawable::IsResourceChanged() const
{
    auto params = static_cast<RSRcdRenderParams*>(renderParams_.get());
    if (params == nullptr) {
        return false;
    }
    return params->GetResourceChanged();
}

BufferRequestConfig RSRcdSurfaceRenderNodeDrawable::GetHardenBufferRequestConfig() const
{
    RS_LOGD("RSRcdSurfaceRenderNodeDrawable::GetHardenBufferRequestConfig");
    BufferRequestConfig config {};
    config.width = static_cast<int32_t>(GetRcdBufferWidth());
    if (GetRcdBufferWidth() != 0) {
        // need to plus 2 while calculating the bufferHeight in hardware dss way
        config.height = static_cast<int32_t>(GetRcdBufferSize() / GetRcdBufferWidth() + GetRcdBufferHeight() + 2);
    }
    config.strideAlignment = 0x8; // default stride is 8 Bytes.  // output parameter, system components can ignore it
    config.format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    config.usage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA
        | PRIV_USAGE_FBC_CLD_LAYER;
    RS_LOGD("RCD: GetHardenBufferRequestConfig Buffer usage %{public}" PRIu64 ", width %{public}d, height %{public}d",
        config.usage, config.width, config.height);
    config.timeout = BUFFER_TIME_OUT; // ms
    return config;
}

bool RSRcdSurfaceRenderNodeDrawable::ProcessRcdSurfaceRenderNode(std::shared_ptr<RSProcessor> processor)
{
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (processor == nullptr || IsInvalidSurface() || renderEngine == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable RSProcessor is null or node invalid!");
        return false;
    }
    auto params = static_cast<RSRcdRenderParams*>(renderParams_.get());
    if (!surfaceHandler_ || !params) {
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable surfaceHandler or params is null!");
        return false;
    }
    sptr<SurfaceBuffer> buffer = surfaceHandler_->GetBuffer();
    if (!params->GetResourceChanged() && buffer != nullptr) {
        processor->ProcessRcdSurfaceForRenderThread(*this);
        return true;
    }

    auto rsSurface = std::static_pointer_cast<RSSurfaceOhos>(GetRSSurface());
    if (rsSurface == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable no RSSurface found");
        return false;
    }
    rsSurface->SetTimeOut(GetHardenBufferRequestConfig().timeout);
    auto renderFrame = renderEngine->RequestFrame(rsSurface,
        GetHardenBufferRequestConfig(), true, false);
    if (renderFrame == nullptr) {
        rsSurface->GetSurface()->CleanCache(true);
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable Request Frame Failed");
        return false;
    }
    renderFrame->Flush();
    if (!ConsumeAndUpdateBuffer()) {
        RS_LOGE("RSRcdRenderVisitor ConsumeAndUpdateBuffer Failed");
        return false;
    }
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    if (surfaceHandler_->GetConsumer() && surfaceHandler_->GetBuffer()) {
        surfaceHandler_->GetConsumer()->SetScalingMode(surfaceHandler_->GetBuffer()->GetSeqNum(), scalingMode);
    }

    processor->ProcessRcdSurfaceForRenderThread(*this);

    return true;
}

bool RSRcdSurfaceRenderNodeDrawable::ConsumeAndUpdateBuffer()
{
    auto params = static_cast<RSRcdRenderParams*>(renderParams_.get());
    if (!surfaceHandler_ || !params) {
        return false;
    }
    auto availableBufferCnt = surfaceHandler_->GetAvailableBufferCount();
    if (availableBufferCnt <= 0) {
        // this node has no new buffer, try use old buffer.
        return true;
    }

    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer == nullptr) {
        return false;
    }

    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    int64_t timestamp = 0;
    Rect damage;
    auto ret = consumer->AcquireBuffer(buffer, acquireFence, timestamp, damage);
    if (buffer == nullptr || ret != SURFACE_ERROR_OK) {
        RS_LOGE("RsDebug RSRcdSurfaceRenderNode(id: %{public}" PRIu64 ") AcquireBuffer failed(ret: %{public}d)!",
            surfaceHandler_->GetNodeId(), ret);
        return false;
    }

    surfaceHandler_->SetBuffer(buffer, acquireFence, damage, timestamp);

    if (!SetHardwareResourceToBuffer()) {
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable SetHardwareResourceToBuffer Failed!");
        return false;
    }

    surfaceHandler_->SetCurrentFrameBufferConsumed();
    surfaceHandler_->ReduceAvailableBuffer();
    return true;
}

bool RSRcdSurfaceRenderNodeDrawable::SetHardwareResourceToBuffer()
{
    RS_LOGD("RCD: Start RSRcdSurfaceRenderNodeDrawable::SetHardwareResourceToBuffer");
    auto layerBitmap = GetRcdBitmap();
    if (!layerBitmap || layerBitmap->IsValid()) {
        RS_LOGE("LayerBitmap is null or invalid");
        return false;
    }
    sptr<SurfaceBuffer> nodeBuffer = surfaceHandler_->GetBuffer();
    if (nodeBuffer == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable buffer is nullptr");
        return false;
    }
    Drawing::ImageInfo imgInfo = Drawing::ImageInfo::MakeN32Premul(nodeBuffer->GetWidth(), nodeBuffer->GetHeight());
    if (!layerBitmap->ReadPixels(imgInfo, reinterpret_cast<void*>(nodeBuffer->GetVirAddr()),
        nodeBuffer->GetStride(), 0, 0)) {
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable:: copy layerBitmap to buffer failed");
        return false;
    }
    if (!FillHardwareResource(layerBitmap->GetWidth(), layerBitmap->GetHeight())) {
        RS_LOGE("RSRcdSurfaceRenderNodeDrawable:: copy hardware resource to buffer failed");
        return false;
    }
    return true;
}

bool RSRcdSurfaceRenderNodeDrawable::FillHardwareResource(int width, int height)
{
    sptr<SurfaceBuffer> nodeBuffer = surfaceHandler_->GetBuffer();
    if (nodeBuffer == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNode buffer is nullptr");
        return false;
    }
    auto params = static_cast<RSRcdRenderParams*>(renderParams_.get());
    if (GetRcdBufferSize() < 0 || params->GetCldWidth() < 0 || params->GetCldHeight() < 0 || width < 0 ||
        height < 0) {
        RS_LOGE("RSRcdSurfaceRenderNode check size failed");
        return false;
    }
    const uint32_t bytesPerPixel = 4; // 4 means four bytes per pixel
    cldInfo_.cldSize = static_cast<uint32_t>(params->GetBufferSize());
    cldInfo_.cldWidth = static_cast<uint32_t>(params->GetCldWidth());
    cldInfo_.cldHeight = static_cast<uint32_t>(params->GetCldHeight());
    cldInfo_.cldStride = static_cast<uint32_t>(params->GetCldWidth() * bytesPerPixel);
    cldInfo_.exWidth = static_cast<uint32_t>(width);
    cldInfo_.exHeight = static_cast<uint32_t>(height);

    int offset = 0;
    int offsetCldInfo = 0;
    int stride = nodeBuffer->GetStride();
    offsetCldInfo = height * stride;
    offset = (height + 1) * stride;
    cldInfo_.cldDataOffset = static_cast<uint32_t>(offset);
    uint8_t *img = static_cast<uint8_t*>(nodeBuffer->GetVirAddr());
    uint32_t bufferSize = nodeBuffer->GetSize();
    if (img == nullptr || offsetCldInfo < 0 || bufferSize < static_cast<uint32_t>(offsetCldInfo) + sizeof(cldInfo_)) {
        RS_LOGE("[%s] check nodebuffer failed", __func__);
        return false;
    }
    errno_t ret = memcpy_s(reinterpret_cast<void*>(img + offsetCldInfo), sizeof(cldInfo_), &cldInfo_, sizeof(cldInfo_));
    if (ret != EOK) {
        RS_LOGE("[%s] memcpy_s failed", __func__);
        return false;
    }
    std::ifstream addBufferFile(params->GetPathBin(), std::ifstream::binary | std::ifstream::in);
    if (addBufferFile) {
        addBufferFile.seekg(0, addBufferFile.end);
        int addBufferSize = addBufferFile.tellg();
        addBufferFile.seekg(0, addBufferFile.beg);
        addBufferFile.read(reinterpret_cast<char*>(img + offset), addBufferSize);
        addBufferFile.close();
    } else {
        RS_LOGE("[%{public}s] hardware fopen error", __func__);
        return false;
    }
    return true;
}

bool RSRcdSurfaceRenderNodeDrawable::DoProcessRenderTask(std::shared_ptr<RSProcessor> processor)
{
    RS_TRACE_BEGIN("RSRcdSurfaceRenderNodeDrawable:DoProcessRenderTask");
    if (!IsRcdEnabled()) {
        RS_LOGD("RSRcdSurfaceRenderNodeDrawable::DoProcessRenderTask: Draw skip, rcd disabled");
        RS_TRACE_END();
        return false;
    }
    if (!processor) {
        RS_LOGE("RCD: processor is null");
        RS_TRACE_END();
        return false;
    }
    auto res = ProcessRcdSurfaceRenderNode(processor);
    RS_TRACE_END();
    return res;
}

const CldInfo& RSRcdSurfaceRenderNodeDrawable::GetCldInfo() const
{
    return cldInfo_;
}

void RSRcdSurfaceRenderNodeDrawable::SetRenderTargetId(NodeId id)
{
    renderTargetId_ = id;
}

RSRenderNodeDrawable::Ptr RSRcdSurfaceRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSRcdSurfaceRenderNodeDrawable(std::move(node));
}
} // namespace OHOS::Rosen::DrawableV2
