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
#include <math.h>
#include <utility>

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using OHOS::HiviewDFX::HiLog;

std::shared_ptr<ColorPicker> ColorPicker::CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap,
    uint32_t &errorCode)
{
    if (pixmap == nullptr) {
        HiLog::Info(LABEL, "[ColorPicker]failed to create ColorPicker with null pixmap.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }
    ColorPicker *colorPicker = new (std::nothrow) ColorPicker(pixmap);
    if (colorPicker == nullptr) {
        HiLog::Info(LABEL, "[ColorPicker]failed to create ColorPicker with pixmap.");
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
    HiLog::Info(LABEL, "[newpix].argb.ret=%{public}d, %{public}x", bSucc, colorVal);
    color = ColorManager::Color(colorVal);
    return SUCCESS;
}

ColorPicker::ColorPicker(std::shared_ptr<Media::PixelMap> pixmap):ColorExtract(pixmap) {}



// 选取数量占比最多的颜色
uint32_t ColorPicker::GetLargestProportionColor(ColorManager::Color &color) const
{
    if (featureColors.empty()) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    color = ColorManager::Color(featureColors[0].first | 0xFF000000); // alpha = oxFF
    return SUCCESS;
}

// 选取饱和度最高的颜色
uint32_t ColorPicker::GetHighestSaturationColor(ColorManager::Color &color) const
{
    if (featureColors.empty()) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    uint32_t colorPicked = 0;
    HSV hsv = {0};
    double maxSaturation = 0.0;
    for (int i = 0; i < featureColors.size(); i++) {
        hsv = RGB2HSV(featureColors[i].first);
        if (hsv.s > maxSaturation) {
            maxSaturation = hsv.s;
            colorPicked = featureColors[i].first;
        }
    }
    color = ColorManager::Color(colorPicked | 0xFF000000);
    return SUCCESS;
}


// 计算图片的平均颜色
uint32_t ColorPicker::GetAverageColor(ColorManager::Color &color) const
{
    uint32_t colorPicked = 0;
    uint32_t redSum = 0;
    uint32_t greenSum = 0;
    uint32_t blueSum = 0;
    int totalPixelNum = 0;
    if (featureColors.empty()) {
        return ERR_EFFECT_INVALID_VALUE;
    }
    for (int i = 0; i < featureColors.size(); i++) {
        totalPixelNum += featureColors[i].second;
        redSum += featureColors[i].second * ((featureColors[i].first >> ARGB_R_SHIFT) & ARGB_MASK);
        greenSum += featureColors[i].second * ((featureColors[i].first >> ARGB_G_SHIFT) & ARGB_MASK);
        blueSum += featureColors[i].second * ((featureColors[i].first >> ARGB_B_SHIFT) & ARGB_MASK);
    }
    uint32_t redMean = round(redSum / (float)totalPixelNum);
    uint32_t greenMean = round(greenSum / (float)totalPixelNum);
    uint32_t blueMean = round(blueSum / (float)totalPixelNum);
    colorPicked = redMean << ARGB_R_SHIFT | greenMean << ARGB_G_SHIFT | blueMean << ARGB_B_SHIFT;
    color = ColorManager::Color(colorPicked | 0xFF000000);
    return SUCCESS;

}

// 判别黑白灰颜色
bool ColorPicker::IsBlackOrWhiteOrGrayColor(uint32_t color) const
{
    HSV hsv = RGB2HSV(color);
    if ((hsv.v > 30 && hsv.s <= 5) || (hsv.v > 15 && hsv.v <= 30 && hsv.s <= 15) || (hsv.v <= 15)) {
        return true;
    }
    return false;
}



bool ColorPicker::IsEquals(double val1, double val2) const
{
    return fabs(val1 - val2) < 0.001;
}

// 将RGB格式转换为HSB格式
HSV ColorPicker::RGB2HSV(uint32_t rgb) const
{
    double r, g, b;
    double h, s, v;
    double minComponent, maxComponent;
    double delta;
    HSV hsv;

    r = GetARGB32ColorR(rgb) / 255.0;
    g = GetARGB32ColorG(rgb) / 255.0;
    b = GetARGB32ColorB(rgb) / 255.0;

    if (r > g) {
        maxComponent = std::max(r, b);
        minComponent = std::min(g, b);
    } else {
        maxComponent = std::max(g, b);
        minComponent = std::min(r, b);
    }

    v = maxComponent;
    delta = maxComponent - minComponent;

    if (IsEquals(maxComponent, 0))
        s = 0.0;
    else
        s = delta / maxComponent;

    if (maxComponent == minComponent) {
        h = 0.0;
    } else {
        if (IsEquals(r, maxComponent) && g >= b) {
            h = 60 * (g - b) / delta + 0;
        } else if (IsEquals(r, maxComponent) && g < b) {
            h = 60 * (g - b) / delta + 360;
        } else if (IsEquals(g, maxComponent)) {
            h = 60 * (b - r) / delta + 120;
        } else {
            h = 60 * (r - g) / delta + 240;
        }
    }

    hsv.h = (int)(h + 0.5);
    hsv.h = (hsv.h > 359) ? (hsv.h - 360) : hsv.h;
    hsv.h = (hsv.h < 0) ? (hsv.h + 360) : hsv.h;
    hsv.s = s * 100;
    hsv.v = v * 100;

    return hsv;
}


// 将HSV转换成RGB
uint32_t ColorPicker::HSVtoRGB(HSV hsv) const
{
    float rgb_min, rgb_max;
    uint32_t r, g, b;
    uint32_t rgb = 0;
    if (hsv.h > 360) {
        hsv.h = 360;
    }
    if (hsv.h < 0) {
        hsv.h = 0;
    }
    if (hsv.s > 100) {
        hsv.s = 100;
    }
    if (hsv.s < 0) {
        hsv.s = 0;
    }
    if (hsv.v > 100) {
        hsv.v = 100;
    }
    if (hsv.v < 0) {
        hsv.v = 0;
    }
    // The brightness is directly proportional to the maximum value that RGB can reach, which is 2.55 times.
    rgb_max = hsv.v * 2.55f;

    // Each time the saturation decreases from 100, the minimum value that RGB can achieve increases linearly by 1/100 from 0 to the maximum value set by the brightness
    rgb_min = rgb_max*(100 - hsv.s)/ 100.0f;

    int i = hsv.h / 60;
    int difs = hsv.h % 60;
    float rgb_Adj = (rgb_max - rgb_min)*difs / 60.0f;

    /**
     * According to the change of H, there are six cases. In each case, a parameter in RBG is linearly
     * transformed (increased or decreased) between the minimum and maximum values that can be achieved
     * by RGB.
     */
    switch (i) {
        case 0:
            r = rgb_max;
            g = rgb_min + rgb_Adj;
            b = rgb_min;
            break;
        case 1:
            r = rgb_max - rgb_Adj;
            g = rgb_max;
            b = rgb_min;
            break;
        case 2:
            r = rgb_min;
            g = rgb_max;
            b = rgb_min + rgb_Adj;
            break;
        case 3:
            r = rgb_min;
            g = rgb_max - rgb_Adj;
            b = rgb_max;
            break;
        case 4:
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
