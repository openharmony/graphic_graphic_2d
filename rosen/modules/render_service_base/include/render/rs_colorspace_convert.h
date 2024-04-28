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

#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "image/image.h"
#include "render/rs_image_base.h"

#include "screen_manager/screen_types.h"

#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "colorspace_converter_display.h"
#endif

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {

using VPE = Media::VideoProcessingEngine;
class RSB_EXPORT RSColorSpaceConvert {
public:
    ~RSColorSpaceConvert();
    static RSColorSpaceConvert Instance();

    bool ColorSpaceConvertor(std::shared_ptr<Drawing::ShaderEffect> inputShader,
        const sptr<SurfaceBuffer>& surfaceBuffer, Drawing::Paint& paint,
        GraphicColorGamut targetColorSpace, ScreenId screenId, int dynamicRangeMode);
    bool SetColorSpaceConverterDisplayParameter(const sptr<SurfaceBuffer>& surfaceBuffer,
        VPE::ColorSpaceConverterDisplayParameter& parameter, GraphicColorGamut targetColorSpace,
        ScreenId screenId, int dynamicRangeMode);
    bool ConvertColorGamutToSpaceInfo(const GraphicColorGamut& colorGamut,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo);

private:
    RSColorSpaceConvert();
    std::shared_ptr<VPE::ColorSpaceConverterDisplay> colorSpaceConverterDisplay_ = nullptr;
};

} // namespace Rosen
} // namespace OHOS

#endif //RENDER_SERVICE_CLIENT_CORE_UI_RS_COLORSPACE_CONVERT_H