/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include <securec.h>
#include <gtest/gtest.h>
#include <surface.h>
#include <surface_buffer_impl.h>
#include <buffer_utils.h>
#include <metadata_helper.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class SurfaceBufferImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    static inline sptr<SurfaceBuffer> buffer = nullptr;
    static inline int32_t val32 = 0;
    static inline int64_t val64 = 0;
};

void SurfaceBufferImplTest::SetUpTestCase()
{
    buffer = nullptr;
    val32 = 0;
    val64 = 0;
}

void SurfaceBufferImplTest::TearDownTestCase()
{
    buffer = nullptr;
}

/*
* Function: GetSeqNum
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. new SurfaceBufferImpl and GetSeqNum
*                  2. new SurfaceBufferImpl again and check GetSeqNum = oldSeq + 1
 */
HWTEST_F(SurfaceBufferImplTest, NewSeqIncrease001, Function | MediumTest | Level2)
{
    buffer = new SurfaceBufferImpl();
    int oldSeq = buffer->GetSeqNum();

    buffer = new SurfaceBufferImpl();
    ASSERT_EQ(oldSeq + 1, buffer->GetSeqNum());
}

/*
* Function: check buffer state
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. check buffer state, such as bufferhandle, virAddr, fileDescriptor and size
 */
HWTEST_F(SurfaceBufferImplTest, State001, Function | MediumTest | Level2)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);
    ASSERT_EQ(buffer->GetVirAddr(), nullptr);
    ASSERT_EQ(buffer->GetFileDescriptor(), -1);
    ASSERT_EQ(buffer->GetSize(), 0u);
}

/*
* Function: check buffer state
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandle and Alloc
*                  2. check buffer state, such as bufferhandle, virAddr and size
*                  3. call Free
*                  4. check ret
 */
HWTEST_F(SurfaceBufferImplTest, State002, Function | MediumTest | Level2)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);

    GSError ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ASSERT_NE(buffer->GetBufferHandle(), nullptr);
    ASSERT_NE(buffer->GetVirAddr(), nullptr);
    ASSERT_NE(buffer->GetSize(), 0u);
    ASSERT_EQ(buffer->GetFormat(), GRAPHIC_PIXEL_FMT_RGBA_8888);
    ASSERT_EQ(buffer->GetUsage(), BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);
    ASSERT_EQ(buffer->GetSurfaceBufferColorGamut(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3);
}

/*
* Function: parcel
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. new SurfaceBufferImpl and Alloc
*                  2. call Set data interface
*                  3. call WriteSurfaceBufferImpl and ReadSurfaceBufferImpl
*                  4. call Get data interface
*                  5. check ret
 */
HWTEST_F(SurfaceBufferImplTest, Parcel001, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> sbi = new SurfaceBufferImpl(0);
    auto sret = sbi->Alloc(requestConfig);
    ASSERT_EQ(sret, OHOS::GSERROR_OK);

    MessageParcel parcel;
    WriteSurfaceBufferImpl(parcel, sbi->GetSeqNum(), sbi);

    sptr<SurfaceBuffer> buffer = nullptr;
    uint32_t seq;
    ReadSurfaceBufferImpl(parcel, seq, buffer);
    ASSERT_NE(buffer, nullptr);
}

/*
* Function: Create
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Call SurfaceBuffer::Create()
*                  2. check ret
 */
HWTEST_F(SurfaceBufferImplTest, Create001, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
}

/*
* Function: Set/Get/List/Erase Metadata
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. new SurfaceBufferImpl and Alloc
*                  2. call Set Metadata interface
*                  3. call Get Metadata interface
*                  4. check ret
*                  5. call List Metadata keys interface
*                  6. check ret
*                  7. call Erase Metadata key interface
*                  8. call List Metadata keys interface again
*                  9. check ret
*/
HWTEST_F(SurfaceBufferImplTest, Metadata001, Function | MediumTest | Level2)
{
    using namespace HDI::Display::Graphic::Common::V1_0;

    sptr<SurfaceBuffer> sbi = new SurfaceBufferImpl(0);
    auto sret = sbi->Alloc(requestConfig);
    ASSERT_EQ(sret, OHOS::GSERROR_OK);

    uint32_t metadataKey = 2;

    uint32_t setMetadata = 4260097;
    std::vector<uint8_t> setData;
    ASSERT_EQ(MetadataHelper::ConvertMetadataToVec(setMetadata, setData), OHOS::GSERROR_OK);
    sret = sbi->SetMetadata(metadataKey, setData);
    ASSERT_TRUE(sret == OHOS::GSERROR_OK || GSErrorStr(sret) == "<500 api call failed>with low error <Not supported>");

    std::vector<uint8_t> getData;
    sret = sbi->GetMetadata(metadataKey, getData);
    ASSERT_TRUE(sret == OHOS::GSERROR_OK || GSErrorStr(sret) == "<500 api call failed>with low error <Not supported>");

    if (sret == OHOS::GSERROR_OK) {
        uint32_t getMetadata;
        ASSERT_EQ(MetadataHelper::ConvertVecToMetadata(getData, getMetadata), OHOS::GSERROR_OK);
        ASSERT_EQ(setMetadata, getMetadata);
    }

    std::vector<uint32_t> keys;

    sret = sbi->ListMetadataKeys(keys);
    ASSERT_TRUE(sret == OHOS::GSERROR_OK || GSErrorStr(sret) == "<500 api call failed>with low error <Not supported>");
    if (sret == OHOS::GSERROR_OK) {
        ASSERT_EQ(sret, OHOS::GSERROR_OK);
        ASSERT_EQ(keys.size(), 1);
        ASSERT_EQ(keys[0], metadataKey);
    }

    sret = sbi->EraseMetadataKey(metadataKey);
    ASSERT_TRUE(sret == OHOS::GSERROR_OK || GSErrorStr(sret) == "<500 api call failed>with low error <Not supported>");

    sret = sbi->ListMetadataKeys(keys);
    ASSERT_TRUE(sret == OHOS::GSERROR_OK || GSErrorStr(sret) == "<500 api call failed>with low error <Not supported>");
    if (sret == OHOS::GSERROR_OK) {
        ASSERT_EQ(keys.size(), 0);
    }
}
}
