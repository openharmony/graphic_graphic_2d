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
           (property_ == nullptr || RSRenderPropertyBase::Marshalling(parcel, property_));
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
           RSRenderPropertyBase::Unmarshalling(parcel, property_);
}

void RSNodeGetShowingPropertyAndCancelAnimation::Process(RSContext& context)
{
    isTimeout_ = false;
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(targetId_);
    if (!node || !property_) {
        success_ = false;
        return;
    }
    auto modifier = node->GetModifier(property_->GetId());
    if (!modifier) {
        success_ = false;
        return;
    }
    property_ = modifier->GetProperty();
    success_ = (property_ != nullptr);
    if (success_) {
        auto& animationManager = node->GetAnimationManager();
        animationManager.CancelAnimationByPropertyId(property_->GetId());
    }
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
            continue;
        }
        auto modifier = node->GetModifier(propertyId);
        if (!modifier) {
            node->GetAnimationManager().AttemptCancelAnimationByAnimationId(animations);
            continue;
        }
        property = modifier->GetProperty();
        if (!property) {
            node->GetAnimationManager().AttemptCancelAnimationByAnimationId(animations);
            continue;
        }
        node->GetAnimationManager().AttemptCancelAnimationByAnimationId(animations);
    }
}
} // namespace Rosen
} // namespace OHOS
