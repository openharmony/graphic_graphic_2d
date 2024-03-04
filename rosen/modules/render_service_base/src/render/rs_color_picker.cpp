/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "render/rs_color_picker.h"
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

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<RSColorPicker> RSColorPicker::CreateColorPicker(const std::shared_ptr<SkPixmap>& pixmap,
    uint32_t &errorCode)
#else
std::shared_ptr<RSColorPicker> RSColorPicker::CreateColorPicker(const std::shared_ptr<Drawing::Pixmap>& pixmap,
    uint32_t &errorCode)
#endif
{
    if (pixmap == nullptr) {
        ROSEN_LOGE("[ColorPicker]failed to create ColorPicker with null pixmap.");
        errorCode = RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }
    
    RSColorPicker *colorPicker = new (std::nothrow) RSColorPicker(pixmap);
    if (colorPicker == nullptr) {
        ROSEN_LOGE("[ColorPicker]failed to create ColorPicker with pixmap.");
        errorCode = RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }

    errorCode = RS_COLOR_PICKER_SUCCESS;
    return std::shared_ptr<RSColorPicker>(colorPicker);
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<RSColorPicker> RSColorPicker::CreateColorPicker(const std::shared_ptr<SkPixmap>& pixmap,
    double* coordinates, uint32_t &errorCode)
#else
std::shared_ptr<RSColorPicker> RSColorPicker::CreateColorPicker(const std::shared_ptr<Drawing::Pixmap>& pixmap,
    double* coordinates, uint32_t &errorCode)
#endif
{
    if (pixmap == nullptr) {
        ROSEN_LOGE("[ColorPicker]failed to create ColorPicker with null pixmap.");
        errorCode = RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }

    RSColorPicker *colorPicker = new (std::nothrow) RSColorPicker(pixmap, coordinates);
    if (colorPicker == nullptr) {
        ROSEN_LOGE("[ColorPicker]failed to create ColorPicker with pixmap.");
        errorCode = RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }

    errorCode = RS_COLOR_PICKER_SUCCESS;
    return std::shared_ptr<RSColorPicker>(colorPicker);
}

#ifndef USE_ROSEN_DRAWING
RSColorPicker::RSColorPicker(std::shared_ptr<SkPixmap> pixmap):RSColorExtract(pixmap) {}
RSColorPicker::RSColorPicker(
    std::shared_ptr<SkPixmap> pixmap, double* coordinates):RSColorExtract(pixmap, coordinates) {}
#else
RSColorPicker::RSColorPicker(std::shared_ptr<Drawing::Pixmap> pixmap):RSColorExtract(pixmap) {}
RSColorPicker::RSColorPicker(
    std::shared_ptr<Drawing::Pixmap> pixmap, double* coordinates):RSColorExtract(pixmap, coordinates) {}
#endif

#ifndef USE_ROSEN_DRAWING
uint32_t RSColorPicker::GetLargestProportionColor(SkColor &color) const
{
    if (featureColors_.empty()) {
        return RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE;
    }
    color = SkColor(featureColors_[0].first | 0xFF000000); // alpha = oxFF
    return RS_COLOR_PICKER_SUCCESS;
}
#else
uint32_t RSColorPicker::GetLargestProportionColor(Drawing::ColorQuad &color) const
{
    if (featureColors_.empty()) {
        return RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE;
    }
    color = featureColors_[0].first | 0xFF000000; // alpha = oxFF
    return RS_COLOR_PICKER_SUCCESS;
}
#endif

#ifndef USE_ROSEN_DRAWING
uint32_t RSColorPicker::GetHighestSaturationColor(SkColor &color) const
#else
uint32_t RSColorPicker::GetHighestSaturationColor(Drawing::ColorQuad &color) const
#endif
{
    if (featureColors_.empty()) {
        return RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE;
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
#ifndef USE_ROSEN_DRAWING
    color = SkColor(colorPicked | 0xFF000000);
#else
    color = colorPicked | 0xFF000000;
#endif
    return RS_COLOR_PICKER_SUCCESS;
}

#ifndef USE_ROSEN_DRAWING
uint32_t RSColorPicker::GetAverageColor(SkColor &color) const
#else
uint32_t RSColorPicker::GetAverageColor(Drawing::ColorQuad &color) const
#endif
{
    uint32_t colorPicked = 0;
    uint32_t redSum = 0;
    uint32_t greenSum = 0;
    uint32_t blueSum = 0;
    int totalPixelNum = 0;
    if (featureColors_.empty()) {
        return RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE;
    }
    for (size_t i = 0; i < featureColors_.size(); i++) {
        totalPixelNum += featureColors_[i].second;
        redSum += featureColors_[i].second * ((featureColors_[i].first >> ARGB_R_SHIFT) & ARGB_MASK);
        greenSum += featureColors_[i].second * ((featureColors_[i].first >> ARGB_G_SHIFT) & ARGB_MASK);
        blueSum += featureColors_[i].second * ((featureColors_[i].first >> ARGB_B_SHIFT) & ARGB_MASK);
    }
    if (totalPixelNum == 0) {
        return RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE;
    }
    uint32_t redMean = round(redSum / (float)totalPixelNum);
    uint32_t greenMean = round(greenSum / (float)totalPixelNum);
    uint32_t blueMean = round(blueSum / (float)totalPixelNum);
    colorPicked = redMean << ARGB_R_SHIFT | greenMean << ARGB_G_SHIFT | blueMean << ARGB_B_SHIFT;
#ifndef USE_ROSEN_DRAWING
    color = SkColor(colorPicked | 0xFF000000);
#else
    color = colorPicked | 0xFF000000;
#endif
    return RS_COLOR_PICKER_SUCCESS;
}

bool RSColorPicker::IsBlackOrWhiteOrGrayColor(uint32_t color) const
{
    HSV hsv = RGB2HSV(color);
    // A color is black, white or gray colr when its hsv satisfy (v>30, s<=5) or (15<v<=30, s<=15) or (v<=15).
    if ((hsv.v > 30 && hsv.s <= 5) || (hsv.v > 15 && hsv.v <= 30 && hsv.s <= 15) || (hsv.v <= 15)) {
        return true;
    }
    return false;
}

bool RSColorPicker::IsEquals(double val1, double val2) const
{
    // 0.001 is used for double number compare.
    return fabs(val1 - val2) < 0.001;
}

// Transform RGB to HSV.
HSV RSColorPicker::RGB2HSV(uint32_t rgb) const
{
    double r;
    double g;
    double b;
    double h;
    double s;
    double v;
    double minComponent;
    double maxComponent;
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
        if (delta == 0) { return hsv; }
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

void RSColorPicker::AdjustHSVToDefinedIterval(HSV& hsv) const
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
uint32_t RSColorPicker::HSVtoRGB(HSV hsv) const
{
    uint32_t r;
    uint32_t g;
    uint32_t b;
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
