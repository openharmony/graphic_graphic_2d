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
#include "common/rs_singleton.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "params/rs_rcd_render_params.h"
#include "transaction/rs_render_service_client.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_processor.h"
#include "rs_rcd_surface_render_node_drawable.h"
#include "rs_round_corner_display_manager.h"
#include "rs_rcd_render_visitor.h"

namespace OHOS {
namespace Rosen {

const float RCD_LAYER_Z_TOP1 = static_cast<float>(0x7FFFFFFF); // toppest
const float RCD_LAYER_Z_TOP2 = static_cast<float>(0x7FFFFEFF); // not set toppest - 1, float only 6 significant digits

RSRcdSurfaceRenderNode::RSRcdSurfaceRenderNode(RCDSurfaceType type, const std::weak_ptr<RSContext>& context)
    : RSRcdSurfaceRenderNode(GenerateId(), type, context)
{}

RSRcdSurfaceRenderNode::RSRcdSurfaceRenderNode(NodeId id, RCDSurfaceType type, const std::weak_ptr<RSContext> &context)
    : RSRenderNode(id, context), surfaceHandler_(std::make_shared<RSSurfaceHandler>(id))
{
    RS_LOGD("RCD: Start Create RSRcdSurfaceRenderNode %{public}d", type);
    rcdExtInfo_.surfaceType = type;
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, 0, MEMORY_TYPE::MEM_RENDER_NODE, ExtractPid(id)};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this));
}

RSRcdSurfaceRenderNode::SharedPtr RSRcdSurfaceRenderNode::Create(
    NodeId id, RCDSurfaceType type, const std::weak_ptr<RSContext> &context)
{
    auto node = std::make_shared<RSRcdSurfaceRenderNode>(type, context);
    node->renderTargetId_ = id;
    node->InitRenderParams();
    RSSingleton<RoundCornerDisplayManager>::GetInstance().AddRoundCornerDisplay(id);
    return node;
}

void RSRcdSurfaceRenderNode::InitRenderParams()
{
    stagingRenderParams_ = std::make_unique<RSRcdRenderParams>(GetId());
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    if (renderDrawable_ == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNode::InitRenderParams failed");
        return;
    }
    auto renderDrawable = std::static_pointer_cast<DrawableV2::RSRcdSurfaceRenderNodeDrawable>(renderDrawable_);
    if (renderDrawable) {
        renderDrawable->SetRenderTargetId(renderTargetId_);
        renderDrawable->SetSurfaceType(static_cast<uint32_t>(rcdExtInfo_.surfaceType));
    }
}

RSRcdSurfaceRenderNode::~RSRcdSurfaceRenderNode()
{
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this));
    RSSingleton<RoundCornerDisplayManager>::GetInstance().RemoveRCDResource(renderTargetId_);
}

const RectI& RSRcdSurfaceRenderNode::GetSrcRect() const
{
    return rcdExtInfo_.srcRect_;
}

const RectI& RSRcdSurfaceRenderNode::GetDstRect() const
{
    return rcdExtInfo_.dstRect_;
}

void RSRcdSurfaceRenderNode::OnSync()
{
    auto rcdParams = static_cast<RSRcdRenderParams*>(stagingRenderParams_.get());
    if (rcdParams == nullptr) {
        RS_LOGE("RSRcdSurfaceRenderNode::OnSync rcdParams is null");
        return;
    }
    if (!renderDrawable_) {
        return;
    }
    rcdParams->SetRcdEnabled(RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable());
    if (rcdParams->GetResourceChanged()) {
        rcdParams->SetNeedSync(true);
    }
    RSRenderNode::OnSync();
}

void RSRcdSurfaceRenderNode::PrepareHardwareResource(std::shared_ptr<rs_rcd::RoundCornerLayer> &layerInfo)
{
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    auto res = visitor->PrepareResourceBuffer(*this, layerInfo);
}

