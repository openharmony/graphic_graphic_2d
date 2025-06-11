/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ANIM_CUSTOM_POINT_MODIFIER_TEST_H
#define ANIM_CUSTOM_POINT_MODIFIER_TEST_H

#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_modifier.h"

namespace OHOS {
namespace Rosen {
class PointCustomModifier : public OHOS::Rosen::RSContentStyleModifier {
public:
    ~PointCustomModifier() = default;
    void Draw(OHOS::Rosen::RSDrawingContext& context) const;
    void SetPosition(float position);

private:
    std::shared_ptr<OHOS::Rosen::RSAnimatableProperty<float>> position_;
    mutable std::vector<float> positionVec_ = {};
};
} // namespace Rosen
} // namespace OHOS

#endif