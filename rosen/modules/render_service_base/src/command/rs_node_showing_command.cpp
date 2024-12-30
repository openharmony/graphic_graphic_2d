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

#include "command/rs_node_showing_command.h"

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSNodeGetShowingPropertyAndCancelAnimation::Registrar RSNodeGetShowingPropertyAndCancelAnimation::instance_;
RSNodeGetShowingPropertiesAndCancelAnimation::Registrar RSNodeGetShowingPropertiesAndCancelAnimation::instance_;

bool RSNodeGetShowingPropertyAndCancelAnimation::Marshalling(Parcel& parcel) const
{
    return RSMarshallingHelper::Marshalling(parcel, commandType) &&
           RSMarshallingHelper::Marshalling(parcel, commandSubType) &&
           RSMarshallingHelper::Marshalling(parcel, targetId_) &&
           RSMarshallingHelper::Marshalling(parcel, timeoutNS_) &&
           RSMarshallingHelper::Marshalling(parcel, isTimeout_) &&
           RSMarshallingHelper::Marshalling(parcel, success_) &&
           RSMarshallingHelper::Marshalling(parcel, property_);
}

RSCommand* RSNodeGetShowingPropertyAndCancelAnimation::Unmarshalling(Parcel& parcel)
{
    NodeId targetId;
    std::shared_ptr<RSRenderPropertyBase> property;
    uint64_t timeoutNS;
    if (!(RSMarshallingHelper::Unmarshalling(parcel, targetId) &&
            RSMarshallingHelper::Unmarshalling(parcel, timeoutNS))) {
        return nullptr;
    }
    auto command = new RSNodeGetShowingPropertyAndCancelAnimation(targetId, property, timeoutNS);
    if (!command->ReadFromParcel(parcel)) {
        delete command;
        return nullptr;
    }
    return command;
}

bool RSNodeGetShowingPropertyAndCancelAnimation::CheckHeader(Parcel& parcel) const
{
    uint16_t type;
    uint16_t subType;
    uint64_t timeoutNS;
    NodeId targetId;
    return RSMarshallingHelper::Unmarshalling(parcel, type) && type == commandType &&
           RSMarshallingHelper::Unmarshalling(parcel, subType) && subType == commandSubType &&
           RSMarshallingHelper::Unmarshalling(parcel, targetId) && targetId == targetId_ &&
           RSMarshallingHelper::Unmarshalling(parcel, timeoutNS) && timeoutNS == timeoutNS_;
}

bool RSNodeGetShowingPropertyAndCancelAnimation::ReadFromParcel(Parcel& parcel)
{
    return RSMarshallingHelper::Unmarshalling(parcel, isTimeout_) &&
           RSMarshallingHelper::Unmarshalling(parcel, success_) &&
           RSMarshallingHelper::Unmarshalling(parcel, property_);
}

void RSNodeGetShowingPropertyAndCancelAnimation::Process(RSContext& context)
{
    isTimeout_ = false;
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(targetId_);
    if (!node || !property_) {
        success_ = false;
        ROSEN_LOGE("RSNodeGetShowingPropertyAndCancelAnimation::Process, "
            "node [%{public}" PRIu64 "] or property is null!", targetId_);
        return;
    }
    std::shared_ptr<RSRenderPropertyBase> property;
    if (auto modifier = node->GetModifier(property_->GetId())) {
        property = modifier->GetProperty();
    } else {
        property = node->GetProperty(property_->GetId());
    }
    if (!property) {
        success_ = false;
        ROSEN_LOGE("RSNodeGetShowingPropertyAndCancelAnimation::Process, modifier is null!");
        return;
    }
    auto& animationManager = node->GetAnimationManager();
    animationManager.CancelAnimationByPropertyId(property_->GetId());
}

bool RSNodeGetShowingPropertyAndCancelAnimation::IsCallingPidValid(pid_t callingPid,
    const RSRenderNodeMap& nodeMap) const
{
    if (ExtractPid(targetId_) != callingPid && !nodeMap.IsUIExtensionSurfaceNode(targetId_)) {
        ROSEN_LOGE("RSNodeGetShowingPropertyAndCancelAnimation::IsCallingPidValid, "
                "callingPid [%{public}d] no permission "
                "EXECUTE_SYNCHRONOUS_TASK on node [%{public}" PRIu64 "] ",
                static_cast<int>(callingPid), targetId_);
        return false;
    }
    return true;
}

bool RSNodeGetShowingPropertiesAndCancelAnimation::Marshalling(Parcel& parcel) const
{
    bool result = RSMarshallingHelper::Marshalling(parcel, commandType) &&
           RSMarshallingHelper::Marshalling(parcel, commandSubType) &&
           RSMarshallingHelper::Marshalling(parcel, timeoutNS_) &&
           RSMarshallingHelper::Marshalling(parcel, success_) &&
           RSMarshallingHelper::Marshalling(parcel, propertiesMap_);
    return result;
}

// construct cancelAnimation & SetProperties
RSCommand* RSNodeGetShowingPropertiesAndCancelAnimation::Unmarshalling(Parcel& parcel)
{
    uint64_t timeoutNS;
    if (!RSMarshallingHelper::Unmarshalling(parcel, timeoutNS)) {
        return nullptr;
    }
    auto command = new RSNodeGetShowingPropertiesAndCancelAnimation(timeoutNS);
    if (!command->ReadFromParcel(parcel)) {
        delete command;
        return nullptr;
    }
    return command;
}

