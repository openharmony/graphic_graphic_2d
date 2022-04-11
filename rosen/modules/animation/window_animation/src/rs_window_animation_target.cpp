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
    parcel.WriteInt32(type_);
    parcel.WriteString(bundleName_);
    parcel.WriteString(abilityName_);
    parcel.WriteFloat(windowBounds_.rect_.left_);
    parcel.WriteFloat(windowBounds_.rect_.top_);
    parcel.WriteFloat(windowBounds_.rect_.width_);
    parcel.WriteFloat(windowBounds_.rect_.height_);
    parcel.WriteFloat(windowBounds_.radius_[0].x_);
    parcel.WriteUint32(windowId_);
    parcel.WriteParcelable(surfaceNode_);
    return true;
}

bool RSWindowAnimationTarget::ReadFromParcel(Parcel& parcel)
{
    type_ = static_cast<WindowAnimationTargetType>(parcel.ReadInt32());
    bundleName_ = parcel.ReadString();
    abilityName_ = parcel.ReadString();
    windowBounds_.rect_.left_ = parcel.ReadFloat();
    windowBounds_.rect_.top_ = parcel.ReadFloat();
    windowBounds_.rect_.width_ = parcel.ReadFloat();
    windowBounds_.rect_.height_ = parcel.ReadFloat();
    windowBounds_.radius_[0].x_ = parcel.ReadFloat();
    windowId_ = parcel.ReadUint32();
    surfaceNode_ = parcel.ReadParcelable<RSSurfaceNode>();
    return true;
}
} // namespace Rosen
} // namespace OHOS
