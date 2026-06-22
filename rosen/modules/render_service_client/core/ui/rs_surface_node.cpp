
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
#include "command_modifier/rs_node_command_modifier.h"
#include "command_modifier/rs_surface_node_command_modifier.h"
#include "common/rs_optional_trace.h"
#include "ipc_callbacks/rs_rt_refresh_callback.h"
#include "modifier_ng/shadow_modifier/rs_bounds_shadow_modifier.h"
#include "modifier_ng/shadow_modifier/rs_frame_shadow_modifier.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "sandbox_utils.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "platform/drawing/rs_surface_converter.h"
#endif
#ifdef RS_ENABLE_GPU
#include "render_context/render_context.h"
#endif
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "feature/composite_layer/rs_composite_layer_utils.h"
#include "feature/delegate_composite/rs_delegate_composite_buffer_manager.h"
#include "rs_trace.h"
#include "ui/rs_proxy_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"
#include "transaction/rs_interfaces.h"

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
constexpr size_t MAX_CACHE_SIZE = 16;
}
#endif

RSSurfaceNode::SharedPtr RSSurfaceNode::Create(
    const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow, std::shared_ptr<RSUIContext> rsUIContext)
{
    if (!isWindow) {
        return Create(surfaceNodeConfig, RSSurfaceNodeType::SELF_DRAWING_NODE, isWindow, false, rsUIContext);
    }
    return Create(surfaceNodeConfig, RSSurfaceNodeType::DEFAULT, isWindow, false, rsUIContext);
}

RSSurfaceNode::SharedPtr RSSurfaceNode::CreateSurfaceNode(const RSSurfaceNodeConfig &surfaceNodeConfig, bool isWindow)
{
    SharedPtr node(new RSSurfaceNode(surfaceNodeConfig, isWindow));
    return node;
}

bool RSSurfaceNode::SendDataToRender(const RSSurfaceNodeConfig& surfaceNodeConfig,
    RSSurfaceNodeType type, bool isWindow, bool unobscured)
{
    auto rsUIContext = GetRSUIContext();
    if (rsUIContext == nullptr) {
        ROSEN_LOGE("RSSurfaceNode::SendDataToRender rsUIContext is nullptr");
        return false;
    }
    rsUIContext->GetMutableNodeMap().RegisterNode(shared_from_this());
    SetSkipCheckInMultiInstance(surfaceNodeConfig.isSkipCheckInMultiInstance);

    // create node in RS
    RSSurfaceRenderNodeConfig config = {
        .id = GetId(),
        .name = name_,
        .additionalData = surfaceNodeConfig.additionalData,
        .isTextureExportNode = surfaceNodeConfig.isTextureExportNode,
        .isSync = surfaceNodeConfig.isSync,
        .surfaceWindowType = surfaceNodeConfig.surfaceWindowType,
    };
    if (!isWindow) {
        config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    } else {
        config.nodeType = type;
    }
    surfaceNodeType_ = config.nodeType;
    RS_TRACE_NAME_FMT("RSSurfaceNode::SendDataToRender name: %s type: %hhu, id: %lu, token:%lu", name_.c_str(),
        config.nodeType, GetId(), rsUIContext ? rsUIContext->GetToken() : 0);
    RS_LOGD("RSSurfaceNode::SendDataToRender name: %{public}s type: %{public}hhu, id: %{public}" PRIu64, name_.c_str(),
        config.nodeType, GetId());
    if (type == RSSurfaceNodeType::LEASH_WINDOW_NODE && IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreateWithConfig>(
            config.id, config.name, static_cast<uint8_t>(config.nodeType), config.surfaceWindowType);
        AddCommand(command, isWindow);
    } else {
#ifndef SCREENLESS_DEVICE
        if (!CreateNodeAndSurface(config, surfaceNodeConfig.surfaceId, unobscured)) {
            ROSEN_LOGE("RSSurfaceNode::SendDataToRender, create node and surface failed");
            return false;
        }
#endif
    }

    SetClipToFrame(true);
    // create node in RT (only when in divided render and isRenderServiceNode_ == false)
    // create node in RT if is TextureExport node
    if (!IsRenderServiceNode()) {
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSSurfaceNodeCreate>(GetId(), config.nodeType, surfaceNodeConfig.isTextureExportNode);
        if (surfaceNodeConfig.isTextureExportNode) {
            AddCommand(command, false);
            SetSurfaceIdToRenderNode();
        } else {
            AddCommand(command, isWindow);
        }
#ifdef ROSEN_OHOS
        if (rsUIContext != nullptr) {
            command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(
                GetId(), rsUIContext->GetConnectToRender());
            AddCommand(command, isWindow);
        } else {
            ROSEN_LOGE("RSSurfaceNode::SendDataToRender,"
                "RSSurfaceNodeConnectToNodeInRenderService rsUIContext is nullptr");
        }
#endif

        RSRTRefreshCallback::Instance().SetRefresh([] { RSRenderThread::Instance().RequestNextVSync(); });
        command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(GetId(), true);
        AddCommand(command, isWindow);
        SetFrameGravity(Gravity::RESIZE);
        // codes for arkui-x
#if defined(USE_SURFACE_TEXTURE) && defined(ROSEN_ANDROID) && !defined(SCREENLESS_DEVICE)
        if (type == RSSurfaceNodeType::SURFACE_TEXTURE_NODE) {
            RSSurfaceExtConfig config = {
                .type = RSSurfaceExtType::SURFACE_TEXTURE,
                .additionalData = nullptr,
            };
            CreateSurfaceExt(config);
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
            CreateSurfaceExt(config);
        }
#endif
    }
    if (GetName().find("battery_panel") != std::string::npos ||
        GetName().find("sound_panel") != std::string::npos ||
        GetName().find("RosenWeb") != std::string::npos) {
        SetFrameGravity(Gravity::TOP_LEFT);
    } else if (!isWindow) {
        SetFrameGravity(Gravity::RESIZE);
    }
    ROSEN_LOGI("RsDebug RSSurfaceNode::SendDataToRender id:%{public}" PRIu64, GetId());
    SetUIContextToken();
    return true;
}

