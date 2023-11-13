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

#ifndef UTILS_METADATA_MANAGER_EXPORT_METADATA_MANAGER_H
#define UTILS_METADATA_MANAGER_EXPORT_METADATA_MANAGER_H

#include <vector>

#include <securec.h>

#include "graphic_common.h"
#include "surface_buffer.h"
#include "v1_0/cm_color_space.h"
#include "v1_0/hdr_static_metadata.h"
#include "hdr_vivid_metadata.h"

namespace OHOS {
namespace MetadataManager {
class MetadataHelper {
public:
    // GSERROR_OK for success, GSERROR_API_FAILED for fail
    template <typename T>
    static GSError ConvertMetadataToVec(const T& metadata, std::vector<uint8_t>& data)
    {
        data.resize(sizeof(T));
        if (memcpy_s(data.data(), data.size(), &metadata, sizeof(T)) != EOK) {
            return GSERROR_API_FAILED;
        }
        return GSERROR_OK;
    }

    template <typename T>
    static GSError ConvertVecToMetadata(const std::vector<uint8_t>& data, T& metadata)
    {
        if (data.size() != sizeof(T)) {
            return GSERROR_NOT_SUPPORT;
        }

        if (memcpy_s(&metadata, sizeof(T), data.data(), data.size()) != EOK) {
            return GSERROR_API_FAILED;
        }
        return GSERROR_OK;
    }

    static GSError ParseColorSpaceType(const HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceType,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo);

    static GSError SetColorSpaceInfo(sptr<SurfaceBuffer>& buffer,
        const HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo);
    static GSError GetColorSpaceInfo(const sptr<SurfaceBuffer>& buffer,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo);

    static GSError SetColorSpaceType(sptr<SurfaceBuffer>& buffer,
        const HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceType);
    static GSError GetColorSpaceType(const sptr<SurfaceBuffer>& buffer,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceType);

    static GSError SetHDRMetadataType(sptr<SurfaceBuffer>& buffer,
        const HDI::Display::Graphic::Common::V1_0::CM_HDR_Metadata_Type& hdrMetadataType);
    static GSError GetHDRMetadataType(const sptr<SurfaceBuffer>& buffer,
        HDI::Display::Graphic::Common::V1_0::CM_HDR_Metadata_Type& hdrMetadataType);

    static GSError SetHDRStaticMetadata(sptr<SurfaceBuffer>& buffer,
        const HDI::Display::Graphic::Common::V1_0::HdrStaticMetadata& hdrStaticMetadata);
    static GSError GetHDRStaticMetadata(const sptr<SurfaceBuffer>& buffer,
        HDI::Display::Graphic::Common::V1_0::HdrStaticMetadata& hdrStaticMetadata);

    static GSError SetHDRVividDynMetadataV1(sptr<SurfaceBuffer>& buffer, const HdrVividMetadataV1& hdrVividMetadata);
    static GSError GetHDRVividDynMetadataV1(const sptr<SurfaceBuffer>& buffer, HdrVividMetadataV1& hdrVividMetadata);

private:
    static constexpr uint32_t TRANSFUNC_OFFSET = 8;
    static constexpr uint32_t MATRIX_OFFSET = 16;
    static constexpr uint32_t RANGE_OFFSET = 21;
};
} // namespace MetadataManager
} // namespace OHOS

#endif // UTILS_METADATA_MANAGER_EXPORT_METADATA_MANAGER_H