
/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "command/rs_base_node_command.h"
#include "command/rs_surface_node_command.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#if !defined(__gnu_linux__) && !defined(_WIN32) && !defined(__APPLE__)
#include "platform/drawing/rs_surface_converter.h"
#endif
#include "render_context/render_context.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_proxy_node.h"

namespace OHOS {
namespace Rosen {
RSSurfaceNode::SharedPtr RSSurfaceNode::Create(const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow)
{
    return Create(surfaceNodeConfig, RSSurfaceNodeType::DEFAULT, isWindow);
}

RSSurfaceNode::SharedPtr RSSurfaceNode::Create(const RSSurfaceNodeConfig& surfaceNodeConfig,
    RSSurfaceNodeType type, bool isWindow)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return nullptr;
    }

    SharedPtr node(new RSSurfaceNode(surfaceNodeConfig, isWindow));
    RSNodeMap::MutableInstance().RegisterNode(node);

    // create node in RS
    RSSurfaceRenderNodeConfig config = {
        .id = node->GetId(),
        .name = node->name_,
        .onRender = surfaceNodeConfig.onRender,
    };
    if (!isWindow) {
        config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    } else {
        config.nodeType = type;
    }

    RS_LOGD("RSSurfaceNode::Create %s type %d", config.name.c_str(), config.nodeType);

    if (!node->CreateNodeAndSurface(config)) {
        ROSEN_LOGE("RSSurfaceNode::Create, create node and surface failed");
        return nullptr;
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
    if (node->GetName().find("battery_panel") != std::string::npos ||
        node->GetName().find("sound_panel") != std::string::npos) {
        node->SetFrameGravity(Gravity::TOP_LEFT);
    } else {
        node->SetFrameGravity(Gravity::RESIZE);
    }
    ROSEN_LOGD("RsDebug RSSurfaceNode::Create id:%" PRIu64, node->GetId());
    return node;
}

void RSSurfaceNode::CreateNodeInRenderThread()
{
    if (!IsRenderServiceNode()) {
        ROSEN_LOGI("RsDebug RSSurfaceNode::CreateNodeInRenderThread id:%" PRIu64 " already has RT Node", GetId());
        return;
    }

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }

    isChildOperationDisallowed_ = true;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(GetId());
    transactionProxy->AddCommand(command, false);

    command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(GetId());
    transactionProxy->AddCommand(command, false);

    command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(
        GetId(), [] { RSRenderThread::Instance().RequestNextVSync(); });
    transactionProxy->AddCommand(command, false);

    command = std::make_unique<RSSurfaceNodeSetSurfaceNodeType>(GetId(), RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    transactionProxy->AddCommand(command, true);
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

FollowType RSSurfaceNode::GetFollowType() const
{
    if (!IsUniRenderEnabled() && !isRenderServiceNode_) {
        return FollowType::FOLLOW_TO_PARENT;
    }
    if (IsUniRenderEnabled() && !isRenderServiceNode_ && !RSSystemProperties::IsUniRenderMode()) {
        return FollowType::FOLLOW_TO_PARENT;
    }
    return FollowType::NONE;
}

void RSSurfaceNode::OnBoundsSizeChanged() const
{
    auto bounds = GetStagingProperties().GetBounds();
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeUpdateSurfaceDefaultSize>(
        GetId(), bounds.z_, bounds.w_);
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
    ROSEN_LOGD("RSSurfaceNode::SetSecurityLayer, surfaceNodeId:[%" PRIu64 "] isSecurityLayer:%s", GetId(),
        isSecurityLayer ? "true" : "false");
}

bool RSSurfaceNode::GetSecurityLayer() const
{
    return isSecurityLayer_;
}

#if !defined(__gnu_linux__) && !defined(_WIN32) && !defined(__APPLE__)
void RSSurfaceNode::SetColorSpace(ColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetColorSpace>(GetId(), colorSpace);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}
#endif

void RSSurfaceNode::SetAbilityBGAlpha(uint8_t alpha)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetAbilityBGAlpha>(GetId(), alpha);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

void RSSurfaceNode::SetIsNotifyUIBufferAvailable(bool available)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetIsNotifyUIBufferAvailable>(GetId(), available);
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
    if (renderServiceClient == nullptr) {
        return false;
    }
    return renderServiceClient->RegisterBufferAvailableListener(GetId(), [weakThis = weak_from_this()]() {
        auto rsSurfaceNode = RSBaseNode::ReinterpretCast<RSSurfaceNode>(weakThis.lock());
        if (rsSurfaceNode == nullptr) {
            ROSEN_LOGE("RSSurfaceNode::SetBufferAvailableCallback this == null");
            return;
        }
        BufferAvailableCallback actualCallback;
        {
            std::lock_guard<std::mutex> lock(rsSurfaceNode->mutex_);
            actualCallback = rsSurfaceNode->callback_;
        }
        actualCallback();
    });
}

void RSSurfaceNode::SetAnimationFinished()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetAnimationFinished>(GetId());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
        transactionProxy->FlushImplicitTransaction();
    }
}

