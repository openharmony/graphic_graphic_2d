/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "color_picker.h"
#include "hilog/log.h"
#include "effect_errors.h"
#include "effect_utils.h"
#include "color.h"
#include "pixel_map.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkRect.h"
#include "include/core/SkImageFilter.h"
#include "include/effects/SkImageFilters.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkFont.h"
#include "include/core/SkTypeface.h"
#include <algorithm>
#include <cmath>
#include <utility>

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using OHOS::HiviewDFX::HiLog;

std::shared_ptr<Media::PixelMap> ColorPicker::CreateScaledPixelMap(const std::shared_ptr<Media::PixelMap>& pixmap)
{
    // Create scaled pixelmap
    if (pixmap == nullptr) {
        EFFECT_LOG_E("[ColorPicker]failed to create ScaledPixelMap with null pixmap.");
        return nullptr;
    }
    OHOS::Media::InitializationOptions options;
    options.alphaType = pixmap->GetAlphaType();
    options.pixelFormat = pixmap->GetPixelFormat();
    options.scaleMode = OHOS::Media::ScaleMode::FIT_TARGET_SIZE;
    options.size.width = 100;  // 100 represents scaled pixelMap's width
    options.size.height = 100; // 100 represents scaled pixelMap's height
    options.editable = true;
    std::unique_ptr<Media::PixelMap> newPixelMap = Media::PixelMap::Create(*pixmap.get(), options);
    return std::move(newPixelMap);
}

