/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_display_render_node.h"

#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSDisplayRenderNode::RSDisplayRenderNode(NodeId id, const RSDisplayNodeConfig& config, std::weak_ptr<RSContext> context)
    : RSBaseRenderNode(id, context), screenId_(config.screenId), offsetX_(0), offsetY_(0),
    isMirroredDisplay_(config.isMirrored)
{}

RSDisplayRenderNode::~RSDisplayRenderNode()
{
    if (renderContext_ != nullptr) {
        RS_LOGD("Destroy renderContext_!!");
        delete renderContext_ ;
        renderContext_  = nullptr;
    }
}

void RSDisplayRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareDisplayRenderNode(*this);
}

void RSDisplayRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->ProcessDisplayRenderNode(*this);
}

RSDisplayRenderNode::CompositeType RSDisplayRenderNode::GetCompositeType() const
{
    return compositeType_;
}

void RSDisplayRenderNode::SetCompositeType(RSDisplayRenderNode::CompositeType type)
{
    compositeType_ = type;
}

void RSDisplayRenderNode::SetForceSoftComposite(bool flag)
{
    forceSoftComposite_ = flag;
}

bool RSDisplayRenderNode::IsForceSoftComposite() const
{
    return forceSoftComposite_;
}

void RSDisplayRenderNode::SetMirrorSource(SharedPtr node)
{
    if (!isMirroredDisplay_ || node == nullptr) {
        return;
    }
    mirrorSource_ = node;
}

void RSDisplayRenderNode::SetDamageRegion(const Rect& damage)
{
    damageRect_ = damage;
}

void RSDisplayRenderNode::SetGlobalZOrder(float globalZOrder)
{
    globalZOrder_ = globalZOrder;
}

float RSDisplayRenderNode::GetGlobalZOrder() const
{
    return globalZOrder_;
}

bool RSDisplayRenderNode::IsMirrorDisplay() const
{
    return isMirroredDisplay_;
}

void RSDisplayRenderNode::SetSecurityDisplay(bool isSecurityDisplay)
{
    isSecurityDisplay_ = isSecurityDisplay;
}

bool RSDisplayRenderNode::GetSecurityDisplay() const
{
    return isSecurityDisplay_;
}

void RSDisplayRenderNode::SetConsumer(const sptr<Surface>& consumer)
{
    consumer_ = consumer;
}

void RSDisplayRenderNode::SetBuffer(const sptr<SurfaceBuffer>& buffer)
{
    if (buffer_ != nullptr) {
        preBuffer_ = buffer_;
        buffer_ = buffer;
    } else {
        buffer_ = buffer;
    }
}

void RSDisplayRenderNode::SetFence(sptr<SyncFence> fence)
{
    preFence_ = fence_;
    fence_ = fence;
}

void RSDisplayRenderNode::IncreaseAvailableBuffer()
{
    bufferAvailableCount_++;
}

int32_t RSDisplayRenderNode::ReduceAvailableBuffer()
{
    return --bufferAvailableCount_;
}

bool RSDisplayRenderNode::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    if (consumer_ != nullptr && surface_ != nullptr) {
        RS_LOGI("RSDisplayRenderNode::CreateSurface already created, return");
        return true;
    }
    consumer_ = Surface::CreateSurfaceAsConsumer("DisplayNode");
    if (consumer_ == nullptr) {
        RS_LOGE("RSDisplayRenderNode::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer_->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSDisplayRenderNode::CreateSurface RegisterConsumerListener fail");
        return false;
    }

    auto producer = consumer_->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);

#ifdef ACE_ENABLE_GL
    // GPU render
    surface_ = std::make_shared<RSSurfaceOhosGl>(surface);
    RS_LOGD("RSDisplayRenderNode::CreateSurface InitializeEglContext");
    renderContext_ = new RenderContext();
    renderContext_->InitializeEglContext();
    surface_->SetRenderContext(renderContext_);
#else
    // CPU render
    surface_ = std::make_shared<RSSurfaceOhosRaster>(surface);
#endif

    RS_LOGI("RSDisplayRenderNode::CreateSurface end");
    surfaceCreated_ = true;
    return true;
}
} // namespace Rosen
} // namespace OHOS
