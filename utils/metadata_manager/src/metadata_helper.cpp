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
namespace MetadataManager {
GSError MetadataHelper::SetColorSpaceType(sptr<SurfaceBuffer>& buffer, const CM_ColorSpaceType& colorSpaceType)
{
    std::vector<uint8_t> colorSpaceTypeVec;
    auto ret = ConvertMetadataToVec(colorSpaceType, colorSpaceTypeVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return buffer->SetMetadata(ATTRKEY_COLORSPACE_TYPE, colorSpaceTypeVec);
}

GSError MetadataHelper::GetColorSpaceType(const sptr<SurfaceBuffer>& buffer, CM_ColorSpaceType& colorSpaceType)
{
    std::vector<uint8_t> colorSpaceTypeVec;
    auto ret = buffer->GetMetadata(ATTRKEY_COLORSPACE_TYPE, colorSpaceTypeVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return ConvertVecToMetadata(colorSpaceTypeVec, colorSpaceType);
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
} // namespace MetadataManager
} // namespace OHOS