
/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#ifdef RS_ENABLE_GPU
#include "render_context/render_context.h"
#endif
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_proxy_node.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"
#include "rs_ui_context.h"

#ifndef ROSEN_CROSS_PLATFORM
#include "surface_utils.h"
#endif

#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "display_engine/rs_vpe_manager.h"
#endif

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
namespace {
constexpr uint32_t WATERMARK_NAME_LENGTH_LIMIT = 128;
}
#endif
RSSurfaceNode::SharedPtr RSSurfaceNode::Create(
    const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow, std::shared_ptr<RSUIContext> rsUIContext)
{
    if (!isWindow) {
        return Create(surfaceNodeConfig, RSSurfaceNodeType::SELF_DRAWING_NODE, isWindow, false, rsUIContext);
    }
    return Create(surfaceNodeConfig, RSSurfaceNodeType::DEFAULT, isWindow);
}

RSSurfaceNode::SharedPtr RSSurfaceNode::Create(const RSSurfaceNodeConfig& surfaceNodeConfig,
    RSSurfaceNodeType type, bool isWindow, bool unobscured, std::shared_ptr<RSUIContext> rsUIContext)
{
    SharedPtr node(new RSSurfaceNode(surfaceNodeConfig, isWindow, rsUIContext));
    if (rsUIContext != nullptr) {
        rsUIContext->GetMutableNodeMap().RegisterNode(node);
    } else {
        RSNodeMap::MutableInstance().RegisterNode(node);
    }

    // create node in RS
    RSSurfaceRenderNodeConfig config = {
        .id = node->GetId(),
        .name = node->name_,
        .additionalData = surfaceNodeConfig.additionalData,
        .isTextureExportNode = surfaceNodeConfig.isTextureExportNode,
        .isSync = surfaceNodeConfig.isSync,
        .surfaceWindowType = surfaceNodeConfig.surfaceWindowType,
    };
    config.nodeType = type;

    RS_TRACE_NAME_FMT("RSSurfaceNode::Create name: %s type: %hhu, id: %lu, token:%lu", node->name_.c_str(),
        config.nodeType, node->GetId(), rsUIContext ? rsUIContext->GetToken() : -1);
    RS_LOGD("RSSurfaceNode::Create name:%{public}s type: %{public}hhu "
        "isWindow %{public}d %{public}d ", config.name.c_str(),
        config.nodeType, isWindow, node->IsRenderServiceNode());

    if (type == RSSurfaceNodeType::LEASH_WINDOW_NODE && node->IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreateWithConfig>(
            config.id, config.name, static_cast<uint8_t>(config.nodeType), config.surfaceWindowType);
        node->AddCommand(command, isWindow);
    } else {
#ifndef SCREENLESS_DEVICE
        if (!node->CreateNodeAndSurface(config, surfaceNodeConfig.surfaceId, unobscured)) {
            ROSEN_LOGE("RSSurfaceNode::Create, create node and surface failed");
            return nullptr;
        }
#endif
    }

    node->SetClipToFrame(true);
    // create node in RT (only when in divided render and isRenderServiceNode_ == false)
    // create node in RT if is TextureExport node
    if (!node->IsRenderServiceNode()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(node->GetId(),
            config.nodeType, surfaceNodeConfig.isTextureExportNode);
        if (surfaceNodeConfig.isTextureExportNode) {
            node->AddCommand(command, false);
            node->SetSurfaceIdToRenderNode();
        } else {
            node->AddCommand(command, isWindow);
        }
        command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(node->GetId());
        node->AddCommand(command, isWindow);

        RSRTRefreshCallback::Instance().SetRefresh([] { RSRenderThread::Instance().RequestNextVSync(); });
        command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(node->GetId(), true);
        node->AddCommand(command, isWindow);
        node->SetFrameGravity(Gravity::RESIZE);
        // codes for arkui-x
#if defined(USE_SURFACE_TEXTURE) && defined(ROSEN_ANDROID) && !defined(SCREENLESS_DEVICE)
        if (type == RSSurfaceNodeType::SURFACE_TEXTURE_NODE) {
            RSSurfaceExtConfig config = {
                .type = RSSurfaceExtType::SURFACE_TEXTURE,
                .additionalData = nullptr,
            };
            node->CreateSurfaceExt(config);
        }
#endif
        // codes for arkui-x
#if defined(USE_SURFACE_TEXTURE) && defined(ROSEN_IOS) && !defined(SCREENLESS_DEVICE)
        if ((type == RSSurfaceNodeType::SURFACE_TEXTURE_NODE) &&
            (surfaceNodeConfig.SurfaceNodeName == "PlatformViewSurface")) {
            RSSurfaceExtConfig config = {
                .type = RSSurfaceExtType::SURFACE_PLATFORM_TEXTURE,
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
    node->SetUIContextToken();
    return node;
}

void RSSurfaceNode::CreateNodeInRenderThread()
{
    if (!IsRenderServiceNode()) {
        ROSEN_LOGI(
            "RsDebug RSSurfaceNode::CreateNodeInRenderThread id:%{public}" PRIu64 " already has RT Node", GetId());
        return;
    }

    isChildOperationDisallowed_ = true;
    isRenderServiceNode_ = false;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetSurfaceNodeType>(
        GetId(), static_cast<uint8_t>(RSSurfaceNodeType::ABILITY_COMPONENT_NODE));
    AddCommand(command, true);

    // create node in RT (only when in divided render and isRenderServiceNode_ == false)
    if (!IsRenderServiceNode()) {
        command = std::make_unique<RSSurfaceNodeCreate>(GetId(), RSSurfaceNodeType::ABILITY_COMPONENT_NODE, false);
        AddCommand(command, false);

        command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(GetId());
        AddCommand(command, false);

        RSRTRefreshCallback::Instance().SetRefresh([] { RSRenderThread::Instance().RequestNextVSync(); });
        command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(GetId(), true);
        AddCommand(command, false);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeMarkUIHidden>(GetId(), isHidden);
    auto transaction = GetRSTransaction();
    if (transaction != nullptr) {
        transaction->AddCommand(command, IsRenderServiceNode());
        if (!isTextureExportNode_) {
            transaction->FlushImplicitTransaction();
        }
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode());
            if (!isTextureExportNode_) {
                transactionProxy->FlushImplicitTransaction();
            }
        }
    }
}

void RSSurfaceNode::OnBoundsSizeChanged() const
{
    auto bounds = GetStagingProperties().GetBounds();
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeUpdateSurfaceDefaultSize>(
        GetId(), bounds.z_, bounds.w_);
    AddCommand(command, true);
#ifdef ROSEN_CROSS_PLATFORM
    if (!IsRenderServiceNode()) {
        std::unique_ptr<RSCommand> commandRt = std::make_unique<RSSurfaceNodeUpdateSurfaceDefaultSize>(
            GetId(), bounds.z_, bounds.w_);
        AddCommand(commandRt, false);
    }
#endif
    std::lock_guard<std::mutex> lock(mutex_);
    if (boundsChangedCallback_) {
        boundsChangedCallback_(bounds);
        RS_TRACE_NAME_FMT("node:[name: %s, id: %" PRIu64 ", bounds:%f %f %f %f] already callback",
            GetName().c_str(), GetId(), bounds.x_, bounds.y_, bounds.z_, bounds.w_);
    }
}

void RSSurfaceNode::SetLeashPersistentId(LeashPersistentId leashPersistentId)
{
    leashPersistentId_ = leashPersistentId;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSurfaceNodeSetLeashPersistentId>(GetId(), leashPersistentId);
    AddCommand(command, true);
    ROSEN_LOGD("RSSurfaceNode::SetLeashPersistentId, \
        surfaceNodeId:[%{public}" PRIu64 "] leashPersistentId:[%{public}" PRIu64 "]", GetId(), leashPersistentId);
}

LeashPersistentId RSSurfaceNode::GetLeashPersistentId() const
{
    return leashPersistentId_;
}

void RSSurfaceNode::SetSecurityLayer(bool isSecurityLayer)
{
    isSecurityLayer_ = isSecurityLayer;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetSecurityLayer>(GetId(), isSecurityLayer);
    AddCommand(command, true);
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
    AddCommand(command, true);
    ROSEN_LOGD("RSSurfaceNode::SetSkipLayer, surfaceNodeId:[%" PRIu64 "] isSkipLayer:%s", GetId(),
        isSkipLayer ? "true" : "false");
}

bool RSSurfaceNode::GetSkipLayer() const
{
    return isSkipLayer_;
}

void RSSurfaceNode::SetSnapshotSkipLayer(bool isSnapshotSkipLayer)
{
    isSnapshotSkipLayer_ = isSnapshotSkipLayer;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetSnapshotSkipLayer>(GetId(), isSnapshotSkipLayer);
    AddCommand(command, true);
    ROSEN_LOGD("RSSurfaceNode::SetSnapshotSkipLayer, surfaceNodeId:[%" PRIu64 "] isSnapshotSkipLayer:%s", GetId(),
        isSnapshotSkipLayer ? "true" : "false");
}

bool RSSurfaceNode::GetSnapshotSkipLayer() const
{
    return isSnapshotSkipLayer_;
}

void RSSurfaceNode::SetFingerprint(bool hasFingerprint)
{
    hasFingerprint_ = hasFingerprint;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetFingerprint>(GetId(), hasFingerprint);
    AddCommand(command, true);
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
    AddCommand(command, true);
}

void RSSurfaceNode::CreateRenderNodeForTextureExportSwitch()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(GetId(),
        RSSurfaceNodeType::SELF_DRAWING_NODE, isTextureExportNode_);
    AddCommand(command, IsRenderServiceNode());
    if (!IsRenderServiceNode()) {
        hasCreateRenderNodeInRT_ = true;
        command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(GetId());
        AddCommand(command, false);

        RSRTRefreshCallback::Instance().SetRefresh([] { RSRenderThread::Instance().RequestNextVSync(); });
        command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(GetId(), true);
        AddCommand(command, false);
    } else {
        hasCreateRenderNodeInRS_ = true;
    }
}

void RSSurfaceNode::SetIsTextureExportNode(bool isTextureExportNode)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetIsTextureExportNode>(GetId(), isTextureExportNode);
    AddCommand(command, false);
    // need to reset isTextureExport sign in renderService
    command = std::make_unique<RSSurfaceNodeSetIsTextureExportNode>(GetId(), isTextureExportNode);
    AddCommand(command, true);
}

void RSSurfaceNode::SetTextureExport(bool isTextureExportNode)
{
    if (isTextureExportNode == isTextureExportNode_) {
        return;
    }
    isTextureExportNode_ = isTextureExportNode;
    if (!IsUniRenderEnabled()) {
        return;
    }
    if (!isTextureExportNode_) {
        SetIsTextureExportNode(isTextureExportNode);
        DoFlushModifier();
        return;
    }
    CreateRenderNodeForTextureExportSwitch();
    SetIsTextureExportNode(isTextureExportNode);
    SetSurfaceIdToRenderNode();
    DoFlushModifier();
}

void RSSurfaceNode::SetAbilityBGAlpha(uint8_t alpha)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetAbilityBGAlpha>(GetId(), alpha);
    AddCommand(command, true);
}

void RSSurfaceNode::SetIsNotifyUIBufferAvailable(bool available)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetIsNotifyUIBufferAvailable>(GetId(), available);
    AddCommand(command, true);
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
    auto transaction = GetRSTransaction();
    if (transaction != nullptr) {
        transaction->AddCommand(command, true);
        transaction->FlushImplicitTransaction();
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, true);
            transactionProxy->FlushImplicitTransaction();
        }
    }
}

bool RSSurfaceNode::Marshalling(Parcel& parcel) const
{
    bool flag = parcel.WriteUint64(GetId()) && parcel.WriteString(name_) && parcel.WriteBool(IsRenderServiceNode());
    if (!flag) {
        ROSEN_LOGE("RSSurfaceNode::Marshalling failed");
    }
    return flag;
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
    RS_LOGI("RSSurfaceNode::Unmarshalling, Node: %{public}" PRIu64 ", Name: %{public}s", id, name.c_str());

    if (auto prevNode = RSNodeMap::Instance().GetNode(id)) { // Planning
        RS_LOGW("RSSurfaceNode::Unmarshalling, the node id is already in the map");
        // if the node id is already in the map, we should not create a new node
        return prevNode->ReinterpretCastTo<RSSurfaceNode>();
    }

    SharedPtr surfaceNode(new RSSurfaceNode(config, isRenderServiceNode, id));
    RSNodeMap::MutableInstance().RegisterNode(surfaceNode); // Planning

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
        AddCommand(command, false);
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

bool RSSurfaceNode::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, SurfaceId surfaceId, bool unobscured)
{
    if (surfaceId == 0) {
        surface_ = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())->
        CreateNodeAndSurface(config, unobscured);
    } else {
#ifndef ROSEN_CROSS_PLATFORM
        sptr<Surface> surface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);
        if (surface == nullptr) {
            ROSEN_LOGE("RSSurfaceNode::CreateNodeAndSurface nodeId is %{public}" PRIu64
                       " cannot find surface by surfaceId %{public}" PRIu64 "",
                GetId(), surfaceId);
            return false;
        }
        surface_ = std::static_pointer_cast<RSRenderServiceClient>(
            RSIRenderClient::CreateRenderServiceClient())->CreateRSSurface(surface);
        if (surface_ == nullptr) {
            ROSEN_LOGE(
                "RSSurfaceNode::CreateNodeAndSurface nodeId is %{public}" PRIu64 " creat RSSurface fail", GetId());
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
    std::unique_ptr<RSCommand> commandRS = std::make_unique<RSSurfaceNodeSetContextAlpha>(GetId(), 0.0f);
    AddCommand(commandRS, true);
}

