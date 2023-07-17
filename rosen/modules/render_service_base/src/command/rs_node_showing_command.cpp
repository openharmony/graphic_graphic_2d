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

RSCommandRegister<RSNodeGetShowingPropertyAndCancelAnimation::commandType,
    RSNodeGetShowingPropertyAndCancelAnimation::commandSubType,
    RSNodeGetShowingPropertyAndCancelAnimation::Unmarshalling>
    RSNodeGetShowingPropertyAndCancelAnimation::registry;

bool RSNodeGetShowingPropertyAndCancelAnimation::Marshalling(Parcel& parcel) const
{
    return RSMarshallingHelper::Marshalling(parcel, commandType) &&
           RSMarshallingHelper::Marshalling(parcel, commandSubType) &&
           RSMarshallingHelper::Marshalling(parcel, targetId_) &&
           RSMarshallingHelper::Marshalling(parcel, timeoutNS_) &&
           RSMarshallingHelper::Marshalling(parcel, result_) &&
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
    return RSMarshallingHelper::Unmarshalling(parcel, result_) &&
           RSRenderPropertyBase::Unmarshalling(parcel, property_);
}

void RSNodeGetShowingPropertyAndCancelAnimation::Process(RSContext& context)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(targetId_);
    if (!node || !property_) {
        result_ = false;
        return;
    }
    auto modifier = node->GetModifier(property_->GetId());
    if (!modifier) {
        result_ = false;
        return;
    }
    property_ = modifier->GetProperty();
    result_ = (property_ != nullptr);
    if (result_) {
        auto& animationManager = node->GetAnimationManager();
        animationManager.CancelAnimationByPropertyId(property_->GetId());
    }
}
} // namespace Rosen
} // namespace OHOS
