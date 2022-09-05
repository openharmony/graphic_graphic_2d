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
#include <ui/rs_node.h>

namespace OHOS {
namespace Rosen {
struct RSWindowAnimationTarget : Parcelable {
    static RSWindowAnimationTarget* Unmarshalling(Parcel& parcel);

    bool Marshalling(Parcel& parcel) const override;

    bool ReadFromParcel(Parcel& parcel);

    std::string bundleName_;
    std::string abilityName_;
    RRect windowBounds_;
    std::shared_ptr<RSNode> surfaceNode_;
    uint32_t windowId_;
    uint64_t displayId_;
    int32_t missionId_;
};
} // namespace Rosen
} // namespace OHOS

#endif // WINDOW_ANIMATION_RS_WINDOW_ANIMATION_TARGET_H