RSSurfaceNode::SharedPtr RSSurfaceNode::Create(const RSSurfaceNodeConfig& surfaceNodeConfig,
    RSSurfaceNodeType type, bool isWindow, bool unobscured, std::shared_ptr<RSUIContext> rsUIContext)
{
    SharedPtr node;
    if (surfaceNodeConfig.nodeId == 0) {
        node.reset(new RSSurfaceNode(surfaceNodeConfig, isWindow, rsUIContext));
    } else {
        node.reset(new RSSurfaceNode(surfaceNodeConfig, isWindow, surfaceNodeConfig.nodeId, rsUIContext));
        node->skipDestroyCommandInDestructor_ = true;
    }
    if (rsUIContext != nullptr) {
        rsUIContext->GetMutableNodeMap().RegisterNode(node);
    } else {
        RSNodeMap::MutableInstance().RegisterNode(node);
    }
    node->SetSkipCheckInMultiInstance(surfaceNodeConfig.isSkipCheckInMultiInstance);

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
    node->surfaceNodeType_ = config.nodeType;
    node->creationConfig_ = config;
    node->creationSurfaceId_ = surfaceNodeConfig.surfaceId;
    node->creationType_ = type;
    node->unobscured_ = unobscured;

    RS_TRACE_NAME_FMT("RSSurfaceNode::Create name: %s type: %hhu, id: %lu, token:%lu", node->name_.c_str(),
        config.nodeType, node->GetId(), rsUIContext ? rsUIContext->GetToken() : 0);
    RS_LOGI("RSSurfaceNode::Create name:%{public}s type: %{public}hhu id %{public}" PRIu64, node->name_.c_str(),
        config.nodeType, node->GetId());

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
        node->CreateRenderThreadNode(type, isWindow);
    }

    if (node->GetName().find("battery_panel") != std::string::npos ||
        node->GetName().find("sound_panel") != std::string::npos ||
        node->GetName().find("RosenWeb") != std::string::npos) {
        node->SetFrameGravity(Gravity::TOP_LEFT);
    } else if (!isWindow) {
        node->SetFrameGravity(Gravity::RESIZE);
    }
    ROSEN_LOGI("RsDebug RSSurfaceNode::Create id:%{public}" PRIu64, node->GetId());
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

#ifdef ROSEN_OHOS
    command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(
        GetId(), GetRSUIContext()->GetConnectToRender());
    AddCommand(command, false);
#endif
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

void RSSurfaceNode::OnBoundsSizeChanged()
{
    auto bounds = GetStagingProperties().GetBounds();
    SetRSCmdProperty<SurfaceDefaultSizeCmdModifier>(SurfaceDefaultSizeCmdParam{
        bounds.z_, bounds.w_
    });
    RS_TRACE_NAME_FMT("OnBoundsSizeChanged, node:%" PRIu64 ", width:%f, height:%f", GetId(), bounds.z_, bounds.w_);
    std::lock_guard<std::mutex> lock(mutex_);
    if (boundsChangedCallback_) {
        boundsChangedCallback_(bounds);
        RS_TRACE_NAME_FMT("node:[name: %s, id: %" PRIu64 ", bounds:%f %f %f %f] already callback",
            GetName().c_str(), GetId(), bounds.x_, bounds.y_, bounds.z_, bounds.w_);
    }
}

void RSSurfaceNode::OnAlphaValueChanged() const
{
    auto alpha = GetStagingProperties().GetAlpha();
    ROSEN_LOGI("RSSurfaceNode::OnAlphaValueChanged, node=[%{public}" PRIu64 ", %{public}s], alpha=%{public}f",
        GetId(), GetName().c_str(), alpha);
    std::lock_guard<std::mutex> lock(mutex_);
    if (alphaChangedCallback_) {
        alphaChangedCallback_(alpha);
    }
}

void RSSurfaceNode::SetLeashPersistentId(LeashPersistentId leashPersistentId)
{
    leashPersistentId_ = leashPersistentId;
    SetRSCmdProperty<LeashPersistentIdCmdModifier>(LeashPersistentIdCmdParam{
        leashPersistentId
    });
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
    SetRSCmdProperty<SecurityLayerCmdModifier>(SecurityLayerCmdParam{
        isSecurityLayer
    });
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
    SetRSCmdProperty<SkipLayerCmdModifier>(SkipLayerCmdParam{
        isSkipLayer
    });
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
    SetRSCmdProperty<SnapshotSkipLayerCmdModifier>(SnapshotSkipLayerCmdParam{
        isSnapshotSkipLayer
    });
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
    SetRSCmdProperty<HasFingerprintCmdModifier>(HasFingerprintCmdParam{
        hasFingerprint
    });
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
    SetRSCmdProperty<ColorSpaceCmdModifier>(ColorSpaceCmdParam{
        colorSpace
    });
}

void RSSurfaceNode::CreateRenderNodeForTextureExportSwitch()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(GetId(),
        RSSurfaceNodeType::SELF_DRAWING_NODE, isTextureExportNode_);
    AddCommand(command, IsRenderServiceNode());
    if (!IsRenderServiceNode()) {
        hasCreateRenderNodeInRT_ = true;
        command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(
            GetId(), GetRSUIContext()->GetConnectToRender());
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
    SetRSCmdProperty<AbilityBGAlphaCmdModifier>(AbilityBGAlphaCmdParam{
        alpha
    });
}

void RSSurfaceNode::SetIsNotifyUIBufferAvailable(bool available)
{
    SetRSCmdProperty<NotifyUIBufferAvailableCmdModifier>(NotifyUIBufferAvailableCmdParam{
        available
    });
}

bool RSSurfaceNode::SetBufferAvailableCallback(BufferAvailableCallback callback)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        callback_ = callback;
    }
    auto result = SetRSCmdPropertyWithResult<BufferAvailableCallbackCmdModifier>(BufferAvailableCallbackCmdParam{
        BufferAvailableCallbackFunc()
    });
    return std::holds_alternative<bool>(result) ? std::get<bool>(result) : false;
}

RSSurfaceNode::BufferAvailableCallback RSSurfaceNode::BufferAvailableCallbackFunc()
{
    return [weakThis = weak_from_this()]() {
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
    };
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

void RSSurfaceNode::SetAlphaChangedCallback(AlphaChangedCallback&& callback)
{
    ROSEN_LOGI("RSSurfaceNode::SetAlphaChangedCallback, node=[%{public}" PRIu64 ", %{public}s]",
        GetId(), GetName().c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    alphaChangedCallback_ = callback;
}

bool RSSurfaceNode::Marshalling(Parcel& parcel) const
{
    bool flag = parcel.WriteUint64(GetId()) && parcel.WriteString(name_) && parcel.WriteBool(IsRenderServiceNode());
    if (!flag) {
        ROSEN_LOGE("RSSurfaceNode::Marshalling failed");
    }
    return flag;
}

std::shared_ptr<RSSurfaceNode> RSSurfaceNode::Unmarshalling(Parcel& parcel, bool skip)
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
        RS_LOGD("RSSurfaceNode::Unmarshalling, the node id is already in the map");
        // if the node id is already in the map, we should not create a new node
        return prevNode->ReinterpretCastTo<RSSurfaceNode>();
    }

    SharedPtr surfaceNode(new RSSurfaceNode(config, isRenderServiceNode, id));
    RSNodeMap::MutableInstance().RegisterNode(surfaceNode);

    // for nodes constructed by unmarshalling, we should not destroy the corresponding render node on destruction
    if (skip) {
        surfaceNode->skipDestroyCommandInDestructor_ = true;
    }

    return surfaceNode;
}

RSSurfaceNode::SharedPtr RSSurfaceNode::CreateShadowSurfaceNode(const std::set<ShadowPropertyType>& shadowPropertyTypes)
{
    bool hasShadowProperty = !shadowPropertyTypes.empty();
    if (hasShadowProperty && modifiersNGCreatedBySetter_.empty()) {
        RS_LOGE("RSSurfaceNode::CreateShadowSurfaceNode, no source modifiers, nodeId=%{public}" PRIu64, GetId());
        return nullptr;
    }

    RSSurfaceNodeConfig config = { GetName() };
    SharedPtr shadowNode(new RSSurfaceNode(config, isRenderServiceNode_, GetId()));
    sptr<IRemoteObject> connectToRender;
    auto preContext = GetRSUIContext();
    if (preContext) {
        connectToRender = preContext->GetConnectToRender();
    }
    auto rsUIContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRender);
    shadowNode->SetRSUIContext(rsUIContext);
    shadowNode->isShadowNode_ = true;
    shadowNode->SetSkipCheckInMultiInstance(true);
    shadowNode->skipDestroyCommandInDestructor_ = true;
    if (!hasShadowProperty) {
        RS_LOGW("RSSurfaceNode::CreateShadowSurfaceNode, shadowPropertyTypes empty, nodeId=%{public}" PRIu64, GetId());
        return shadowNode;
    }
    if (!InitShadowModifiers(shadowNode, shadowPropertyTypes)) {
        return nullptr;
    }

    shadowNode->existsDuplicateModifier_ = true;
    existsDuplicateModifier_ = true;
    return shadowNode;
}

