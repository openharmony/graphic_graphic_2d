/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "pipeline/rs_draw_cmd_list.h"

#include <algorithm>
#include <sstream>

#include "draw/canvas.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "recording/draw_cmd_list.h"
#include "recording/recording_canvas.h"
namespace OHOS {
namespace Rosen {

RSDrawCmdList::RSDrawCmdList(
    const std::shared_ptr<Drawing::DrawCmdList> startValue, const std::shared_ptr<Drawing::DrawCmdList> endValue)
{
    if (startValue && startValue->GetType() == Drawing::CmdList::Type::RS_DRAW_CMD_LIST) {
        auto rsDrawCmdList = std::static_pointer_cast<RSDrawCmdList>(startValue);
        startValue_.first = rsDrawCmdList->GetEndDrawCmdList();
    } else {
        startValue_.first = startValue;
    }
    endValue_.first = endValue;

    startValue_.second = DrawCmdListOpacity(1.f, 0.f, 1.f, 1.f);
    endValue_.second = DrawCmdListOpacity();
}

void RSDrawCmdList::Playback(Drawing::Canvas& canvas, const Drawing::Rect* rect)
{
    auto& paintFilterCanvas = static_cast<RSPaintFilterCanvas&>(canvas);
    auto processValue = [&paintFilterCanvas, rect](const auto& valuePair) {
        if (!valuePair.first)
            return;
        paintFilterCanvas.SaveAlpha();
        const float clampedOpacity = std::clamp(valuePair.second.opacity, 0.0f, 1.0f);
        paintFilterCanvas.MultiplyAlpha(clampedOpacity);
        valuePair.first->Playback(paintFilterCanvas, rect);
        paintFilterCanvas.RestoreAlpha();
    };

    processValue(endValue_);
    processValue(startValue_);
}

int32_t RSDrawCmdList::GetWidth() const
{
    if (endValue_.first) {
        return endValue_.first->GetWidth();
    }
    return Drawing::DrawCmdList::GetWidth();
}

int32_t RSDrawCmdList::GetHeight() const
{
    if (endValue_.first) {
        return endValue_.first->GetHeight();
    }
    return Drawing::DrawCmdList::GetHeight();
}

bool RSDrawCmdList::IsEmpty() const
{
    if (endValue_.first) {
        return endValue_.first->IsEmpty();
    }
    return false;
}

void RSDrawCmdList::Estimate(float fraction)
{
    if (ROSEN_EQ<float>(fraction, 1.0f)) {
        CleanOpacity();
        return;
    }
    startValue_.second.lastOpacity = startValue_.second.opacity;
    startValue_.second.opacity =
        startValue_.second.startOpacity * (1.0f - fraction) + startValue_.second.endOpacity * fraction;

    endValue_.second.lastOpacity = endValue_.second.opacity;
    endValue_.second.opacity =
        endValue_.second.startOpacity * (1.0f - fraction) + endValue_.second.endOpacity * fraction;
}

std::string RSDrawCmdList::ToString() const
{
    std::ostringstream oss;
    oss << "endOpacity:" << endValue_.second.opacity;

    if (startValue_.first) {
        oss << ", startOpacity:" << startValue_.second.opacity;
    }

    return oss.str();
}

void RSDrawCmdList::CleanOpacity()
{
    startValue_.first.reset();
    endValue_.second.opacity = 1.0f;
}

std::shared_ptr<Drawing::DrawCmdList> RSDrawCmdList::GetEndDrawCmdList() const
{
    return endValue_.first;
}
namespace Drawing {
RSB_EXPORT DrawCmdListPtr operator+(const DrawCmdListPtr& lhs, const DrawCmdListPtr& rhs)
{
    return lhs;
}
RSB_EXPORT DrawCmdListPtr operator-(const DrawCmdListPtr& lhs, const DrawCmdListPtr& rhs)
{
    return lhs;
}
RSB_EXPORT DrawCmdListPtr operator*(const DrawCmdListPtr& lhs, float rhs)
{
    return lhs;
}
RSB_EXPORT bool operator==(const DrawCmdListPtr& lhs, const DrawCmdListPtr& rhs)
{
    return false;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS