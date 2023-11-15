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

#include "metadata_helper.h"

#include "v1_0/buffer_handle_meta_key_type.h"

using namespace OHOS::HDI::Display::Graphic::Common::V1_0;

namespace OHOS {
GSError MetadataHelper::ConvertColorSpaceTypeToInfo(const CM_ColorSpaceType& colorSpaceType,
    CM_ColorSpaceInfo& colorSpaceInfo)
{
    colorSpaceInfo.primaries = static_cast<CM_ColorPrimaries>(colorSpaceType & CM_PRIMARIES_MASK);
    colorSpaceInfo.transfunc = static_cast<CM_TransFunc>((colorSpaceType & CM_TRANSFUNC_MASK) >> TRANSFUNC_OFFSET);
    colorSpaceInfo.matrix = static_cast<CM_Matrix>((colorSpaceType & CM_MATRIX_MASK) >> MATRIX_OFFSET);
    colorSpaceInfo.range = static_cast<CM_Range>((colorSpaceType & CM_RANGE_MASK) >> RANGE_OFFSET);
    return GSERROR_OK;
}

GSError MetadataHelper::ConvertColorSpaceInfoToType(const CM_ColorSpaceInfo& colorSpaceInfo,
    CM_ColorSpaceType& colorSpaceType)
{
    colorSpaceType = static_cast<CM_ColorSpaceType>(colorSpaceInfo.primaries |
        (colorSpaceInfo.transfunc << TRANSFUNC_OFFSET) | (colorSpaceInfo.matrix << MATRIX_OFFSET) |
        (colorSpaceInfo.range << RANGE_OFFSET));

    return GSERROR_OK;
}

GSError MetadataHelper::SetColorSpaceInfo(sptr<SurfaceBuffer>& buffer, const CM_ColorSpaceInfo& colorSpaceInfo)
{
    std::vector<uint8_t> colorSpaceInfoVec;
    auto ret = ConvertMetadataToVec(colorSpaceInfo, colorSpaceInfoVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return buffer->SetMetadata(ATTRKEY_COLORSPACE_INFO, colorSpaceInfoVec);
}

GSError MetadataHelper::GetColorSpaceInfo(const sptr<SurfaceBuffer>& buffer, CM_ColorSpaceInfo& colorSpaceInfo)
{
    std::vector<uint8_t> colorSpaceInfoVec;
    auto ret = buffer->GetMetadata(ATTRKEY_COLORSPACE_INFO, colorSpaceInfoVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return ConvertVecToMetadata(colorSpaceInfoVec, colorSpaceInfo);
}

GSError MetadataHelper::SetColorSpaceType(sptr<SurfaceBuffer>& buffer, const CM_ColorSpaceType& colorSpaceType)
{
    CM_ColorSpaceInfo colorSpaceInfo;
    auto ret = ConvertColorSpaceTypeToInfo(colorSpaceType, colorSpaceInfo);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return SetColorSpaceInfo(buffer, colorSpaceInfo);
}

GSError MetadataHelper::GetColorSpaceType(const sptr<SurfaceBuffer>& buffer, CM_ColorSpaceType& colorSpaceType)
{
    CM_ColorSpaceInfo colorSpaceInfo;
    auto ret = GetColorSpaceInfo(buffer, colorSpaceInfo);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return ConvertColorSpaceInfoToType(colorSpaceInfo, colorSpaceType);
}

GSError MetadataHelper::SetHDRMetadataType(sptr<SurfaceBuffer>& buffer, const CM_HDR_Metadata_Type& hdrMetadataType)
{
    std::vector<uint8_t> hdrMetadataTypeVec;
    auto ret = ConvertMetadataToVec(hdrMetadataType, hdrMetadataTypeVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return buffer->SetMetadata(ATTRKEY_HDR_METADATA_TYPE, hdrMetadataTypeVec);
}

GSError MetadataHelper::GetHDRMetadataType(const sptr<SurfaceBuffer>& buffer, CM_HDR_Metadata_Type& hdrMetadataType)
{
    std::vector<uint8_t> hdrMetadataTypeVec;
    auto ret = buffer->GetMetadata(ATTRKEY_HDR_METADATA_TYPE, hdrMetadataTypeVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return ConvertVecToMetadata(hdrMetadataTypeVec, hdrMetadataType);
}

GSError MetadataHelper::SetHDRStaticMetadata(sptr<SurfaceBuffer>& buffer,
    const HdrStaticMetadata& hdrStaticMetadata)
{
    std::vector<uint8_t> hdrStaticMetadataVec;
    auto ret = ConvertMetadataToVec(hdrStaticMetadata, hdrStaticMetadataVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return buffer->SetMetadata(ATTRKEY_HDR_STATIC_METADATA, hdrStaticMetadataVec);
}

GSError MetadataHelper::GetHDRStaticMetadata(const sptr<SurfaceBuffer>& buffer,
    HdrStaticMetadata& hdrStaticMetadata)
{
    std::vector<uint8_t> hdrStaticMetadataVec;
    auto ret = buffer->GetMetadata(ATTRKEY_HDR_STATIC_METADATA, hdrStaticMetadataVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return ConvertVecToMetadata(hdrStaticMetadataVec, hdrStaticMetadata);
}

GSError MetadataHelper::ConvertVecToHDRVividDynMetadataV1(const std::vector<uint8_t>& data,
    HdrVividMetadataV1& hdrVividMetadata)
{
    Pos pos = {0, 0};
    bool success = true;

    success &= ExtractBits(data, SYSTEM_START_CODE_BITS, hdrVividMetadata.systemStartCode, pos);
    if (hdrVividMetadata.systemStartCode != SYSTEM_START_CODE) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    success &= ExtractBits(data, MINIMUM_MAXRGB_BITS, hdrVividMetadata.minimumMaxRgbPq, pos);
    success &= ExtractBits(data, AVERAGE_MAXRGB_BITS, hdrVividMetadata.averageMaxRgbPq, pos);
    success &= ExtractBits(data, VARIANCE_MAXRGB_BITS, hdrVividMetadata.varianceMaxRgbPq, pos);
    success &= ExtractBits(data, MAXIMUM_MAXRGB_BITS, hdrVividMetadata.maximumMaxRgbPq, pos);

    success &= ExtractBits(data, TONE_MAPPING_MODE_BITS, hdrVividMetadata.toneMappingMode, pos);
    success &= ExtractBits(data, TONE_MAPPING_PARAM_NUM_BITS, hdrVividMetadata.toneMappingParamNum, pos);
    hdrVividMetadata.toneMappingParamNum++;
    for (uint32_t i = 0; i < hdrVividMetadata.toneMappingParamNum; ++i) {
        success &= ExtractBits(data, TARGETED_SYSTEM_DISPLAY_MAXIMUM_LUMINANCE_BITS,
            hdrVividMetadata.targetedSystemDisplayMaximumLuminance[i], pos);
        success &= ExtractBits(data, BASE_FLAG_BITS, hdrVividMetadata.baseFlag[i], pos);
        if (hdrVividMetadata.baseFlag[i] == 0) {
            continue;
        }

        success &= ExtractBits(data, BASE_PARAM_MP_BITS, hdrVividMetadata.baseParamMp[i], pos);
        success &= ExtractBits(data, BASE_PARAM_MM_BITS, hdrVividMetadata.baseParamMm[i], pos);
        success &= ExtractBits(data, BASE_PARAM_MA_BITS, hdrVividMetadata.baseParamMa[i], pos);
        success &= ExtractBits(data, BASE_PARAM_MB_BITS, hdrVividMetadata.baseParamMb[i], pos);
        success &= ExtractBits(data, BASE_PARAM_MN_BITS, hdrVividMetadata.baseParamMn[i], pos);
        success &= ExtractBits(data, BASE_PARAM_K1_BITS, hdrVividMetadata.baseParamK1[i], pos);
        success &= ExtractBits(data, BASE_PARAM_K2_BITS, hdrVividMetadata.baseParamK2[i], pos);
        success &= ExtractBits(data, BASE_PARAM_K3_BITS, hdrVividMetadata.baseParamK3[i], pos);
        success &= ExtractBits(data, BASE_PARAM_DELTA_MODE_BITS, hdrVividMetadata.baseParamDeltaMode[i], pos);
        success &= ExtractBits(data, BASE_PARAM_DELTA_BITS, hdrVividMetadata.baseParamDelta[i], pos);

        success &= ExtractBits(data, THREE_SPLINE_FLAG_BITS, hdrVividMetadata.threeSplineFlag[i], pos);
        if (hdrVividMetadata.threeSplineFlag[i] == 0) {
            hdrVividMetadata.threeSplineNum[i] = 1;
            hdrVividMetadata.threeSplineThMode[i][0] = 0;
            continue;
        }

        success &= ExtractBits(data, THREE_SPLINE_NUM_BITS, hdrVividMetadata.threeSplineNum[i], pos);
        hdrVividMetadata.threeSplineNum[i]++;
        for (uint32_t j = 0; j < hdrVividMetadata.threeSplineNum[i]; ++j) {
            success &= ExtractBits(data, THREE_SPLINE_TH_MODE_BITS, hdrVividMetadata.threeSplineThMode[i][j], pos);
            if (hdrVividMetadata.threeSplineThMode[i][j] == 0 || hdrVividMetadata.threeSplineThMode[i][j] == 2) {
                success &= ExtractBits(data, THREE_SPLINE_TH_MB_BITS, hdrVividMetadata.threeSplineThMb[i][j], pos);
            }

            for (uint32_t k = 0; k < 3; ++k) {
                success &= ExtractBits(data, THREE_SPLINE_TH_BITS[k], hdrVividMetadata.threeSplineTh[i][j][k], pos);
            }

            success &= ExtractBits(data, THREE_SPLINE_STRENGTH_BITS, hdrVividMetadata.threeSplineStrength[i][j], pos);
        }
    }

    success &= ExtractBits(data, COLOR_SATURATION_MAPPING_FLAG_BITS, hdrVividMetadata.colorSaturationMappingFlag, pos);
    if (hdrVividMetadata.colorSaturationMappingFlag == 1) {
        success &= ExtractBits(data, COLOR_SATURATION_NUM_BITS, hdrVividMetadata.colorSaturationNum, pos);
        for (uint32_t i = 0; i < hdrVividMetadata.colorSaturationNum; ++i) {
            success &= ExtractBits(data, COLOR_SATURATION_GAIN_BITS, hdrVividMetadata.colorSaturationGain[i], pos);
        }
    }

    if (success) {
        return GSERROR_OK;
    } else {
        return GSERROR_INVALID_ARGUMENTS;
    }
}

GSError MetadataHelper::ConvertHDRVividDynMetadataV1ToVec(const HdrVividMetadataV1& hdrVividMetadata,
    std::vector<uint8_t>& data)
{
    Pos pos = {0, 0};
    bool success = true;
    data = {0};

    if (hdrVividMetadata.systemStartCode != SYSTEM_START_CODE) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    success &= InsertBits(hdrVividMetadata.systemStartCode, SYSTEM_START_CODE_BITS, data, pos);

    success &= InsertBits(hdrVividMetadata.minimumMaxRgbPq, MINIMUM_MAXRGB_BITS, data, pos);
    success &= InsertBits(hdrVividMetadata.averageMaxRgbPq, AVERAGE_MAXRGB_BITS, data, pos);
    success &= InsertBits(hdrVividMetadata.varianceMaxRgbPq, VARIANCE_MAXRGB_BITS, data, pos);
    success &= InsertBits(hdrVividMetadata.maximumMaxRgbPq, MAXIMUM_MAXRGB_BITS, data, pos);

    success &= InsertBits(hdrVividMetadata.toneMappingMode, TONE_MAPPING_MODE_BITS, data, pos);
    if (hdrVividMetadata.toneMappingParamNum > TONE_MAPPING_PARAM_NUM_MAX) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    success &= InsertBits(hdrVividMetadata.toneMappingParamNum - 1, TONE_MAPPING_PARAM_NUM_BITS, data, pos);
    for (uint32_t i = 0; i < hdrVividMetadata.toneMappingParamNum - 1; ++i) {
        success &= InsertBits(hdrVividMetadata.targetedSystemDisplayMaximumLuminance[i],
            TARGETED_SYSTEM_DISPLAY_MAXIMUM_LUMINANCE_BITS, data, pos);
        success &= InsertBits(hdrVividMetadata.baseFlag[i], BASE_FLAG_BITS, data, pos);
        if (hdrVividMetadata.baseFlag[i] == 0) {
            continue;
        }

        success &= InsertBits(hdrVividMetadata.baseParamMp[i], BASE_PARAM_MP_BITS, data, pos);
        success &= InsertBits(hdrVividMetadata.baseParamMm[i], BASE_PARAM_MM_BITS, data, pos);
        success &= InsertBits(hdrVividMetadata.baseParamMa[i], BASE_PARAM_MA_BITS, data, pos);
        success &= InsertBits(hdrVividMetadata.baseParamMb[i], BASE_PARAM_MB_BITS, data, pos);
        success &= InsertBits(hdrVividMetadata.baseParamMn[i], BASE_PARAM_MN_BITS, data, pos);
        success &= InsertBits(hdrVividMetadata.baseParamK1[i], BASE_PARAM_K1_BITS, data, pos);
        success &= InsertBits(hdrVividMetadata.baseParamK2[i], BASE_PARAM_K2_BITS, data, pos);
        success &= InsertBits(hdrVividMetadata.baseParamK3[i], BASE_PARAM_K3_BITS, data, pos);
        success &= InsertBits(hdrVividMetadata.baseParamDeltaMode[i], BASE_PARAM_DELTA_MODE_BITS, data, pos);
        success &= InsertBits(hdrVividMetadata.baseParamDelta[i], BASE_PARAM_DELTA_BITS, data, pos);

        success &= InsertBits(hdrVividMetadata.threeSplineFlag[i], THREE_SPLINE_FLAG_BITS, data, pos);
        if (hdrVividMetadata.threeSplineFlag[i] == 0) {
            continue;
        }

        if (hdrVividMetadata.threeSplineNum[i] > THREE_SPLINE_NUM_MAX) {
            return GSERROR_INVALID_ARGUMENTS;
        }
        success &= InsertBits(hdrVividMetadata.threeSplineNum[i], THREE_SPLINE_NUM_BITS, data, pos);
        for (uint32_t j = 0; j < hdrVividMetadata.threeSplineNum[i] - 1; ++j) {
            success &= InsertBits(hdrVividMetadata.threeSplineThMode[i][j], THREE_SPLINE_TH_MODE_BITS, data, pos);
            if (hdrVividMetadata.threeSplineThMode[i][j] == 0 || hdrVividMetadata.threeSplineThMode[i][j] == 2) {
                success &= InsertBits(hdrVividMetadata.threeSplineThMb[i][j], THREE_SPLINE_TH_MB_BITS, data, pos);
            }

            for (uint32_t k = 0; k < 3; ++k) {
                success &= InsertBits(hdrVividMetadata.threeSplineTh[i][j][k], THREE_SPLINE_TH_BITS[k], data, pos);
            }

            success &= InsertBits(hdrVividMetadata.threeSplineStrength[i][j], THREE_SPLINE_STRENGTH_BITS, data, pos);
        }
    }

    success &= InsertBits(hdrVividMetadata.colorSaturationMappingFlag, COLOR_SATURATION_MAPPING_FLAG_BITS, data, pos);
    if (hdrVividMetadata.colorSaturationMappingFlag == 1) {
        if (hdrVividMetadata.colorSaturationNum > COLOR_SATURATION_NUM_MAX) {
            return GSERROR_INVALID_ARGUMENTS;
        }
        success &= InsertBits(hdrVividMetadata.colorSaturationNum, COLOR_SATURATION_NUM_BITS, data, pos);
        for (uint32_t i = 0; i < hdrVividMetadata.colorSaturationNum; ++i) {
            success &= InsertBits(hdrVividMetadata.colorSaturationGain[i], COLOR_SATURATION_NUM_BITS, data, pos);
        }
    }

    if (success) {
        return GSERROR_OK;
    } else {
        return GSERROR_INVALID_ARGUMENTS;
    }
}

GSError MetadataHelper::SetHDRVividDynMetadataV1(sptr<SurfaceBuffer>& buffer,
    const HdrVividMetadataV1& hdrVividMetadata)
{
    std::vector<uint8_t> hdrVividMetadataVec;
    auto ret = ConvertMetadataToVec(hdrVividMetadata, hdrVividMetadataVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return buffer->SetMetadata(ATTRKEY_HDR_DYNAMIC_METADATA, hdrVividMetadataVec);
}

GSError MetadataHelper::GetHDRVividDynMetadataV1(const sptr<SurfaceBuffer>& buffer,
    HdrVividMetadataV1& hdrVividMetadata)
{
    std::vector<uint8_t> hdrVividMetadataVec;
    auto ret = buffer->GetMetadata(ATTRKEY_HDR_DYNAMIC_METADATA, hdrVividMetadataVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return ConvertVecToMetadata(hdrVividMetadataVec, hdrVividMetadata);
}

bool MetadataHelper::ExtractBits(const std::vector<uint8_t>& data, uint32_t numBits, uint32_t& value, Pos& pos)
{
    value = 0;
    while (pos.bit + numBits > UINT8_BITS) {
        if (pos.index >= data.size()) {
            return false;
        }
        uint32_t rest = UINT8_BITS - pos.bit;
        value = (value << rest) | (data[pos.index] & ((1 << rest) - 1));
        numBits -= rest;
        pos.index++;
        pos.bit = 0;
    }

    if (pos.index >= data.size()) {
        return false;
    }
    value = (value << numBits) | ((data[pos.index] >> (UINT8_BITS - pos.bit - numBits)) & ((1 << numBits) - 1));
    pos.bit += numBits;
    return true;
}

bool MetadataHelper::InsertBits(uint32_t value, uint32_t numBits, std::vector<uint8_t>& data, Pos& pos)
{
    if (value >= (1 << numBits)) {
        return false;
    }

    while (pos.bit + numBits > UINT8_BITS) {
        uint32_t rest = UINT8_BITS - pos.bit;
        data[pos.index] |= (value >> (numBits - rest));
        value &= (1 << (numBits - rest)) - 1;
        numBits -= rest;
        data.push_back(0);
        pos.index++;
        pos.bit = 0;
    }

    data[pos.index] |= value << (UINT8_BITS - pos.bit - numBits);
    pos.bit += numBits;
    return true;
}
} // namespace OHOS