bool RSSurfaceNode::InitShadowModifiers(SharedPtr shadowNode, const std::set<ShadowPropertyType>& shadowPropertyTypes)
{
    std::map<ModifierId, std::shared_ptr<ModifierNG::RSModifier>> modifierMap;
    for (const auto& type : shadowPropertyTypes) {
        std::shared_ptr<ModifierNG::RSModifier> modifier = nullptr;
        switch (type) {
            case ShadowPropertyType::BOUNDS:
                modifier = CreateShadowModifierAndProperty<ModifierNG::RSBoundsShadowModifier, Vector4f>(
                    shadowNode, ModifierNG::RSPropertyType::BOUNDS);
                break;
            case ShadowPropertyType::FRAME:
                modifier = CreateShadowModifierAndProperty<ModifierNG::RSFrameShadowModifier, Vector4f>(
                    shadowNode, ModifierNG::RSPropertyType::FRAME);
                break;
            default:
                break;
        }

        if (modifier != nullptr) {
            modifierMap.emplace(modifier->GetId(), modifier);
        }
    }

    if (modifierMap.empty()) {
        return false;
    }

    for (auto& [_, modifier] : modifierMap) {
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSAddModifierNG>(shadowNode->GetId(), modifier->CreateRenderModifier());
        shadowNode->AddCommand(
            command, shadowNode->IsRenderServiceNode(), shadowNode->GetFollowType(), shadowNode->GetId());
    }
    return true;
}

template<typename Modifier, typename ValueType>
std::shared_ptr<ModifierNG::RSModifier> RSSurfaceNode::CreateShadowModifierAndProperty(
    SharedPtr shadowNode, ModifierNG::RSPropertyType propertyType)
{
    auto srcModifier = GetModifierCreatedBySetter(Modifier::Type);
    if (srcModifier == nullptr) {
        RS_LOGE("RSSurfaceNode::CreateShadowModifierAndProperty, no source modifier, nodeId=%{public}" PRIu64
            ", propertyType=%{public}hu", GetId(), propertyType);
        return nullptr;
    }
    auto srcProperty = srcModifier->GetProperty(propertyType);
    if (srcProperty == nullptr) {
        RS_LOGE("RSSurfaceNode::CreateShadowModifierAndProperty, no source property, nodeId=%{public}" PRIu64
            ", propertyType=%{public}hu", GetId(), propertyType);
        return nullptr;
    }

    auto shadowModifier = shadowNode->GetModifierCreatedBySetter(Modifier::Type);
    if (shadowModifier == nullptr) {
        shadowModifier = std::make_shared<Modifier>();
        shadowModifier->id_ = srcModifier->GetId();
        shadowModifier->OnAttach(*shadowNode);
        std::unique_lock<std::recursive_mutex> lock(shadowNode->propertyMutex_);
        shadowNode->modifiersNG_.emplace(shadowModifier->GetId(), shadowModifier);
        shadowNode->modifiersNGCreatedBySetter_.emplace(shadowModifier->GetType(), shadowModifier);
    }
    auto shadowProperty =
        std::make_shared<RSProperty<ValueType>>(std::static_pointer_cast<RSProperty<ValueType>>(srcProperty)->Get());
    shadowProperty->id_ = srcProperty->GetId();
    shadowModifier->AttachProperty(propertyType, shadowProperty);
    return shadowModifier;
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
    auto rsUIContext = GetRSUIContext();
    if (rsUIContext == nullptr || rsUIContext->GetRSRenderInterface() == nullptr) {
        ROSEN_LOGE("RSDisplayNode::CreateNode uiContext is nullptr");
        return false;
    }
    return rsUIContext->GetRSRenderInterface()->CreateNode(config);
}

bool RSSurfaceNode::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, SurfaceId surfaceId, bool unobscured)
{
    if (surfaceId == 0) {
        auto rsUIContext = GetRSUIContext();
        std::shared_ptr<RSSurface> surface = nullptr;
        if (rsUIContext == nullptr || rsUIContext->GetRSRenderInterface() == nullptr) {
            ROSEN_LOGW("RSSurfaceNode::CreateNode uiContext is nullptr");
            std::shared_ptr<RSRenderInterface> rsRenderInterface =
                std::shared_ptr<RSRenderInterface>(new RSRenderInterface());
            surface = rsRenderInterface->CreateNodeAndSurface(config, unobscured);
        } else {
            surface = rsUIContext->GetRSRenderInterface()->CreateNodeAndSurface(config, unobscured);
        }
        if (GetNodeState() == RSNodeState::ACTIVE) {
            surface_ = surface;
        }
    } else {
#ifndef ROSEN_CROSS_PLATFORM
        sptr<Surface> surface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);
        if (surface == nullptr) {
            ROSEN_LOGE("RSSurfaceNode::CreateNodeAndSurface nodeId is %{public}" PRIu64
                       " cannot find surface by surfaceId %{public}" PRIu64 "",
                GetId(), surfaceId);
            return false;
        }
        surface_ = std::static_pointer_cast<RSRenderPipelineClient>(
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
    SetRSCmdProperty<ContainerWindowCmdModifier>(ContainerWindowCmdParam{
        hasContainerWindow, rrect
    });
}

void RSSurfaceNode::SetWindowId(uint32_t windowId)
{
    windowId_ = windowId;
}

void RSSurfaceNode::SetFreeze(bool isFreeze, bool isMarkedByUI)
{
    if (!IsUniRenderEnabled()) {
        ROSEN_LOGE("SetFreeze is not supported in separate render");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSSurfaceNode::SetFreeze id:%llu", GetId());
    SetRSCmdProperty<FreezeCmdModifier>(FreezeCmdParam{
        isFreeze, isMarkedByUI
    });
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
#ifdef ROSEN_ARKUI_X
    RSRenderThread::Instance().PostTask([surface = std::move(surface_)]() {});
#endif
    if (isShadowNode_) {
        auto rsUIContext = GetRSUIContext();
        if (rsUIContext) {
            RSUIContextManager::MutableInstance().DestroyContext(rsUIContext->GetToken());
        }
        return;
    }
    RS_LOGI("RSSurfaceNode::~RSSurfaceNode, Node: %{public}" PRIu64 ", Name: %{public}s", GetId(), GetName().c_str());
    // both divided and unirender need to unregister listener when surfaceNode destroy
#ifndef ROSEN_CROSS_PLATFORM
    delegateCompositeBufMgr_ = nullptr;
#endif
    auto rsUIContext = GetRSUIContext();
    if (rsUIContext != nullptr && rsUIContext->GetRSRenderInterface() != nullptr) {
        rsUIContext->GetRSRenderInterface()->UnregisterBufferAvailableListener(GetId());
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
        HILOG_COMM_INFO("RSSurfaceNode:attach to display, node:[name: %{public}s, id: %{public}" PRIu64 "], "
            "screen id: %{public}" PRIu64, GetName().c_str(), GetId(), screenId);
        RS_TRACE_NAME_FMT("RSSurfaceNode:attach to display, node:[name: %s, id: %" PRIu64 "], "
            "screen id: %" PRIu64, GetName().c_str(), GetId(), screenId);
    }
}

void RSSurfaceNode::DetachToDisplay(uint64_t screenId)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeDetachToDisplay>(GetId(), screenId);
    if (AddCommand(command, IsRenderServiceNode())) {
        HILOG_COMM_INFO("RSSurfaceNode:detach from display, node:[name: %{public}s, id: %{public}" PRIu64 "], "
                "screen id: %{public}" PRIu64,
            GetName().c_str(), GetId(), screenId);
        RS_TRACE_NAME_FMT("RSSurfaceNode:detach from display, node:[name: %s, id: %" PRIu64 "], "
                          "screen id: %" PRIu64,
            GetName().c_str(), GetId(), screenId);
    }
}

void RSSurfaceNode::SetHardwareEnabled(bool isEnabled, SelfDrawingNodeType selfDrawingType, bool dynamicHardwareEnable)
{
    SetRSCmdPropertyWithResult<HardwareEnabledCmdModifier>(HardwareEnabledCmdParam{
        isEnabled, selfDrawingType, dynamicHardwareEnable
    });
}

void RSSurfaceNode::SetForceHardwareAndFixRotation(bool flag)
{
    SetRSCmdProperty<ForceHardwareAndFixRotationCmdModifier>(ForceHardwareAndFixRotationCmdParam{
        flag
    });
}

void RSSurfaceNode::SetBootAnimation(bool isBootAnimation)
{
    isBootAnimation_ = isBootAnimation;
    SetRSCmdProperty<BootAnimationCmdModifier>(BootAnimationCmdParam{
        isBootAnimation
    });
    ROSEN_LOGD("RSSurfaceNode::SetBootAnimation, surfaceNodeId:[%{public}" PRIu64 "] isBootAnimation:%s",
        GetId(), isBootAnimation ? "true" : "false");
}

bool RSSurfaceNode::GetBootAnimation() const
{
    return isBootAnimation_;
}

void RSSurfaceNode::SetGlobalPositionEnabled(bool isEnabled)
{
    if (isGlobalPositionEnabled_ == isEnabled && !existsDuplicateModifier_) {
        return;
    }

    isGlobalPositionEnabled_ = isEnabled;
    SetRSCmdProperty<GlobalPositionEnabledCmdModifier>(GlobalPositionEnabledCmdParam{
        isEnabled
    });
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
    texture->UpdateSurfaceExtConfig(config);
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
#else
    RSSurfaceTextureConfig config = {
        .type = RSSurfaceExtType::SURFACE_TEXTURE,
        .additionalData = nullptr
    };
    auto texture = surface_->GetSurfaceExt(config);
    if (texture) {
        texture->SetInitTypeCallback(initTypeCallback);
    }
#endif // ROSEN_IOS
}

void RSSurfaceNode::SetSurfaceCaptureCallback(std::function<std::shared_ptr<Media::PixelMap>()> callback)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeSetSurfaceCaptureCallBack>(GetId(), callback);
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

void RSSurfaceNode::SetClonedNodeInfo(NodeId nodeId, bool needOffscreen, bool isRelated)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        ROSEN_LOGD("RSSurfaceNode::SetClonedNodeInfo transactionProxy is null");
        return;
    }
    SetRSCmdProperty<ClonedNodeInfoCmdModifier>(ClonedNodeInfoCmdParam{
        nodeId, needOffscreen, isRelated
    });
}

void RSSurfaceNode::SetForceUIFirst(bool forceUIFirst)
{
    SetRSCmdProperty<ForceUIFirstCmdModifier>(ForceUIFirstCmdParam{
        forceUIFirst
    });
}

void RSSurfaceNode::SetAncoFlags(uint32_t flags)
{
    SetRSCmdProperty<AncoFlagsCmdModifier>(AncoFlagsCmdParam{
        flags
    });
}

void RSSurfaceNode::SetSkipDraw(bool skip)
{
    if (isSkipDraw_ == skip) {
        return;
    }
    isSkipDraw_ = skip;
    SetRSCmdProperty<SkipDrawCmdModifier>(SkipDrawCmdParam{
        skip
    });
}

