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
#include "property/rs_color_picker_def.h"

#ifndef ROSEN_CROSS_PLATFORM
#include "color_space_convertor.h"
#endif
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

RSColor::RSColor(int16_t red, int16_t green, int16_t blue, int16_t alpha, GraphicColorGamut colorSpace) noexcept
{
    if (colorSpace == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        alphaF_ = Float32ToFloat16(std::clamp<float>(
            static_cast<float>(alpha) / static_cast<float>(RGB_MAX_VALUE), 0.0f, 1.0f));
        redF_ = Float32ToFloat16(std::clamp<float>(
            static_cast<float>(red) / static_cast<float>(RGB_MAX_VALUE), 0.0f, 1.0f));
        greenF_ = Float32ToFloat16(std::clamp<float>(
            static_cast<float>(green) / static_cast<float>(RGB_MAX_VALUE), 0.0f, 1.0f));
        blueF_ = Float32ToFloat16(std::clamp<float>(
            static_cast<float>(blue) / static_cast<float>(RGB_MAX_VALUE), 0.0f, 1.0f));
    } else {
        alpha_ = alpha;
        red_ = red;
        green_ = green;
        blue_ = blue;
    }
    colorSpace_ = static_cast<int8_t>(colorSpace);
}

RSColor::RSColor(float red, float green, float blue, float alpha,
                 GraphicColorGamut colorSpace, float headroom) noexcept
{
    if (colorSpace == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        alphaF_ = Float32ToFloat16(std::clamp<float>(alpha, 0.0f, 1.0f));
        redF_ = Float32ToFloat16(std::clamp<float>(red, 0.0f, 1.0f));
        greenF_ = Float32ToFloat16(std::clamp<float>(green, 0.0f, 1.0f));
        blueF_ = Float32ToFloat16(std::clamp<float>(blue, 0.0f, 1.0f));
    } else {
        alpha_ = std::clamp<int16_t>(
            static_cast<int16_t>(round(alpha * RGB_MAX_VALUE)), 0, UINT8_MAX);
        red_ = std::clamp<int16_t>(
            static_cast<int16_t>(round(red * RGB_MAX_VALUE)), 0, UINT8_MAX);
        green_ = std::clamp<int16_t>(
            static_cast<int16_t>(round(green * RGB_MAX_VALUE)), 0, UINT8_MAX);
        blue_ = std::clamp<int16_t>(
            static_cast<int16_t>(round(blue * RGB_MAX_VALUE)), 0, UINT8_MAX);
    }
    colorSpace_ = static_cast<int8_t>(colorSpace);
    if (headroom < 1.0f) {
        headroom_ = Float32ToFloat16(1.0f);
    } else {
        headroom_ = Float32ToFloat16(headroom);
    }
}

bool RSColor::operator==(const RSColor& rhs) const
{
    return red_ == rhs.red_ && green_ == rhs.green_ && blue_ == rhs.blue_ && alpha_ == rhs.alpha_ &&
           colorSpace_ == rhs.colorSpace_ && placeholder_ == rhs.placeholder_ &&
           OHOS::ColorManager::FloatNearlyEqual(GetHeadroom(), Float16ToFloat32(rhs.headroom_));
}

bool RSColor::IsNearEqual(const RSColor& other, int16_t threshold) const
{
    return (std::abs(red_ - other.red_) <= threshold) && (std::abs(green_ - other.green_) <= threshold) &&
           (std::abs(blue_ - other.blue_) <= threshold) && (std::abs(alpha_ - other.alpha_) <= threshold) &&
           (colorSpace_ == other.colorSpace_) && (placeholder_ == other.placeholder_) &&
           OHOS::ColorManager::FloatNearlyEqual(GetHeadroom(), Float16ToFloat32(other.headroom_));
}