std::shared_ptr<ColorPicker> ColorPicker::CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap,
    uint32_t &errorCode)
{
    if (pixmap == nullptr) {
        EFFECT_LOG_E("[ColorPicker]failed to create ColorPicker with null pixmap.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }
    
    std::shared_ptr<Media::PixelMap> scaledPixelMap = CreateScaledPixelMap(pixmap);
    if (scaledPixelMap == nullptr) {
        EFFECT_LOG_E("[ColorPicker]failed to scale pixelmap, scaledPixelMap is nullptr.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }
    std::shared_ptr<ColorPicker> colorPicker = std::make_shared<ColorPicker>(scaledPixelMap);
    errorCode = SUCCESS;
    return colorPicker;
}

std::shared_ptr<ColorPicker> ColorPicker::CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap,
    double* coordinates, uint32_t &errorCode)
{
    if (pixmap == nullptr) {
        EFFECT_LOG_E("[ColorPicker]failed to create ColorPicker with null pixmap.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> scaledPixelMap = CreateScaledPixelMap(pixmap);
    std::shared_ptr<ColorPicker> colorPicker = std::make_shared<ColorPicker>(scaledPixelMap, coordinates);
    errorCode = SUCCESS;
    return colorPicker;
}

std::shared_ptr<Media::PixelMap> ColorPicker::GetScaledPixelMap()
{
    // Create scaled pixelmap
    OHOS::Media::InitializationOptions options;
    options.alphaType = pixelmap_->GetAlphaType();
    options.pixelFormat = pixelmap_->GetPixelFormat();
    options.scaleMode = OHOS::Media::ScaleMode::FIT_TARGET_SIZE;
    options.size.width = 1;
    options.size.height = 1;
    options.editable = true;
    std::unique_ptr<Media::PixelMap> newPixelMap = Media::PixelMap::Create(*pixelmap_.get(), options);
    return std::move(newPixelMap);
}

uint32_t ColorPicker::GetMainColor(ColorManager::Color &color)
{
    if (pixelmap_ == nullptr) {
        return ERR_EFFECT_INVALID_VALUE;
    }

    // get color
    uint32_t colorVal = 0;
    int x = 0;
    int y = 0;
    std::shared_ptr<Media::PixelMap> pixelMap = GetScaledPixelMap();
    if (pixelMap == nullptr) {
        EFFECT_LOG_E("ColorPicker::GetMainColor pixelMap is nullptr");
        return ERR_EFFECT_INVALID_VALUE;
    }
    bool bSucc = false;
    if (format_ == Media::PixelFormat::RGBA_1010102) {
        bSucc = pixelMap->GetRGBA1010102Color(x, y, colorVal);
        float r = static_cast<float>(GetRGBA1010102ColorR(colorVal)) / RGB101010_COLOR_MAX;
        float g = static_cast<float>(GetRGBA1010102ColorG(colorVal)) / RGB101010_COLOR_MAX;
        float b = static_cast<float>(GetRGBA1010102ColorB(colorVal)) / RGB101010_COLOR_MAX;
        float a = static_cast<float>(GetRGBA1010102ColorA(colorVal)) / RGB101010_ALPHA_MAX;
        color = ColorManager::Color(r, g, b, a);
    } else {
        bSucc = pixelMap->GetARGB32Color(x, y, colorVal);
        color = ColorManager::Color(colorVal);
    }
    EFFECT_LOG_I("[newpix].argb.ret=%{public}d, %{public}x", bSucc, colorVal);
    if (!bSucc) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    return SUCCESS;
}

ColorPicker::ColorPicker(std::shared_ptr<Media::PixelMap> pixmap):ColorExtract(pixmap) {}
ColorPicker::ColorPicker(
    std::shared_ptr<Media::PixelMap> pixmap, double* coordinates):ColorExtract(pixmap, coordinates) {}

uint32_t ColorPicker::GetLargestProportionColor(ColorManager::Color &color) const
{
    if (featureColors_.empty()) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    color = ColorManager::Color(featureColors_[0].first | 0xFF000000); // alpha = oxFF
    return SUCCESS;
}

uint32_t ColorPicker::GetHighestSaturationColor(ColorManager::Color &color) const
{
    if (featureColors_.empty()) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    uint32_t colorPicked = 0;
    HSV hsv = {0};
    double maxSaturation = 0.0;
    for (size_t i = 0; i < featureColors_.size(); i++) {
        hsv = RGB2HSV(featureColors_[i].first);
        if (hsv.s >= maxSaturation) {
            maxSaturation = hsv.s;
            colorPicked = featureColors_[i].first;
        }
    }
    color = ColorManager::Color(colorPicked | 0xFF000000);
    return SUCCESS;
}

uint32_t ColorPicker::GetAverageColor(ColorManager::Color &color) const
{
    uint32_t colorPicked = 0;
    uint32_t redSum = 0;
    uint32_t greenSum = 0;
    uint32_t blueSum = 0;
    int totalPixelNum = 0;
    if (featureColors_.empty()) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    for (size_t i = 0; i < featureColors_.size(); i++) {
        totalPixelNum += featureColors_[i].second;
        redSum += featureColors_[i].second * ((featureColors_[i].first >> ARGB_R_SHIFT) & ARGB_MASK);
        greenSum += featureColors_[i].second * ((featureColors_[i].first >> ARGB_G_SHIFT) & ARGB_MASK);
        blueSum += featureColors_[i].second * ((featureColors_[i].first >> ARGB_B_SHIFT) & ARGB_MASK);
    }
    if (totalPixelNum == 0) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    uint32_t redMean = round(redSum / (float)totalPixelNum);
    uint32_t greenMean = round(greenSum / (float)totalPixelNum);
    uint32_t blueMean = round(blueSum / (float)totalPixelNum);
    colorPicked = redMean << ARGB_R_SHIFT | greenMean << ARGB_G_SHIFT | blueMean << ARGB_B_SHIFT;
    color = ColorManager::Color(colorPicked | 0xFF000000);
    return SUCCESS;
}

bool ColorPicker::IsBlackOrWhiteOrGrayColor(uint32_t color) const
{
    HSV hsv = RGB2HSV(color);
    // A color is black, white or gray colr when its hsv satisfy (v>30, s<=5) or (15<v<=30, s<=15) or (v<=15).
    if ((hsv.v > 30 && hsv.s <= 5) || (hsv.v > 15 && hsv.v <= 30 && hsv.s <= 15) || (hsv.v <= 15)) {
        return true;
    }
    return false;
}

// Transfrom rgb to ligthtness
uint32_t ColorPicker::RGB2GRAY(uint32_t color) const
{
    uint32_t r = GetARGB32ColorR(color);
    uint32_t g = GetARGB32ColorG(color);
    uint32_t b = GetARGB32ColorB(color);
    return static_cast<uint32_t>(r * GRAY_RATIO_RED + g * GRAY_RATIO_GREEN + b * GRAY_RATIO_BLUE);
}

// Calculate Lightness Variance
uint32_t ColorPicker::CalcGrayVariance() const
{
    long long int grayVariance = 0;

    ColorManager::Color color;
    bool rst = GetAverageColor(color);
    if (rst != SUCCESS) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    uint32_t averageColor = ((color.PackValue() >> 32) & 0xFFFFFFFF);
    uint32_t averageGray = RGB2GRAY(averageColor);
    for (size_t i = 0; i < featureColors_.size(); i++) {
        // 2 is square
        grayVariance += pow(static_cast<long long int>(RGB2GRAY(featureColors_[i].first)) - averageGray, 2) *
                        featureColors_[i].second;
    }
    grayVariance /= colorValLen_;
    return static_cast<uint32_t>(grayVariance);
}

// Relative luminance calculation, normalized to 0 - 1
double ColorPicker::CalcRelaticeLuminance(uint32_t color) const
{
    uint32_t r = GetARGB32ColorR(color);
    uint32_t g = GetARGB32ColorG(color);
    uint32_t b = GetARGB32ColorB(color);
    return (r * LUMINANCE_RATIO_RED + g * LUMINANCE_RATIO_GREEN + b * LUMINANCE_RATIO_BLUE) / 255; // 255 is max value.
}

double ColorPicker::CalcContrastRatioWithWhite() const
{
    double lightColorDegree = 0;
    for (size_t i = 0; i < featureColors_.size(); i++) {
        // 0.05 is used to calculate contrast ratio.
        lightColorDegree += (((1 + 0.05) / (CalcRelaticeLuminance(featureColors_[i].first) + 0.05))
                            * featureColors_[i].second);
    }
    lightColorDegree /= colorValLen_;
    return lightColorDegree;
}

// Discriminate wallpaper color shade mode
uint32_t ColorPicker::DiscriminatePitureLightDegree(PictureLightColorDegree &degree) const
{
    if (featureColors_.empty()) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    uint32_t grayVariance = grayMsd_;
    // Gray variance less than 6000 means not extremely flowery picture.
    if (grayVariance < 6000) {
        double lightColorDegree = contrastToWhite_;
        // LightColorDegree less than 1.5 means extremely light color picture.
        if (lightColorDegree < 1.5) {
            degree = EXTREMELY_LIGHT_COLOR_PICTURE;
        // LightColorDegree between 1.5 and 1.9 means light color picture.
        } else if (lightColorDegree >= 1.5 && lightColorDegree < 1.9) {
            degree = LIGHT_COLOR_PICTURE;
        // LightColorDegree between 1.9 and 7 means flowery picture.
        } else if (lightColorDegree >= 1.9 && lightColorDegree <= 7) {
            degree = FLOWERY_PICTURE;
        } else {
            // GrayVariance more than 3000 means dark color picture.
            if (grayVariance >= 3000) {
                degree = DARK_COLOR_PICTURE;
            } else {
                degree = EXTREMELY_DARK_COLOR_PICTURE;
            }
        }
    } else {
        degree = EXTREMELY_FLOWERY_PICTURE;
    }
    return SUCCESS;
}

// Reverse picture color
uint32_t ColorPicker::GetReverseColor(ColorManager::Color &color) const
{
    PictureLightColorDegree lightColorDegree;
    bool rst = DiscriminatePitureLightDegree(lightColorDegree);
    if (rst != SUCCESS) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    if (lightColorDegree == EXTREMELY_LIGHT_COLOR_PICTURE) {
        uint32_t black = 0xFF000000;
        color = ColorManager::Color(black);
        return SUCCESS;
    } else {
        uint32_t white = 0xFFFFFFFF;
        color = ColorManager::Color(white);
        return SUCCESS;
    }
};

void ColorPicker::GenerateMorandiBackgroundColor(HSV& hsv) const
{
    hsv.s = 9; // 9 is morandi background color's saturation.
    hsv.v = 84; // 84 is morandi background color's value.
    return;
}

// Get morandi background color
uint32_t ColorPicker::GetMorandiBackgroundColor(ColorManager::Color &color) const
{
    bool rst = GetLargestProportionColor(color);
    if (rst != SUCCESS) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    uint32_t dominantColor = ((color.PackValue() >> 32) & 0xFFFFFFFF);
    HSV hsv = RGB2HSV(dominantColor);
    bool isBWGColor = IsBlackOrWhiteOrGrayColor(dominantColor);
    if (isBWGColor) {
        uint32_t nextDominantColor = 0;
        uint32_t nextDominantColorCnt = 0;
        bool existColor = false;
        for (size_t i = 0; i < featureColors_.size(); i++) {
            if (!IsBlackOrWhiteOrGrayColor(featureColors_[i].first) &&
                featureColors_[i].second > nextDominantColorCnt) {
                nextDominantColor = featureColors_[i].first;
                nextDominantColorCnt = featureColors_[i].second;
                existColor = true;
            }
        }
        if (existColor) {
            HSV nextColorHsv = RGB2HSV(nextDominantColor);
            GenerateMorandiBackgroundColor(nextColorHsv);
            nextDominantColor = HSVtoRGB(nextColorHsv);
            color = ColorManager::Color(nextDominantColor | 0xFF000000);
            return SUCCESS;
        } else {
            hsv.s = 0;
            hsv.v = 77; // Adjust value to 77.
            dominantColor = HSVtoRGB(hsv);
            color = ColorManager::Color(dominantColor | 0xFF000000);
            return SUCCESS;
        }
    } else {
        GenerateMorandiBackgroundColor(hsv);
        dominantColor = HSVtoRGB(hsv);
        color = ColorManager::Color(dominantColor | 0xFF000000);
        return SUCCESS;
    }
}

void ColorPicker::GenerateMorandiShadowColor(HSV& hsv) const
{
    // When hue between 20 and 60, adjust s and v.
    if (hsv.h > 20 && hsv.h <= 60) {
        hsv.s = 53; // Adjust saturation to 53.
        hsv.v = 46; // Adjust value to 46.
    // When hue between 60 and 190, adjust s and v.
    } else if (hsv.h > 60 && hsv.h <= 190) {
        hsv.s = 23; // Adjust saturation to 23.
        hsv.v = 36; // Adjust value to 36.
    // When hue between 190 and 270, adjust s and v.
    } else if (hsv.h > 190 && hsv.h <= 270) {
        hsv.s = 34; // Adjust saturation to 34.
        hsv.v = 35; // Adjust value to 35.
    } else {
        hsv.s = 48; // Adjust saturation to 48.
        hsv.v = 40; // Adjust value to 40.
    }
}

// Get morandi shadow color
uint32_t ColorPicker::GetMorandiShadowColor(ColorManager::Color &color) const
{
    bool rst = GetLargestProportionColor(color);
    if (rst != SUCCESS) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    uint32_t dominantColor = ((color.PackValue() >> 32) & 0xFFFFFFFF);

    HSV hsv = RGB2HSV(dominantColor);
    bool isBWGColor = IsBlackOrWhiteOrGrayColor(dominantColor);
    if (isBWGColor) {
        uint32_t nextDominantColor = 0;
        uint32_t nextDominantColorCnt = 0;
        bool existColor = false;
        for (size_t i = 0; i < featureColors_.size(); i++) {
            if (!IsBlackOrWhiteOrGrayColor(featureColors_[i].first) &&
                featureColors_[i].second > nextDominantColorCnt) {
                nextDominantColor = featureColors_[i].first;
                nextDominantColorCnt = featureColors_[i].second;
                existColor = true;
            }
        }
        if (existColor) {
            HSV nextColorHsv = RGB2HSV(nextDominantColor);
            GenerateMorandiShadowColor(nextColorHsv);
            nextDominantColor = HSVtoRGB(nextColorHsv);
            color = ColorManager::Color(nextDominantColor | 0xFF000000);
            return SUCCESS;
        } else {
            hsv.s = 0;
            hsv.v = 26; // Adjust value to 26.
            dominantColor = HSVtoRGB(hsv);
            color = ColorManager::Color(dominantColor | 0xFF000000);
            return SUCCESS;
        }
    } else {
        GenerateMorandiShadowColor(hsv);
        dominantColor = HSVtoRGB(hsv);
        color = ColorManager::Color(dominantColor | 0xFF000000);
        return SUCCESS;
    }
}

ColorBrightnessMode ColorPicker::DiscriminateDarkOrBrightColor(const HSV& hsv) const
{
    // 80 is dark color judgement condition.
    if (hsv.v <= 80) {
        return ColorBrightnessMode::DARK_COLOR;
    } else {
        // 20 and 50 is color judgement condition.
        if (hsv.h > 20 && hsv.h <= 50) {
            // 60 is color judgement condition.
            if (hsv.s > 60) {
                return ColorBrightnessMode::HIGH_SATURATION_BRIGHT_COLOR;
            } else {
                return ColorBrightnessMode::LOW_SATURATION_BRIGHT_COLOR;
            }
        // 50 and 269 is color judgement condition.
        } else if (hsv.h > 50 && hsv.h <= 269) {
            // 40 is color judgement condition.
            if (hsv.s > 40) {
                return ColorBrightnessMode::DARK_COLOR;
            } else {
                return ColorBrightnessMode::LOW_SATURATION_BRIGHT_COLOR;
            }
        } else {
            // // 50 is color judgement condition.
            if (hsv.s > 50) {
                return ColorBrightnessMode::HIGH_SATURATION_BRIGHT_COLOR;
            } else {
                return ColorBrightnessMode::LOW_SATURATION_BRIGHT_COLOR;
            }
        }
    }
}

void ColorPicker::ProcessToBrightColor(HSV& hsv) const
{
    static const double valueUpperLimit = 95.0;
    // Value more than 95, adjust to 95.
    hsv.v = std::min(hsv.v, valueUpperLimit);
}

void ColorPicker::AdjustToBasicColor(HSV& hsv, double basicS, double basicV) const
{
    double x = hsv.s + hsv.v;
    double y = basicS + basicV;
    if (x <= y) {
        return;
    } else {
        double z = x - y;
        hsv.s = hsv.s - hsv.s / x * z;
        hsv.v = hsv.v - hsv.v / x * z;
        return;
    }
}

void ColorPicker::ProcessToDarkColor(HSV& hsv) const
{
    // 18 and 69 is basic color threshold.
    if (hsv.h >= 18 && hsv.h <= 69) {
        AdjustToBasicColor(hsv, 70, 60); // 70 and 60 is basic color's s and v
    // 69 and 189 is basic color threshold.
    } else if (hsv.h > 69 && hsv.h <= 189) {
        AdjustToBasicColor(hsv, 50, 50); // 50 is basic color's s and v
    // 189 and 269 is basic color threshold.
    } else if (hsv.h > 189 && hsv.h <= 269) {
        AdjustToBasicColor(hsv, 70, 70); // 70 is basic color's s and v
    } else {
        AdjustToBasicColor(hsv, 60, 60); // 60 is basic color's s and v
    }
}

void ColorPicker::AdjustLowSaturationBrightColor(HSV &colorHsv, HSV &mainHsv, HSV &secondaryHsv,
                                                 const std::pair<uint32_t, uint32_t> &primaryColor,
                                                 const std::pair<uint32_t, uint32_t> &secondaryColor) const
{
    if (colorHsv.s >= 10) { // 10 is the saturate's threshold
        ProcessToBrightColor(mainHsv);
        colorHsv = mainHsv;
        return;
    }
    ColorBrightnessMode secondaryColorBrightMode = DiscriminateDarkOrBrightColor(secondaryHsv);
    if (secondaryColorBrightMode == ColorBrightnessMode::LOW_SATURATION_BRIGHT_COLOR) {
        ProcessToBrightColor(mainHsv);
        colorHsv = mainHsv;
    } else {
        // 10 used to calculate threshold.
        if (primaryColor.second - secondaryColor.second > colorValLen_ / 10) {
            ProcessToBrightColor(mainHsv);
            colorHsv = mainHsv;
        } else {
            secondaryHsv.s = 10; // Adjust secondary color's s to 10
            secondaryHsv.v = 95; // Adjust secondary color's v to 95
            colorHsv = secondaryHsv;
        }
    }
}

// Get immersive background color
uint32_t ColorPicker::GetImmersiveBackgroundColor(ColorManager::Color &color) const
{
    uint32_t colorPicked = 0;
    HSV colorHsv;
    std::pair<uint32_t, uint32_t> primaryColor;
    std::pair<uint32_t, uint32_t> secondaryColor;
    if (featureColors_.empty()) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    bool hasPrimaryColor = GetDominantColor(primaryColor, secondaryColor);
    HSV mainHsv = RGB2HSV(primaryColor.first);
    HSV secondaryHsv = RGB2HSV(secondaryColor.first);
    if (hasPrimaryColor || (mainHsv.s >= secondaryHsv.s)) {
        colorHsv = mainHsv;
    } else {
        colorHsv = secondaryHsv;
    }
    ColorBrightnessMode colorBrightMode = DiscriminateDarkOrBrightColor(colorHsv);
    switch (colorBrightMode) {
        case ColorBrightnessMode::HIGH_SATURATION_BRIGHT_COLOR:
            ProcessToDarkColor(colorHsv);
            break;
        case ColorBrightnessMode::LOW_SATURATION_BRIGHT_COLOR:
            AdjustLowSaturationBrightColor(colorHsv, mainHsv, secondaryHsv, primaryColor, secondaryColor);
            break;
        case ColorBrightnessMode::DARK_COLOR:
            ProcessToDarkColor(colorHsv);
            break;
        default:
            break;
    }
    colorPicked = HSVtoRGB(colorHsv);
    color = ColorManager::Color(colorPicked | 0xFF000000);
    return SUCCESS;
}

// Get immersive foreground color
uint32_t ColorPicker::GetImmersiveForegroundColor(ColorManager::Color &color) const
{
    // Get mask color
    bool rst = GetImmersiveBackgroundColor(color);
    if (rst != SUCCESS) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    uint32_t colorPicked = ((color.PackValue() >> 32) & 0xFFFFFFFF);

    HSV colorHsv = RGB2HSV(colorPicked);
    ColorBrightnessMode colorBrightMode = DiscriminateDarkOrBrightColor(colorHsv);
    if ((colorBrightMode == ColorBrightnessMode::HIGH_SATURATION_BRIGHT_COLOR) ||
        (colorBrightMode == ColorBrightnessMode::DARK_COLOR)) {
        ProcessToDarkColor(colorHsv);
        if (colorHsv.s >= 20) { // 20 is saturation threshold.
            colorHsv.s = 20; // Adjust saturation to 20
            colorHsv.v = 100; // Adjust value to 100.
        } else {
            colorHsv.v = 100; // Adjust value to 100.
        }
    } else {
        ProcessToBrightColor(colorHsv);
        colorHsv.s = 30; // Adjust saturation to 30.
        colorHsv.v = 40; // Adjust value to 40.
    }
    colorPicked = HSVtoRGB(colorHsv);
    color = ColorManager::Color(colorPicked | 0xFF000000);
    return SUCCESS;
}

bool ColorPicker::GetDominantColor(
    std::pair<uint32_t, uint32_t>& primaryColor, std::pair<uint32_t, uint32_t>& secondaryColor) const
{
    if (featureColors_.empty()) {
        primaryColor.first = 0;
        primaryColor.second = 0;
        secondaryColor.first = 0;
        secondaryColor.second = 0;
        return false;
    }
    if (featureColors_.size() == 1) {
        primaryColor.first = featureColors_[0].first;
        primaryColor.second = featureColors_[0].second;
        secondaryColor.first = 0;
        secondaryColor.second = 0;
        return true;
    } else {
        primaryColor.first = featureColors_[0].first;
        primaryColor.second = featureColors_[0].second;
        secondaryColor.first = featureColors_[1].first;
        secondaryColor.second = featureColors_[1].second;
        // 20 used to calculate threshold.
        if (primaryColor.second - secondaryColor.second > colorValLen_ / 20) {
            return true;
        }
        return false;
    }
}

// Gradient Mask Coloring - Deepening the Immersion Color (Fusing with the Background but Deeper than the Background)
uint32_t ColorPicker::GetDeepenImmersionColor(ColorManager::Color &color) const
{
    uint32_t colorPicked = 0;
    std::pair<uint32_t, uint32_t> primaryColor;
    std::pair<uint32_t, uint32_t> secondaryColor;
    if (featureColors_.empty()) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    bool hasPrimaryColor = GetDominantColor(primaryColor, secondaryColor);
    if (hasPrimaryColor) {
        HSV hsv = RGB2HSV(primaryColor.first);
        if (hsv.v >= 40) { // 40 is value threshold.
            hsv.v = 30; // Adjust value to 30.
        } else if (hsv.v >= 20 && hsv.v < 40) { // 20, 40 is value threshold.
            hsv.v -= 10; // 10 used to decrease value.
        } else {
            hsv.v += 20; // 20 used to increse saturation.
        }
        colorPicked = HSVtoRGB(hsv);
    } else {
        // If there is no dominant color, return black-0x00000000
        colorPicked = 0xFF000000;
    }
    color = ColorManager::Color(colorPicked | 0xFF000000);
    return SUCCESS;
}

// Get top proportion colors
std::vector<ColorManager::Color> ColorPicker::GetTopProportionColors(uint32_t colorsNum) const
{
    if (featureColors_.empty() || colorsNum == 0) {
        return {};
    }
    std::vector<ColorManager::Color> colors;
    uint32_t num = std::min(static_cast<uint32_t>(featureColors_.size()), colorsNum);

    for (uint32_t i = 0; i < num; ++i) {
        colors.emplace_back(ColorManager::Color(featureColors_[i].first | 0xFF000000));
    }
    return colors;
};

bool ColorPicker::IsEquals(double val1, double val2) const
{
    // 0.001 is used for double number compare.
    return fabs(val1 - val2) < 0.001;
}

// Transform RGB to HSV.
HSV ColorPicker::RGB2HSV(uint32_t rgb) const
{
    double r, g, b;
    double h, s, v;
    double minComponent, maxComponent;
    double delta;
    HSV hsv;
    r = GetARGB32ColorR(rgb) / 255.0;  // 255.0 is used to normalize color to [0, 1].
    g = GetARGB32ColorG(rgb) / 255.0;  // 255.0 is used to normalize color to [0, 1].
    b = GetARGB32ColorB(rgb) / 255.0;  // 255.0 is used to normalize color to [0, 1].
    if (r > g) {
        maxComponent = std::max(r, b);
        minComponent = std::min(g, b);
    } else {
        maxComponent = std::max(g, b);
        minComponent = std::min(r, b);
    }
    v = maxComponent;
    delta = maxComponent - minComponent;

    if (IsEquals(maxComponent, 0)) {
        s = 0.0;
    } else {
        s = delta / maxComponent;
    }

    if (maxComponent == minComponent) {
        h = 0.0;
    } else {
        if (delta == 0) {
            return hsv;
        }
        if (IsEquals(r, maxComponent) && g >= b) {
            h = 60 * (g - b) / delta + 0; // 60 is used to calculate color's hue, ranging between 0 and 360.
        } else if (IsEquals(r, maxComponent) && g < b) {
            h = 60 * (g - b) / delta + 360; // 60,360 is used to calculate color's hue, ranging between 0 and 360.
        } else if (IsEquals(g, maxComponent)) {
            h = 60 * (b - r) / delta + 120; // 60,120 is used to calculate color's hue, ranging between 0 and 360.
        } else {
            h = 60 * (r - g) / delta + 240; // 60,240 is used to calculate color's hue, ranging between 0 and 360.
        }
    }
    hsv.h = (int)(h + 0.5); // Hue add 0.5 to round up.
    hsv.h = (hsv.h > 359) ? (hsv.h - 360) : hsv.h; // 359 is used to adjust hue to range [0, 360].
    hsv.h = (hsv.h < 0) ? (hsv.h + 360) : hsv.h; // Adjust hue to range [0, 360].
    hsv.s = s * 100; // Adjust saturation to range [0, 100].
    hsv.v = v * 100; // Adjust value to range [0, 100].

    return hsv;
}

void ColorPicker::AdjustHSVToDefinedInterval(HSV& hsv) const
{
    if (hsv.h > 360) { // Adjust hue to range [0, 360].
        hsv.h = 360; // Adjust hue to range [0, 360].
    }
    if (hsv.h < 0) {
        hsv.h = 0;
    }
    if (hsv.s > 100) { // Adjust saturation to range [0, 100].
        hsv.s = 100; // Adjust saturation to range [0, 100].
    }
    if (hsv.s < 0) {
        hsv.s = 0;
    }
    if (hsv.v > 100) { // Adjust value to range [0, 100].
        hsv.v = 100; // Adjust value to range [0, 100].
    }
    if (hsv.v < 0) {
        hsv.v = 0;
    }
    return;
}

// Transform HSV to RGB.
uint32_t ColorPicker::HSVtoRGB(HSV hsv) const
{
    uint32_t r, g, b;
    uint32_t rgb = 0;
    AdjustHSVToDefinedInterval(hsv);

    // The brightness is directly proportional to the maximum value that RGB can reach, which is 2.55 times.
    float rgb_max = hsv.v * 2.55f;

    /**
    * Each time the saturation decreases from 100, the minimum value that RGB can achieve increases
    * linearly by 1/100 from 0 to the maximum value set by the brightness.
    */
    float rgb_min = rgb_max * (100 - hsv.s) / 100.0f;

    int i = hsv.h / 60;
    int difs = hsv.h % 60;
    float rgb_Adj = (rgb_max - rgb_min) * difs / 60.0f;

    /**
     * According to the change of H, there are six cases. In each case, a parameter in RBG is linearly
     * transformed (increased or decreased) between the minimum and maximum values that can be achieved
     * by RGB.
     */
    switch (i) {
        case 0: // 0: when hue's range is [0, 60).
            r = rgb_max;
            g = rgb_min + rgb_Adj;
            b = rgb_min;
            break;
        case 1: // 1: when hue's range is [60, 120).
            r = rgb_max - rgb_Adj;
            g = rgb_max;
            b = rgb_min;
            break;
        case 2: // 2: when hue's range is [120, 180).
            r = rgb_min;
            g = rgb_max;
            b = rgb_min + rgb_Adj;
            break;
        case 3: // 3: when hue's range is [180, 240).
            r = rgb_min;
            g = rgb_max - rgb_Adj;
            b = rgb_max;
            break;
        case 4: // 4: when hue's range is [240, 300).
            r = rgb_min + rgb_Adj;
            g = rgb_min;
            b = rgb_max;
            break;
        default:
            r = rgb_max;
            g = rgb_min;
            b = rgb_max - rgb_Adj;
            break;
    }
    rgb = r << ARGB_R_SHIFT | g << ARGB_G_SHIFT | b << ARGB_B_SHIFT;
    return rgb;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