bool RSSurfaceNode::GetSkipDraw() const
{
    return isSkipDraw_;
}

void RSSurfaceNode::SetDarkColorMode(bool isDark)
{
    if (isDarkColorMode_ == isDark) {
        return;
    }
    isDarkColorMode_ = isDark;
    SetRSCmdProperty<DarkColorModeCmdModifier>(DarkColorModeCmdParam{
        isDark
    });
    ROSEN_LOGD("RSSurfaceNode::SetDarkColorMode, surfaceNodeId:[%" PRIu64 "] isDarkColorMode:%s", GetId(),
        isDark ? "true" : "false");
}

bool RSSurfaceNode::GetDarkColorMode() const
{
    return isDarkColorMode_;
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
    SetRSCmdProperty<WatermarkEnabledCmdModifier>(WatermarkEnabledCmdParam{
        name, isEnabled
    });
#endif
}

void RSSurfaceNode::SetAbilityState(RSSurfaceNodeAbilityState abilityState)
{
    if (abilityState_ == abilityState) {
        ROSEN_LOGD("RSSurfaceNode::SetAbilityState, surfaceNodeId:[%{public}" PRIu64 "], "
            "ability state same with before: %{public}s",
            GetId(), abilityState == RSSurfaceNodeAbilityState::FOREGROUND ? "foreground" : "background");
    }
    abilityState_ = abilityState;
    SetRSCmdProperty<AbilityStateCmdModifier>(AbilityStateCmdParam{
        abilityState
    });
}

RSSurfaceNodeAbilityState RSSurfaceNode::GetAbilityState() const
{
    return abilityState_;
}

RSInterfaceErrorCode RSSurfaceNode::SetHidePrivacyContent(bool needHidePrivacyContent)
{
    auto result = SetRSCmdPropertyWithResult<HidePrivacyContentCmdModifier>(HidePrivacyContentCmdParam{
        needHidePrivacyContent
    });
    return std::holds_alternative<RSInterfaceErrorCode>(result) ?
        std::get<RSInterfaceErrorCode>(result) : RSInterfaceErrorCode::UNKNOWN_ERROR;
}

void RSSurfaceNode::SetHardwareEnableHint(bool enable)
{
    SetRSCmdProperty<HardwareEnableHintCmdModifier>(HardwareEnableHintCmdParam{
        enable
    });
}

void RSSurfaceNode::SetApiCompatibleVersion(uint32_t version)
{
    SetRSCmdProperty<ApiCompatibleVersionCmdModifier>(ApiCompatibleVersionCmdParam{
        version
    });
}

void RSSurfaceNode::SetSourceVirtualDisplayId(ScreenId screenId)
{
    SetRSCmdProperty<VirtualDisplayIdCmdModifier>(VirtualDisplayIdCmdParam{
        screenId
    });
}

void RSSurfaceNode::AttachToWindowContainer(ScreenId screenId)
{
    SetRSCmdProperty<AttachToWindowContainerCmdModifier>(AttachToWindowContainerCmdParam{
        screenId
    });
}

void RSSurfaceNode::DetachFromWindowContainer(ScreenId screenId)
{
    SetRSCmdProperty<DetachFromWindowContainerCmdModifier>(DetachFromWindowContainerCmdParam{
        screenId
    });
}

void RSSurfaceNode::SetRegionToBeMagnified(const Vector4<int>& regionToBeMagnified)
{
    SetRSCmdProperty<RegionToBeMagnifiedCmdModifier>(RegionToBeMagnifiedCmdParam{
        regionToBeMagnified
    });
}

bool RSSurfaceNode::IsSelfDrawingNode() const
{
    if (surfaceNodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE) {
        return true;
    } else {
        return false;
    }
}

void RSSurfaceNode::MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (isNodeSingleFrameComposer_ != isNodeSingleFrameComposer) {
        isNodeSingleFrameComposer_ = isNodeSingleFrameComposer;
        SetRSCmdProperty<MarkNodeSingleFrameComposerCmdModifier>(MarkNodeSingleFrameComposerCmdParam{
            isNodeSingleFrameComposer, GetRealPid()
        });
    }
}

bool RSSurfaceNode::SetCompositeLayer(TopLayerZOrder zOrder)
{
    if (!RSSystemProperties::GetCompositeLayerEnabled()) {
        return false;
    }
    RS_TRACE_NAME_FMT("RSSurfaceNode::SetCompositeLayer %llu zOrder: %u", GetId(), zOrder);
    uint32_t topLayerZOrder = static_cast<uint32_t>(zOrder);
    if (IsSelfDrawingNode()) {
        RS_LOGI("RSSurfaceNode::SetCompositeLayer selfDrawingNode %{public}" PRIu64 " setLayerTop directly", GetId());
        SetRSCmdPropertyWithResult<CompositeLayerCmdModifier>(CompositeLayerCmdParam{
            true, topLayerZOrder
        });
        return true;
    }
    compositeLayerUtils_ = std::make_shared<RSCompositeLayerUtils>(shared_from_this(), topLayerZOrder);
    if (zOrder == TopLayerZOrder::CHARGE_3D_MOTION) {
        if (GetChildren().size() == 1) {
            bool ret = compositeLayerUtils_->DealWithSelfDrawCompositeNode(GetChildren()[0].lock(), topLayerZOrder);
            compositeLayerUtils_ = nullptr;
            return ret;
        }
        return false;
    }
    return compositeLayerUtils_->CreateCompositeLayer();
}

// LCOV_EXCL_START
void RSSurfaceNode::SetStaticCached(bool isFreeze)
{
    if (isStaticFreeze_ == isFreeze) {
        return;
    }
    isStaticFreeze_ = isFreeze;
    SetRSCmdProperty<StaticCachedCmdModifier>(StaticCachedCmdParam{
        isFreeze
    });
}

std::shared_ptr<RSCompositeLayerUtils> RSSurfaceNode::GetCompositeLayerUtils() const
{
    return compositeLayerUtils_;
}

void RSSurfaceNode::SetFrameGravityNewVersionEnabled(bool isEnabled)
{
    if (isFrameGravityNewVersionEnabled_ == isEnabled) {
        return;
    }

    isFrameGravityNewVersionEnabled_ = isEnabled;
    SetRSCmdProperty<FrameGravityNewVersionEnabledCmdModifier>(FrameGravityNewVersionEnabledCmdParam{
        isEnabled
    });
}

bool RSSurfaceNode::GetFrameGravityNewVersionEnabled() const
{
    return isFrameGravityNewVersionEnabled_;
}

void RSSurfaceNode::SetSurfaceBufferOpaque(bool isOpaque)
{
    if (isSurfaceBufferOpaque_ == isOpaque) {
        return;
    }

    isSurfaceBufferOpaque_ = isOpaque;
    SetRSCmdProperty<SurfaceBufferOpaqueCmdModifier>(SurfaceBufferOpaqueCmdParam{
        isOpaque
    });
}

void RSSurfaceNode::SetContainerWindowTransparent(bool isContainerWindowTransparent)
{
    SetRSCmdProperty<ContainerWindowTransparentCmdModifier>(ContainerWindowTransparentCmdParam{
        isContainerWindowTransparent
    });
}

void RSSurfaceNode::CreateRenderNode()
{
    const auto& node = this;
    RS_TRACE_NAME_FMT("RSSurfaceNode::CreateRenderNode name: %s type: %hhu, id: %lu, token:%lu", node->name_.c_str(),
        creationConfig_.nodeType, node->GetId(), GetRSUIContext() ? GetRSUIContext()->GetToken() : 0);
    RS_LOGD("RSSurfaceNode::CreateRenderNode name: %{public}s type: %{public}hhu, id: %{public}" PRIu64,
        node->name_.c_str(), creationConfig_.nodeType, node->GetId());
    
    bool isWindow = creationConfig_.nodeType == RSSurfaceNodeType::SELF_DRAWING_NODE;

    if (creationType_ == RSSurfaceNodeType::LEASH_WINDOW_NODE && node->IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreateWithConfig>(creationConfig_.id,
            creationConfig_.name, static_cast<uint8_t>(creationConfig_.nodeType), creationConfig_.surfaceWindowType);
        node->AddCommand(command, isWindow);
    } else {
#ifndef SCREENLESS_DEVICE
#ifndef ROSEN_CROSS_PLATFORM
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSSurfaceNodeRecreateNodeAndSurface>(creationConfig_, creationSurfaceId_, unobscured_);
        node->AddCommand(command, true);
#endif
#endif
    }

    if (!node->IsRenderServiceNode()) {
        node->CreateRenderThreadNode(creationType_, isWindow);
    }
}

bool RSSurfaceNode::SetNodeState(RSNodeState state)
{
    if (GetNodeState() == state) {
        return true;
    }
    return RSNode::SetNodeState(state);
}

void RSSurfaceNode::SetAppRotationCorrection(ScreenRotation appRotationCorrection)
{
    if (appRotationCorrection > ScreenRotation::INVALID_SCREEN_ROTATION) {
        RS_LOGE(
            "RSSurfaceNode::SetAppRotationCorrection %{public}" PRIu64 " set invalid AppRotationCorrection", GetId());
        return;
    }
    SetRSCmdProperty<AppRotationCorrectionCmdModifier>(AppRotationCorrectionCmdParam{
        appRotationCorrection
    });
    RS_LOGI("RSSurfaceNode::SetAppRotationCorrection: Node: %{public}" PRIu64 ", appRotationCorrection: %{public}u",
        GetId(), appRotationCorrection);
}

void RSSurfaceNode::SetHDRType(uint32_t hdrType)
{
    SetRSCmdProperty<HDRTypeCmdModifier>(HDRTypeCmdParam{
        hdrType
    });
}
void RSSurfaceNode::SetHDRBrightnessWithType(const float& hdrBrightness, uint32_t hdrType)
{
    RSNode::SetHDRBrightness(hdrBrightness);
    SetHDRType(hdrType);
    RS_LOGD("SurfaceNode::SetHDRBrightnessWithType set with hdrType:%{public}d", hdrType);
#ifdef USE_VIDEO_PROCESSING_ENGINE
    switch (static_cast<HDRType>(hdrType)) {
        case HDRType::DEFAULT: {
            RSSurfaceRenderNodeConfig config = {
                .id = GetId(),
                .name = GetName(),
            };
            RSVpeManager::GetInstance().DisableVpeVideo(config);
            break;
        }
        case HDRType::AIHDR: {
            RSSurfaceRenderNodeConfig config = {
                .id = GetId(),
                .name = GetName(),
            };
            RSVpeManager::GetInstance().EnableVpeVideo(config);
            break;
        }
        default:
            break;
    }
#endif
}