bool RSNodeGetShowingPropertiesAndCancelAnimation::CheckHeader(Parcel& parcel) const
{
    uint16_t type;
    uint16_t subType;
    uint64_t timeoutNS;

    return RSMarshallingHelper::Unmarshalling(parcel, type) && type == commandType &&
           RSMarshallingHelper::Unmarshalling(parcel, subType) && subType == commandSubType &&
           RSMarshallingHelper::Unmarshalling(parcel, timeoutNS) && timeoutNS == timeoutNS_;
}

bool RSNodeGetShowingPropertiesAndCancelAnimation::ReadFromParcel(Parcel& parcel)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, success_)) {
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, propertiesMap_)) {
        return false;
    }
    return true;
}

void RSNodeGetShowingPropertiesAndCancelAnimation::Process(RSContext& context)
{
    success_ = true;
    auto& nodeMap = context.GetNodeMap();
    for (auto& [key, value] : propertiesMap_) {
        // value should already initialized as nullptr
        auto& [nodeId, propertyId] = key;
        auto& [property, animations] = value;
        auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
        if (!node) {
            ROSEN_LOGE("RSNodeGetShowingPropertiesAndCancelAnimation::Process, "
                "node [%{public}" PRIu64 "] is null!", nodeId);
            continue;
        }
        if (auto modifier = node->GetModifier(propertyId)) {
            property = modifier->GetProperty();
        } else {
            property = node->GetProperty(propertyId);
        }
        node->GetAnimationManager().AttemptCancelAnimationByAnimationId(animations);
    }
}

bool RSNodeGetShowingPropertiesAndCancelAnimation::IsCallingPidValid(pid_t callingPid,
    const RSRenderNodeMap& nodeMap) const
{
    for (auto& [key, _] : propertiesMap_) {
        auto& nodeId = key.first;
        if (ExtractPid(nodeId) != callingPid && !nodeMap.IsUIExtensionSurfaceNode(nodeId)) {
            ROSEN_LOGE("RSNodeGetShowingPropertiesAndCancelAnimation::IsCallingPidValid, "
                "callingPid [%{public}d] no permission "
                "EXECUTE_SYNCHRONOUS_TASK on node [%{public}" PRIu64 "] ",
                static_cast<int>(callingPid), nodeId);
            return false;
        }
    }
    return true;
}

bool RSNodeGetAnimationsValueFraction::Marshalling(Parcel& parcel) const
{
    bool result = RSMarshallingHelper::Marshalling(parcel, commandType) &&
           RSMarshallingHelper::Marshalling(parcel, commandSubType) &&
           RSMarshallingHelper::Marshalling(parcel, timeoutNS_) &&
           RSMarshallingHelper::Marshalling(parcel, success_) &&
           RSMarshallingHelper::Marshalling(parcel, nodeId_) &&
           RSMarshallingHelper::Marshalling(parcel, animationId_);
    return result;
}

RSCommand* RSNodeGetAnimationsValueFraction::Unmarshalling(Parcel& parcel)
{
    uint64_t timeoutNS;
    if (!RSMarshallingHelper::Unmarshalling(parcel, timeoutNS)) {
        return nullptr;
    }
    auto command = new RSNodeGetAnimationsValueFraction(timeoutNS);
    if (!command->ReadFromParcel(parcel)) {
        delete command;
        return nullptr;
    }
    return command;
}

bool RSNodeGetAnimationsValueFraction::CheckHeader(Parcel& parcel) const
{
    uint16_t type;
    uint16_t subType;
    uint64_t timeoutNS;
    return RSMarshallingHelper::Unmarshalling(parcel, type) && type == commandType &&
           RSMarshallingHelper::Unmarshalling(parcel, subType) && subType == commandSubType &&
           RSMarshallingHelper::Unmarshalling(parcel, timeoutNS) && timeoutNS == timeoutNS_;
}

bool RSNodeGetAnimationsValueFraction::ReadFromParcel(Parcel& parcel)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, success_)) {
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, nodeId_)) {
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, animationId_)) {
        return false;
    }
    return true;
}

void RSNodeGetAnimationsValueFraction::Process(RSContext& context)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId_);
    if (node == nullptr) {
        ROSEN_LOGE("RSNodeGetAnimationsValueFraction::Process, node is null!");
        return;
    }
    auto animation = node->GetAnimationManager().GetAnimation(animationId_);
    if (animation == nullptr) {
        ROSEN_LOGE("RSNodeGetAnimationsValueFraction::Process, animation is null!");
        return;
    }
    success_ = true;
    fraction_ = animation->GetValueFraction();
}

bool RSNodeGetAnimationsValueFraction::IsCallingPidValid(pid_t callingPid,
    const RSRenderNodeMap& nodeMap) const
{
    if (ExtractPid(nodeId_) != callingPid && !nodeMap.IsUIExtensionSurfaceNode(nodeId_)) {
        ROSEN_LOGE("RSNodeGetAnimationsValueFraction::IsCallingPidValid, "
                "callingPid [%{public}d] no permission "
                "EXECUTE_SYNCHRONOUS_TASK on node [%{public}" PRIu64 "] ",
                static_cast<int>(callingPid), nodeId_);
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
