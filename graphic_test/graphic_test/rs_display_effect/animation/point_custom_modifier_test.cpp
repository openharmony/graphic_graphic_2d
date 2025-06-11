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

#include "point_custom_modifier_test.h"

#include "drawing/draw/core_canvas.h"
#include "draw/pen.h"
#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_property_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float POINT_WIDTH = 5;
constexpr float POINT_INTERVAL = 8;
} // namespace

void PointCustomModifier::SetPosition(float position)
{
    if (position_ == nullptr) {
        position_ = std::make_shared<RSAnimatableProperty<float>>(position);
        AttachProperty(position_);
    } else {
        position_->Set(position);
    }
}

void PointCustomModifier::Draw(RSDrawingContext& context) const
{
    if (!position_ || !context.canvas) {
        std::cout << "AnimationCustomModifier:Draw: position_ or canvas is nullptr\n";
        return;
    }

    Drawing::Brush brush;
    brush.SetColor(OHOS::Rosen::Drawing::Color(0xFF000000));
    brush.SetAntiAlias(true);

    Drawing::Pen pen(OHOS::Rosen::Drawing::Color(0xFFFF0000));
    pen.SetWidth(POINT_WIDTH);

    const auto& canvas = context.canvas;
    canvas->AttachBrush(brush);
    canvas->AttachPen(pen);

    positionVec_.push_back(position_->Get());
    for (int i = 0; i < positionVec_.size(); i++) {
        auto point = OHOS::Rosen::Drawing::Point(i * POINT_INTERVAL, positionVec_[i]);
        canvas->DrawPoint(point);
    }

    canvas->DetachBrush();
    canvas->DetachPen();
}
} // namespace Rosen
} // namespace OHOS