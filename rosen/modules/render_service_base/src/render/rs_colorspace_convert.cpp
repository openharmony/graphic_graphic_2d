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

#include "render/rs_colorspace_convert.h"

#include "effect/image_filter.h"
#include "metadata_helper.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSColorSpaceConvert::RSColorSpaceConvert()
{
#ifdef USE_VIDEO_PROCESSING_ENGINE
    colorSpaceConverterDisplay_ = VPE::ColorSpaceConverterDisplay::Create();
#endif
}

RSColorSpaceConvert::~RSColorSpaceConvert()
{}

RSColorSpaceConvert RSColorSpaceConvert::Instance()
{
    static RSColorSpaceConvert instance;
    return instance;
}

bool RSColorSpaceConvert::ColorSpaceConvertor(std::shared_ptr<Drawing::ShaderEffect> inputShader,
    const sptr<SurfaceBuffer>& surfaceBuffer, Drawing::Paint& paint, GraphicColorGamut targetColorSpace,
    ScreenId screenId, int dynamicRangeMode)
{
    /**
     * HDR color converter. Go to the interface first.
    */
    return true;
}

bool RSColorSpaceConvert::SetColorSpaceConverterDisplayParameter(const sptr<SurfaceBuffer>& surfaceBuffer,
    VPE::ColorSpaceConverterDisplayParameter& parameter, GraphicColorGamut targetColorSpace,
    ScreenId screenId, int dynamicRangeMode)
{
    /**
     * HDR color converter. Go to the interface first.
    */
    return true;
}

bool RSColorSpaceConvert::ConvertColorGamutToSpaceInfo(const GraphicColorGamut& colorGamut,
    HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo)
{
    /**
     * HDR color converter. Go to the interface first.
    */
    return true;
}


} // namespace Rosen
} // namespace OHOS
