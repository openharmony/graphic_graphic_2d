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

#include "pipeline/rs_surface_render_node.h"

#include "command/rs_surface_node_command.h"
#include "display_type.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "transaction/rs_render_service_client.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSSurfaceRenderNode::RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context) : RSRenderNode(id, context) {}
RSSurfaceRenderNode::RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context)
    : RSRenderNode(config.id, context), name_(config.name)
{}

RSSurfaceRenderNode::~RSSurfaceRenderNode() {}

void RSSurfaceRenderNode::SetConsumer(const sptr<Surface>& consumer)
{
    consumer_ = consumer;
}

void RSSurfaceRenderNode::SetBuffer(const sptr<SurfaceBuffer>& buffer)
{
    if (buffer_ != nullptr) {
        preBuffer_ = buffer_;
        buffer_ = buffer;
    } else {
        buffer_ = buffer;
    }
}

void RSSurfaceRenderNode::SetFence(sptr<SyncFence> fence)
{
    preFence_ = fence_;
    fence_ = std::move(fence);
}

void RSSurfaceRenderNode::SetDamageRegion(const Rect& damage)
{
    damageRect_ = damage;
}

void RSSurfaceRenderNode::IncreaseAvailableBuffer()
{
    bufferAvailableCount_++;
}

int32_t RSSurfaceRenderNode::ReduceAvailableBuffer()
{
    return --bufferAvailableCount_;
}

void RSSurfaceRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareSurfaceRenderNode(*this);
}

void RSSurfaceRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->ProcessSurfaceRenderNode(*this);
}

void RSSurfaceRenderNode::SetMatrix(const SkMatrix& matrix, bool sendMsg)
{
    if (matrix_ == matrix) {
        return;
    }
    matrix_ = matrix;
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetMatrix>(GetId(), matrix);
    SendPropertyCommand(command);
}

const SkMatrix& RSSurfaceRenderNode::GetMatrix() const
{
    return matrix_;
}

void RSSurfaceRenderNode::SetAlpha(float alpha, bool sendMsg)
{
    if (alpha_ == alpha) {
        return;
    }
    alpha_ = alpha;
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetAlpha>(GetId(), alpha);
    SendPropertyCommand(command);
}

float RSSurfaceRenderNode::GetAlpha() const
{
    return alpha_;
}

void RSSurfaceRenderNode::SetClipRegion(Vector4f clipRegion, bool sendMsg)
{
    if (clipRect_ == clipRegion) {
        return;
    }
    clipRect_ = clipRegion;
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetClipRegion>(GetId(), clipRegion);
    SendPropertyCommand(command);
}

void RSSurfaceRenderNode::SetGlobalZOrder(float globalZOrder)
{
    globalZOrder_ = globalZOrder;
}

float RSSurfaceRenderNode::GetGlobalZOrder() const
{
    return globalZOrder_;
}

void RSSurfaceRenderNode::SetParentId(NodeId parentId, bool sendMsg)
{
    parentId_ = parentId;
    if (!sendMsg) {
        return;
    }
    // find parent surface
    auto node = GetParent().lock();
    std::unique_ptr<RSCommand> command;
    while (true) {
        if (node == nullptr) {
            return;
        } else if (auto rootnode = node->ReinterpretCastTo<RSRootRenderNode>()) {
            command = std::make_unique<RSSurfaceNodeSetParentSurface>(GetId(), rootnode->GetRSSurfaceNodeId());
            break;
        } else if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
            command = std::make_unique<RSSurfaceNodeSetParentSurface>(GetId(), surfaceNode->GetId());
            break;
        } else {
            node = node->GetParent().lock();
        }
    }
    // send a Command
    if (command) {
        SendPropertyCommand(command);
    }
}

NodeId RSSurfaceRenderNode::GetParentId() const
{
    return parentId_;
}

void RSSurfaceRenderNode::UpdateSurfaceDefaultSize(float width, float height)
{
    consumer_->SetDefaultWidthAndHeight(width, height);
}

void RSSurfaceRenderNode::SendPropertyCommand(std::unique_ptr<RSCommand>& command)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommandFromRT(command);
    }
}

BlendType RSSurfaceRenderNode::GetBlendType()
{
    return blendType_;
}

void RSSurfaceRenderNode::SetBlendType(BlendType blendType)
{
    blendType_ = blendType;
}

void RSSurfaceRenderNode::RegisterBufferAvailableListener(sptr<RSIBufferAvailableCallback> callback)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        callback_ = callback;
    }
}

void RSSurfaceRenderNode::ConnectToNodeInRenderService()
{
    ROSEN_LOGI("RSSurfaceRenderNode::ConnectToNodeInRenderService nodeId = %llu", this->GetId());
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        renderServiceClient->RegisterBufferAvailableListener(
            GetId(), [weakThis = weak_from_this()](bool isBufferAvailable) {
                auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(weakThis.lock());
                if (node == nullptr) {
                    return;
                }
                node->NotifyBufferAvailable(isBufferAvailable);
            });
    }
}

void RSSurfaceRenderNode::NotifyBufferAvailable(bool isBufferAvailable)
{
    ROSEN_LOGI("RSSurfaceRenderNode::NotifyBufferAvailable nodeId = %llu", this->GetId());

    // In RS, "isBufferAvailable_ = true" means buffer is ready and need to trigger ipc callback.
    // In RT, "isBufferAvailable_ = true" means RT know that RS have had available buffer
    // and ready to trigger "callbackForRenderThreadRefresh_" to "clip" on parent surface.
    isBufferAvailable_ = isBufferAvailable;

    if (isBufferAvailable == true && callbackForRenderThreadRefresh_ != nullptr) {
        callbackForRenderThreadRefresh_();
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback_ != nullptr) {
            callback_->OnBufferAvailable(true);
        }
    }
}

bool RSSurfaceRenderNode::IsBufferAvailable() const
{
    return isBufferAvailable_;
}

void RSSurfaceRenderNode::SetCallbackForRenderThreadRefresh(std::function<void(void)> callback)
{
    callbackForRenderThreadRefresh_ = callback;
}

bool RSSurfaceRenderNode::NeedSetCallbackForRenderThreadRefresh()
{
    return (callbackForRenderThreadRefresh_ == nullptr);
}

} // namespace Rosen
} // namespace OHOS
