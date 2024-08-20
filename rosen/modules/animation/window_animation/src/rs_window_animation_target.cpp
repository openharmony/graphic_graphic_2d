/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_window_animation_target.h"

#include "rs_window_animation_log.h"

#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
RSWindowAnimationTarget* RSWindowAnimationTarget::Unmarshalling(Parcel& parcel)
{
    auto windowAnimationTarget = new (std::nothrow) RSWindowAnimationTarget();
    if (windowAnimationTarget != nullptr && !windowAnimationTarget->ReadFromParcel(parcel)) {
        WALOGE("Failed to unmarshalling window animation target!");
        delete windowAnimationTarget;
        windowAnimationTarget = nullptr;
    }

    return windowAnimationTarget;
}

bool RSWindowAnimationTarget::Marshalling(Parcel& parcel) const
{
    if (!(parcel.WriteString(bundleName_) &&
        parcel.WriteString(abilityName_) &&
        parcel.WriteFloat(windowBounds_.rect_.left_) &&
        parcel.WriteFloat(windowBounds_.rect_.top_) &&
        parcel.WriteFloat(windowBounds_.rect_.width_) &&
        parcel.WriteFloat(windowBounds_.rect_.height_) &&
        parcel.WriteFloat(windowBounds_.radius_[0].x_))) {
        WALOGE("RSWindowAnimationTarget::Marshalling, write param failed");
        return false;
    }

    // marshalling as RSSurfaceNode
    if (!surfaceNode_) {
        if (!parcel.WriteBool(false)) {
            WALOGE("RSWindowAnimationTarget::Marshalling, write param failed");
            return false;
        }
    } else if (auto surfaceNode = surfaceNode_->ReinterpretCastTo<RSSurfaceNode>()) {
        if (!(parcel.WriteBool(true) && surfaceNode->Marshalling(parcel))) {
            WALOGE("RSWindowAnimationTarget::Marshalling, write param failed");
            return false;
        }
    } else {
        return false;
    }

    if (!(parcel.WriteUint32(windowId_) &&
        parcel.WriteUint64(displayId_) &&
        parcel.WriteInt32(missionId_))) {
        WALOGE("RSWindowAnimationTarget::Marshalling, write param failed");
        return false;
    }
    return true;
}

bool RSWindowAnimationTarget::ReadFromParcel(Parcel& parcel)
{
    if (!(parcel.ReadString(bundleName_) &&
        parcel.ReadString(abilityName_) &&
        parcel.ReadFloat(windowBounds_.rect_.left_) &&
        parcel.ReadFloat(windowBounds_.rect_.top_) &&
        parcel.ReadFloat(windowBounds_.rect_.width_) &&
        parcel.ReadFloat(windowBounds_.rect_.height_) &&
        parcel.ReadFloat(windowBounds_.radius_[0].x_))) {
        WALOGE("RSWindowAnimationTarget::ReadFromParcel, read param failed");
        return false;
    }
    bool isRSProxyNode;
    if (!parcel.ReadBool(isRSProxyNode)) {
        WALOGE("RSWindowAnimationTarget::ReadFromParcel, read param failed");
        return false;
    }
    // unmarshalling as RSProxyNode
    if (isRSProxyNode) {
        surfaceNode_ = RSSurfaceNode::UnmarshallingAsProxyNode(parcel);
    }
    if (!(parcel.ReadUint32(windowId_) &&
        parcel.ReadUint64(displayId_) &&
        parcel.ReadInt32(missionId_))) {
        WALOGE("RSWindowAnimationTarget::ReadFromParcel, read param failed");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