void RSSurfaceNode::SetContainerWindow(bool hasContainerWindow, RRect rrect)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetContainerWindow>(GetId(), hasContainerWindow, rrect);
    AddCommand(command, true);
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSSurfaceNode::SetFreeze id:%llu", GetId());
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetFreeze>(GetId(), isFreeze);
    AddCommand(command, true);
}

std::pair<std::string, std::string> RSSurfaceNode::SplitSurfaceNodeName(std::string surfaceNodeName)
{
    if (auto position = surfaceNodeName.find("#");  position != std::string::npos) {
        return std::make_pair(surfaceNodeName.substr(0, position), surfaceNodeName.substr(position + 1));
    }
    return std::make_pair("", surfaceNodeName);
}

RSSurfaceNode::RSSurfaceNode(
    const RSSurfaceNodeConfig& config, bool isRenderServiceNode, std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, config.isTextureExportNode, rsUIContext, true), name_(config.SurfaceNodeName)
{}

RSSurfaceNode::RSSurfaceNode(
    const RSSurfaceNodeConfig& config, bool isRenderServiceNode, NodeId id, std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, id, config.isTextureExportNode, rsUIContext, true), name_(config.SurfaceNodeName)
{}

RSSurfaceNode::~RSSurfaceNode()
{
    RS_LOGI("RSSurfaceNode::~RSSurfaceNode, Node: %{public}" PRIu64 ", Name: %{public}s", GetId(), GetName().c_str());
    // both divided and unirender need to unregister listener when surfaceNode destroy
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        renderServiceClient->UnregisterBufferAvailableListener(GetId());
#ifdef USE_VIDEO_PROCESSING_ENGINE
        RSVpeManager::GetInstance().ReleaseVpeVideo(GetId());
#endif
    }

    // For abilityComponent and remote window, we should destroy the corresponding render node in RenderThread
    // The destructor of render node in RenderService should controlled by application
    // Command sent only in divided render
    if (skipDestroyCommandInDestructor_ && !IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(GetId());
        AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, GetId());
        return;
    }

    // For self-drawing surfaceNode, we should destroy the corresponding render node in RenderService
    // Command sent only in divided render
    if (!IsRenderServiceNode()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(GetId());
        AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, GetId());
        return;
    }
}

void RSSurfaceNode::AttachToDisplay(uint64_t screenId)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeAttachToDisplay>(GetId(), screenId);
    if (AddCommand(command, IsRenderServiceNode())) {
        RS_LOGI("RSSurfaceNode:attach to display, node:[name: %{public}s, id: %{public}" PRIu64 "], "
            "screen id: %{public}" PRIu64, GetName().c_str(), GetId(), screenId);
        RS_TRACE_NAME_FMT("RSSurfaceNode:attach to display, node:[name: %s, id: %" PRIu64 "], "
            "screen id: %" PRIu64, GetName().c_str(), GetId(), screenId);
    }
}

void RSSurfaceNode::DetachToDisplay(uint64_t screenId)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeDetachToDisplay>(GetId(), screenId);
    if (AddCommand(command, IsRenderServiceNode())) {
        RS_LOGI("RSSurfaceNode:detach from display, node:[name: %{public}s, id: %{public}" PRIu64 "], "
                "screen id: %{public}" PRIu64,
            GetName().c_str(), GetId(), screenId);
        RS_TRACE_NAME_FMT("RSSurfaceNode:detach from display, node:[name: %s, id: %" PRIu64 "], "
                          "screen id: %" PRIu64,
            GetName().c_str(), GetId(), screenId);
    }
}

