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

#ifndef RS_COLOR_PICKER_H
#define RS_COLOR_PICKER_H

#include <iostream>
#include "image/pixmap.h"
#include "draw/color.h"
#include "rs_color_extract.h"

namespace OHOS {
namespace Rosen {

struct HSV {
    int h; // 色度取值(0-360)
    double s; // 饱和度取值 (0-100)
    double v; // 亮度取值 (0-100)
};

class RSColorPicker : public RSColorExtract {
public:
    ~RSColorPicker() {}
    NATIVEEXPORT static std::shared_ptr<RSColorPicker> CreateColorPicker(
        const std::shared_ptr<Drawing::Pixmap>& pixmap, uint32_t &errorCode);
    NATIVEEXPORT static std::shared_ptr<RSColorPicker> CreateColorPicker(
        const std::shared_ptr<Drawing::Pixmap>& pixmap, double* coordinates, uint32_t &errorCode);

    NATIVEEXPORT uint32_t GetLargestProportionColor(Drawing::ColorQuad &color) const;
    NATIVEEXPORT uint32_t GetHighestSaturationColor(Drawing::ColorQuad &color) const;
    NATIVEEXPORT uint32_t GetAverageColor(Drawing::ColorQuad &color) const;
    NATIVEEXPORT bool IsBlackOrWhiteOrGrayColor(uint32_t color) const;

private:
    RSColorPicker(std::shared_ptr<Drawing::Pixmap> pixmap);
    RSColorPicker(std::shared_ptr<Drawing::Pixmap> pixmap, double* coordinates);
    bool IsEquals(double val1, double val2) const;
    HSV RGB2HSV(uint32_t rgb) const;
    void AdjustHSVToDefinedIterval(HSV& hsv) const;
    uint32_t HSVtoRGB(HSV hsv) const;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_COLOR_PICKER_H