void RSRcdSurfaceRenderNode::DoProcessRenderMainThreadTask(bool resourceChanged, std::shared_ptr<RSProcessor> processor)
{
    RS_TRACE_BEGIN("RSRcdSurfaceRenderNode:DoRCDProcessMainThreadTask");
    if (!RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        RS_LOGD("RSRcdSurfaceRenderNode:DoRCDProcessMainThreadTask: Draw skip, rcd disabled");
        RS_TRACE_END();
        return;
    }
    if (!processor) {
        RS_LOGE("RCD: Processor in MainThread is incorrect");
        RS_TRACE_END();
        return;
    }
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    visitor->SetUniProcessor(processor);
    visitor->ProcessRcdSurfaceRenderNode(*this, resourceChanged);
    RS_TRACE_END();
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

bool RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer(const std::shared_ptr<rs_rcd::RoundCornerLayer>& layerInfo)
{
    RS_LOGD("RCD: Start PrepareHardwareResourceBuffer");
    if (layerInfo == nullptr) {
        RS_LOGE("RCD: layerInfo is nullptr");
        return false;
    }

    cldLayerInfo_.pathBin = std::string(rs_rcd::PATH_CONFIG_DIR) + "/" + layerInfo->binFileName;
    cldLayerInfo_.bufferSize = layerInfo->bufferSize;
    cldLayerInfo_.cldWidth = layerInfo->cldWidth;
    cldLayerInfo_.cldHeight = layerInfo->cldHeight;

    if (layerInfo->curBitmap == nullptr) {
        RS_LOGE("layerInfo->curBitmap is nullptr");
        return false;
    }
    layerBitmap_ = *(layerInfo->curBitmap);
    uint32_t bitmapHeight = static_cast<uint32_t>(layerBitmap_.GetHeight());
    uint32_t bitmapWidth = static_cast<uint32_t>(layerBitmap_.GetWidth());
    if (bitmapHeight <= 0 || bitmapWidth <= 0 || displayRect_.GetHeight() <= 0) {
        RS_LOGE("bitmapHeight, bitmapWidth or layerHeight is wrong value");
        return false;
    }
    SetRcdBufferHeight(bitmapHeight);
    SetRcdBufferWidth(bitmapWidth);
    SetRcdBufferSize(cldLayerInfo_.bufferSize);

    if (IsTopSurface()) {
        rcdExtInfo_.srcRect_ = RectI(0, 0, bitmapWidth, bitmapHeight);
        rcdExtInfo_.dstRect_ = RectI(displayRect_.GetLeft(), displayRect_.GetTop(), bitmapWidth, bitmapHeight);
        surfaceHandler_->SetGlobalZOrder(RCD_LAYER_Z_TOP1);
    } else {
        rcdExtInfo_.srcRect_ = RectI(0, 0, bitmapWidth, bitmapHeight);
        rcdExtInfo_.dstRect_ = RectI(displayRect_.GetLeft(), displayRect_.GetHeight() - bitmapHeight +
            displayRect_.GetTop(), bitmapWidth, bitmapHeight);
        surfaceHandler_->SetGlobalZOrder(RCD_LAYER_Z_TOP2);
    }
    return true;
}

void RSRcdSurfaceRenderNode::UpdateRcdRenderParams(
    bool resourceChanged, const std::shared_ptr<Drawing::Bitmap> curBitmap)
{
    auto rcdParams = static_cast<RSRcdRenderParams*>(stagingRenderParams_.get());
    if (resourceChanged) {
        rcdParams->SetPathBin(cldLayerInfo_.pathBin);
        rcdParams->SetBufferSize(cldLayerInfo_.bufferSize);
        rcdParams->SetCldWidth(cldLayerInfo_.cldWidth);
        rcdParams->SetCldHeight(cldLayerInfo_.cldHeight);
        rcdParams->SetSrcRect(rcdExtInfo_.srcRect_);
        rcdParams->SetDstRect(rcdExtInfo_.dstRect_);
        rcdParams->SetRcdBitmap(curBitmap);
    }
    rcdParams->SetResourceChanged(resourceChanged);
    if (rcdParams->NeedSync()) {
        AddToPendingSyncList();
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

bool RSRcdSurfaceRenderNode::IsTopSurface() const
{
    return rcdExtInfo_.surfaceType == RCDSurfaceType::TOP;
}

bool RSRcdSurfaceRenderNode::IsBottomSurface() const
{
    return rcdExtInfo_.surfaceType == RCDSurfaceType::BOTTOM;
}

bool RSRcdSurfaceRenderNode::IsInvalidSurface() const
{
    return rcdExtInfo_.surfaceType == RCDSurfaceType::INVALID;
}

const CldInfo RSRcdSurfaceRenderNode::GetCldInfo() const
{
    auto renderDrawable = std::static_pointer_cast<DrawableV2::RSRcdSurfaceRenderNodeDrawable>(renderDrawable_);
    if (!renderDrawable) {
        return CldInfo();
    }
    return renderDrawable->GetCldInfo();
}
} // namespace Rosen
} // namespace OHOS