void RSSurfaceNode::DumpSubClass(std::string& out) const
{
    if (isShadowNode_) {
        out += "], isShadowNode[true";
    }

    if (existsDuplicateModifier_) {
        out += "], existsDuplicateModifier[true";
    }
}

void RSSurfaceNode::SetIsDepthResource(bool isDepthResource) {}

<<<<<<< HEAD
void RSSurfaceNode::CreateRenderThreadNode(RSSurfaceNodeType type, bool isWindow)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(GetId(), type, isTextureExportNode_);
    if (isTextureExportNode_) {
        AddCommand(command, false);
        SetSurfaceIdToRenderNode();
    } else {
        AddCommand(command, isWindow);
    }
    command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(GetId(),
        GetRSUIContext()->GetConnectToRender());
    AddCommand(command, isWindow);

    RSRTRefreshCallback::Instance().SetRefresh([] { RSRenderThread::Instance().RequestNextVSync(); });
    command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(GetId(), true);
    AddCommand(command, isWindow);
    SetFrameGravity(Gravity::RESIZE);
    // codes for arkui-x
#if defined(USE_SURFACE_TEXTURE) && defined(ROSEN_ANDROID) && !defined(SCREENLESS_DEVICE)
    if (type == RSSurfaceNodeType::SURFACE_TEXTURE_NODE) {
        RSSurfaceExtConfig config = {
            .type = RSSurfaceExtType::SURFACE_TEXTURE,
            .additionalData = nullptr,
        };
        CreateSurfaceExt(config);
    }
#endif
    // codes for arkui-x
#if defined(USE_SURFACE_TEXTURE) && defined(ROSEN_IOS) && !defined(SCREENLESS_DEVICE)
    if ((type == RSSurfaceNodeType::SURFACE_TEXTURE_NODE) && (name_ == "PlatformViewSurface")) {
        RSSurfaceExtConfig config = {
            .type = RSSurfaceExtType::SURFACE_PLATFORM_TEXTURE,
            .additionalData = nullptr,
        };
        CreateSurfaceExt(config);
    }
#endif
}
=======
#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceNode::SetDamageRegion(std::vector<Rect> rects)
{
    if (!delegateCompositeBufMgr_) {
        return;
    }
    delegateCompositeBufMgr_->SetDamageRegion(rects);
}

void RSSurfaceNode::SetBufferTransform(GraphicTransformType transform)
{
    if (!delegateCompositeBufMgr_) {
        return;
    }
    delegateCompositeBufMgr_->SetBufferTransform(transform);
}

void RSSurfaceNode::SetBuffer(sptr<SurfaceBuffer> buffer,
    UniqueFd fenceFd, const BufferReleaseCallback& releaseCallback)
{
    if (!delegateCompositeBufMgr_) {
        return;
    }
    delegateCompositeBufMgr_->SetBuffer(buffer, std::move(fenceFd), releaseCallback);
}

void RSSurfaceNode::CleanBuffer(bool cleanAll)
{
    if (!delegateCompositeBufMgr_) {
        return;
    }
    delegateCompositeBufMgr_->CleanBuffer(cleanAll);
}

void RSSurfaceNode::SetDesiredPresentTime(int64_t desiredPresentTime)
{
    if (!delegateCompositeBufMgr_) {
        return;
    }
    delegateCompositeBufMgr_->SetDesiredPresentTime(desiredPresentTime);
}

GSError RSSurfaceNode::ReleaseBuffer(uint32_t sequence, sptr<SyncFence> fence)
{
    if (!delegateCompositeBufMgr_) {
        return GSERROR_NOT_INIT;
    }
    return delegateCompositeBufMgr_->ReleaseBuffer(sequence, fence);
}

void RSSurfaceNode::SetDelegateDstRect(float positionX, float positionY, float positionZ, float positionW)
{
    if (!delegateCompositeBufMgr_) {
        return;
    }
    delegateCompositeBufMgr_->SetDelegateDstRect(positionX, positionY, positionZ, positionW);
}

void RSSurfaceNode::SetDelegateSrcRect(float positionX, float positionY, float positionZ, float positionW)
{
    if (!delegateCompositeBufMgr_) {
        return;
    }
    delegateCompositeBufMgr_->SetDelegateSrcRect(positionX, positionY, positionZ, positionW);
}

bool RSSurfaceNode::SetDelegateMode(bool isDelegateMode)
{
    if (delegateCompositeBufMgr_) {
        return false;
    }
    if (!isDelegateMode) {
        return true;
    }
    delegateCompositeBufMgr_ = std::make_shared<RSDelegateCompositeBufferManager>(
        GetRSUIContext(), GetSurface(), GetId(), GetName());
    if (!delegateCompositeBufMgr_) {
        RS_LOGE("RSSurfaceNode::SetDelegateMode fail: create delegateCompositeBufMgr fail");
        return false;
    }
    return delegateCompositeBufMgr_->SetDelegateMode(isDelegateMode);
}
#endif
>>>>>>> master
} // namespace Rosen
} // namespace OHOS
