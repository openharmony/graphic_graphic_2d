
/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ui/rs_surface_node.h"

#include <algorithm>
#include <string>

#include "command/rs_surface_node_command.h"
#include "command/rs_base_node_command.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface_converter.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "render_context/render_context.h"

namespace OHOS {
namespace Rosen {

RSSurfaceNode::SharedPtr RSSurfaceNode::Create(const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return nullptr;
    }

    SharedPtr node(new RSSurfaceNode(surfaceNodeConfig, isWindow));
    RSNodeMap::MutableInstance().RegisterNode(node);

    // create node in RS
    RSSurfaceRenderNodeConfig config = { .id = node->GetId(), .name = node->name_ };
    if (isWindow && isUniRenderEnabled_) {
        if (!node->CreateNode(config)) {
            ROSEN_LOGE("RSSurfaceNode::Create, create node failed");
            return nullptr;
        }
    } else {
        if (!node->CreateNodeAndSurface(config)) {
            ROSEN_LOGE("RSSurfaceNode::Create, create node and surface failed");
            return nullptr;
        }
    }

    node->SetClipToFrame(true);
    // create node in RT
    if (!isWindow) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(node->GetId());
        transactionProxy->AddCommand(command, isWindow);

        command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(node->GetId());
        transactionProxy->AddCommand(command, isWindow);

        command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(
            node->GetId(), [] { RSRenderThread::Instance().RequestNextVSync(); });
        transactionProxy->AddCommand(command, isWindow);
        node->SetFrameGravity(Gravity::RESIZE);
    }
    ROSEN_LOGD("RsDebug RSSurfaceNode::Create id:%llu", node->GetId());
    return node;
}

void RSSurfaceNode::CreateNodeInRenderThread(bool isProxy)
{
    if (!IsRenderServiceNode()) {
        ROSEN_LOGI("RsDebug RSSurfaceNode::CreateNodeInRenderThread id:%llu already has RT Node", GetId());
        return;
    }

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }

    isChildOperationDisallowed_ = true;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(GetId());
    transactionProxy->AddCommand(command, false);

    if (isProxy) {
        command = std::make_unique<RSSurfaceNodeSetProxy>(GetId());
    } else {
        command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(GetId());
    }
    transactionProxy->AddCommand(command, false);

    // for proxied nodes, we don't need commands sent to RT and refresh callbacks
    if (isProxy) {
        isRenderServiceNode_ = true;
        return;
    }

    command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(
        GetId(), [] { RSRenderThread::Instance().RequestNextVSync(); });
    transactionProxy->AddCommand(command, false);
    isRenderServiceNode_ = false;
}

void RSSurfaceNode::AddChild(std::shared_ptr<RSBaseNode> child, int index)
{
    if (isChildOperationDisallowed_) {
        ROSEN_LOGE("RSSurfaceNode::AddChild for non RenderServiceNodeType surfaceNode is not allowed");
        return;
    }
    RSBaseNode::AddChild(child, index);
}

void RSSurfaceNode::RemoveChild(std::shared_ptr<RSBaseNode> child)
{
    if (isChildOperationDisallowed_) {
        ROSEN_LOGE("RSSurfaceNode::RemoveChild for non RenderServiceNodeType surfaceNode is not allowed");
        return;
    }
    RSBaseNode::RemoveChild(child);
}

void RSSurfaceNode::ClearChildren()
{
    if (isChildOperationDisallowed_) {
        ROSEN_LOGE("RSSurfaceNode::ClearChildren for non RenderServiceNodeType surfaceNode is not allowed");
        return;
    }
    RSBaseNode::ClearChildren();
}

void RSSurfaceNode::OnBoundsSizeChanged() const
{
    if (surface_ == nullptr) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeUpdateSurfaceDefaultSize>(
        GetId(), GetStagingProperties().GetBoundsWidth(), GetStagingProperties().GetBoundsHeight());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

void RSSurfaceNode::SetSecurityLayer(bool isSecurityLayer)
{
    isSecurityLayer_ = isSecurityLayer;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetSecurityLayer>(GetId(), isSecurityLayer);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSSurfaceNode::SetSecurityLayer, surfaceNodeId:[%llu] isSecurityLayer:%s", GetId(),
        isSecurityLayer ? "true" : "false");
}

bool RSSurfaceNode::GetSecurityLayer() const
{
    return isSecurityLayer_;
}

void RSSurfaceNode::SetColorSpace(ColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

void RSSurfaceNode::SetAbilityBGAlpha(uint8_t alpha)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetAbilityBGAlpha>(GetId(), alpha);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

bool RSSurfaceNode::SetBufferAvailableCallback(BufferAvailableCallback callback)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        callback_ = callback;
    }
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        return renderServiceClient->RegisterBufferAvailableListener(GetId(), [weakThis = weak_from_this()]() {
            auto rsSurfaceNode = RSBaseNode::ReinterpretCast<RSSurfaceNode>(weakThis.lock());
            if (rsSurfaceNode) {
                BufferAvailableCallback actualCallback;
                {
                    std::lock_guard<std::mutex> lock(rsSurfaceNode->mutex_);
                    actualCallback = rsSurfaceNode->callback_;
                }
                actualCallback();
            } else {
                ROSEN_LOGE("RSSurfaceNode::SetBufferAvailableCallback this == null");
            }
        });
    } else {
        return false;
    }
}

bool RSSurfaceNode::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint64(GetId()) && parcel.WriteString(name_) && parcel.WriteBool(IsRenderServiceNode());
}

RSSurfaceNode::SharedPtr RSSurfaceNode::Unmarshalling(Parcel& parcel)
{
    uint64_t id = UINT64_MAX;
    std::string name;
    bool isRenderServiceNode = false;
    if (!(parcel.ReadUint64(id) && parcel.ReadString(name) && parcel.ReadBool(isRenderServiceNode))) {
        ROSEN_LOGE("RSSurfaceNode::Unmarshalling, read param failed");
        return nullptr;
    }
    RSSurfaceNodeConfig config = { name };

    if (auto prevNode = RSNodeMap::Instance().GetNode(id)) {
        // if the node id is already in the map, we should not create a new node
        return prevNode->ReinterpretCastTo<RSSurfaceNode>();
    }

    SharedPtr surfaceNode(new RSSurfaceNode(config, isRenderServiceNode));
    surfaceNode->SetId(id);
    RSNodeMap::MutableInstance().RegisterNode(surfaceNode);

    return surfaceNode;
}

bool RSSurfaceNode::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    return std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())
               ->CreateNode(config);
}

bool RSSurfaceNode::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    surface_ = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())
                   ->CreateNodeAndSurface(config);
    return (surface_ != nullptr);
}

#ifdef ROSEN_OHOS
sptr<OHOS::Surface> RSSurfaceNode::GetSurface() const
{
    if (surface_ == nullptr) {
        ROSEN_LOGE("RSSurfaceNode::GetSurface, surface_ is nullptr");
        return nullptr;
    }
    auto ohosSurface = RSSurfaceConverter::ConvertToOhosSurface(surface_);
    return ohosSurface;
}
#endif

bool RSSurfaceNode::NeedForcedSendToRemote() const
{
    if (IsRenderServiceNode()) {
        // RSRenderSurfaceNode in RS only need send property message to RenderService.
        return false;
    } else {
        // RSRenderSurfaceNode in RT need send property message both to RenderService & RenderThread.
        return true;
    }
}

RSSurfaceNode::RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode)
    : RSNode(isRenderServiceNode), name_(config.SurfaceNodeName)
{}

RSSurfaceNode::~RSSurfaceNode() {
    if (!IsRenderServiceNode()) {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(GetId());
            transactionProxy->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, GetId());
        }
    }
}

} // namespace Rosen
} // namespace OHOS