bool RSSurfaceNode::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint64(GetId()) && parcel.WriteString(name_) && parcel.WriteBool(IsRenderServiceNode());
}

std::shared_ptr<RSSurfaceNode> RSSurfaceNode::Unmarshalling(Parcel& parcel)
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

    SharedPtr surfaceNode(new RSSurfaceNode(config, isRenderServiceNode, id));
    RSNodeMap::MutableInstance().RegisterNode(surfaceNode);

    // for nodes constructed by unmarshalling, we should not destroy the corresponding render node on destruction
    surfaceNode->skipDestroyCommandInDestructor_ = true;

    return surfaceNode;
}

RSNode::SharedPtr RSSurfaceNode::UnmarshallingAsProxyNode(Parcel& parcel)
{
    uint64_t id = UINT64_MAX;
    std::string name;
    bool isRenderServiceNode = false;
    if (!(parcel.ReadUint64(id) && parcel.ReadString(name) && parcel.ReadBool(isRenderServiceNode))) {
        ROSEN_LOGE("RSSurfaceNode::Unmarshalling, read param failed");
        return nullptr;
    }

    // Create RSProxyNode by unmarshalling RSSurfaceNode, return existing node if it exists in RSNodeMap.
    return RSProxyNode::Create(id, name);
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

#if !defined(__gnu_linux__) && !defined(_WIN32) && !defined(__APPLE__)
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

void RSSurfaceNode::ResetContextAlpha() const
{
    // temporarily fix: manually set contextAlpha in RT and RS to 0.0f, to avoid residual alpha/context matrix from
    // previous animation. this value will be overwritten in RenderThreadVisitor::ProcessSurfaceRenderNode.
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }

    std::unique_ptr<RSCommand> commandRS = std::make_unique<RSSurfaceNodeSetContextAlpha>(GetId(), 0.0f);
    transactionProxy->AddCommand(commandRS, true);
}

void RSSurfaceNode::SetAppFreeze(bool isAppFreeze)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetAppFreeze>(GetId(), isAppFreeze);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

void RSSurfaceNode::SetContainerWindow(bool hasContainerWindow, float density)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetContainerWindow>(GetId(), hasContainerWindow, density);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

void RSSurfaceNode::SetWindowId(uint32_t windowId)
{
    windowId_ = windowId;
}

RSSurfaceNode::RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode)
    : RSNode(isRenderServiceNode), name_(config.SurfaceNodeName)
{}

RSSurfaceNode::RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode, NodeId id)
    : RSNode(isRenderServiceNode, id), name_(config.SurfaceNodeName)
{}

RSSurfaceNode::~RSSurfaceNode()
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (skipDestroyCommandInDestructor_) {
        // for ability view and remote window, we should destroy the corresponding render node in RenderThread
        if (transactionProxy != nullptr) {
            std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(GetId());
            transactionProxy->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, GetId());
        }
        return;
    }
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        renderServiceClient->UnregisterBufferAvailableListener(GetId());
    }
    if (!IsRenderServiceNode() && transactionProxy != nullptr) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(GetId());
        transactionProxy->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, GetId());
    }
}

} // namespace Rosen
} // namespace OHOS