void RSSurfaceNode::SetHardwareEnabled(bool isEnabled, SelfDrawingNodeType selfDrawingType, bool dynamicHardwareEnable)
{
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        renderServiceClient->SetHardwareEnabled(GetId(), isEnabled, selfDrawingType, dynamicHardwareEnable);
    }
}

/**
 * @brief Enable Camera Rotation Unchanged
 *
 * @param flag If flag is set to true, the camera fix rotation is enabled.
 * @return void
 */
void RSSurfaceNode::SetForceHardwareAndFixRotation(bool flag)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetHardwareAndFixRotation>(GetId(), flag);
    AddCommand(command, true);
}

void RSSurfaceNode::SetBootAnimation(bool isBootAnimation)
{
    isBootAnimation_ = isBootAnimation;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetBootAnimation>(GetId(), isBootAnimation);
    AddCommand(command, true);
    ROSEN_LOGD("RSSurfaceNode::SetBootAnimation, surfaceNodeId:[%" PRIu64 "] isBootAnimation:%s",
        GetId(), isBootAnimation ? "true" : "false");
}

bool RSSurfaceNode::GetBootAnimation() const
{
    return isBootAnimation_;
}

void RSSurfaceNode::SetGlobalPositionEnabled(bool isEnabled)
{
    if (isGlobalPositionEnabled_ == isEnabled) {
        return;
    }

    isGlobalPositionEnabled_ = isEnabled;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetGlobalPositionEnabled>(GetId(), isEnabled);
    AddCommand(command, true);
    ROSEN_LOGI("RSSurfaceNode::SetGlobalPositionEnabled, surfaceNodeId:[%{public}" PRIu64 "] isEnabled:%{public}s",
        GetId(), isEnabled ? "true" : "false");
}

bool RSSurfaceNode::GetGlobalPositionEnabled() const
{
    return isGlobalPositionEnabled_;
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
#ifdef ROSEN_IOS
    if (texture->GetSurfaceExtConfig().additionalData == nullptr) {
        texture->UpdateSurfaceExtConfig(config);
    }
#endif
    ROSEN_LOGD("RSSurfaceNode::CreateSurfaceExt %{public}" PRIu64 " type %{public}u", GetId(), config.type);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeCreateSurfaceExt>(GetId(), texture);
    AddCommand(command, false);
}

void RSSurfaceNode::SetSurfaceTexture(const RSSurfaceExtConfig& config)
{
    CreateSurfaceExt(config);
}

void RSSurfaceNode::MarkUiFrameAvailable(bool available)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetIsNotifyUIBufferAvailable>(GetId(), available);
    auto transaction = GetRSTransaction();
    if (transaction != nullptr) {
        transaction->AddCommand(command, false);
        transaction->FlushImplicitTransaction();
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, false);
            transactionProxy->FlushImplicitTransaction();
        }
    }
}

