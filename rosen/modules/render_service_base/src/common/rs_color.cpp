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

#include "common/rs_color.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
RSColor::RSColor(uint32_t rgba) noexcept
{
    alpha_ = static_cast<int16_t>(rgba & 0xFF);               // 0xff byte mask
    red_ = static_cast<int16_t>((rgba & 0xFF000000) >> 24);   // 0xff000000 red mask, 24 red shift
    green_ = static_cast<int16_t>((rgba & 0x00FF0000) >> 16); // 0x00ff0000 green mask, 16 green shift
    blue_ = static_cast<int16_t>((rgba & 0x0000FF00) >> 8);   // 0x0000ff00 blue mask, 8 blue shift
}

RSColor::RSColor(int16_t red, int16_t green, int16_t blue) noexcept : RSColor(red, green, blue, UINT8_MAX) {}

RSColor::RSColor(int16_t red, int16_t green, int16_t blue, int16_t alpha) noexcept
{
    alpha_ = alpha;
    red_ = red;
    green_ = green;
    blue_ = blue;
}

bool RSColor::operator==(const RSColor& rhs) const
{
    return red_ == rhs.red_ && green_ == rhs.green_ && blue_ == rhs.blue_ && alpha_ == rhs.alpha_;
}

bool RSColor::IsNearEqual(const RSColor& other, int16_t threshold) const
{
    return (std::abs(red_ - other.red_) <= threshold) && (std::abs(green_ - other.green_) <= threshold) &&
           (std::abs(blue_ - other.blue_) <= threshold) && (std::abs(alpha_ - other.alpha_) <= threshold);
}

RSColor RSColor::operator+(const RSColor& rhs) const
{
    return RSColor(red_ + rhs.red_, green_ + rhs.green_, blue_ + rhs.blue_, alpha_ + rhs.alpha_);
}

RSColor RSColor::operator-(const RSColor& rhs) const
{
    return RSColor(red_ - rhs.red_, green_ - rhs.green_, blue_ - rhs.blue_, alpha_ - rhs.alpha_);
}

RSColor RSColor::operator*(float scale) const
{
    scale = std::clamp<float>(scale, INT16_MIN, INT16_MAX);
    return RSColor(round(std::clamp<float>(red_ * scale, INT16_MIN, INT16_MAX)),
        round(std::clamp<float>(green_ * scale, INT16_MIN, INT16_MAX)),
        round(std::clamp<float>(blue_ * scale, INT16_MIN, INT16_MAX)),
        round(std::clamp<float>(alpha_ * scale, INT16_MIN, INT16_MAX)));
}

RSColor& RSColor::operator*=(float scale)
{
    scale = std::clamp<float>(scale, INT16_MIN, INT16_MAX);
    red_ = round(std::clamp<float>(red_ * scale, INT16_MIN, INT16_MAX));
    green_ = round(std::clamp<float>(green_ * scale, INT16_MIN, INT16_MAX));
    blue_ = round(std::clamp<float>(blue_ * scale, INT16_MIN, INT16_MAX));
    alpha_ = round(std::clamp<float>(alpha_ * scale, INT16_MIN, INT16_MAX));
    return *this;
}

RSColor RSColor::operator/(float scale) const
{
    if (ROSEN_EQ<float>(scale, 0)) {
        return *this;
    }
    return operator*(1 / scale);
}

uint32_t RSColor::AsRgbaInt() const
{
    return (static_cast<uint32_t>(std::clamp<int16_t>(alpha_, 0, UINT8_MAX))) |
           ((static_cast<uint32_t>(std::clamp<int16_t>(red_, 0, UINT8_MAX))) << 24) |    // 24 red shift
           ((static_cast<uint32_t>(std::clamp<int16_t>(green_, 0, UINT8_MAX))) << 16) |  // 16 green shift
           ((static_cast<uint32_t>(std::clamp<int16_t>(blue_, 0, UINT8_MAX))) << 8);     // 8 blue shift
}

RSColor RSColor::FromRgbaInt(uint32_t rgba)
{
    return RSColor(rgba);
}

uint32_t RSColor::AsArgbInt() const
{
    return ((static_cast<uint32_t>(std::clamp<int16_t>(alpha_, 0, UINT8_MAX))) << 24) |   // 24 alpha shift
           ((static_cast<uint32_t>(std::clamp<int16_t>(red_, 0, UINT8_MAX))) << 16) |     // 16 red shift
           ((static_cast<uint32_t>(std::clamp<int16_t>(green_, 0, UINT8_MAX))) << 8) |    // 8 green shift
           (static_cast<uint32_t>(std::clamp<int16_t>(blue_, 0, UINT8_MAX)));
}

RSColor RSColor::FromArgbInt(uint32_t argb)
{
    uint32_t alpha = (argb >> 24) & 0xFF;               // 24 is alpha shift in ARGB, 0xFF is a byte mask
    return RSColor(((argb << 8) & 0xFFFFFF00) | alpha); // 8 is rgb shift, 0xFFFFFF00 is a rgb mask
}

uint32_t RSColor::AsBgraInt() const
{
    return (static_cast<uint32_t>(std::clamp<int16_t>(alpha_, 0, UINT8_MAX))) |
           ((static_cast<uint32_t>(std::clamp<int16_t>(red_, 0, UINT8_MAX))) << 8) |      // 8 red shift
           ((static_cast<uint32_t>(std::clamp<int16_t>(green_, 0, UINT8_MAX))) << 16) |   // 16 green shift
           ((static_cast<uint32_t>(std::clamp<int16_t>(blue_, 0, UINT8_MAX))) << 24);     // 24 blue shift
}

RSColor RSColor::FromBgraInt(uint32_t bgra)
{
    union {
        struct {
            uint8_t blu_ : 8;
            uint8_t gre_ : 8;
            uint8_t red_ : 8;
            uint8_t alp_ : 8;
        };
        uint32_t bgra_;
    } color;
    color.bgra_ = bgra;
    return RSColor(color.red_, color.gre_, color.blu_, color.alp_);
}

int16_t RSColor::GetBlue() const
{
    return blue_;
}

int16_t RSColor::GetGreen() const
{
    return green_;
}

int16_t RSColor::GetRed() const
{
    return red_;
}

int16_t RSColor::GetAlpha() const
{
    return alpha_;
}

void RSColor::SetBlue(int16_t blue)
{
    blue_ = blue;
}

void RSColor::SetGreen(int16_t green)
{
    green_ = green;
}

void RSColor::SetRed(int16_t red)
{
    red_ = red;
}

void RSColor::SetAlpha(int16_t alpha)
{
    alpha_ = alpha;
}

void RSColor::MultiplyAlpha(float alpha)
{
    alpha_ = static_cast<int16_t>(alpha_ * std::clamp(alpha, 0.0f, 1.0f));
}

void RSColor::Dump(std::string& out) const
{
    constexpr int32_t colorStrWidth = 8;
    std::stringstream ss;
    ss << "[RGBA-0x" << std::hex << std::setfill('0') << std::setw(colorStrWidth) << std::uppercase;
    ss << AsRgbaInt() << ']';
    out += ss.str();
}
} // namespace Rosen
} // namespace OHOS
