/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_rcd_surface_render_node.h"
#include <fstream>
#include "common/rs_singleton.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "pipeline/rs_canvas_render_node.h"
#include "rs_round_corner_display_manager.h"

namespace OHOS {
namespace Rosen {

const unsigned long long PRIV_USAGE_FBC_CLD_LAYER = 1ULL << 56; // 56 means the buffer usage is hardware
const float RCD_LAYER_Z_TOP1 = static_cast<float>(0x7FFFFFFF); // toppest
const float RCD_LAYER_Z_TOP2 = static_cast<float>(0x7FFFFEFF); // not set toppest - 1, float only 6 significant digits
const int32_t BUFFER_TIME_OUT = 500;

RSRcdSurfaceRenderNode::RSRcdSurfaceRenderNode(
    NodeId id, RCDSurfaceType type, const std::weak_ptr<RSContext>& context)
    : RSRenderNode(id, context), RSSurfaceHandler(id)
{
    RS_LOGD("RCD: Start Create RSRcdSurfaceRenderNode %{public}d", type);
    rcdExtInfo_.surfaceType = type;
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this));
}

RSRcdSurfaceRenderNode::SharedPtr RSRcdSurfaceRenderNode::Create(NodeId id, RCDSurfaceType type)
{
    return std::make_shared<RSRcdSurfaceRenderNode>(id, type);
}

RSRcdSurfaceRenderNode::~RSRcdSurfaceRenderNode()
{
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this));
}

const RectI& RSRcdSurfaceRenderNode::GetSrcRect() const
{
    return rcdExtInfo_.srcRect_;
}

const RectI& RSRcdSurfaceRenderNode::GetDstRect() const
{
    return rcdExtInfo_.dstRect_;
}

void RSRcdSurfaceRenderNode::SetRenderTargetId(NodeId id)
{
    renerTargetId_ = id;
}

bool RSRcdSurfaceRenderNode::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    RS_LOGD("RCD: Start RSRcdSurfaceRenderNode CreateSurface");
    if (consumer_ != nullptr && surface_ != nullptr) {
        RS_LOGD("RSRcdSurfaceRenderNode::CreateSurface already created, return");
        return true;
    }
    std::string surfaceName = "";
    RoundCornerDisplayManager::RCDLayerType type = RoundCornerDisplayManager::RCDLayerType::INVALID;
    if (IsTopSurface()) {
        surfaceName = "RCDTopSurfaceNode" + std::to_string(renerTargetId_);
        type = RoundCornerDisplayManager::RCDLayerType::TOP;
    } else {
        surfaceName = "RCDBottomSurfaceNode" + std::to_string(renerTargetId_);
        type = RoundCornerDisplayManager::RCDLayerType::BOTTOM;
    }
    consumer_ = IConsumerSurface::Create(surfaceName.c_str());
    RSSingleton<RoundCornerDisplayManager>::GetInstance().AddLayer(surfaceName, renerTargetId_, type);
    if (consumer_ == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNode::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer_->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSRcdSurfaceRenderNode::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    consumerListener_ = listener;
    auto producer = consumer_->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    surface_ = client->CreateRSSurface(surface);
    rcdExtInfo_.surfaceCreated = true;
    return true;
}

void RSRcdSurfaceRenderNode::SetRcdBufferWidth(uint32_t width)
{
    rcdSourceInfo.bufferWidth = width;
}

uint32_t RSRcdSurfaceRenderNode::GetRcdBufferWidth() const
{
    return rcdSourceInfo.bufferWidth;
}

void RSRcdSurfaceRenderNode::SetRcdBufferHeight(uint32_t height)
{
    rcdSourceInfo.bufferHeight = height;
}

uint32_t RSRcdSurfaceRenderNode::GetRcdBufferHeight() const
{
    return rcdSourceInfo.bufferHeight;
}

void RSRcdSurfaceRenderNode::SetRcdBufferSize(uint32_t bufferSize)
{
    rcdSourceInfo.bufferSize = bufferSize;
}

uint32_t RSRcdSurfaceRenderNode::GetRcdBufferSize() const
{
    return rcdSourceInfo.bufferSize;
}

BufferRequestConfig RSRcdSurfaceRenderNode::GetHardenBufferRequestConfig() const
{
    RS_LOGD("RCD: Start GetHardenBufferRequestConfig");
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

bool RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer(const std::shared_ptr<rs_rcd::RoundCornerLayer>& layerInfo)
{
    RS_LOGD("RCD: Start PrepareHardwareResourceBuffer");

    if (layerInfo == nullptr) {
        RS_LOGE("RCD: layerInfo is nullptr");
        return false;
    }

    cldLayerInfo.pathBin = std::string(rs_rcd::PATH_CONFIG_DIR) + "/" + layerInfo->binFileName;
    cldLayerInfo.bufferSize = layerInfo->bufferSize;
    cldLayerInfo.cldWidth = layerInfo->cldWidth;
    cldLayerInfo.cldHeight = layerInfo->cldHeight;

    if (layerInfo->curBitmap == nullptr) {
        RS_LOGE("layerInfo->curBitmap is nullptr");
        return false;
    }
    layerBitmap = *(layerInfo->curBitmap);

    uint32_t bitmapHeight = static_cast<uint32_t>(layerBitmap.GetHeight());
    uint32_t bitmapWidth = static_cast<uint32_t>(layerBitmap.GetWidth());
    if (bitmapHeight <= 0 || bitmapWidth <= 0 || displayRect_.GetHeight() <= 0) {
        RS_LOGE("bitmapHeight, bitmapWidth or layerHeight is wrong value");
        return false;
    }
    SetRcdBufferHeight(bitmapHeight);
    SetRcdBufferWidth(bitmapWidth);
    SetRcdBufferSize(cldLayerInfo.bufferSize);

    if (IsTopSurface()) {
        rcdExtInfo_.srcRect_ = RectI(0, 0, bitmapWidth, bitmapHeight);
        rcdExtInfo_.dstRect_ = RectI(displayRect_.GetLeft(), displayRect_.GetTop(), bitmapWidth, bitmapHeight);
        SetGlobalZOrder(RCD_LAYER_Z_TOP1);
    } else {
        rcdExtInfo_.srcRect_ = RectI(0, 0, bitmapWidth, bitmapHeight);
        rcdExtInfo_.dstRect_ = RectI(displayRect_.GetLeft(), displayRect_.GetHeight() - bitmapHeight +
            displayRect_.GetTop(), bitmapWidth, bitmapHeight);
        SetGlobalZOrder(RCD_LAYER_Z_TOP2);
    }
    return true;
}

bool RSRcdSurfaceRenderNode::SetHardwareResourceToBuffer()
{
    RS_LOGD("RCD: Start RSRcdSurfaceRenderNode::SetHardwareResourceToBuffer");
    if (layerBitmap.IsValid()) {
        RS_LOGE("LayerBitmap is not valid");
        return false;
    }
    sptr<SurfaceBuffer> nodeBuffer = GetBuffer();
    if (nodeBuffer == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNode buffer is nullptr");
        return false;
    }
    Drawing::ImageInfo imgInfo = Drawing::ImageInfo::MakeN32Premul(nodeBuffer->GetWidth(), nodeBuffer->GetHeight());
    if (!layerBitmap.ReadPixels(imgInfo, reinterpret_cast<void*>(nodeBuffer->GetVirAddr()),
        nodeBuffer->GetStride(), 0, 0)) {
        RS_LOGE("RSRcdSurfaceRenderNode:: copy layerBitmap to buffer failed");
        return false;
    }
    if (!FillHardwareResource(cldLayerInfo, layerBitmap.GetHeight(), layerBitmap.GetWidth())) {
        RS_LOGE("RSRcdSurfaceRenderNode:: copy hardware resource to buffer failed");
        return false;
    }
    return true;
}

bool RSRcdSurfaceRenderNode::FillHardwareResource(HardwareLayerInfo &cldLayerInfo,
    int height, int width)
{
    sptr<SurfaceBuffer> nodeBuffer = GetBuffer();
    if (nodeBuffer == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNode buffer is nullptr");
        return false;
    }
    if (cldLayerInfo.bufferSize < 0 || cldLayerInfo.cldWidth < 0 ||
        cldLayerInfo.cldHeight < 0 || width < 0 || height < 0) {
        RS_LOGE("RSRcdSurfaceRenderNode check cldLayerInfo and size failed");
        return false;
    }
    const uint32_t bytesPerPixel = 4; // 4 means four bytes per pixel
    cldInfo_.cldSize = static_cast<uint32_t>(cldLayerInfo.bufferSize);
    cldInfo_.cldWidth = static_cast<uint32_t>(cldLayerInfo.cldWidth);
    cldInfo_.cldHeight = static_cast<uint32_t>(cldLayerInfo.cldHeight);
    cldInfo_.cldStride = static_cast<uint32_t>(cldLayerInfo.cldWidth * bytesPerPixel);
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
    std::ifstream addBufferFile(cldLayerInfo.pathBin, std::ifstream::binary | std::ifstream::in);
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

void RSRcdSurfaceRenderNode::PrintRcdNodeInfo()
{
    std::string surfaceName = (IsTopSurface() ? "RCDTopSurfaceNode" : "RCDBottomSurfaceNode")  +
        std::to_string(renerTargetId_);
    RS_LOGI("[%{public}s] %{public}s node info", __func__, surfaceName.c_str());
    RS_LOGI("[%{public}s] rcd layerBitmap size %{public}d X %{public}d", __func__,
        layerBitmap.GetWidth(), layerBitmap.GetHeight());
    RS_LOGI("[%{public}s] rcd cld info %{public}u, %{public}u, %{public}u, %{public}u, %{public}u, %{public}u",
        __func__, cldInfo_.cldSize, cldInfo_.cldWidth, cldInfo_.cldHeight, cldInfo_.cldStride, cldInfo_.exWidth,
        cldInfo_.exHeight);
}

bool RSRcdSurfaceRenderNode::IsSurfaceCreated() const
{
    return rcdExtInfo_.surfaceCreated;
}

std::shared_ptr<RSSurface> RSRcdSurfaceRenderNode::GetRSSurface() const
{
    return surface_;
}

sptr<IBufferConsumerListener> RSRcdSurfaceRenderNode::GetConsumerListener() const
{
    return consumerListener_;
}

void RSRcdSurfaceRenderNode::ClearBufferCache()
{
    if (surface_ != nullptr) {
        surface_->ClearBuffer();
    }
    if (consumer_ != nullptr) {
        consumer_->GoBackground();
    }
}

void RSRcdSurfaceRenderNode::ResetCurrFrameState()
{
    rcdExtInfo_.srcRect_.Clear();
    rcdExtInfo_.dstRect_.Clear();
    rcdExtInfo_.surfaceBounds.Clear();
    rcdExtInfo_.frameBounds.Clear();
    rcdExtInfo_.frameViewPort.Clear();
}

void RSRcdSurfaceRenderNode::Reset()
{
    ResetCurrFrameState();
}

bool RSRcdSurfaceRenderNode::IsBottomSurface() const
{
    return rcdExtInfo_.surfaceType == RCDSurfaceType::BOTTOM;
}

bool RSRcdSurfaceRenderNode::IsTopSurface() const
{
    return rcdExtInfo_.surfaceType == RCDSurfaceType::TOP;
}

bool RSRcdSurfaceRenderNode::IsInvalidSurface() const
{
    return rcdExtInfo_.surfaceType == RCDSurfaceType::INVALID;
}

float RSRcdSurfaceRenderNode::GetSurfaceWidth() const
{
    return rcdExtInfo_.surfaceBounds.GetWidth();
}

float RSRcdSurfaceRenderNode::GetSurfaceHeight() const
{
    return rcdExtInfo_.surfaceBounds.GetHeight();
}

float RSRcdSurfaceRenderNode::GetFrameOffsetX() const
{
    return rcdExtInfo_.GetFrameOffsetX();
}

float RSRcdSurfaceRenderNode::GetFrameOffsetY() const
{
    return rcdExtInfo_.GetFrameOffsetY();
}

const CldInfo& RSRcdSurfaceRenderNode::GetCldInfo() const
{
    return cldInfo_;
}
} // namespace Rosen
} // namespace OHOS