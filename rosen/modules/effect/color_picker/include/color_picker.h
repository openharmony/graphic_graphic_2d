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

struct HSV {
    int h;  // 色度取值(0-360)
    double s;  // 饱和度取值 (0-100)
    double v;  // 亮度取值 (0-100)
};

class ColorPicker : public ColorExtract {
public:
    ~ColorPicker() {}
    static std::shared_ptr<Media::PixelMap> CreateScaledPixelMap(const std::shared_ptr<Media::PixelMap>& pixmap);
    NATIVEEXPORT static std::shared_ptr<ColorPicker> CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap,
                                                                        uint32_t &errorCode);
    NATIVEEXPORT static std::shared_ptr<ColorPicker> CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap,
                                                                        double* coordinates, uint32_t &errorCode);
    NATIVEEXPORT std::shared_ptr<Media::PixelMap> GetScaledPixelMap();
    NATIVEEXPORT uint32_t GetMainColor(ColorManager::Color &color);

    NATIVEEXPORT uint32_t GetLargestProportionColor(ColorManager::Color &color) const;
    NATIVEEXPORT uint32_t GetHighestSaturationColor(ColorManager::Color &color) const;
    NATIVEEXPORT uint32_t GetAverageColor(ColorManager::Color &color) const;
    NATIVEEXPORT bool IsBlackOrWhiteOrGrayColor(uint32_t color) const;
    NATIVEEXPORT std::vector<ColorManager::Color> GetTopProportionColors(uint32_t colorsNum) const;

private:
    ColorPicker(std::shared_ptr<Media::PixelMap> pixmap);
    ColorPicker(std::shared_ptr<Media::PixelMap> pixmap, double* coordinates);
    bool IsEquals(double val1, double val2) const;
    HSV RGB2HSV(uint32_t rgb) const;
    void AdjustHSVToDefinedIterval(HSV& hsv) const;
    uint32_t HSVtoRGB(HSV hsv) const;
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // COLOR_PICKER_H
