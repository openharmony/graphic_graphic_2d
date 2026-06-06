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
#include "rs_graphic_test_director.h"

namespace OHOS {
namespace Rosen {
AnimationCustomModifier::~AnimationCustomModifier()
{
    RSGraphicTestDirector::Instance().RemoveAnimationFrameCallback(animationFrameCallbackId_);
}

void AnimationCustomModifier::RegisterAnimationFrameCallback()
{
    if (animationFrameCallbackId_ != 0) {
        return;
    }
    auto weakModifier = weak_from_this();
    animationFrameCallbackId_ = RSGraphicTestDirector::Instance().AddAnimationFrameCallback([weakModifier]() {
        auto modifier = weakModifier.lock();
        auto customModifier = std::dynamic_pointer_cast<AnimationCustomModifier>(modifier);
        if (customModifier) {
            customModifier->RecordCurrentPosition();
        }
    });
}

void AnimationCustomModifier::SetPosition(float position)
{
    RegisterAnimationFrameCallback();
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

void AnimationCustomModifier::RecordCurrentPosition() const
{
    if (!position_) {
        return;
    }
    positionVec_.push_back(position_->Get());
}

void AnimationCustomModifier::DrawEmpty(DrawingContext& context) const
{
    std::cout << "AnimationCustomModifier position_ is nullptr, Draw none\n";
    Drawing::Rect rect;
    Drawing::Brush brush;
    context.canvas->AttachBrush(brush);
    context.canvas->DrawRect(rect);
    context.canvas->DetachBrush();
}

void AnimationCustomModifier::DrawPositionCurve(DrawingContext& context) const
{
    Drawing::Brush brush;
    brush.SetColor(pointColor_);
    brush.SetAntiAlias(true);
    context.canvas->AttachBrush(brush);
    auto lastPoint = OHOS::Rosen::Drawing::Point(0, positionVec_.empty() ? position_->Get() : positionVec_[0]);
    if (positionVec_.size() <= 1) {
        context.canvas->DrawPoint(lastPoint);
        context.canvas->DetachBrush();
        return;
    }

    for (size_t i = 1; i < positionVec_.size(); i++) {
        auto currentPoint = OHOS::Rosen::Drawing::Point(timeInterval_ * i, positionVec_[i]);
        context.canvas->DrawLine(lastPoint, currentPoint);
        lastPoint = currentPoint;
    }
    context.canvas->DetachBrush();
}

void AnimationCustomModifier::Draw(DrawingContext& context) const
{
    if (!position_) {
        DrawEmpty(context);
        return;
    }
    DrawPositionCurve(context);
}
} // namespace Rosen
} // namespace OHOS
