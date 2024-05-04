/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_COLORSPACE_CONVERT_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_COLORSPACE_CONVERT_H

#include "colorspace_converter_display.h"
#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "image/image.h"
#include "render/rs_image_base.h"

#include "screen_manager/screen_types.h"

#define HDIV HDI::Display::Graphic::Common::V1_0

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {

using DynamicRangeMode = enum {
    HIGH = 0,
    CONSTRAINT = 1,
    STANDARD = 2,
}

using VPEConvert = Media::VideoProcessingEngine::ColorSpaceConverterDisplay;
using VPEParameter = Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter;
class RSB_EXPORT RSColorSpaceConvert {
public:
    ~RSColorSpaceConvert();
    static RSColorSpaceConvert Instance();

    bool ColorSpaceConvertor(std::shared_ptr<Drawing::ShaderEffect> inputShader,
        const sptr<SurfaceBuffer>& surfaceBuffer, Drawing::Paint& paint, GraphicColorGamut targetColorSpace,
        ScreenId screenId, uint32_t dynamicRangeMode);
    bool SetColorSpaceConverterDisplayParameter(const sptr<SurfaceBuffer>& surfaceBuffer, VPEParameter& parameter,
        GraphicColorGamut targetColorSpace, ScreenId screenId, uint32_t dynamicRangeMode);
    bool ConvertColorGamutToSpaceInfo(const GraphicColorGamut& colorGamut, HDIV::CM_ColorSpaceInfo& colorSpaceInfo);

private:
    RSColorSpaceConvert();
    std::shared_ptr<VPEConvert> colorSpaceConverterDisplay_ = nullptr;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_COLORSPACE_CONVERT_H