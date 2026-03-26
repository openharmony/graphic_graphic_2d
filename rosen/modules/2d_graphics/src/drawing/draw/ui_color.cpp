/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "draw/ui_color.h"

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <cmath>

namespace OHOS {
namespace Rosen {
namespace Drawing {

constexpr float EPSILON = 1e-6f;

UIColor::UIColor() noexcept : red_(0.0), green_(0.0), blue_(0.0), alpha_(1.0), headroom_(1.0)
{
}

UIColor::UIColor(const UIColor& c) noexcept
{
    alpha_ = c.GetAlpha();
    red_ = c.GetRed();
    green_ = c.GetGreen();
    blue_ = c.GetBlue();
    headroom_ = c.GetHeadroom();
}

UIColor::UIColor(float red, float green, float blue, float alpha, float headroom) noexcept
{
    red_ = red < 0.0 ? 0.0 : red;
    green_ = green < 0.0 ? 0.0 : green;
    blue_ = blue < 0.0 ? 0.0 : blue;
    alpha_ = alpha < 0.0 ? 0.0 : alpha;
    headroom_ = alpha < 1.0 ? 1.0 : headroom_;
}

float UIColor::GetRed() const
{
    return red_;
}

float UIColor::GetGreen() const
{
    return green_;
}

float UIColor::GetBlue() const
{
    return blue_;
}

float UIColor::GetAlpha() const
{
    return alpha_;
}

float UIColor::GetHeadroom() const
{
    return headroom_;
}

void UIColor::SetRed(float red)
{
    red_ = red;
    if (red_ < 0.0) {
        red_ = 0.0;
    }
}

void UIColor::SetGreen(float green)
{
    green_ = green;
    if (green_ < 0.0) {
        green_ = 0.0;
    }
}

void UIColor::SetBlue(float blue)
{
    blue_ = blue;
    if (blue_ < 0.0) {
        blue_ = 0.0;
    }
}

void UIColor::SetAlpha(float alpha)
{
    alpha_ = alpha;
    if (alpha_ < 0.0) {
        alpha_ = 0.0;
    }
}

void UIColor::SetHeadroom(float headroom)
{
    headroom_ = headroom;
    if (headroom_ < 1.0) {
        headroom_ = 1.0;
    }
}

void UIColor::SetRgb(float red, float green, float blue)
{
    red_ = red < 0.0 ? 0.0 : red;
    green_ = green < 0.0 ? 0.0 : green;
    blue_ = blue < 0.0 ? 0.0 : blue;
}

void UIColor::SetRgbF(float red, float green, float blue, float alpha)
{
    red_ = red < 0.0 ? 0.0 : red;
    green_ = green < 0.0 ? 0.0 : green;
    blue_ = blue < 0.0 ? 0.0 : blue;
    alpha_ = alpha < 0.0 ? 0.0 : alpha;
}

bool UIColor::operator==(const UIColor& that) const
{
    return std::fabs(red_ - that.red_) < EPSILON && std::fabs(green_ - that.green_) < EPSILON &&
        std::fabs(blue_ - that.blue_) < EPSILON && std::fabs(alpha_ - that.alpha_) < EPSILON &&
        std::fabs(headroom_ - that.headroom_) < EPSILON;
}

bool UIColor::operator!=(const UIColor& that) const
{
    return !operator==(that);
}

void UIColor::Dump(std::string& out) const
{
    std::stringstream ss;
    ss << "[A: " << std::to_string(alpha_) << ", R: " << std::to_string(red_) << ", G: " << std::to_string(green_)
        << ", B: " << std::to_string(blue_) << ", headroom: "<< std::to_string(headroom_) << "]";
    out += ss.str();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS