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

GSError MetadataHelper::SetHDRDynamicMetadata(sptr<SurfaceBuffer>& buffer,
    const std::vector<uint8_t>& hdrDynamicMetadata)
{
    return buffer->SetMetadata(ATTRKEY_HDR_DYNAMIC_METADATA, hdrDynamicMetadata);
}

GSError MetadataHelper::GetHDRDynamicMetadata(const sptr<SurfaceBuffer>& buffer,
    std::vector<uint8_t>& hdrDynamicMetadata)
{
    return buffer->GetMetadata(ATTRKEY_HDR_DYNAMIC_METADATA, hdrDynamicMetadata);
}

GSError MetadataHelper::SetHDRStaticMetadata(sptr<SurfaceBuffer>& buffer,
    const std::vector<uint8_t>& hdrStaticMetadata)
{
    return buffer->SetMetadata(ATTRKEY_HDR_STATIC_METADATA, hdrStaticMetadata);
}

GSError MetadataHelper::GetHDRStaticMetadata(const sptr<SurfaceBuffer>& buffer,
    std::vector<uint8_t>& hdrStaticMetadata)
{
    return buffer->GetMetadata(ATTRKEY_HDR_STATIC_METADATA, hdrStaticMetadata);
}
} // namespace OHOS