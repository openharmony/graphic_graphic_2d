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

#ifndef SKIA_YUV_INFO_H
#define SKIA_YUV_INFO_H

#include "include/core/SkYUVAInfo.h"

#include "image/yuv_info.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class SkiaYUVInfo {
public:
    static SkYUVAInfo::PlaneConfig ConvertToSkPlaneConfig(const YUVInfo::PlaneConfig& config)
    {
        switch (config) {
            case YUVInfo::PlaneConfig::Y_UV:
                return SkYUVAInfo::PlaneConfig::kY_UV;
            case YUVInfo::PlaneConfig::Y_VU:
                return SkYUVAInfo::PlaneConfig::kY_VU;
            case YUVInfo::PlaneConfig::UNKNOWN:
                return SkYUVAInfo::PlaneConfig::kUnknown;
            default:
                return SkYUVAInfo::PlaneConfig::kUnknown;
        }
    }

    static SkYUVAInfo::Subsampling ConvertToSkSubSampling(const YUVInfo::SubSampling& sampling)
    {
        switch (sampling) {
            case YUVInfo::SubSampling::K420:
                return SkYUVAInfo::Subsampling::k420;
            case YUVInfo::SubSampling::UNKNOWN:
                return SkYUVAInfo::Subsampling::kUnknown;
            default:
                return SkYUVAInfo::Subsampling::kUnknown;
        }
    }

    static SkYUVColorSpace ConvertToSkYUVColorSpace(const YUVInfo::YUVColorSpace& colorspace)
    {
        switch (colorspace) {
            case YUVInfo::YUVColorSpace::JPEG_FULL_YUVCOLORSPACE:
                return SkYUVColorSpace::kJPEG_Full_SkYUVColorSpace;
            case YUVInfo::YUVColorSpace::IDENTITY_YUVCOLORSPACE:
                return SkYUVColorSpace::kIdentity_SkYUVColorSpace;
            default:
                return SkYUVColorSpace::kIdentity_SkYUVColorSpace;
        }
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif