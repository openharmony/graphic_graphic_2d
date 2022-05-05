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

#ifndef WINDOW_ANIMATION_RS_WINDOW_ANIMATION_TARGET_H
#define WINDOW_ANIMATION_RS_WINDOW_ANIMATION_TARGET_H

#include <memory>
#include <string>

#include <common/rs_rect.h>
#include <iremote_broker.h>
#include <parcel.h>
#include <ui/rs_surface_node.h>

namespace OHOS {
namespace Rosen {
enum WindowAnimationTargetType {
    NORMAL_APP = 0,
    LAUNCHER,
    SCREEN_LOCK,
    SYSTEM_UI,
};

enum WindowAnimationActionType {
    NO_CHANGE = 0,
    GO_FOREGROUND,
    GO_BACKGROUND,
    MINIMIZE,
    CLOSE
};

struct RSWindowAnimationTarget : Parcelable {
    static RSWindowAnimationTarget* Unmarshalling(Parcel& parcel);

    bool Marshalling(Parcel& parcel) const override;

    bool ReadFromParcel(Parcel& parcel);

    WindowAnimationTargetType type_{ WindowAnimationTargetType::NORMAL_APP };
    std::string bundleName_;
    std::string abilityName_;
    RRect windowBounds_;
    uint32_t windowId_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    WindowAnimationActionType actionType_ = WindowAnimationActionType::NO_CHANGE;
};
} // namespace Rosen
} // namespace OHOS

#endif // WINDOW_ANIMATION_RS_WINDOW_ANIMATION_TARGET_H
