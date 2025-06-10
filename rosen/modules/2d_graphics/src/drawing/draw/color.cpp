/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "draw/color.h"
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <sstream>

namespace OHOS {
namespace Rosen {
namespace Drawing {
static uint32_t ClampColor(uint32_t value)
{
    if (value > Color::RGB_MAX) {
        return Color::RGB_MAX;
    }
    return value;
}

Color::Color() noexcept : alpha_(RGB_MAX), red_(0), green_(0), blue_(0)
{
    UpdateValueToFloat();
}

Color::Color(const Color& c) noexcept
{
    // Tell the compiler there is no alias and to select wider load/store instructions.
    uint32_t alpha = c.GetAlpha();
    uint32_t red = c.GetRed();
    uint32_t green = c.GetGreen();
    uint32_t blue = c.GetBlue();
    alpha_ = alpha;
    red_ = red;
    green_ = green;
    blue_ = blue;
    color4f_.redF_ = c.GetRedF();
    color4f_.greenF_ = c.GetGreenF();
    color4f_.blueF_ = c.GetBlueF();
    color4f_.alphaF_ = c.GetAlphaF();
}

Color::Color(uint32_t r, uint32_t g, uint32_t b, uint32_t a) noexcept : alpha_(ClampColor(a)),
    red_(ClampColor(r)), green_(ClampColor(g)), blue_(ClampColor(b))
{
    UpdateValueToFloat();
}

// Return alpha byte, red component, green component and blue component of color rgba.
Color::Color(ColorQuad rgba) noexcept
{
    alpha_ = rgba >> 24;
    red_ = (rgba >> 16) & 0xff;
    green_ = (rgba >> 8) & 0xff;
    blue_ = (rgba >> 0) & 0xff;
    UpdateValueToFloat();
}

uint32_t Color::GetRed() const
{
    return red_;
}

uint32_t Color::GetGreen() const
{
    return green_;
}

uint32_t Color::GetBlue() const
{
    return blue_;
}

uint32_t Color::GetAlpha() const
{
    return alpha_;
}

void Color::SetRed(uint32_t r)
{
    red_ = ClampColor(r);
    color4f_.redF_ = static_cast<scalar>(red_) / RGB_MAX;
}

void Color::SetGreen(uint32_t g)
{
    green_ = ClampColor(g);
    color4f_.greenF_ = static_cast<scalar>(green_) / RGB_MAX;
}

void Color::SetBlue(uint32_t b)
{
    blue_ = ClampColor(b);
    color4f_.blueF_ = static_cast<scalar>(blue_) / RGB_MAX;
}

void Color::SetAlpha(uint32_t a)
{
    alpha_ = ClampColor(a);
    color4f_.alphaF_ = static_cast<scalar>(alpha_) / RGB_MAX;
}

scalar Color::GetRedF() const
{
    return color4f_.redF_;
}

scalar Color::GetGreenF() const
{
    return color4f_.greenF_;
}

scalar Color::GetBlueF() const
{
    return color4f_.blueF_;
}

scalar Color::GetAlphaF() const
{
    return color4f_.alphaF_;
}

const Color4f& Color::GetColor4f()
{
    return color4f_;
}

void Color::SetRedF(scalar r)
{
    color4f_.redF_ = std::clamp(r, 0.0f, 1.0f);
    red_ = static_cast<uint32_t>(round(color4f_.redF_ * RGB_MAX));
}

void Color::SetGreenF(scalar g)
{
    color4f_.greenF_ = std::clamp(g, 0.0f, 1.0f);
    green_ = static_cast<uint32_t>(round(color4f_.greenF_ * RGB_MAX));
}

void Color::SetBlueF(scalar b)
{
    color4f_.blueF_ = std::clamp(b, 0.0f, 1.0f);
    blue_ = static_cast<uint32_t>(round(color4f_.blueF_ * RGB_MAX));
}

void Color::SetAlphaF(scalar a)
{
    color4f_.alphaF_ = std::clamp(a, 0.0f, 1.0f);
    alpha_ = static_cast<uint32_t>(round(color4f_.alphaF_ * RGB_MAX));
}

void Color::SetRgb(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
    alpha_ = ClampColor(a);
    red_ = ClampColor(r);
    green_ = ClampColor(g);
    blue_ = ClampColor(b);
    UpdateValueToFloat();
}

void Color::SetRgbF(scalar r, scalar g, scalar b, scalar a)
{
    color4f_.redF_ = std::clamp(r, 0.0f, 1.0f);
    color4f_.greenF_ = std::clamp(g, 0.0f, 1.0f);
    color4f_.blueF_ = std::clamp(b, 0.0f, 1.0f);
    color4f_.alphaF_ = std::clamp(a, 0.0f, 1.0f);
    UpdateValueToInt();
}

void Color::SetColorQuad(uint32_t c)
{
    alpha_ = Color::ColorQuadGetA(c);
    red_ = Color::ColorQuadGetR(c);
    green_ = Color::ColorQuadGetG(c);
    blue_ = Color::ColorQuadGetB(c);
    UpdateValueToFloat();
}

bool operator==(const Color& c1, const Color& c2)
{
    return c1.alpha_ == c2.alpha_ && c1.red_ == c2.red_ && c1.green_ == c2.green_ && c1.blue_ == c2.blue_;
}
bool operator!=(const Color& c1, const Color& c2)
{
    return c1.alpha_ != c2.alpha_ || c1.red_ != c2.red_ || c1.green_ != c2.green_ || c1.blue_ != c2.blue_;
}

void Color::Dump(std::string& out) const
{
    constexpr int32_t colorStrWidth = 8;
    std::stringstream ss;
    ss << "[ARGB-0x" << std::hex << std::setfill('0') << std::setw(colorStrWidth) << std::uppercase;
    ss << CastToColorQuad() << ']';
    out += ss.str();
}
void Color::UpdateValueToFloat()
{
    color4f_.redF_ = static_cast<scalar>(red_) / RGB_MAX;
    color4f_.greenF_ = static_cast<scalar>(green_) / RGB_MAX;
    color4f_.blueF_ = static_cast<scalar>(blue_) / RGB_MAX;
    color4f_.alphaF_ = static_cast<scalar>(alpha_) / RGB_MAX;
}
void Color::UpdateValueToInt()
{
    alpha_ = static_cast<uint32_t>(round(color4f_.alphaF_ * RGB_MAX));
    red_ = static_cast<uint32_t>(round(color4f_.redF_ * RGB_MAX));
    green_ = static_cast<uint32_t>(round(color4f_.greenF_ * RGB_MAX));
    blue_ = static_cast<uint32_t>(round(color4f_.blueF_ * RGB_MAX));
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