RSColor RSColor::operator+(const RSColor& rhs) const
{
    if (UNLIKELY(placeholder_ != 0)) {
        return *this;
    }
    if (UNLIKELY(rhs.placeholder_ != 0)) {
        return rhs;
    }
    RSColor bigColor(rhs);
    RSColor smallColor(*this);
    if (smallColor.GetHeadroom() > bigColor.GetHeadroom()) {
        bigColor = *this;
        smallColor = rhs;
    }
    if (bigColor.GetHeadroom() < 1.0f || smallColor.GetHeadroom() < 1.0f) {
        return rhs;
    }
    if (bigColor.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020 ||
        smallColor.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        bigColor.ConvertToBT2020ColorSpace();
        smallColor.ConvertToBT2020ColorSpace();
        return RSColor(
            smallColor.GetRedF() * smallColor.GetHeadroom() / bigColor.GetHeadroom() + bigColor.GetRedF(),
            smallColor.GetGreenF() * smallColor.GetHeadroom() / bigColor.GetHeadroom() + bigColor.GetGreenF(),
            smallColor.GetBlueF() * smallColor.GetHeadroom() / bigColor.GetHeadroom() + bigColor.GetBlueF(),
            smallColor.GetAlphaF() * smallColor.GetHeadroom() / bigColor.GetHeadroom() + bigColor.GetAlphaF(),
            GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, bigColor.GetHeadroom());
    } else {
        auto colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
        if (bigColor.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3 ||
            smallColor.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3) {
            bigColor.ConvertToP3ColorSpace();
            smallColor.ConvertToP3ColorSpace();
            colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
        }
        RSColor color = RSColor(
            smallColor.GetRed() * smallColor.GetHeadroom() / bigColor.GetHeadroom() + bigColor.GetRed(),
            smallColor.GetGreen() * smallColor.GetHeadroom() / bigColor.GetHeadroom() + bigColor.GetGreen(),
            smallColor.GetBlue() * smallColor.GetHeadroom() / bigColor.GetHeadroom() + bigColor.GetBlue(),
            smallColor.GetAlpha() * smallColor.GetHeadroom() / bigColor.GetHeadroom() + bigColor.GetAlpha(),
            colorSpace);
        color.SetHeadroom(bigColor.GetHeadroom());
        return color;
    }
}

RSColor RSColor::operator-(const RSColor& rhs) const
{
    if (UNLIKELY(placeholder_ != 0)) {
        return *this;
    }
    if (UNLIKELY(rhs.placeholder_ != 0)) {
        return rhs;
    }
    RSColor lhsColor(*this);
    RSColor rhsColor(rhs);
    if (lhsColor.GetHeadroom() < 1.0f || rhsColor.GetHeadroom() < 1.0f) {
        return rhs;
    }
    auto colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    if (lhsColor.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020 ||
        rhsColor.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        lhsColor.ConvertToBT2020ColorSpace();
        rhsColor.ConvertToBT2020ColorSpace();
        colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020;
    } else if (lhsColor.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3 ||
        rhsColor.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3) {
        lhsColor.ConvertToP3ColorSpace();
        rhsColor.ConvertToP3ColorSpace();
        colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    }
    if (rhsColor.GetHeadroom() > lhsColor.GetHeadroom()) {
        return RSColor(
            lhsColor.GetRedF() * lhsColor.GetHeadroom() / rhsColor.GetHeadroom() - rhsColor.GetRedF(),
            lhsColor.GetGreenF() * lhsColor.GetHeadroom() / rhsColor.GetHeadroom() - rhsColor.GetGreenF(),
            lhsColor.GetBlueF() * lhsColor.GetHeadroom() / rhsColor.GetHeadroom() - rhsColor.GetBlueF(),
            lhsColor.GetAlphaF() * lhsColor.GetHeadroom() / rhsColor.GetHeadroom() - rhsColor.GetAlphaF(),
            colorSpace, rhsColor.GetHeadroom());
    } else {
        return RSColor(
            lhsColor.GetRedF() - rhsColor.GetRedF() * rhsColor.GetHeadroom() / lhsColor.GetHeadroom(),
            lhsColor.GetGreenF() - rhsColor.GetGreenF() * rhsColor.GetHeadroom() / lhsColor.GetHeadroom(),
            lhsColor.GetBlueF() - rhsColor.GetBlueF() * rhsColor.GetHeadroom() / lhsColor.GetHeadroom(),
            lhsColor.GetAlphaF() - rhsColor.GetAlphaF() * rhsColor.GetHeadroom() / lhsColor.GetHeadroom(),
            colorSpace, lhsColor.GetHeadroom());
    }
}

RSColor RSColor::operator*(float scale) const
{
    if (UNLIKELY(placeholder_ != 0)) {
        return *this;
    }
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return RSColor(GetRedF() * scale, GetGreenF() * scale, GetBlueF() * scale, GetAlphaF() * scale,
            GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, GetHeadroom());
    }
    RSColor color = RSColor(round(red_ * scale), round(green_ * scale), round(blue_ * scale), round(alpha_ * scale));
    color.SetHeadroom(GetHeadroom());
    return color;
}

