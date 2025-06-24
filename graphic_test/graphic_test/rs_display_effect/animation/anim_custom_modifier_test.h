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

#ifndef ANIM_CUSTOM_MODIFIER_TEST_H
#define ANIM_CUSTOM_MODIFIER_TEST_H

#if defined(MODIFIER_NG)
#include "modifier_ng/custom/rs_content_style_modifier.h"
#else
#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_modifier.h"
#endif

namespace OHOS {
namespace Rosen {
#if defined(MODIFIER_NG)
using ContentStyleModifier = Rosen::ModifierNG::RSContentStyleModifier;
using DrawingContext = Rosen::ModifierNG::RSDrawingContext;
#else
using ContentStyleModifier = Rosen::RSContentStyleModifier;
using DrawingContext = Rosen::RSDrawingContext;
#endif
class AnimationCustomModifier : public ContentStyleModifier {
public:
    ~AnimationCustomModifier() = default;

    void Draw(DrawingContext& context) const;
    void SetPosition(float position);
    // Set the horizontal pixel unit of an animation graph
    void SetTimeInterval(float timeInterval);
    void SetPointColor(Drawing::Color color);

private:
    std::shared_ptr<OHOS::Rosen::RSAnimatableProperty<float>> position_;
    float timeInterval_ = 1.0f;
    mutable std::vector<float> positionVec_ = {};
    Drawing::Color pointColor_ = Drawing::Color::COLOR_BLACK;
};
} // namespace Rosen
} // namespace OHOS
#endif