void RSSurfaceNode::SetSurfaceTextureAttachCallBack(const RSSurfaceTextureAttachCallBack& attachCallback)
{
#if defined(ROSEN_IOS)
    RSSurfaceTextureConfig config = {
        .type = RSSurfaceExtType::SURFACE_PLATFORM_TEXTURE,
    };
    auto texture = surface_->GetSurfaceExt(config);
    if (texture) {
        texture->SetAttachCallback(attachCallback);
    }
#else
    RSSurfaceTextureConfig config = {
        .type = RSSurfaceExtType::SURFACE_TEXTURE,
    };
    auto texture = surface_->GetSurfaceExt(config);
    if (texture) {
        texture->SetAttachCallback(attachCallback);
    }
#endif // ROSEN_IOS
}

void RSSurfaceNode::SetSurfaceTextureUpdateCallBack(const RSSurfaceTextureUpdateCallBack& updateCallback)
{
#if defined(ROSEN_IOS)
    RSSurfaceTextureConfig config = {
        .type = RSSurfaceExtType::SURFACE_PLATFORM_TEXTURE,
    };
    auto texture = surface_->GetSurfaceExt(config);
    if (texture) {
        texture->SetUpdateCallback(updateCallback);
    }
#else
    RSSurfaceTextureConfig config = {
        .type = RSSurfaceExtType::SURFACE_TEXTURE,
        .additionalData = nullptr
    };
    auto texture = surface_->GetSurfaceExt(config);
    if (texture) {
        texture->SetUpdateCallback(updateCallback);
    }
#endif // ROSEN_IOS
}

void RSSurfaceNode::SetSurfaceTextureInitTypeCallBack(const RSSurfaceTextureInitTypeCallBack& initTypeCallback)
{
#if defined(ROSEN_IOS)
    RSSurfaceTextureConfig config = {
        .type = RSSurfaceExtType::SURFACE_PLATFORM_TEXTURE,
    };
    auto texture = surface_->GetSurfaceExt(config);
    if (texture) {
        texture->SetInitTypeCallback(initTypeCallback);
    }
#endif // ROSEN_IOS
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
    AddCommand(commandRS, true);
    AddCommand(commandRT, false);
}

void RSSurfaceNode::SetClonedNodeInfo(NodeId nodeId, bool needOffscreen)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetClonedNodeId>(GetId(), nodeId, needOffscreen);
    AddCommand(command, true);
}

void RSSurfaceNode::SetForceUIFirst(bool forceUIFirst)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetForceUIFirst>(GetId(), forceUIFirst);
    AddCommand(command, true);
}

void RSSurfaceNode::SetAncoFlags(uint32_t flags)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetAncoFlags>(GetId(), flags);
    AddCommand(command, true);
}
void RSSurfaceNode::SetHDRPresent(bool hdrPresent, NodeId id)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetHDRPresent>(id, hdrPresent);
    ROSEN_LOGD("SetHDRPresent  RSSurfaceNode");
    AddCommand(command, true);
}

void RSSurfaceNode::SetSkipDraw(bool skip)
{
    isSkipDraw_ = skip;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetSkipDraw>(GetId(), skip);
    AddCommand(command, true);
    ROSEN_LOGD("RSSurfaceNode::SetSkipDraw, surfaceNodeId:[%" PRIu64 "] skipdraw:%s", GetId(),
        skip ? "true" : "false");
}

bool RSSurfaceNode::GetSkipDraw() const
{
    return isSkipDraw_;
}

void RSSurfaceNode::RegisterNodeMap()
{
    auto rsContext = GetRSUIContext();
    if (rsContext == nullptr) {
        return;
    }
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.RegisterNode(shared_from_this());
}

void RSSurfaceNode::SetWatermarkEnabled(const std::string& name, bool isEnabled)
{
#ifdef ROSEN_OHOS
    if (name.empty() || name.length() > WATERMARK_NAME_LENGTH_LIMIT) {
        ROSEN_LOGE("SetWatermarkEnabled name[%{public}s] is error.", name.c_str());
        return;
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetWatermarkEnabled>(GetId(), name, isEnabled);
    ROSEN_LOGI("SetWatermarkEnabled[%{public}s, %{public}" PRIu64 " watermark:%{public}s]",
        GetName().c_str(), GetId(), name.c_str());
    AddCommand(command, true);
#endif
}

void RSSurfaceNode::SetAbilityState(RSSurfaceNodeAbilityState abilityState)
{
    if (abilityState_ == abilityState) {
        ROSEN_LOGD("RSSurfaceNode::SetAbilityState, surfaceNodeId:[%{public}" PRIu64 "], "
            "ability state same with before: %{public}s",
            GetId(), abilityState == RSSurfaceNodeAbilityState::FOREGROUND ? "foreground" : "background");
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetAbilityState>(GetId(), abilityState);
    AddCommand(command, true);
    abilityState_ = abilityState;
    ROSEN_LOGI("RSSurfaceNode::SetAbilityState, surfaceNodeId:[%{public}" PRIu64 "], ability state: %{public}s",
        GetId(), abilityState_ == RSSurfaceNodeAbilityState::FOREGROUND ? "foreground" : "background");
}

RSSurfaceNodeAbilityState RSSurfaceNode::GetAbilityState() const
{
    return abilityState_;
}

RSInterfaceErrorCode RSSurfaceNode::SetHidePrivacyContent(bool needHidePrivacyContent)
{
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        return static_cast<RSInterfaceErrorCode>(
            renderServiceClient->SetHidePrivacyContent(GetId(), needHidePrivacyContent));
    }
    return RSInterfaceErrorCode::UNKNOWN_ERROR;
}

void RSSurfaceNode::SetHardwareEnableHint(bool enable)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetHardwareEnableHint>(GetId(), enable);
    AddCommand(command, true);
}

void RSSurfaceNode::SetApiCompatibleVersion(uint32_t version)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetApiCompatibleVersion>(GetId(), version);
    AddCommand(command, true);
    RS_LOGD("RSSurfaceNode::SetApiCompatibleVersion: Node: %{public}" PRIu64 ", version: %{public}u", GetId(), version);
}

void RSSurfaceNode::SetSourceVirtualDisplayId(ScreenId screenId)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetSourceVirtualDisplayId>(GetId(), screenId);
    AddCommand(command, true);
}

void RSSurfaceNode::AttachToWindowContainer(ScreenId screenId)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeAttachToWindowContainer>(GetId(), screenId);
    AddCommand(command, true);
    RS_LOGD("RSSurfaceNode::AttachToWindowContainer: Node: %{public}" PRIu64 ", screenId: %{public}" PRIu64,
        GetId(), screenId);
}

void RSSurfaceNode::DetachFromWindowContainer(ScreenId screenId)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeDetachFromWindowContainer>(GetId(), screenId);
    AddCommand(command, true);
    RS_LOGD("RSSurfaceNode::DetachFromWindowContainer: Node: %{public}" PRIu64 ", screenId: %{public}" PRIu64,
        GetId(), screenId);
}

void RSSurfaceNode::SetRegionToBeMagnified(const Vector4f& regionToBeMagnified)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetRegionToBeMagnified>(GetId(), regionToBeMagnified);
    AddCommand(command, true);
    RS_LOGI_LIMIT("RSSurfaceNode::SetRegionToBeMagnified, regionToBeMagnified left=%f, top=%f, width=%f, hight=%f",
        regionToBeMagnified.x_, regionToBeMagnified.y_, regionToBeMagnified.z_, regionToBeMagnified.w_);
}

void RSSurfaceNode::SetFrameGravityNewVersionEnabled(bool isEnabled)
{
    if (isFrameGravityNewVersionEnabled_ == isEnabled) {
        return;
    }

    isFrameGravityNewVersionEnabled_ = isEnabled;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetFrameGravityNewVersionEnabled>(GetId(), isEnabled);
    AddCommand(command, true);
}

bool RSSurfaceNode::GetFrameGravityNewVersionEnabled() const
{
    return isFrameGravityNewVersionEnabled_;
}
} // namespace Rosen
} // namespace OHOS