RSColor& RSColor::operator*=(float scale)
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        redF_ = Float32ToFloat16(std::clamp<float>(GetRedF() * scale, 0.0f, 1.0f));
        greenF_ = Float32ToFloat16(std::clamp<float>(GetGreenF() * scale, 0.0f, 1.0f));
        blueF_ = Float32ToFloat16(std::clamp<float>(GetBlueF() * scale, 0.0f, 1.0f));
        alphaF_ = Float32ToFloat16(std::clamp<float>(GetAlphaF() * scale, 0.0f, 1.0f));
    } else {
        red_ = std::clamp<int16_t>(round(red_ * scale), 0, UINT8_MAX);
        green_ = std::clamp<int16_t>(round(green_ * scale), 0, UINT8_MAX);
        blue_ = std::clamp<int16_t>(round(blue_ * scale), 0, UINT8_MAX);
        alpha_ = std::clamp<int16_t>(round(alpha_ * scale), 0, UINT8_MAX);
    }
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
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return (static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetAlphaF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) |
               ((static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetRedF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) << 24) |   // 24 red
               ((static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetGreenF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) << 16) | // 16 green
               ((static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetBlueF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) << 8);    // 8 blue
    }
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
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return ((static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetAlphaF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) << 24) |   // 24 alpha
               ((static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetRedF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) << 16) |     // 16 red
               ((static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetGreenF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) << 8) |    // 8 green
               (static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetBlueF() * RGB_MAX_VALUE)), 0, UINT8_MAX)));
    }
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
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return (static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetAlphaF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) |
               ((static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetRedF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) << 8) |      // 8 red
               ((static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetGreenF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) << 16) |   // 16 green
               ((static_cast<uint32_t>(std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetBlueF() * RGB_MAX_VALUE)), 0, UINT8_MAX))) << 24);     // 24 blue
    }
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
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetBlueF() * RGB_MAX_VALUE)), 0, UINT8_MAX);
    }
    return blue_;
}

int16_t RSColor::GetGreen() const
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetGreenF() * RGB_MAX_VALUE)), 0, UINT8_MAX);
    }
    return green_;
}

int16_t RSColor::GetRed() const
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetRedF() * RGB_MAX_VALUE)), 0, UINT8_MAX);
    }
    return red_;
}

int16_t RSColor::GetAlpha() const
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return std::clamp<int16_t>(
                    static_cast<int16_t>(round(GetAlphaF() * RGB_MAX_VALUE)), 0, UINT8_MAX);
    }
    return alpha_;
}

scalar RSColor::GetRedF() const
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return static_cast<scalar>(Float16ToFloat32(redF_));
    }
    return static_cast<scalar>(red_) / RGB_MAX_VALUE;
}

scalar RSColor::GetGreenF() const
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return static_cast<scalar>(Float16ToFloat32(greenF_));
    }
    return static_cast<scalar>(green_) / RGB_MAX_VALUE;
}

scalar RSColor::GetBlueF() const
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return static_cast<scalar>(Float16ToFloat32(blueF_));
    }
    return static_cast<scalar>(blue_) / RGB_MAX_VALUE;
}

scalar RSColor::GetAlphaF() const
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return static_cast<scalar>(Float16ToFloat32(alphaF_));
    }
    return static_cast<scalar>(alpha_) / RGB_MAX_VALUE;
}

Drawing::Color4f RSColor::GetColor4f() const
{
    Drawing::Color4f color4f;
    color4f.redF_ = GetRedF();
    color4f.greenF_ = GetGreenF();
    color4f.blueF_ = GetBlueF();
    color4f.alphaF_ = GetAlphaF();
    return color4f;
}

GraphicColorGamut RSColor::GetColorSpace() const
{
    return static_cast<GraphicColorGamut>(colorSpace_);
}

void RSColor::SetBlue(int16_t blue)
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        blueF_ = Float32ToFloat16(std::clamp<float>(
            static_cast<float>(blue) / static_cast<float>(RGB_MAX_VALUE), 0.0f, 1.0f));
    } else {
        blue_ = blue;
    }
}

void RSColor::SetGreen(int16_t green)
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        greenF_ = Float32ToFloat16(std::clamp<float>(
            static_cast<float>(green) / static_cast<float>(RGB_MAX_VALUE), 0.0f, 1.0f));
    } else {
        green_ = green;
    }
}

void RSColor::SetRed(int16_t red)
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        redF_ = Float32ToFloat16(std::clamp<float>(
            static_cast<float>(red) / static_cast<float>(RGB_MAX_VALUE), 0.0f, 1.0f));
    } else {
        red_ = red;
    }
}

void RSColor::SetAlpha(int16_t alpha)
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        alphaF_ = Float32ToFloat16(std::clamp<float>(
            static_cast<float>(alpha) / static_cast<float>(RGB_MAX_VALUE), 0.0f, 1.0f));
    } else {
        alpha_ = alpha;
    }
}

void RSColor::SetColorSpace(const GraphicColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

void RSColor::MultiplyAlpha(float alpha)
{
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        alphaF_ = Float32ToFloat16(GetAlphaF() * std::clamp(alpha, 0.0f, 1.0f));
    } else {
        alpha_ = static_cast<int16_t>(alpha_ * std::clamp(alpha, 0.0f, 1.0f));
    }
}

void RSColor::ConvertToP3ColorSpace()
{
#ifndef ROSEN_CROSS_PLATFORM
    if (colorSpace_ == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3) {
        return;
    }
    OHOS::ColorManager::Vector3 rgbF = {GetRedF(), GetGreenF(), GetBlueF()};
    OHOS::ColorManager::Vector3 rgbInP3 = rgbF;
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        rgbInP3 = OHOS::ColorManager::ColorSpaceConvertor::ConvertBT2020ToP3ColorSpace(rgbF);
    } else {
        rgbInP3 = OHOS::ColorManager::ColorSpaceConvertor::ConvertSRGBToP3ColorSpace(rgbF);
    }
    red_ = static_cast<int16_t>(round(rgbInP3[COLOR_ARRAY_RED_INDEX] * RGB_MAX_VALUE));
    green_ = static_cast<int16_t>(round(rgbInP3[COLOR_ARRAY_GREEN_INDEX] * RGB_MAX_VALUE));
    blue_ = static_cast<int16_t>(round(rgbInP3[COLOR_ARRAY_BLUE_INDEX] * RGB_MAX_VALUE));
    colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
#endif
}

void RSColor::ConvertToSRGBColorSpace()
{
#ifndef ROSEN_CROSS_PLATFORM
    if (colorSpace_ == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB) {
        return;
    }
    OHOS::ColorManager::Vector3 rgbF = {GetRedF(), GetGreenF(), GetBlueF()};
    OHOS::ColorManager::Vector3 rgbInSRGB = rgbF;
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        rgbInSRGB = OHOS::ColorManager::ColorSpaceConvertor::ConvertBT2020ToSRGBColorSpace(rgbF);
    } else {
        rgbInSRGB = OHOS::ColorManager::ColorSpaceConvertor::ConvertP3ToSRGBColorSpace(rgbF);
    }
    red_ = static_cast<int16_t>(round(rgbInSRGB[COLOR_ARRAY_RED_INDEX] * RGB_MAX_VALUE));
    green_ = static_cast<int16_t>(round(rgbInSRGB[COLOR_ARRAY_GREEN_INDEX] * RGB_MAX_VALUE));
    blue_ = static_cast<int16_t>(round(rgbInSRGB[COLOR_ARRAY_BLUE_INDEX] * RGB_MAX_VALUE));
    colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
#endif
}

void RSColor::ConvertToBT2020ColorSpace()
{
#ifndef ROSEN_CROSS_PLATFORM
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020) {
        return;
    }
    OHOS::ColorManager::Vector3 rgbF = {GetRedF(), GetGreenF(), GetBlueF()};
    OHOS::ColorManager::Vector3 rgbInBT2020 = rgbF;
    if (GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3) {
        rgbInBT2020 = OHOS::ColorManager::ColorSpaceConvertor::ConvertP3ToBT2020ColorSpace(rgbF);
    } else {
        rgbInBT2020 = OHOS::ColorManager::ColorSpaceConvertor::ConvertSRGBToBT2020ColorSpace(rgbF);
    }
    redF_ = Float32ToFloat16(rgbInBT2020[COLOR_ARRAY_RED_INDEX]);
    greenF_ = Float32ToFloat16(rgbInBT2020[COLOR_ARRAY_GREEN_INDEX]);
    blueF_ = Float32ToFloat16(rgbInBT2020[COLOR_ARRAY_BLUE_INDEX]);
    alphaF_ = Float32ToFloat16(GetAlphaF());
    colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020;
#endif
}

Drawing::Color RSColor::ConvertToDrawingColor() const
{
    auto color = Drawing::Color(AsArgbInt());
    color.SetPlaceholder(GetPlaceholder());
    return color;
}

void RSColor::Dump(std::string& out) const
{
    constexpr int32_t colorStrWidth = 8;
    std::stringstream ss;
    ss << "[RGBA-0x" << std::hex << std::setfill('0') << std::setw(colorStrWidth) << std::uppercase;
    ss << AsRgbaInt() << ',';
    out += ss.str();
    switch (colorSpace_) {
        case GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB:
            out += " colorSpace: SRGB]";
            break;
        case GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3:
            out += " colorSpace: DISPLAY_P3]";
            break;
        case GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020:
            out += " colorSpace: BT2020]";
            break;
        default:
            out += " colorSpace: OTHER]";
            break;
    }
    if (IsPlaceholder()) {
        out += " placeholder[" + std::to_string(placeholder_) + "]";
    }
    if (!OHOS::ColorManager::FloatNearlyEqual(GetHeadroom(), 1.0f)) {
        out += " headroom[" + std::to_string(GetHeadroom()) + "]";
    }
}
ColorPlaceholder RSColor::GetPlaceholder() const
{
    return static_cast<ColorPlaceholder>(placeholder_);
}
bool RSColor::IsPlaceholder() const
{
    return placeholder_ != 0;
}
void RSColor::SetPlaceholder(ColorPlaceholder ph)
{
    placeholder_ = static_cast<uint8_t>(ph);
}

float RSColor::GetHeadroom() const
{
    return Float16ToFloat32(headroom_);
}

void RSColor::SetHeadroom(float headroom)
{
    if (headroom < 1.0f) {
        headroom_ = Float32ToFloat16(1.0f);
        return;
    }
    headroom_ = Float32ToFloat16(headroom);
}

float RSColor::Float16ToFloat32(float16 half) const
{
    int sign = (half & 0x8000) ? -1 : 1; //checking the most significant bit (MSB) of the half variable
    int exponent = (half & 0x7C00) >> 10; // isolate the exponent bits in the half-precision floating-point format
    int mantissa = half & 0x03FF; // isolate the mantissa bits in the half-precision floating-point format

    if (exponent == 0) {
        return sign * std::ldexp(static_cast<float>(mantissa), -24); // 24:offset for float16 subnormal
    } else if (exponent == 31) { // 31:Max value for a 5-bit exponent in float16, representing infinity or NaN
        return mantissa ? std::numeric_limits<float>::quiet_NaN() : sign * std::numeric_limits<float>::infinity();
    } else {
        return sign * std::ldexp(static_cast<float>(mantissa | 0x0400), exponent -25); // 25:offset for float16 normal
    }
}

float16 RSColor::Float32ToFloat16(float value) const
{
    int f = *reinterpret_cast<int *>(&value);
    int sign = (f >> 16) & 0x8000; //checking the most significant bit (MSB) of the half variable
    int exponent = ((f >> 23) & 0xFF) - (127 -15); //127:Bias for float32, 15:Bias for float16
    int mantissa = f & 0x007FFFFF;  // isolate the exponent bits

    if (exponent <= 0) {
        if (exponent < -10) { // 10:Smallest representable exponent in float16 subnormals
            return sign;
        }
        mantissa = (mantissa | 0x00800000) >> (1 - exponent);  // isolate the exponent bits
        return sign | (mantissa >> 13); // 13:Shift from 23-bit to 10-bit mantissa for float16
    } else if (exponent == 0xFF - (127 -15)) { // 127:Bias for float32, 15:Bias for float16
        return sign | 0x7C00 | (mantissa ? 1 : 0); // isolate the exponent bits
    } else {
        if (exponent > 30) { // 30:Max exponent value for float16
            return sign | 0x7C00;  // isolate the exponent bits
        }
        return sign | (exponent << 10) | (mantissa >> 13); // 10:Shift for exponent in float16, 13:23-bit to 10-bit
    }
}
} // namespace Rosen
} // namespace OHOS
