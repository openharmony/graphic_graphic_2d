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

#include <gtest/gtest.h>

#include "metadata_helper.h"
#include "surface_buffer_impl.h"

using namespace testing::ext;
using namespace OHOS::HDI::Display::Graphic::Common::V1_0;

namespace OHOS {
class MetadataManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase() {}

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    static inline sptr<SurfaceBuffer> buffer_ = nullptr;
    static inline sptr<SurfaceBuffer> nullBuffer_ = nullptr;
};

void MetadataManagerTest::SetUpTestCase()
{
    buffer_ = new SurfaceBufferImpl(0);
    auto ret = buffer_->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: MetadataManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: test ConvertMetadataToVec
*/
HWTEST_F(MetadataManagerTest, ConvertMetadataToVecTest, Function | SmallTest | Level2)
{
    uint32_t metadata = 0;
    std::vector<uint8_t> vec;
    ASSERT_EQ(MetadataHelper::ConvertMetadataToVec(metadata, vec), GSERROR_OK);

    ASSERT_EQ(vec.size(), 4);
    for (uint32_t i = 0; i < vec.size(); ++i) {
        ASSERT_EQ(vec[i], 0);
    }
}

/*
* Function: MetadataManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: test ConvertVecToMetadata
*/
HWTEST_F(MetadataManagerTest, ConvertVecToMetadataTest, Function | SmallTest | Level2)
{
    std::vector<uint8_t> vec;
    uint32_t metadata = 1;
    ASSERT_EQ(MetadataHelper::ConvertVecToMetadata(vec, metadata), GSERROR_NOT_SUPPORT);

    vec.assign(4, 0);
    ASSERT_EQ(MetadataHelper::ConvertVecToMetadata(vec, metadata), GSERROR_OK);
    ASSERT_EQ(metadata, 0);
}

/*
* Function: MetadataManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: test ConvertColorSpaceTypeToInfo
*/
HWTEST_F(MetadataManagerTest, ConvertColorSpaceTypeToInfoTest, Function | SmallTest | Level2)
{
    CM_ColorSpaceInfo colorSpaceInfo;
    ASSERT_EQ(MetadataHelper::ConvertColorSpaceTypeToInfo(CM_SRGB_FULL, colorSpaceInfo), GSERROR_OK);

    ASSERT_EQ(colorSpaceInfo.primaries, COLORPRIMARIES_SRGB);
    ASSERT_EQ(colorSpaceInfo.transfunc, TRANSFUNC_SRGB);
    ASSERT_EQ(colorSpaceInfo.matrix, MATRIX_BT601_N);
    ASSERT_EQ(colorSpaceInfo.range, RANGE_FULL);
}

/*
* Function: MetadataManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: test ConvertColorSpaceInfoToType
*/
HWTEST_F(MetadataManagerTest, ConvertColorSpaceInfoToTypeTest, Function | SmallTest | Level2)
{
    CM_ColorSpaceInfo colorSpaceInfo = {
        .primaries = COLORPRIMARIES_SRGB,
        .transfunc = TRANSFUNC_SRGB,
        .matrix = MATRIX_BT601_N,
        .range = RANGE_FULL,
    };
    CM_ColorSpaceType colorSpaceType;
    ASSERT_EQ(MetadataHelper::ConvertColorSpaceInfoToType(colorSpaceInfo, colorSpaceType), GSERROR_OK);

    ASSERT_EQ(colorSpaceType, CM_SRGB_FULL);
}

/*
* Function: MetadataManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: test SetColorSpaceInfo and GetColorSpaceInfo
*/
HWTEST_F(MetadataManagerTest, ColorSpaceInfoTest, Function | SmallTest | Level2)
{
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_SRGB,
        .transfunc = TRANSFUNC_SRGB,
        .matrix = MATRIX_BT709,
        .range = RANGE_FULL,
    };

    auto retSet = MetadataHelper::SetColorSpaceInfo(buffer_, infoSet);
    ASSERT_TRUE(retSet == GSERROR_OK || GSErrorStr(retSet) == "<500 api call failed>with low error <Not supported>");

    CM_ColorSpaceInfo infoGet;
    auto retGet = MetadataHelper::GetColorSpaceInfo(buffer_, infoGet);
    ASSERT_TRUE(retGet == GSERROR_OK || GSErrorStr(retGet) == "<500 api call failed>with low error <Not supported>");

    if (retSet == GSERROR_OK && retGet == GSERROR_OK) {
        ASSERT_EQ(infoSet.primaries, infoGet.primaries);
        ASSERT_EQ(infoSet.transfunc, infoGet.transfunc);
        ASSERT_EQ(infoSet.matrix, infoGet.matrix);
        ASSERT_EQ(infoSet.range, infoGet.range);
    }

    ASSERT_EQ(MetadataHelper::SetColorSpaceInfo(nullBuffer_, infoSet), GSERROR_NO_BUFFER);
    ASSERT_EQ(MetadataHelper::GetColorSpaceInfo(nullBuffer_, infoGet), GSERROR_NO_BUFFER);
}

