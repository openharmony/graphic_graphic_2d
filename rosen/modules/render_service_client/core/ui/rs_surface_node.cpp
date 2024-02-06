
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
#include "command/rs_node_command.h"
#include "command/rs_surface_node_command.h"
#include "ipc_callbacks/rs_rt_refresh_callback.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "platform/drawing/rs_surface_converter.h"
#endif
#ifdef NEW_RENDER_CONTEXT
#include "render_context_base.h"
#else
#include "render_context/render_context.h"
#endif
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_proxy_node.h"

#ifndef ROSEN_CROSS_PLATFORM
#include "surface_utils.h"
#endif

namespace OHOS {
namespace Rosen {
RSSurfaceNode::SharedPtr RSSurfaceNode::Create(const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow)
{
    if (!isWindow) {
        return Create(surfaceNodeConfig, RSSurfaceNodeType::SELF_DRAWING_NODE, isWindow);
    }
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
        .bundleName = node->bundleName_,
        .additionalData = surfaceNodeConfig.additionalData,
        .isTextureExportNode = surfaceNodeConfig.isTextureExportNode,
        .isSync = surfaceNodeConfig.isSync,
    };
    config.nodeType = type;

    RS_LOGD("RSSurfaceNode::Create name:%{public}s bundleName: %{public}s type: %{public}hhu "
        "isWindow %{public}d %{public}d ", config.name.c_str(), config.bundleName.c_str(),
        config.nodeType, isWindow, node->IsRenderServiceNode());

    if (!node->CreateNodeAndSurface(config, surfaceNodeConfig.surfaceId)) {
        ROSEN_LOGE("RSSurfaceNode::Create, create node and surface failed");
        return nullptr;
    }

    node->SetClipToFrame(true);
    // create node in RT (only when in divided render and isRenderServiceNode_ == false)
    // create node in RT if is TextureExport node
    if (!node->IsRenderServiceNode()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(node->GetId(),
            config.nodeType, surfaceNodeConfig.isTextureExportNode);
        if (surfaceNodeConfig.isTextureExportNode) {
            transactionProxy->AddCommand(command, false);
            node->SetSurfaceIdToRenderNode();
        } else {
            transactionProxy->AddCommand(command, isWindow);
        }

        command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(node->GetId());
        transactionProxy->AddCommand(command, isWindow);

        RSRTRefreshCallback::Instance().SetRefresh([] { RSRenderThread::Instance().RequestNextVSync(); });
        command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(node->GetId(), true);
        transactionProxy->AddCommand(command, isWindow);
        node->SetFrameGravity(Gravity::RESIZE);
#if defined(USE_SURFACE_TEXTURE) && defined(ROSEN_ANDROID)
        if (type == RSSurfaceNodeType::SURFACE_TEXTURE_NODE) {
            RSSurfaceExtConfig config = {
                .type = RSSurfaceExtType::SURFACE_TEXTURE,
                .additionalData = nullptr,
            };
            node->CreateSurfaceExt(config);
        }
#endif
    }

    if (node->GetName().find("battery_panel") != std::string::npos ||
        node->GetName().find("sound_panel") != std::string::npos ||
        node->GetName().find("RosenWeb") != std::string::npos) {
        node->SetFrameGravity(Gravity::TOP_LEFT);
    } else if (!isWindow) {
        node->SetFrameGravity(Gravity::RESIZE);
    }
    ROSEN_LOGD("RsDebug RSSurfaceNode::Create id:%{public}" PRIu64, node->GetId());
    return node;
}

void RSSurfaceNode::CreateNodeInRenderThread()
{
    if (!IsRenderServiceNode()) {
        ROSEN_LOGI("RsDebug RSSurfaceNode::CreateNodeInRenderThread id:%{public}" PRIu64 " already has RT Node",
            GetId());
        return;
    }

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }

    isChildOperationDisallowed_ = true;
    isRenderServiceNode_ = false;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetSurfaceNodeType>(GetId(),
        static_cast<uint8_t>(RSSurfaceNodeType::ABILITY_COMPONENT_NODE));
    transactionProxy->AddCommand(command, true);

    // create node in RT (only when in divided render and isRenderServiceNode_ == false)
    if (!IsRenderServiceNode()) {
        command = std::make_unique<RSSurfaceNodeCreate>(GetId(), RSSurfaceNodeType::ABILITY_COMPONENT_NODE, false);
        transactionProxy->AddCommand(command, false);

        command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(GetId());
        transactionProxy->AddCommand(command, false);

        RSRTRefreshCallback::Instance().SetRefresh([] { RSRenderThread::Instance().RequestNextVSync(); });
        command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(GetId(), true);
        transactionProxy->AddCommand(command, false);
    }
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
    if (IsRenderServiceNode()) {
        return FollowType::NONE;
    } else {
        return FollowType::FOLLOW_TO_PARENT;
    }
}

void RSSurfaceNode::MarkUIHidden(bool isHidden)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeMarkUIHidden>(GetId(), isHidden);
    transactionProxy->AddCommand(command, IsRenderServiceNode());
    transactionProxy->FlushImplicitTransaction();
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
#ifdef ROSEN_CROSS_PLATFORM
    if (!IsRenderServiceNode()) {
        std::unique_ptr<RSCommand> commandRt = std::make_unique<RSSurfaceNodeUpdateSurfaceDefaultSize>(
            GetId(), bounds.z_, bounds.w_);
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(commandRt, false);
        }
    }
#endif
    std::lock_guard<std::mutex> lock(mutex_);
    if (boundsChangedCallback_) {
        boundsChangedCallback_(bounds);
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
    ROSEN_LOGI("RSSurfaceNode::SetSecurityLayer, surfaceNodeId:[%{public}" PRIu64 "] isSecurityLayer:%{public}s",
        GetId(), isSecurityLayer ? "true" : "false");
}

bool RSSurfaceNode::GetSecurityLayer() const
{
    return isSecurityLayer_;
}

void RSSurfaceNode::SetSkipLayer(bool isSkipLayer)
{
    isSkipLayer_ = isSkipLayer;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetSkipLayer>(GetId(), isSkipLayer);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSSurfaceNode::SetSkipLayer, surfaceNodeId:[%" PRIu64 "] isSkipLayer:%s", GetId(),
        isSkipLayer ? "true" : "false");
}

bool RSSurfaceNode::GetSkipLayer() const
{
    return isSkipLayer_;
}

void RSSurfaceNode::SetFingerprint(bool hasFingerprint)
{
    hasFingerprint_ = hasFingerprint;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetFingerprint>(GetId(), hasFingerprint);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSSurfaceNode::SetFingerprint, surfaceNodeId:[%{public}" PRIu64 "] hasFingerprint:%{public}s", GetId(),
        hasFingerprint ? "true" : "false");
}

bool RSSurfaceNode::GetFingerprint() const
{
    return hasFingerprint_;
}

void RSSurfaceNode::SetColorSpace(GraphicColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetColorSpace>(GetId(), colorSpace);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

void RSSurfaceNode::CreateTextExportRenderNodeInRT()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(GetId(),
        RSSurfaceNodeType::SELF_DRAWING_NODE, true);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    transactionProxy->AddCommand(command, false);
}

void RSSurfaceNode::SetIsTextureExportNode(bool isTextureExportNode)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetIsTextureExportNode>(GetId(),
        isTextureExportNode);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    transactionProxy->AddCommand(command, false);
    // need to reset isTextureExport sign in renderService
    transactionProxy->AddCommand(command, true);
}

void RSSurfaceNode::SetTextureExport(bool isTextureExportNode)
{
    if (isTextureExportNode == isTextureExportNode_) {
        return;
    }
    isTextureExportNode_ = isTextureExportNode;
    if (!isTextureExportNode_) {
        SetIsTextureExportNode(isTextureExportNode);
        DoFlushModifier();
        return;
    }
    CreateTextExportRenderNodeInRT();
    SetIsTextureExportNode(isTextureExportNode);
    SetSurfaceIdToRenderNode();
    DoFlushModifier();
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
        rsSurfaceNode->bufferAvailable_ = true;
        if (actualCallback) {
            actualCallback();
        }
    });
}

bool RSSurfaceNode::IsBufferAvailable() const
{
    return bufferAvailable_;
}

void RSSurfaceNode::SetBoundsChangedCallback(BoundsChangedCallback callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    boundsChangedCallback_ = callback;
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

void RSSurfaceNode::SetSurfaceIdToRenderNode()
{
#ifndef ROSEN_CROSS_PLATFORM
    auto surface = GetSurface();
    if (surface) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSurfaceNodeSetSurfaceId>(GetId(),
            surface->GetUniqueId());
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            return;
        }
        transactionProxy->AddCommand(command, false);
    }
#endif
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
    return std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())->
        CreateNode(config);
}

bool RSSurfaceNode::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, SurfaceId surfaceId)
{
    if (surfaceId == 0) {
        surface_ = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())->
        CreateNodeAndSurface(config);
    } else {
#ifndef ROSEN_CROSS_PLATFORM
        sptr<Surface> surface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);
        if (surface == nullptr) {
            ROSEN_LOGE("RSSurfaceNode::CreateNodeAndSurface nodeId is %llu cannot find surface by surfaceId %llu",
                GetId(), surfaceId);
            return false;
        }
        surface_ = std::static_pointer_cast<RSRenderServiceClient>(
            RSIRenderClient::CreateRenderServiceClient())->CreateRSSurface(surface);
        if (surface_ == nullptr) {
            ROSEN_LOGE("RSSurfaceNode::CreateNodeAndSurface nodeId is %llu creat RSSurface fail", GetId());
            return false;
        }
#endif
    }
    return (surface_ != nullptr);
}

#ifndef ROSEN_CROSS_PLATFORM
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
        // Property message should be sent to RenderService.
        return false;
    } else {
        // Only when in divided render and isRenderServiceNode_ == false
        // property message should be sent to RenderService & RenderThread.
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

void RSSurfaceNode::SetFreeze(bool isFreeze)
{
    if (!IsUniRenderEnabled()) {
        ROSEN_LOGE("SetFreeze is not supported in separate render");
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetFreeze>(GetId(), isFreeze);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

std::pair<std::string, std::string> RSSurfaceNode::SplitSurfaceNodeName(std::string surfaceNodeName)
{
    if (auto position = surfaceNodeName.find("#");  position != std::string::npos) {
        return std::make_pair(surfaceNodeName.substr(0, position), surfaceNodeName.substr(position + 1));
    }
    return std::make_pair("", surfaceNodeName);
}

RSSurfaceNode::RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode)
    : RSNode(isRenderServiceNode, config.isTextureExportNode)
{
    auto result = SplitSurfaceNodeName(config.SurfaceNodeName);
    bundleName_ = result.first;
    name_ = result.second;
}

RSSurfaceNode::RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode, NodeId id)
    : RSNode(isRenderServiceNode, id, config.isTextureExportNode)
{
    auto result = SplitSurfaceNodeName(config.SurfaceNodeName);
    bundleName_ = result.first;
    name_ = result.second;
}

RSSurfaceNode::~RSSurfaceNode()
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }

    // both divided and unirender need to unregister listener when surfaceNode destroy
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        renderServiceClient->UnregisterBufferAvailableListener(GetId());
    }

    // For abilityComponent and remote window, we should destroy the corresponding render node in RenderThread
    // The destructor of render node in RenderService should controlled by application
    // Command sent only in divided render
    if (skipDestroyCommandInDestructor_ && !IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(GetId());
        transactionProxy->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, GetId());
        return;
    }

    // For self-drawing surfaceNode, we should destroy the corresponding render node in RenderService
    // Command sent only in divided render
    if (!IsRenderServiceNode()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(GetId());
        transactionProxy->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, GetId());
        return;
    }
}

