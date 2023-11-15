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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_METADATA_MANAGER_H
#define FRAMEWORKS_SURFACE_INCLUDE_METADATA_MANAGER_H

#include <vector>

#include <securec.h>

#include "graphic_common.h"
#include "surface_buffer.h"
#include "v1_0/cm_color_space.h"
#include "v1_0/hdr_static_metadata.h"
#include "hdr_vivid_metadata.h"

namespace OHOS {
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

    static GSError ConvertColorSpaceTypeToInfo(
        const HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceType,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo);
    static GSError ConvertColorSpaceInfoToType(
        const HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceType);

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

    static GSError ConvertVecToHDRVividDynMetadataV1(const std::vector<uint8_t>& data,
        HdrVividMetadataV1& hdrVividMetadata);
    static GSError ConvertHDRVividDynMetadataV1ToVec(const HdrVividMetadataV1& hdrVividMetadata,
        std::vector<uint8_t>& data);

    static GSError SetHDRVividDynMetadataV1(sptr<SurfaceBuffer>& buffer, const HdrVividMetadataV1& hdrVividMetadata);
    static GSError GetHDRVividDynMetadataV1(const sptr<SurfaceBuffer>& buffer, HdrVividMetadataV1& hdrVividMetadata);

private:
    static constexpr uint32_t TRANSFUNC_OFFSET = 8;
    static constexpr uint32_t MATRIX_OFFSET = 16;
    static constexpr uint32_t RANGE_OFFSET = 21;

    // The following constants define the number of bits of each metadata according to CUVA standard 7.4
    static constexpr uint32_t SYSTEM_START_CODE_BITS = 8;
    static constexpr uint32_t SYSTEM_START_CODE = 0x01;
    static constexpr uint32_t MINIMUM_MAXRGB_BITS = 12;
    static constexpr uint32_t AVERAGE_MAXRGB_BITS = 12;
    static constexpr uint32_t VARIANCE_MAXRGB_BITS = 12;
    static constexpr uint32_t MAXIMUM_MAXRGB_BITS = 12;
    static constexpr uint32_t TONE_MAPPING_MODE_BITS = 1;
    static constexpr uint32_t TONE_MAPPING_PARAM_NUM_BITS = 1;
    static constexpr uint32_t TONE_MAPPING_PARAM_NUM_MAX = 1 << TONE_MAPPING_PARAM_NUM_BITS;
    static constexpr uint32_t TARGETED_SYSTEM_DISPLAY_MAXIMUM_LUMINANCE_BITS = 12;
    static constexpr uint32_t BASE_FLAG_BITS = 1;
    static constexpr uint32_t BASE_PARAM_MP_BITS = 14;
    static constexpr uint32_t BASE_PARAM_MM_BITS = 6;
    static constexpr uint32_t BASE_PARAM_MA_BITS = 10;
    static constexpr uint32_t BASE_PARAM_MB_BITS = 10;
    static constexpr uint32_t BASE_PARAM_MN_BITS = 6;
    static constexpr uint32_t BASE_PARAM_K1_BITS = 2;
    static constexpr uint32_t BASE_PARAM_K2_BITS = 2;
    static constexpr uint32_t BASE_PARAM_K3_BITS = 4;
    static constexpr uint32_t BASE_PARAM_DELTA_MODE_BITS = 3;
    static constexpr uint32_t BASE_PARAM_DELTA_BITS = 7;
    static constexpr uint32_t THREE_SPLINE_FLAG_BITS = 1;
    static constexpr uint32_t THREE_SPLINE_NUM_BITS = 1;
    static constexpr uint32_t THREE_SPLINE_NUM_MAX = 1 << THREE_SPLINE_NUM_BITS;
    static constexpr uint32_t THREE_SPLINE_TH_MODE_BITS = 2;
    static constexpr uint32_t THREE_SPLINE_TH_MB_BITS = 8;
    static constexpr uint32_t THREE_SPLINE_TH_BITS[3] = {12, 10, 10};
    static constexpr uint32_t THREE_SPLINE_STRENGTH_BITS = 8;
    static constexpr uint32_t COLOR_SATURATION_MAPPING_FLAG_BITS = 1;
    static constexpr uint32_t COLOR_SATURATION_NUM_BITS = 3;
    static constexpr uint32_t COLOR_SATURATION_NUM_MAX = (1 << COLOR_SATURATION_NUM_BITS) - 1;
    static constexpr uint32_t COLOR_SATURATION_GAIN_BITS = 8;

    static constexpr uint32_t UINT8_BITS = 8;

    struct Pos {
        uint32_t index;
        uint32_t bit;
    };

    static bool ExtractBits(const std::vector<uint8_t>& data, uint32_t numBits, uint32_t& value, Pos& pos);
    static bool InsertBits(uint32_t value, uint32_t numBits, std::vector<uint8_t>& data, Pos& pos);
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_METADATA_MANAGER_H