/*
* Function: MetadataManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: test SetColorSpaceType and GetColorSpaceType
*/
HWTEST_F(MetadataManagerTest, ColorSpaceTypeTest, Function | SmallTest | Level2)
{
    auto retSet = MetadataHelper::SetColorSpaceType(buffer_, CM_SRGB_FULL);
    ASSERT_TRUE(retSet == GSERROR_OK || GSErrorStr(retSet) == "<500 api call failed>with low error <Not supported>");

    CM_ColorSpaceType colorSpaceType;
    auto retGet = MetadataHelper::GetColorSpaceType(buffer_, colorSpaceType);
    ASSERT_TRUE(retGet == GSERROR_OK || GSErrorStr(retGet) == "<500 api call failed>with low error <Not supported>");

    if (retSet == GSERROR_OK && retGet == GSERROR_OK) {
        ASSERT_EQ(colorSpaceType, CM_SRGB_FULL);
    }

    ASSERT_EQ(MetadataHelper::SetColorSpaceType(nullBuffer_, CM_SRGB_FULL), GSERROR_NO_BUFFER);
    ASSERT_EQ(MetadataHelper::GetColorSpaceType(nullBuffer_, colorSpaceType), GSERROR_NO_BUFFER);
}

/*
* Function: MetadataManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: test SetHDRMetadataType and GetHDRMetadataType
*/
HWTEST_F(MetadataManagerTest, HDRMetadataTypeTest, Function | SmallTest | Level2)
{
    auto retSet = MetadataHelper::SetHDRMetadataType(buffer_, CM_VIDEO_HDR_VIVID);
    ASSERT_TRUE(retSet == GSERROR_OK || GSErrorStr(retSet) == "<500 api call failed>with low error <Not supported>");

    CM_HDR_Metadata_Type hdrMetadataType;
    auto retGet = MetadataHelper::GetHDRMetadataType(buffer_, hdrMetadataType);
    ASSERT_TRUE(retGet == GSERROR_OK || GSErrorStr(retGet) == "<500 api call failed>with low error <Not supported>");

    if (retSet == GSERROR_OK && retGet == GSERROR_OK) {
        ASSERT_EQ(hdrMetadataType, CM_VIDEO_HDR_VIVID);
    }

    ASSERT_EQ(MetadataHelper::SetHDRMetadataType(nullBuffer_, CM_VIDEO_HDR_VIVID), GSERROR_NO_BUFFER);
    ASSERT_EQ(MetadataHelper::GetHDRMetadataType(nullBuffer_, hdrMetadataType), GSERROR_NO_BUFFER);
}

/*
* Function: MetadataManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: test SetHDRStaticMetadata and GetHDRStaticMetadata
*/
HWTEST_F(MetadataManagerTest, HDRStaticMetadataTest, Function | SmallTest | Level2)
{
    HdrStaticMetadata metadataSet = {
        .smpte2086 = {
            .displayPrimaryRed = {0.1f, 0.1f},
            .displayPrimaryGreen = {0.2f, 0.2f},
            .displayPrimaryBlue = {0.3f, 0.3f},
            .whitePoint = {0.4f, 0.4f},
            .maxLuminance = 1000.0f,
            .minLuminance = 0.1f,
        },
        .cta861 = {
            .maxContentLightLevel = 500.0f,
            .maxFrameAverageLightLevel = 300.0f,
        },
    };

    auto retSet = MetadataHelper::SetHDRStaticMetadata(buffer_, metadataSet);
    ASSERT_TRUE(retSet == GSERROR_OK || GSErrorStr(retSet) == "<500 api call failed>with low error <Not supported>");

    HdrStaticMetadata metadataGet;
    auto retGet = MetadataHelper::GetHDRStaticMetadata(buffer_, metadataGet);
    ASSERT_TRUE(retGet == GSERROR_OK || GSErrorStr(retGet) == "<500 api call failed>with low error <Not supported>");

    if (retSet == GSERROR_OK && retGet == GSERROR_OK) {
        ASSERT_EQ(metadataSet.smpte2086.displayPrimaryRed.x, metadataGet.smpte2086.displayPrimaryRed.x);
        ASSERT_EQ(metadataSet.smpte2086.displayPrimaryRed.y, metadataGet.smpte2086.displayPrimaryRed.y);
        ASSERT_EQ(metadataSet.smpte2086.displayPrimaryGreen.x, metadataGet.smpte2086.displayPrimaryGreen.x);
        ASSERT_EQ(metadataSet.smpte2086.displayPrimaryGreen.y, metadataGet.smpte2086.displayPrimaryGreen.y);
        ASSERT_EQ(metadataSet.smpte2086.displayPrimaryBlue.x, metadataGet.smpte2086.displayPrimaryBlue.x);
        ASSERT_EQ(metadataSet.smpte2086.displayPrimaryBlue.y, metadataGet.smpte2086.displayPrimaryBlue.y);
        ASSERT_EQ(metadataSet.smpte2086.whitePoint.x, metadataGet.smpte2086.whitePoint.x);
        ASSERT_EQ(metadataSet.smpte2086.whitePoint.y, metadataGet.smpte2086.whitePoint.y);
        ASSERT_EQ(metadataSet.smpte2086.maxLuminance, metadataGet.smpte2086.maxLuminance);
        ASSERT_EQ(metadataSet.smpte2086.minLuminance, metadataGet.smpte2086.minLuminance);
        ASSERT_EQ(metadataSet.cta861.maxContentLightLevel, metadataGet.cta861.maxContentLightLevel);
        ASSERT_EQ(metadataSet.cta861.maxFrameAverageLightLevel, metadataGet.cta861.maxFrameAverageLightLevel);
    }

    ASSERT_EQ(MetadataHelper::SetHDRStaticMetadata(nullBuffer_, metadataSet), GSERROR_NO_BUFFER);
    ASSERT_EQ(MetadataHelper::GetHDRStaticMetadata(nullBuffer_, metadataGet), GSERROR_NO_BUFFER);
}

/*
* Function: MetadataManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: test SetHDRDynamicMetadata and GetHDRDynamicMetadata
*/
HWTEST_F(MetadataManagerTest, HDRDynamicMetadataTest, Function | SmallTest | Level2)
{
    std::vector<uint8_t> metadataSet{1, 18, 119, 33, 196, 253, 112, 171, 74, 230, 99, 23, 0, 244, 82, 138, 13, 158, 100,
        41, 50, 189, 111, 144, 3, 153, 75, 210, 243, 237, 19, 12, 128};

    auto retSet = MetadataHelper::SetHDRDynamicMetadata(buffer_, metadataSet);
    ASSERT_TRUE(retSet == GSERROR_OK || GSErrorStr(retSet) == "<500 api call failed>with low error <Not supported>");

    std::vector<uint8_t> metadataGet;
    auto retGet = MetadataHelper::GetHDRDynamicMetadata(buffer_, metadataGet);
    ASSERT_TRUE(retGet == GSERROR_OK || GSErrorStr(retGet) == "<500 api call failed>with low error <Not supported>");

    if (retSet == GSERROR_OK && retGet == GSERROR_OK) {
        ASSERT_EQ(metadataSet.size(), metadataGet.size());
        for (uint32_t i = 0; i < metadataSet.size(); i++) {
            ASSERT_EQ(metadataSet[i], metadataGet[i]);
        }
    }

    ASSERT_EQ(MetadataHelper::SetHDRDynamicMetadata(nullBuffer_, metadataSet), GSERROR_NO_BUFFER);
    ASSERT_EQ(MetadataHelper::GetHDRDynamicMetadata(nullBuffer_, metadataGet), GSERROR_NO_BUFFER);
}

/*
* Function: MetadataManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: test SetHDRStaticMetadata and GetHDRStaticMetadata
*/
HWTEST_F(MetadataManagerTest, HDRStaticMetadataVecTest, Function | SmallTest | Level2)
{
    HdrStaticMetadata metadata = {
        .smpte2086 = {
            .displayPrimaryRed = {0.1f, 0.1f},
            .displayPrimaryGreen = {0.2f, 0.2f},
            .displayPrimaryBlue = {0.3f, 0.3f},
            .whitePoint = {0.4f, 0.4f},
            .maxLuminance = 1000.0f,
            .minLuminance = 0.1f,
        },
        .cta861 = {
            .maxContentLightLevel = 500.0f,
            .maxFrameAverageLightLevel = 300.0f,
        },
    };

    std::vector<uint8_t> metadataSet;
    ASSERT_EQ(MetadataHelper::ConvertMetadataToVec(metadata, metadataSet), GSERROR_OK);

    auto retSet = MetadataHelper::SetHDRStaticMetadata(buffer_, metadataSet);
    ASSERT_TRUE(retSet == GSERROR_OK || GSErrorStr(retSet) == "<500 api call failed>with low error <Not supported>");

    std::vector<uint8_t> metadataGet;
    auto retGet = MetadataHelper::GetHDRStaticMetadata(buffer_, metadataGet);
    ASSERT_TRUE(retGet == GSERROR_OK || GSErrorStr(retGet) == "<500 api call failed>with low error <Not supported>");

    if (retSet == GSERROR_OK && retGet == GSERROR_OK) {
        ASSERT_EQ(metadataSet.size(), metadataGet.size());
        for (uint32_t i = 0; i < metadataSet.size(); i++) {
            ASSERT_EQ(metadataSet[i], metadataGet[i]);
        }
    }

    ASSERT_EQ(MetadataHelper::SetHDRStaticMetadata(nullBuffer_, metadataSet), GSERROR_NO_BUFFER);
    ASSERT_EQ(MetadataHelper::GetHDRStaticMetadata(nullBuffer_, metadataGet), GSERROR_NO_BUFFER);
}
}