void RSSurfaceNode::AttachToDisplay(uint64_t screenId)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeAttachToDisplay>(GetId(), screenId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSSurfaceNode::DetachToDisplay(uint64_t screenId)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeDetachToDisplay>(GetId(), screenId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSSurfaceNode::SetHardwareEnabled(bool isEnabled, SelfDrawingNodeType selfDrawingType)
{
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        renderServiceClient->SetHardwareEnabled(GetId(), isEnabled, selfDrawingType);
    }
}

void RSSurfaceNode::SetBootAnimation(bool isBootAnimation)
{
    isBootAnimation_ = isBootAnimation;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetBootAnimation>(GetId(), isBootAnimation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSSurfaceNode::SetBootAnimation, surfaceNodeId:[%" PRIu64 "] isBootAnimation:%s",
        GetId(), isBootAnimation ? "true" : "false");
}

bool RSSurfaceNode::GetBootAnimation() const
{
    return isBootAnimation_;
}

#ifdef USE_SURFACE_TEXTURE
void RSSurfaceNode::CreateSurfaceExt(const RSSurfaceExtConfig& config)
{
    auto texture = surface_->GetSurfaceExt(config);
    if (texture == nullptr) {
        texture = surface_->CreateSurfaceExt(config);
    }
    if (texture == nullptr) {
        ROSEN_LOGE("RSSurfaceNode::CreateSurfaceExt failed %{public}" PRIu64 " type %{public}u",
        GetId(), config.type);
        return;
    }
    ROSEN_LOGD("RSSurfaceNode::CreateSurfaceExt %{public}" PRIu64 " type %{public}u %{public}p",
        GetId(), config.type, texture.get());
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeCreateSurfaceExt>(GetId(), texture);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, false);
    }
}

void RSSurfaceNode::SetSurfaceTexture(const RSSurfaceExtConfig& config)
{
    CreateSurfaceExt(config);
}

void RSSurfaceNode::MarkUiFrameAvailable(bool available)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetIsNotifyUIBufferAvailable>(GetId(), available);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, false);
    }
}

void RSSurfaceNode::SetSurfaceTextureAttachCallBack(const RSSurfaceTextureAttachCallBack& attachCallback)
{
    RSSurfaceTextureConfig config = {
        .type = RSSurfaceExtType::SURFACE_TEXTURE,
    };
    auto texture = surface_->GetSurfaceExt(config);
    if (texture) {
        texture->SetAttachCallback(attachCallback);
    }
}

void RSSurfaceNode::SetSurfaceTextureUpdateCallBack(const RSSurfaceTextureUpdateCallBack& updateCallback)
{
    RSSurfaceTextureConfig config = {
        .type = RSSurfaceExtType::SURFACE_TEXTURE,
        .additionalData = nullptr
    };
    auto texture = surface_->GetSurfaceExt(config);
    if (texture) {
        texture->SetUpdateCallback(updateCallback);
    }
}
#endif

void RSSurfaceNode::SetForeground(bool isForeground)
{
    ROSEN_LOGD("RSSurfaceNode::SetForeground, surfaceNodeId:[%" PRIu64 "] isForeground:%s",
        GetId(), isForeground ? "true" : "false");
    std::unique_ptr<RSCommand> commandRS =
        std::make_unique<RSSurfaceNodeSetForeground>(GetId(), isForeground);
    std::unique_ptr<RSCommand> commandRT =
        std::make_unique<RSSurfaceNodeSetForeground>(GetId(), isForeground);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(commandRS, true);
        transactionProxy->AddCommand(commandRT, false);
    }
}

void RSSurfaceNode::SetForceUIFirst(bool forceUIFirst)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetForceUIFirst>(GetId(), forceUIFirst);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}
} // namespace Rosen
} // namespace OHOS
