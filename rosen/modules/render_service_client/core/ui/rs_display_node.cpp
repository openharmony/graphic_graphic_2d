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

#include "ui/rs_display_node.h"

#include "rs_trace.h"

#include "command/rs_display_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
namespace OHOS {
namespace Rosen {

RSDisplayNode::SharedPtr RSDisplayNode::Create(const RSDisplayNodeConfig& displayNodeConfig)
{
    SharedPtr node(new RSDisplayNode(displayNodeConfig));
    RSNodeMap::MutableInstance().RegisterNode(node);

    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeCreate>(node->GetId(), displayNodeConfig);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGI("RSDisplayNode::Create, id:%{public}" PRIu64, node->GetId());
    return node;
}

bool RSDisplayNode::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint64(GetId()) && parcel.WriteUint64(screenId_) && parcel.WriteBool(isMirroredDisplay_);
}

RSDisplayNode::SharedPtr RSDisplayNode::Unmarshalling(Parcel& parcel)
{
    uint64_t id = UINT64_MAX;
    uint64_t screenId = UINT64_MAX;
    bool isMirrored = false;
    if (!(parcel.ReadUint64(id) && parcel.ReadUint64(screenId) && parcel.ReadBool(isMirrored))) {
        ROSEN_LOGE("RSDisplayNode::Unmarshalling, read param failed");
        return nullptr;
    }

    if (auto prevNode = RSNodeMap::Instance().GetNode(id)) {
        // if the node id is already in the map, we should not create a new node
        return prevNode->ReinterpretCastTo<RSDisplayNode>();
    }

    RSDisplayNodeConfig config { .screenId = screenId, .isMirrored = isMirrored };

    SharedPtr displayNode(new RSDisplayNode(config, id));
    RSNodeMap::MutableInstance().RegisterNode(displayNode);

    return displayNode;
}

void RSDisplayNode::ClearChildren()
{
    auto children = GetChildren();
    for (auto child : children) {
        if (auto childPtr = RSNodeMap::Instance().GetNode(child)) {
            RemoveChild(childPtr);
        }
    }
}

void RSDisplayNode::SetScreenId(uint64_t screenId)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetScreenId>(GetId(), screenId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGI(
        "RSDisplayNode::SetScreenId, DisplayNode: %{public}" PRIu64 ", ScreenId: %{public}" PRIu64, GetId(), screenId);
    RS_TRACE_NAME_FMT("RSDisplayNode::SetScreenId, DisplayNode: %" PRIu64 ", ScreenId: %" PRIu64, GetId(), screenId);
}

void RSDisplayNode::OnBoundsSizeChanged() const
{
    auto bounds = GetStagingProperties().GetBounds();
    ROSEN_LOGD("RSDisplayNode::OnBoundsSizeChanged, w: %{public}d, h: %{public}d.",
        (uint32_t)bounds.z_, (uint32_t)bounds.w_);
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetRogSize>(GetId(), bounds.z_, bounds.w_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

void RSDisplayNode::SetDisplayOffset(int32_t offsetX, int32_t offsetY)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetDisplayOffset>(GetId(), offsetX, offsetY);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSDisplayNode::SetDisplayOffset, offsetX:%{public}d, offsetY:%{public}d", offsetX, offsetY);
}

void RSDisplayNode::SetSecurityDisplay(bool isSecurityDisplay)
{
    isSecurityDisplay_ = isSecurityDisplay;
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetSecurityDisplay>(GetId(), isSecurityDisplay);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSDisplayNode::SetSecurityDisplay, displayNodeId:[%{public}" PRIu64 "]"
        " isSecurityDisplay:[%{public}s]", GetId(), isSecurityDisplay ? "true" : "false");
}

bool RSDisplayNode::GetSecurityDisplay() const
{
    return isSecurityDisplay_;
}

void RSDisplayNode::SetDisplayNodeMirrorConfig(const RSDisplayNodeConfig& displayNodeConfig)
{
    isMirroredDisplay_ = displayNodeConfig.isMirrored;
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetDisplayMode>(GetId(), displayNodeConfig);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSDisplayNode::SetDisplayNodeMirrorConfig, displayNodeId:[%{public}" PRIu64 "]"
        " isMirrored:[%{public}s]", GetId(), displayNodeConfig.isMirrored ? "true" : "false");
}

void RSDisplayNode::SetScreenRotation(const uint32_t& rotation)
{
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    switch (rotation) {
        case 0: // Rotation::ROTATION_0
            screenRotation = ScreenRotation::ROTATION_0;
            break;
        case 1: // Rotation::ROTATION_90
            screenRotation = ScreenRotation::ROTATION_90;
            break;
        case 2: // Rotation::ROTATION_180
            screenRotation = ScreenRotation::ROTATION_180;
            break;
        case 3: // Rotation::ROTATION_270
            screenRotation = ScreenRotation::ROTATION_270;
            break;
        default:
            screenRotation = ScreenRotation::INVALID_SCREEN_ROTATION;
            break;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetScreenRotation>(GetId(), screenRotation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGI("RSDisplayNode::SetScreenRotation, displayNodeId:[%{public}" PRIu64 "]"
               " screenRotation:[%{public}d]", GetId(), rotation);
}

bool RSDisplayNode::IsMirrorDisplay() const
{
    return isMirroredDisplay_;
}

RSDisplayNode::RSDisplayNode(const RSDisplayNodeConfig& config)
    : RSNode(true), screenId_(config.screenId), offsetX_(0), offsetY_(0), isMirroredDisplay_(config.isMirrored)
{
    (void)screenId_;
    (void)offsetX_;
    (void)offsetY_;
}

RSDisplayNode::RSDisplayNode(const RSDisplayNodeConfig& config, NodeId id)
    : RSNode(true, id), screenId_(config.screenId), offsetX_(0), offsetY_(0), isMirroredDisplay_(config.isMirrored)
{}

void RSDisplayNode::SetBootAnimation(bool isBootAnimation)
{
    isBootAnimation_ = isBootAnimation;
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetBootAnimation>(GetId(), isBootAnimation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

bool RSDisplayNode::GetBootAnimation() const
{
    return isBootAnimation_;
}

RSDisplayNode::~RSDisplayNode() = default;

} // namespace Rosen
} // namespace OHOS
