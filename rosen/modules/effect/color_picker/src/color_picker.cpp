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
        EFFECT_LOG_I("[ColorPicker]failed to create ColorPicker with null pixmap.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }
    
    std::shared_ptr<Media::PixelMap> scaledPixelMap = CreateScaledPixelMap(pixmap);
    if (scaledPixelMap == nullptr) {
        EFFECT_LOG_I("[ColorPicker]failed to scale pixelmap, scaledPixelMap is nullptr.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }
    ColorPicker *colorPicker = new (std::nothrow) ColorPicker(scaledPixelMap);
    if (colorPicker == nullptr) {
        EFFECT_LOG_I("[ColorPicker]failed to create ColorPicker with pixmap.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }

    errorCode = SUCCESS;
    return std::shared_ptr<ColorPicker>(colorPicker);
}

std::shared_ptr<ColorPicker> ColorPicker::CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap,
    double* coordinates, uint32_t &errorCode)
{
    if (pixmap == nullptr) {
        EFFECT_LOG_I("[ColorPicker]failed to create ColorPicker with null pixmap.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> scaledPixelMap = CreateScaledPixelMap(pixmap);
    ColorPicker *colorPicker = new (std::nothrow) ColorPicker(scaledPixelMap, coordinates);
    if (colorPicker == nullptr) {
        EFFECT_LOG_I("[ColorPicker]failed to create ColorPicker with pixmap.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }

    errorCode = SUCCESS;
    return std::shared_ptr<ColorPicker>(colorPicker);
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
    std::shared_ptr<Media::PixelMap> pixelMap = GetScaledPixelMap();

    // get color
    uint32_t colorVal = 0;
    int x = 0;
    int y = 0;
    bool bSucc = pixelMap->GetARGB32Color(x, y, colorVal);
    EFFECT_LOG_I("[newpix].argb.ret=%{public}d, %{public}x", bSucc, colorVal);
    color = ColorManager::Color(colorVal);
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

void ColorPicker::AdjustHSVToDefinedIterval(HSV& hsv) const
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
    AdjustHSVToDefinedIterval(hsv);

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
