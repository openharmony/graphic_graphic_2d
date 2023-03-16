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

#include "rs_driven_surface_render_node.h"

#include "common/rs_obj_abs_geometry.h"
#include "platform/common/rs_log.h"
#include "rs_driven_render_ext.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_render_service_client.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSDrivenSurfaceRenderNode::RSDrivenSurfaceRenderNode(
    NodeId id, DrivenSurfaceType type, std::weak_ptr<RSContext> context)
    : RSRenderNode(id, context), RSSurfaceHandler(id)
{
    drivenExtInfo_.surfaceType_ = type;
}

void RSDrivenSurfaceRenderNode::SetDrivenCanvasNode(RSBaseRenderNode::SharedPtr node)
{
    drivenCanvasNode_ = node;
}

RSBaseRenderNode::SharedPtr RSDrivenSurfaceRenderNode::GetDrivenCanvasNode() const
{
    return drivenCanvasNode_;
}

const RectI& RSDrivenSurfaceRenderNode::GetSrcRect() const
{
    return drivenExtInfo_.srcRect_;
}

const RectI& RSDrivenSurfaceRenderNode::GetDstRect() const
{
    return drivenExtInfo_.dstRect_;
}

BufferRequestConfig RSDrivenSurfaceRenderNode::GetBufferRequestConfig() const
{
    BufferRequestConfig config {};
    config.width = static_cast<int32_t>(GetSurfaceWidth());
    config.height = static_cast<int32_t>(GetSurfaceHeight());
    config.strideAlignment = 0x8; // default stride is 8 Bytes.
    config.format = PIXEL_FMT_RGBA_8888;
    config.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB;
    config.timeout = 0;
    return config;
}

bool RSDrivenSurfaceRenderNode::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    if (consumer_ != nullptr && surface_ != nullptr) {
        RS_LOGI("RSDrivenSurfaceRenderNode::CreateSurface already created, return");
        return true;
    }
    consumer_ = IConsumerSurface::Create("DrivenSurfaceNode");
    if (consumer_ == nullptr) {
        RS_LOGE("RSDrivenSurfaceRenderNode::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer_->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSDrivenSurfaceRenderNode::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    consumerListener_ = listener;
    auto producer = consumer_->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    surface_ = client->CreateRSSurface(surface);
    RS_LOGI("RSDrivenSurfaceRenderNode::CreateSurface end");
    drivenExtInfo_.surfaceCreated_ = true;
    return true;
}

bool RSDrivenSurfaceRenderNode::IsSurfaceCreated() const
{
    return drivenExtInfo_.surfaceCreated_;
}

std::shared_ptr<RSSurface> RSDrivenSurfaceRenderNode::GetRSSurface() const
{
    return surface_;
}

sptr<IBufferConsumerListener> RSDrivenSurfaceRenderNode::GetConsumerListener() const
{
    return consumerListener_;
}

void RSDrivenSurfaceRenderNode::ClearBufferCache()
{
    if (surface_ && consumer_) {
        surface_->ClearBuffer();
        consumer_->GoBackground();
    }
}

void RSDrivenSurfaceRenderNode::PushFramePaintItem(Vector4f paintItem, int32_t itemIndex)
{
    drivenExtInfo_.currentState_.framePaintItems.emplace_back(std::make_pair(itemIndex, paintItem));
}

void RSDrivenSurfaceRenderNode::ResetCurrFrameState()
{
    drivenExtInfo_.renderMode_ = DrivenSurfaceRenderMode::DISABLED;
    drivenExtInfo_.currentState_.Clear();
    drivenExtInfo_.srcRect_.Clear();
    drivenExtInfo_.dstRect_.Clear();
    drivenExtInfo_.yOffset_ = 0.0;
}

void RSDrivenSurfaceRenderNode::Reset()
{
    ResetCurrFrameState();
    drivenExtInfo_.activateState_.Clear();
    drivenCanvasNode_ = nullptr;
}

void RSDrivenSurfaceRenderNode::DisabledRenderMode()
{
    RSDrivenRenderExt::Instance().DisabledRenderMode(drivenExtInfo_);
}

void RSDrivenSurfaceRenderNode::SetCurrFrameBounds(const RectF& bounds, const RectF& viewPort,
    const RectI& contentAbsRect)
{
    RSDrivenRenderExt::Instance().SetCurrFrameBounds(drivenExtInfo_, bounds, viewPort, contentAbsRect);
}

void RSDrivenSurfaceRenderNode::UpdateActivateFrameState(
    const RectI& dstRect, bool backgroundDirty, bool contentDirty, bool nonContentDirty)
{
    RSDrivenRenderExt::Instance().UpdateActivateFrameState(
        drivenExtInfo_, dstRect, backgroundDirty, contentDirty, nonContentDirty);
}

bool RSDrivenSurfaceRenderNode::IsExpandedMode() const
{
    return drivenExtInfo_.renderMode_ == DrivenSurfaceRenderMode::EXPANDED;
}

bool RSDrivenSurfaceRenderNode::IsReusableMode() const
{
    return drivenExtInfo_.renderMode_ == DrivenSurfaceRenderMode::REUSABLE;
}

bool RSDrivenSurfaceRenderNode::IsDisabledMode() const
{
    return drivenExtInfo_.renderMode_ == DrivenSurfaceRenderMode::DISABLED;
}

bool RSDrivenSurfaceRenderNode::IsBackgroundSurface() const
{
    return drivenExtInfo_.surfaceType_ == DrivenSurfaceType::BACKGROUND;
}

bool RSDrivenSurfaceRenderNode::IsContentSurface() const
{
    return drivenExtInfo_.surfaceType_ == DrivenSurfaceType::CONTENT;
}

bool RSDrivenSurfaceRenderNode::IsInvalidSurface() const
{
    return drivenExtInfo_.surfaceType_ == DrivenSurfaceType::INVALID;
}

float RSDrivenSurfaceRenderNode::GetSurfaceWidth() const
{
    return drivenExtInfo_.activateState_.surfaceBounds.GetWidth();
}

float RSDrivenSurfaceRenderNode::GetSurfaceHeight() const
{
    return drivenExtInfo_.activateState_.surfaceBounds.GetHeight();
}

float RSDrivenSurfaceRenderNode::GetFrameOffsetX() const
{
    return drivenExtInfo_.activateState_.GetFrameOffsetX();
}

float RSDrivenSurfaceRenderNode::GetFrameOffsetY() const
{
    return drivenExtInfo_.activateState_.GetFrameOffsetY();
}

const RectI& RSDrivenSurfaceRenderNode::GetFrameClipRect() const
{
    return drivenExtInfo_.activateState_.frameClipRect;
}
} // namespace Rosen
} // namespace OHOS
