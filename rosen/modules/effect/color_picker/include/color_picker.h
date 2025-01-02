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

#ifndef COLOR_PICKER_H
#define COLOR_PICKER_H

#include <iostream>
#include "effect_type.h"
#include "color_extract.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace ColorManager {
class Color;
}
namespace Rosen {

enum class ColorBrightnessMode : uint16_t {
    HIGH_SATURATION_BRIGHT_COLOR = 0,
    LOW_SATURATION_BRIGHT_COLOR = 1,
    DARK_COLOR = 2,
};

enum PictureLightColorDegree {
    UNKOWN_LIGHT_COLOR_DEGREE_PICTURE = 0,
    EXTREMELY_LIGHT_COLOR_PICTURE = 1,
    LIGHT_COLOR_PICTURE = 2,
    DARK_COLOR_PICTURE = 3,
    EXTREMELY_DARK_COLOR_PICTURE = 4,
    FLOWERY_PICTURE = 5,
    EXTREMELY_FLOWERY_PICTURE = 6,
};


struct HSV {
    int h;  // 色度取值(0-360)
    double s;  // 饱和度取值 (0-100)
    double v;  // 亮度取值 (0-100)
};

class ColorPicker : public ColorExtract {
public:
    ColorPicker(std::shared_ptr<Media::PixelMap> pixmap);
    ColorPicker(std::shared_ptr<Media::PixelMap> pixmap, double* coordinates);
    ~ColorPicker() {}
    static std::shared_ptr<Media::PixelMap> CreateScaledPixelMap(const std::shared_ptr<Media::PixelMap>& pixmap);
    NATIVEEXPORT static std::shared_ptr<ColorPicker> CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap,
                                                                        uint32_t &errorCode);
    NATIVEEXPORT static std::shared_ptr<ColorPicker> CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap,
                                                                        double* coordinates, uint32_t &errorCode);
    NATIVEEXPORT std::shared_ptr<Media::PixelMap> GetScaledPixelMap();
    NATIVEEXPORT uint32_t GetMainColor(ColorManager::Color &color);

    static constexpr double GRAY_RATIO_RED = 0.299;
    static constexpr double GRAY_RATIO_GREEN = 0.587;
    static constexpr double GRAY_RATIO_BLUE = 0.114;
    static constexpr double LUMINANCE_RATIO_RED = 0.2126;
    static constexpr double LUMINANCE_RATIO_GREEN = 0.7152;
    static constexpr double LUMINANCE_RATIO_BLUE = 0.0722;
    NATIVEEXPORT uint32_t GetLargestProportionColor(ColorManager::Color &color) const;
    NATIVEEXPORT uint32_t GetHighestSaturationColor(ColorManager::Color &color) const;
    NATIVEEXPORT uint32_t GetAverageColor(ColorManager::Color &color) const;
    NATIVEEXPORT bool IsBlackOrWhiteOrGrayColor(uint32_t color) const;
    NATIVEEXPORT uint32_t GetMorandiBackgroundColor(ColorManager::Color &color) const;
    NATIVEEXPORT uint32_t GetMorandiShadowColor(ColorManager::Color &color) const;
    NATIVEEXPORT uint32_t GetDeepenImmersionColor(ColorManager::Color &color) const;
    NATIVEEXPORT uint32_t GetImmersiveBackgroundColor(ColorManager::Color &color) const;
    NATIVEEXPORT uint32_t GetImmersiveForegroundColor(ColorManager::Color &color) const;
    NATIVEEXPORT uint32_t DiscriminatePitureLightDegree(PictureLightColorDegree &degree) const;
    NATIVEEXPORT uint32_t GetReverseColor(ColorManager::Color &color) const;
    NATIVEEXPORT std::vector<ColorManager::Color> GetTopProportionColors(uint32_t colorsNum) const;

private:
    void AdjustLowSaturationBrightColor(HSV &colorHsv, HSV &mainHsv, HSV &secondaryHsv,
                                        const std::pair<uint32_t, uint32_t> &mainColor,
                                        const std::pair<uint32_t, uint32_t> &secondaryColor) const;
    void GenerateMorandiBackgroundColor(HSV& hsv) const;
    void GenerateMorandiShadowColor(HSV& hsv) const;
    bool GetDominantColor(
        std::pair<uint32_t, uint32_t>& mainColor, std::pair<uint32_t, uint32_t>& secondaryColor) const;
    ColorBrightnessMode DiscriminateDarkOrBrightColor(const HSV& hsv) const;
    void ProcessToDarkColor(HSV& hsv) const;
    void ProcessToBrightColor(HSV& hsv) const;
    void AdjustToBasicColor(HSV& hsv, double basicS, double basicV) const;
    uint32_t RGB2GRAY(uint32_t color) const;
    bool IsEquals(double val1, double val2) const;
    HSV RGB2HSV(uint32_t rgb) const;
    void AdjustHSVToDefinedInterval(HSV& hsv) const;
    uint32_t HSVtoRGB(HSV hsv) const;
    uint32_t CalcGrayVariance() const;
    double CalcContrastRatioWithWhite() const;
    double CalcRelaticeLuminance(uint32_t color) const;
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // COLOR_PICKER_H
