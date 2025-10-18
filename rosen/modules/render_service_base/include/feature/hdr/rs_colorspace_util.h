/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RS_COLORSPACE_UTIL_H
#define RS_COLORSPACE_UTIL_H

#include "color_space.h"
#include "draw/canvas.h"
#include "effect/color_space.h"
#ifndef ROSEN_CROSS_PLATFORM
#include <v1_0/cm_color_space.h>
#endif
#include "surface_type.h"

#define HDIV HDI::Display::Graphic::Common::V1_0

namespace OHOS {
namespace Rosen {

class RSColorSpaceUtil {
public:
    RSColorSpaceUtil() = default;
    ~RSColorSpaceUtil() = default;

    static std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(
        ColorManager::ColorSpaceName colorSpaceName);
    static GraphicColorGamut DrawingColorSpaceToGraphicColorGamut(
        const std::shared_ptr<Drawing::ColorSpace>& colorSpace);
    static GraphicColorGamut GetColorGamutFromCanvas(const Drawing::Canvas& canvas);
    static GraphicColorGamut ColorSpaceNameToGraphicGamut(OHOS::ColorManager::ColorSpaceName name);
    static GraphicColorGamut SelectBigGamut(GraphicColorGamut gamut1, GraphicColorGamut gamut2);

#ifndef ROSEN_CROSS_PLATFORM
    static GraphicColorGamut PrimariesToGraphicGamut(HDIV::CM_ColorPrimaries primary);
#endif
};

} // namespace Rosen
} // namespace OHOS
#endif // RS_COLORSPACE_UTIL_H