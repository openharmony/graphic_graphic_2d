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

#include "anim_custom_modifier_test.h"

#include "drawing/draw/core_canvas.h"
#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_property_modifier.h"

namespace OHOS {
namespace Rosen {
void AnimationCustomModifier::SetPosition(float position)
{
    if (position_ == nullptr) {
        position_ = std::make_shared<RSAnimatableProperty<float>>(position);
        AttachProperty(position_);
    } else {
        position_->Set(position);
    }
}

void AnimationCustomModifier::SetTimeInterval(float timeInterval)
{
    timeInterval_ = timeInterval;
}

void AnimationCustomModifier::SetPointColor(Drawing::Color color)
{
    pointColor_ = color;
}

void AnimationCustomModifier::Draw(DrawingContext& context) const
{
    if (!position_) {
        std::cout << "AnimationCustomModifier position_ is nullptr, Draw none\n";
        Drawing::Rect rect;
        Drawing::Brush brush;
        context.canvas->AttachBrush(brush);
        context.canvas->DrawRect(rect);
        context.canvas->DetachBrush();
        return;
    }
    auto position = position_->Get();
    positionVec_.push_back(position);
    Drawing::Brush brush;
    brush.SetColor(pointColor_);
    brush.SetAntiAlias(true);
    context.canvas->AttachBrush(brush);
    auto lastPoint = OHOS::Rosen::Drawing::Point(0, positionVec_[0]);
    if (positionVec_.size() == 1) {
        std::cout << "AnimationCustomModifier positionVec_ size is 1\n";
        context.canvas->DrawPoint(lastPoint);
    } else {
        for (int i = 1; i < positionVec_.size(); i++) {
            auto currentPoint = OHOS::Rosen::Drawing::Point(timeInterval_ * i, positionVec_[i]);
            context.canvas->DrawLine(lastPoint, currentPoint);
            lastPoint = currentPoint;
        }
    }
    context.canvas->DetachBrush();
}
} // namespace Rosen
} // namespace OHOS